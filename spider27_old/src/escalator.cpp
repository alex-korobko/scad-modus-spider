#include "global.h"

/*
Local functions
*/
static int is_connected(int sock, fd_set* rd, fd_set* wr, fd_set* ex)
{
	int err;
	size_t 	length = sizeof(err);

	if (!FD_ISSET(sock, rd) && !FD_ISSET(sock, wr))
		return 0;

	if (getsockopt(sock, SOL_SOCKET, SO_ERROR, &err, &length) < 0)
		return 0;
	errno = err;
	
	return err == 0;
}


static int connect_to_server(in_addr_t addr)
{
	int		sock;
	struct	sockaddr_in address;
	fd_set	readFD, writeFD, exFD;
	struct timeval timeout;
	int		flags;
	int		result;

	address.sin_addr.s_addr = addr;
	address.sin_port = htons(system_settings::MODBUS_TCP_PORT);
	address.sin_family = AF_INET;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (!sock)
	{
		g_system_settings.sys_message(system_settings::ERROR_MSG,
															string("Fail to create sock"));
		return 0;
	}

    const int on = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)))
    		g_system_settings.sys_message(system_settings::ERROR_MSG,
    															string("Fail to set socket options"));
     	
	if ((flags = fcntl(sock, F_GETFL, 0)) < 0)
	{
    		g_system_settings.sys_message(system_settings::ERROR_MSG, 
    															string("Fail to get file status flags"));

		close(sock);
		return 0;
	}
	if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) < 0)
	{
    		g_system_settings.sys_message(system_settings::ERROR_MSG, 
    															string("Fail to set file status flags"));

		close(sock);
		return 0;
	}

	if ((result = connect(sock, (sockaddr*)&address, sizeof(address))) && errno != EINPROGRESS)
	{
   		g_system_settings.sys_message(system_settings::ERROR_MSG, 
   															string("Fail to connect"));

		close(sock);
		return 0;
	}

	if (!result)
	{
		if (fcntl(sock, F_SETFL, flags) < 0)
		{
    		g_system_settings.sys_message(system_settings::ERROR_MSG, 
    															string("Fail to set file status flags"));
			close(sock);
			return 0;
		}
		
		return sock;
	}

	FD_ZERO(&readFD);
	FD_SET(sock, &readFD);
	writeFD = readFD;
	exFD = readFD;

	timeout.tv_sec = 1;
	timeout.tv_usec = 0;

	result = select(sock + 1, &readFD, &writeFD, &exFD, &timeout);
	if (result < 0)
	{
 		g_system_settings.sys_message(system_settings::ERROR_MSG, 
													string("Fail to select socket"));

		close(sock);
		return 0;
	}
	else if (!result)
	{
 		g_system_settings.sys_message(system_settings::ERROR_MSG, 
													string("Connect timeout expected"));

		close(sock);
		return 0;
	}
	else if (is_connected(sock, &readFD, &writeFD, &exFD))
	{
		if (fcntl(sock, F_SETFL, flags) < 0)
		{
	 		g_system_settings.sys_message(system_settings::ERROR_MSG, 
													string("Fail to set file status flags"));

			close(sock);
			return 0;
		}
		
		return sock;
	}
	else
	{
		close(sock);
		return 0;
	}
}

static int send_buffer(int sock, 
							metro_escalator::buffer_data_type output,  
							metro_escalator::buffer_data_type input)
{
	int		readNum, dataSize;
	struct	timeval timeout;
	int		result;
	fd_set 	allFd, readFd;
	static	int		total = 0, lost = 0;
	uint64_t 	clock1, clock2;
    float 			diffTime;
    
    	send(sock, &output[0], output.size(), 0);    	
    	total++;
//    	printf("Send [%d] uid=%d cmd=%d -> ... ", total, output[6], output[7]);
	clock1 = ClockCycles();
	FD_ZERO(&allFd);
	FD_SET(sock, &allFd);

	timeout.tv_sec = system_settings::RECEIVE_TIMEOUT;
	timeout.tv_usec = 900000;
	
	readFd = allFd;
	result = select(sock + 1, &readFd, NULL, NULL, &timeout);
	if (result < 0)
	{
		
		string mess="Fail to select on port. ";
		mess+=strerror(errno);
		g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
		
		return 0;
	}
	else if (result == 0)
	{
		lost++;
		clock2 = ClockCycles();
		diffTime = (clock2-clock1)*1.0/SYSPAGE_ENTRY(qtime)->cycles_per_sec;
//		printf("Response - failed (%f sec)\n", diffTime);
		return 0;
	}

	readNum = recv(sock, &input[0], 6, 0);
	
	if (readNum < 0)
	{
		string mess="Fail to recv data";
		mess+=strerror(errno);
		g_system_settings.sys_message(system_settings::ERROR_MSG, mess);

	} else if (readNum == 0)
		return 0;
	if (input[0] || input[1] || input[2] || input[3] || input[4])
	{
		return 0;
	}

	dataSize = input[5];

	timeout.tv_sec = 1;
	timeout.tv_usec = 0;

	readFd = allFd;
	result = select(sock + 1, &readFd, NULL, NULL, &timeout);
	if (result < 0)
	{
		string mess="Fail to select on port. ";
		mess+=strerror(errno);
		g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
	
		return 0;
	}
	else if (result == 0)
	{
		return 0;
	}
	
	readNum = recv(sock, &input[0], dataSize, 0);

	if (readNum < 0)
	{
		string mess="Fail to recv data";
		mess+=strerror(errno);
		g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
	}
	else if (readNum == 0)
		return 0;

	clock2 = ClockCycles();
	diffTime = (clock2-clock1)*1.0/SYSPAGE_ENTRY(qtime)->cycles_per_sec;
//	printf("Response - OK (%f sec)\n", diffTime);
	
	return readNum;
}

/*
Metro escalator  metods
*/
int metro_escalator::update_leds()
	{
	PtArg_t arg;

	if (sys_sett_obj->get_paneled_escalator_id()==this->get_id()
		&& !g_metro_devices_types.empty())
	{
		devices_types_container::iterator_devices_types iter_type;
		iter_type=g_metro_devices_types.find(type);
			
		if (iter_type!=g_metro_devices_types.end())
			{
				metro_device_type::iterator_blocks iter_block;
				for (iter_block=iter_type->second.blocks_begin(); 
						iter_block!=iter_type->second.blocks_end();
						iter_block++)
				{
					metro_device_type::iterator_signals iter_signal;	
					device_block::iterator_signals_id iter_signals_id;
					for (iter_signals_id=iter_block->second.signals_id_begin();
							iter_signals_id!=iter_block->second.signals_id_end();
							iter_signals_id++)
							{
							iter_signal=iter_type->second.signals_find(*iter_signals_id);
							
							if (iter_signal==iter_type->second.signals_end()) 
								{
								vector<char> tmp_chars(5);
								string mess = "Not found signal ";
								itoa(iter_signal->second.get_index(), &tmp_chars[0] , 10);
								mess+=&tmp_chars[0]; 
								mess+=" in block ";
								itoa(iter_block->first, &tmp_chars[0] , 10);
								mess+=&tmp_chars[0]; 
								mess+=" escalator ";
								itoa(this->get_id(), &tmp_chars[0] , 10);
								mess+=&tmp_chars[0]; 
								
							 		sys_sett_obj->sys_message(system_settings::ERROR_MSG, 
											mess);
								return 0;			
								};
								
							if (iter_signal->second.get_led()==NULL) 
								{
								vector<char> tmp_chars(5);
								string mess = "Not found led to signal ";
								itoa(iter_signal->second.get_index(), &tmp_chars[0] , 10);
								mess+=&tmp_chars[0]; 
								mess+=" block ";
								itoa(iter_block->first, &tmp_chars[0] , 10);
								mess+=&tmp_chars[0]; 
								mess+=" escalator ";
								itoa(this->get_id(), &tmp_chars[0] , 10);
								mess+=&tmp_chars[0]; 
								
						 		sys_sett_obj->sys_message(system_settings::ERROR_MSG, 
								mess);
								return 0;			
								} 
	
							int temp = iter_signal->second.get_index() / 8;
							int shift = 16 - ((iter_signal->second.get_index() % 8) + 1)*2;
							int signal = (dataBlock.signals[temp] >> shift) & 0x3;
							switch(signal)
								{	
								case 0:
								PtSetArg(&arg, Pt_ARG_LABEL_IMAGE, sys_sett_obj->get_image(system_settings::BLOCK_GREEN_LED), 0);
								break;
								
								case 1:
 								PtSetArg(&arg, Pt_ARG_LABEL_IMAGE, sys_sett_obj->get_image(system_settings::BLOCK_BLUE_LED), 0);
								break;
								
								case 2:
								PtSetArg(&arg, Pt_ARG_LABEL_IMAGE, sys_sett_obj->get_image(system_settings::BLOCK_RED_LED), 0);
								break;

								case 4:
								default:				
 								PtSetArg(&arg, Pt_ARG_LABEL_IMAGE, sys_sett_obj->get_image(system_settings::BLOCK_GREY_LED), 0);
								break;				
								}			
							PtSetResources(iter_signal->second.get_led(), 1, &arg);		
						
							}//for (iter_signal=it
				}//for (iter_block=
			}  else { //if (iter_type!=..
	 		sys_sett_obj->sys_message(system_settings::ERROR_MSG, 
			string ("metro_escalator::update_leds  - escalator types empty"));
			return(0);};
	}
	return 1;
	};



void metro_escalator::update_escalator()
   	{
	vector<PtArg_t> args(2);

	
	if (this->get_block_label_widget()==NULL ||
		this->get_arrow_label_widget()==NULL) return;

	if (this->get_enabled()== system_settings::ENABLED)
	{
		if (this->get_online() == system_settings::ONLINE)
		{
	 	PtSetArg(&args[0], Pt_ARG_TEXT_STRING, "", 0);			
	 	PtSetArg(&args[1], Pt_ARG_INLINE_COLOR, system_settings::NO_BLOCK_COLOR, 0);			
		if (this->get_blocked_status()!=system_settings::BLOCK_NORMA)
			{
			 	PtSetArg(&args[0], Pt_ARG_TEXT_STRING, "БЛОК", 0);			
			 	PtSetArg(&args[1], Pt_ARG_INLINE_COLOR, system_settings::BLOCK_COLOR, 0);			
			};
	

 		PtSetResources(this->get_block_label_widget(), args.size(), &args[0]);			
 		
		if (this->get_mode() == system_settings::MODE_MAIN_DRIVE_AT_REMOTE_CONTROL)
			{
				switch(this->get_status())
				{		
					case system_settings::STATE_UP_MAIN_DRIVE:
					case system_settings::STATE_UP_SMALL_DRIVE:
						PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, sys_sett_obj->get_image(system_settings::GREEN_UP), 0);
						break;
					case system_settings::STATE_STOP:

						if (this->get_pref_direction() == system_settings::DIRECTION_UP) {
								if (this->get_ready()!=system_settings::STATE_NOT_READY) {
								PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, sys_sett_obj->get_image(system_settings::YELLOW_UP), 0);
								} else {
								PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, sys_sett_obj->get_image(system_settings::RED_UP), 0);
								};
					    } else if (this->get_pref_direction() ==  system_settings::DIRECTION_DOWN) {
								if (dataBlock.ready!=system_settings::STATE_NOT_READY) {
								PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, sys_sett_obj->get_image(system_settings::YELLOW_DOWN), 0);
								} else {
								PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, sys_sett_obj->get_image(system_settings::RED_DOWN), 0);
								};
					  	} else if (this->get_pref_direction() ==  system_settings::DIRECTION_REVERSE) {
							if (this->get_direction() == system_settings::DIRECTION_UP)
								{
									if (this->get_ready()!=system_settings::STATE_NOT_READY) {
									PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, sys_sett_obj->get_image(system_settings::YELLOW_UP), 0);
									} else {
									PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, sys_sett_obj->get_image(system_settings::RED_UP), 0);
									};
								} else if (this->get_direction() == system_settings::DIRECTION_DOWN) {
									if (this->get_ready()!=system_settings::STATE_NOT_READY) {
									PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, sys_sett_obj->get_image(system_settings::YELLOW_DOWN), 0);
									} else {
									PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, sys_sett_obj->get_image(system_settings::RED_DOWN), 0);
									};
								} else {
									PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, sys_sett_obj->get_image(system_settings::GREY_STOP), 0); 
								};
						} else {
							PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, sys_sett_obj->get_image(system_settings::GREY_STOP), 0);
						};
						break;
					case system_settings::STATE_DOWN_MAIN_DRIVE:
					case system_settings::STATE_DOWN_SMALL_DRIVE:
						PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, sys_sett_obj->get_image(system_settings::GREEN_DOWN), 0);
						break;
					default:
						PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, sys_sett_obj->get_image(system_settings::GREY_STOP), 0);
				} // switch(this->get_status())
			} 	else {
				switch(this->get_status())
				{		
					case system_settings::STATE_UP_MAIN_DRIVE:
					case system_settings::STATE_UP_SMALL_DRIVE:
						PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, sys_sett_obj->get_image(system_settings::GREEN_S_UP), 0);
						break;

					case system_settings::STATE_STOP:
						if (this->get_pref_direction() == system_settings::DIRECTION_UP)
							{					
								if (this->get_ready()!=system_settings::STATE_NOT_READY) {
								PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, sys_sett_obj->get_image(system_settings::YELLOW_S_UP), 0);
								} else {
								PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, sys_sett_obj->get_image(system_settings::RED_S_UP), 0);
								};
							} else if (this->get_pref_direction() == system_settings::DIRECTION_DOWN) {
								if (this->get_ready()!=system_settings::STATE_NOT_READY) {
								PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, sys_sett_obj->get_image(system_settings::YELLOW_S_DOWN), 0);
								} else {
								PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, sys_sett_obj->get_image(system_settings::RED_S_DOWN), 0);
								};
							} else if (this->get_pref_direction() ==  system_settings::DIRECTION_REVERSE) 	{
								if (this->get_direction() == system_settings::DIRECTION_UP)
									{
										if (this->get_ready()!=system_settings::STATE_NOT_READY) 
											{
											PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, sys_sett_obj->get_image(system_settings::YELLOW_S_UP), 0);
											} else {
											PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, sys_sett_obj->get_image(system_settings::RED_S_UP), 0);
											};
									} else if (this->get_direction() == system_settings::DIRECTION_DOWN) {
										if (this->get_ready()!=system_settings::STATE_NOT_READY) 
											{
											PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, sys_sett_obj->get_image(system_settings::YELLOW_S_DOWN), 0);
											} else {
											PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, sys_sett_obj->get_image(system_settings::RED_S_DOWN), 0);
											};
									} else {
										PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, sys_sett_obj->get_image(system_settings::GREY_STOP), 0);
									};
							} else {
								PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, sys_sett_obj->get_image(system_settings::GREY_STOP), 0);
							};
						break;
					case system_settings::STATE_DOWN_MAIN_DRIVE:
					case system_settings::STATE_DOWN_SMALL_DRIVE:
						PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, sys_sett_obj->get_image(system_settings::GREEN_S_DOWN), 0);
						break;
					default:
						PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, sys_sett_obj->get_image(system_settings::GREY_STOP), 0);
						break;
				}
			}	
			PtSetArg(&args[1], Pt_ARG_FLAGS, Pt_FALSE, Pt_BLOCKED);
			
			PtSetResources(this->get_arrow_label_widget(), args.size(), &args[0]);		
		}
		else
		{
				PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, sys_sett_obj->get_image(system_settings::GREY_STOP), 0);
				PtSetArg(&args[1], Pt_ARG_FLAGS, Pt_TRUE, Pt_BLOCKED);
				PtSetResources(this->get_arrow_label_widget(), args.size(), &args[0]);		
		} //	if (this->get_online()...
	}
	else
	{
		PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, sys_sett_obj->get_image(system_settings::GREY_OFF), 0);
		PtSetArg(&args[1], Pt_ARG_FLAGS, Pt_TRUE, Pt_BLOCKED);
		PtSetResources(this->get_arrow_label_widget(), args.size(), &args[0]);		
	} // //	if (this->get_enabled()...
	};


int metro_escalator::create_station_window(int x, int y)
	{
	vector<PtArg_t> args;
	vector<char> label_buffer(10);

	ApDBase_t*  dbase=sys_sett_obj->get_widgets_dbase();

    PtSetParentWidget(get_station_widget());
    args.resize(1);
	PtSetArg(&args[0], Pt_ARG_USER_DATA, this->get_id(), 0);
	if (!args.empty())
				{
			    wnd = ApCreateWidget(dbase, "escalator_wnd", x, y, args.size(), &args[0]);
				} else {
				wnd=NULL;
				};
    	
    if (wnd==NULL)   		
		return 0;

	if (!args.empty()) {
			arrow = ApCreateWidget(dbase, "Arrow", -1, -1, args.size(), &args[0]);

			args.resize(2);
		 	PtSetArg(&args[0], Pt_ARG_TEXT_STRING, "\0", 0);			
 			PtSetArg(&args[1], Pt_ARG_INLINE_COLOR, system_settings::NO_BLOCK_COLOR, 0);			
			block_label = ApCreateWidget(dbase, "blocking", -1, -1, args.size(), &args[0]);

			ltoa(number, &label_buffer[0], 10);
			args.resize(1);
			PtSetArg(&args[0], Pt_ARG_TEXT_STRING, &label_buffer[0], 0);
			num_label = ApCreateWidget(dbase, "escalator_num", -1, -1, args.size(), &args[0]);

			} else {
			arrow=NULL;
			num_label=NULL;
			block_label=NULL;
			};
			
    if (!arrow && !block_label && !num_label)
		return 0;
  
	this->update_escalator();
	return 1;
	};

void metro_escalator::update_panel()
	{
	vector<PtArg_t> args(3);

	if (sys_sett_obj->get_paneled_escalator_id()==this->get_id())
	{
		if (this->get_online()==system_settings::ONLINE)
		{			
			PtSetArg(&args[0], Pt_ARG_FILL_COLOR, system_settings::INDICATOR_ENABLED_NORMA_FILL, 0);
			PtSetArg(&args[1], Pt_ARG_COLOR, system_settings::INDICATOR_ENABLED_NORMA_TEXT, 0);

			PtSetArg(&args[2], Pt_ARG_TEXT_STRING, (sys_sett_obj->get_escalator_mode_text(this->get_mode())).c_str(), 0);
			PtSetResources(ABW_ModeIndicator, args.size(), &args[0]);
			
			PtSetArg(&args[2], Pt_ARG_TEXT_STRING, (sys_sett_obj->get_escalator_ready_text(this->get_ready())).c_str(), 0);
			PtSetResources(ABW_ReadyIndicator, args.size(), &args[0]);

			vector<char> tmp_str(10);
			itoa (this->get_running_path_value(), &tmp_str[0], 10);
			PtSetArg(&args[2], Pt_ARG_TEXT_STRING, &tmp_str[0], 0);
			PtSetResources(ABW_RunningIndicator, args.size(), &args[0]);

			if (this->get_breaking_status()!=system_settings::BREAKING_PATH_NORMA) 
				{
					PtSetArg(&args[0], Pt_ARG_FILL_COLOR, system_settings::INDICATOR_ENABLED_NOT_NORMA_FILL, 0);
					PtSetArg(&args[1], Pt_ARG_COLOR, system_settings::INDICATOR_ENABLED_NOT_NORMA_TEXT, 0);
				};

			itoa (this->get_breaking_path_value(), &tmp_str[0], 10);
			PtSetArg(&args[2], Pt_ARG_TEXT_STRING, &tmp_str[0], 0);
			PtSetResources(ABW_BreakingPathIndicator, args.size(), &args[0]);
		
		} ;// if (online=
		}
	};

PtWidget_t*  metro_escalator::get_station_widget() 
{

metro_stations_container::iterator_metro_stations iter=g_stations.find(id_station);

if(iter!=g_stations.end())  
		{
		return(iter->second.get_wnd());
		}
		else
		{
		string mess="Station widget not found for esc id ";
		vector<char> tmp_buff(10);
		mess+=itoa(id_escalator, &tmp_buff[0], 10);
		sys_sett_obj->sys_message(system_settings::ERROR_MSG, mess);
		return(NULL);
		};
};


int metro_escalator::confirm_direction(int new_direction)
{
	string message_text;
	vector<char*> btns_txt;
	vector<char> tmp_btns(10);
	
	itoa(this->get_number(), &tmp_btns[0], 10);
	message_text="Вы действительно хотите сменить направление движения\nэскатора ";
	message_text+=&tmp_btns[0];
	message_text+=" ?\nПо умолчанию направление движения : ";
	
	switch(this->get_pref_direction())
		{		
			case system_settings::DIRECTION_UP:
				message_text+="ВВЕРХ";
				break;
			case system_settings::DIRECTION_DOWN:
				message_text+="ВНИЗ";
				break;
			case system_settings::DIRECTION_REVERSE:
				message_text+="РЕРСИВНОЕ";
				break;
			default:
				message_text+="НЕ ОПРЕДЕЛЕНО";
				break;
		}

	message_text+="\nПследнее направление движения : ";
	
	switch(this->get_direction())
		{		
			case system_settings::DIRECTION_UP:
				message_text+="ВВЕРХ";
				break;
			case system_settings::DIRECTION_DOWN:
				message_text+="ВНИЗ";
				break;
			default:
				message_text+="НЕ ОПРЕДЕЛЕНО";
				break;
		}

	message_text+="\nНовое направление движения : ";
	
	switch(new_direction)
		{		
			case system_settings::DIRECTION_UP:
				message_text+="ВВЕРХ";
				break;
			case system_settings::DIRECTION_DOWN:
				message_text+="ВНИЗ";
				break;
			default:
				message_text+="НЕ ОПРЕДЕЛЕНО";
				break;
		}

	btns_txt.push_back("Да");
	btns_txt.push_back("Нет");
	
	int ret_val=-1;
	if (!message_text.empty())
	{
	ret_val=PtAlert( ABW_main_wnd, 
							NULL, 
							"Внимание !", 
							NULL, 
							message_text.c_str(),
	      				    	NULL,
	      				    btns_txt.size(),
	      				   const_cast<const char**> (&btns_txt[0]),
	      				    NULL,
	      				    1,
	      				    2,
	      				    Pt_BLOCK_ALL
	      		);
	   };

//	   char* messages Yes and No created in stack and automatically deleted
//	 while (!btns_txt.empty())
//	 {
//	 	delete(*(btns_txt.rbegin()));
//	 	btns_txt.pop_back();
//	 };
	 
	return(ret_val);	  
}




int metro_escalator::set_data()
{
	vector<byte>	buffer;
	vector<PtArg_t> args(2);
	vector<char> 	block_circut(80);
	system_settings::bytes tmp_bytes;

	metro_escalator::lock_mutex data_mutex(this->get_in_mutex());
		this->swap_in_buffer(buffer);
	delete (&data_mutex);

	if(buffer.empty() || buffer.size()<system_settings::MODBUS_BUFFER_SIZE) 
		{
		return (0);
		}

	tmp_bytes.resize(2);
	tmp_bytes[0]=buffer[0];
	tmp_bytes[1]=buffer[1];
	dataBlock.status = ntohs(sys_sett_obj->type_from_bytes<word>(tmp_bytes));
	
	tmp_bytes[0]=buffer[2];
	tmp_bytes[1]=buffer[3];
	dataBlock.mode = ntohs(sys_sett_obj->type_from_bytes<word>(tmp_bytes));

	tmp_bytes[0]=buffer[4];
	tmp_bytes[1]=buffer[5];
	dataBlock.ready = ntohs(sys_sett_obj->type_from_bytes<word>(tmp_bytes));

	tmp_bytes[0]=buffer[6];
	tmp_bytes[1]=buffer[7];
	dataBlock.block_circuts_status = ntohs(sys_sett_obj->type_from_bytes<word>(tmp_bytes));

	tmp_bytes[0]=buffer[8];
	tmp_bytes[1]=buffer[9];
	dataBlock.breaking_path_status = ntohs(sys_sett_obj->type_from_bytes<word>(tmp_bytes));

	tmp_bytes[0]=buffer[10];
	tmp_bytes[1]=buffer[11];
	dataBlock.channels = ntohs(sys_sett_obj->type_from_bytes<word>(tmp_bytes));

	tmp_bytes[0]=buffer[12];
	tmp_bytes[1]=buffer[13];
	dataBlock.message_number = ntohs(sys_sett_obj->type_from_bytes<word>(tmp_bytes));

	tmp_bytes[0]=buffer[14];
	tmp_bytes[1]=buffer[15];
	dataBlock.count_of_messages = ntohs(sys_sett_obj->type_from_bytes<word>(tmp_bytes));

	tmp_bytes[0]=buffer[16];
	tmp_bytes[1]=buffer[17];
	dataBlock.reserved1 = ntohs(sys_sett_obj->type_from_bytes<word>(tmp_bytes));
	
	tmp_bytes.resize(4);
	tmp_bytes[0]=buffer[18];
	tmp_bytes[1]=buffer[19];
	tmp_bytes[2]=buffer[20];
	tmp_bytes[3]=buffer[21];
	dataBlock.breaking_path_value = ntohl(sys_sett_obj->type_from_bytes<dword>(tmp_bytes));
	
	tmp_bytes[0]=buffer[22];
	tmp_bytes[1]=buffer[23];
	tmp_bytes[2]=buffer[24];
	tmp_bytes[3]=buffer[25];
	dataBlock.running_path_value = ntohl(sys_sett_obj->type_from_bytes<dword>(tmp_bytes));

	tmp_bytes.resize(2);	
	tmp_bytes[0]=buffer[26];
	tmp_bytes[1]=buffer[27];
	dataBlock.reserved2 = ntohs(sys_sett_obj->type_from_bytes<word>(tmp_bytes));

	tmp_bytes[0]=buffer[26];
	tmp_bytes[1]=buffer[27];
	dataBlock.reserved3 = ntohs(sys_sett_obj->type_from_bytes<word>(tmp_bytes));


	data_block::data_container::size_type tmp_size = dataBlock.signals.size();
	for(data_block::data_container::size_type i=0; i<tmp_size; i++)
		{
			tmp_bytes[0]=buffer[30+i*2];
			tmp_bytes[1]=buffer[30+i*2+1];
		
			dataBlock.signals[i] = ntohs(sys_sett_obj->type_from_bytes<word>(tmp_bytes));
		}

	tmp_size = dataBlock.messages.size();
	for(data_block::data_container::size_type i=0; i<tmp_size; i++)
		{
			tmp_bytes[0]=buffer[60+i*2];
			tmp_bytes[1]=buffer[60+i*2+1];
		
			dataBlock.messages[i] = ntohs(sys_sett_obj->type_from_bytes<word>(tmp_bytes));
		}

	for(int i=0; i<dataBlock.count_of_messages; i++)
	{
		int msg = dataBlock.messages[i];
		if (msg > 0 && msg < 140)
		{
			if (msg == 119)
			{
				msg |=	dataBlock.breaking_path_value << 16;

				msg_dict_container::msg_dict_iterator tmp_msg_iter=g_msg_dictionary.find(msg);
				if(tmp_msg_iter==g_msg_dictionary.end())
					{
							vector<char> tmp_chars(10);
							string mess("Not found message id ");
							itoa(msg, &tmp_chars[0], 10);
							mess +=&tmp_chars[0];

							sys_sett_obj->sys_message(system_settings::ERROR_MSG, mess);
					};
				int msg_type=tmp_msg_iter->second.get_type();

				int tmp_id=g_main_log.get_records_autoincrement();
				g_main_log.set_records_autoincrement(++tmp_id);
				g_main_log.insert(log_record(
															tmp_id, 
															msg, 
															msg_type, 
															id_station, 
															number, 
															time(NULL))); 
					// NOTICE: time(NULL) must be replaced by time from escalator
				
			} else {

				msg_dict_container::msg_dict_iterator tmp_msg_iter=g_msg_dictionary.find(msg);
				if(tmp_msg_iter==g_msg_dictionary.end())
					{
							vector<char> tmp_chars(10);
							string mess("Not found message id ");
							itoa(msg, &tmp_chars[0], 10);
							mess +=&tmp_chars[0];

							sys_sett_obj->sys_message(system_settings::ERROR_MSG, mess);
					};
				int msg_type=tmp_msg_iter->second.get_type();

				int tmp_id=g_main_log.get_records_autoincrement();
				g_main_log.set_records_autoincrement(++tmp_id);
				g_main_log.insert(log_record(tmp_id, 
															msg, 
															msg_type, 
															id_station, 
															number, 
															time(NULL))); 
						// NOTICE: time(NULL) must be replaced by time from escalator

			};
		}else if (msg & 0x8000)
		{

				msg_dict_container::msg_dict_iterator tmp_msg_iter=g_msg_dictionary.find(msg);
				if(tmp_msg_iter==g_msg_dictionary.end())
					{
							vector<char> tmp_chars(10);
							string mess("Not found message id ");
							itoa(msg, &tmp_chars[0], 10);
							mess +=&tmp_chars[0];

							sys_sett_obj->sys_message(system_settings::ERROR_MSG, mess);
					};
				int msg_type=tmp_msg_iter->second.get_type();

				int tmp_id=g_main_log.get_records_autoincrement();
				g_main_log.set_records_autoincrement(++tmp_id);
				g_main_log.insert(log_record(tmp_id,
															msg,
															msg_type,
															id_station,
															number,
															time(NULL)));
					// NOTICE: time(NULL) must be replaced by time from escalator

		};

	}
		// RELEASE
		if (dataBlock.status == system_settings::STATE_STOP && !direction)
		// TEST ONLY
		//if ((data.state.direction == system_settings::STATE_STOP || data.state.direction == 0) && !direction)
		{
			if (prefDirection != system_settings::DIRECTION_REVERSE)
				direction = prefDirection;
			else
			 	direction = system_settings::DIRECTION_STOP; //  NOTICE :	Is it work?????
			// add msg dialog
		}
		else if (dataBlock.status == system_settings::STATE_UP_MAIN_DRIVE || dataBlock.status == system_settings::STATE_UP_SMALL_DRIVE)
			direction = system_settings::DIRECTION_UP;
		else if (dataBlock.status == system_settings::STATE_DOWN_MAIN_DRIVE || dataBlock.status == system_settings::STATE_DOWN_SMALL_DRIVE)
			direction = system_settings::DIRECTION_DOWN;
			
		update_escalator();
		update_panel();
		update_leds();
	return 1;
}

void metro_escalator::set_state(dword state)
{
	string	buffer;
	metro_stations_container::iterator_metro_stations station_iter;

//	printf("Set state %d\n", id);
	if (state!=system_settings::ONLINE)
	{
			station_iter=g_stations.find(id_station);
			if (station_iter!=g_stations.end())
				{
				vector<char> tmp_char(10);
				itoa(number,&tmp_char[0],10);
				
				buffer="Потеряна связь с эскалатором ";
				buffer+=&tmp_char[0];
				buffer+=" на станции \"";
				buffer+=station_iter->second.get_stl_name_string();
				buffer+="\"";

				g_alerts.push_back(alert_record(time(NULL), buffer));
	//			g_main_log.AddMessage(141, parent->GetID(), this->number);
				this->set_online(system_settings::OFFLINE);	
				this->update_escalator();
				this->update_panel();
		
				} else {
				this->set_online(system_settings::ONLINE);	
				this->update_escalator();
				this->update_panel();

				};
	};
}

void* Run(void* arg)
{
	int		readNum;
	int		sock;
	metro_escalator*		curEscalator;		
	metro_escalator::buffer_data_type output;
	metro_escalator::buffer_data_type input;

	int			reconnect;
//	int 		time_correct_int=0;	
	
	curEscalator = (metro_escalator*) arg;

	if (!curEscalator)
	{
		string mess="Invalid escalator pointer";
		
		g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
		return 0;
	}

		string mess="Start escalator thread tid=";
		vector<char> tmp(10);		
		itoa (pthread_self(), &tmp[0], 10);
		mess+=&tmp[0];
		
		g_system_settings.sys_message(system_settings::INFO_MSG, mess);

	do
	{		
		sock = connect_to_server(curEscalator->get_ip());
		
		if (!sock)
		{
			// sleep before reconnect
			sleep(1);
			continue;
		}
		
		reconnect = 0;		
						
		do
		{
			metro_escalator::lock_mutex data_mutex(curEscalator->get_out_mutex());
			curEscalator->swap_out_buffer(output);
			delete (&data_mutex);
		
			if (output.empty())
			{
				// timeout before store data
				usleep(1000);
				continue;
			}
			
			// если эскалатор не включен - выход
			if (curEscalator->get_enabled()!=system_settings::ENABLED)
				return 0;
/*
// time set up
			if (time_correct_int==0) curEscalator->send_time();
			if (++time_correct_int>metro_escalator::TIME_CORR_INTER) time_correct_int=0;
*/			
			readNum = send_buffer(sock, output, input);			
						 			
// 			printf ("Readnum %d\n", readNum);
 			
			if (!readNum)
			{
				if (curEscalator->get_online() == system_settings::ONLINE)
				{
					MsgSendPulse(curEscalator->get_connection_id(), SIGEV_PULSE_PRIO_INHERIT, 0, curEscalator->get_id());
					reconnect = 1;
				}
			}
			else		
			{			
				if (curEscalator->get_online() <= system_settings::OFFLINE)
				{
					curEscalator->set_online(1);
					MsgSendPulse(curEscalator->get_connection_id(), SIGEV_PULSE_PRIO_INHERIT, 1, curEscalator->get_id());
					// после коннекта - запрос данных
					curEscalator->get_data();
				}
/*
			if (curEscalator->get_escalator_type()==2)
				{
				// DEBUG dump modbus packet
				printf("Packet [%d] tid %d socket %d:\n", curEscalator->get_escalator_id(), pthread_self(), sock);
				for(int i=0; i<readNum; i++)
					printf( "%0x ", input[i]);
				printf("\n\n"); 
				// DEBUG
				};
*/		
				// проверяем команду
				switch(input[1])
				{
					case 1:
					 	// смена статуса
						if (input[3] & 1)
						{							
							curEscalator->get_data();
						}
						break;
					case 4:
						// пришел блок
//						printf("In block %d esc %d\n", input[0], curEscalator->get_escalator_number());
						if ((input[0] == curEscalator->get_number()) && input[2] == 76)
						{
							metro_escalator::buffer_data_iterator iter_first, iter_last;

							iter_first=iter_last=input.begin();
							//determination appends range
							advance(iter_first, 3); //third element
							advance(iter_last, readNum-3); //readNum-3 element
							
							metro_escalator::lock_mutex data_mutex(curEscalator->get_in_mutex());
							curEscalator->append_in_buffer(iter_first, iter_last);
							delete(&data_mutex);
							
							MsgSendPulse(curEscalator->get_connection_id(), SIGEV_PULSE_PRIO_INHERIT, 3, curEscalator->get_id());								
							
						} //if ((input[0
						break;
					default:				
						break;
				} // switch (input[1])
			}			
		} while(!reconnect);
			
		close(sock);

	} while(1);
	
	return 0; 
}

void metro_escalator::send_command(byte cmd)
{
	vector<byte> buffer(14);
	word		crc;
	
//	printf("Send cmd %0x\n", cmd);

	buffer[0] = buffer[1] = buffer[2] = buffer[3] = buffer[4] = 0;
	buffer[5] = 8;
	buffer[6] = number;	// адрес устройства
	buffer[7] = 6;		// команда
	buffer[8] = 0;
	buffer[9] = 0;
	buffer[10] = 0;
	buffer[11] = cmd;
	vector<byte>::iterator iter_beg=buffer.begin();
	vector<byte>::iterator  iter_end=buffer.end();
	advance(iter_beg,6);
	advance(iter_end,6+6);
	crc = sys_sett_obj->crc(vector<byte>(iter_beg, iter_end));
	
	system_settings::bytes tmp_bytes=sys_sett_obj->bytes_of_type<word>(crc);
	buffer[12] =tmp_bytes[1];
	buffer[13] =tmp_bytes[0];
						
	metro_escalator::lock_mutex data_mutex(this->get_out_mutex());
		this->append_out_buffer(buffer.begin(), buffer.end());
	delete(&data_mutex);
}


void metro_escalator::send_time()
{
	vector<byte>		buffer(19);
	word		crc;
	time_t 	time_now;
	
/*
// ATTENTION 
//Begin: Checking  - remove it!!!
	tm temp_time;
	
	temp_time.tm_sec=10;
	temp_time.tm_min=11;
	temp_time.tm_hour=18;
	
	temp_time.tm_mday=6;
	temp_time.tm_mon=0;
	temp_time.tm_year=103;
	temp_time.tm_wday=1;
	temp_time.tm_yday=6;
	temp_time.tm_isdst=1;
	
// End: Checking  - remove it!!!
*/
	
	printf("Send time to %d\n", number);
	
	buffer[0] = buffer[1] = buffer[2] = buffer[3] = buffer[4] = 0;
	buffer[5] = 13;
	buffer[6] = number;	// адрес устрйства
	buffer[7] = 16;		// команда
	// начиная со орго реистра (нерация с 0)
	buffer[8] = 0;
	buffer[9] = 1;
	//ва регистра
	buffer[10] = 0;
	buffer[11] = 2;
	//счетчик байт
	buffer[12] = 4;
	
	//текущее время
//	time_now=mktime(&temp_time);
	time_now=time(NULL);
	
/*
	// получение старшего слова
	reg_time_t=(word)((time_now>>1*16) & 0xFFFF);
	buffer[13] =sys_sett_obj->second_byte(reg_time_t);
	buffer[14] =sys_sett_obj->first_byte(reg_time_t);
	// получеие ладшео слова	
	reg_time_t=(word)((time_now>>0*16) & 0xFFFF);
	buffer[15] =sys_sett_obj->second_byte(reg_time_t);
	buffer[16] =sys_sett_obj->first_byte(reg_time_t);
*/


	system_settings::bytes tmp_bytes=sys_sett_obj->bytes_of_type<time_t>(time_now);
	buffer[13] =tmp_bytes[1];
	buffer[14] =tmp_bytes[0];
	
	buffer[15] =tmp_bytes[2];
	buffer[16] =tmp_bytes[3];

	

	vector<byte>::iterator  iter_beg=buffer.begin();
	vector<byte>::iterator  iter_end=buffer.end();
	advance(iter_beg,6);
	advance(iter_end,6+11);

	crc = sys_sett_obj->crc(vector<byte>(iter_beg, iter_end));
/*
	buffer[17] =sys_sett_obj->second_byte(crc);
	buffer[18] =sys_sett_obj->first_byte(crc);
*/

	tmp_bytes=sys_sett_obj->bytes_of_type<word>(crc);
	buffer[17] =tmp_bytes[1];
	buffer[18] =tmp_bytes[0];


	metro_escalator::lock_mutex data_mutex(this->get_out_mutex());
		this->append_out_buffer(buffer.begin(), buffer.end());
	delete(&data_mutex);
}


void metro_escalator::check_status()
{
	vector<byte>		buffer(14);
	word		crc;

	buffer[0] = buffer[1] = buffer[2] = buffer[3] = buffer[4] = 0;
	buffer[5] = 6;
	buffer[6] = number;	// адрес устройства
	buffer[7] = 1;		// команда
	buffer[8] = 0;
	buffer[9] = 0;
	buffer[10] = 0;
	buffer[11] = 1;
	
	vector<byte>::iterator  iter_beg=buffer.begin();
	vector<byte>::iterator  iter_end=buffer.end();
	advance(iter_beg,6);
	advance(iter_end,6+6);
/*
	crc = sys_sett_obj->crc(vector<byte>(iter_beg, iter_end));
	buffer[12] =sys_sett_obj->second_byte(crc);
	buffer[13] =sys_sett_obj->first_byte(crc);
*/
	system_settings::bytes tmp_bytes=sys_sett_obj->bytes_of_type<word>(crc);
	buffer[12] =tmp_bytes[1];
	buffer[13] =tmp_bytes[0];


	metro_escalator::lock_mutex data_mutex(this->get_out_mutex());
		this->append_out_buffer(buffer.begin(), buffer.end());
	delete(&data_mutex);
}

void metro_escalator::get_data()
{
	vector<byte> buffer(14);
	word		crc;

	buffer[0] = buffer[1] = buffer[2] = buffer[3] = buffer[4] = 0;
	buffer[5] = 8;
	buffer[6] = number;	// с устройств
	buffer[7] = 4;				// команда
	buffer[8] = 0;
	buffer[9] = 0;
	buffer[10] = 0;
	buffer[11] = 38;

	vector<byte>::iterator iter_beg=buffer.begin();
	vector<byte>::iterator  iter_end=buffer.end();
	advance(iter_beg,6);
	advance(iter_end,6+6);
/*
	crc = sys_sett_obj->crc(vector<byte>(iter_beg, iter_end));
	buffer[12] =sys_sett_obj->second_byte(crc);
	buffer[13] =sys_sett_obj->first_byte(crc);
*/
	system_settings::bytes tmp_bytes=sys_sett_obj->bytes_of_type<word>(crc);
	buffer[12] =tmp_bytes[1];
	buffer[13] =tmp_bytes[0];


	metro_escalator::lock_mutex data_mutex(this->get_out_mutex());
		this->append_out_buffer(buffer.begin(), buffer.end());
	delete(&data_mutex);
}

int metro_escalator::connect_to_channel(int channel)
{
	connection_id = ConnectAttach(0, 0, channel, _NTO_SIDE_CHANNEL, 0 );
	if (!connection_id)
	{
 		sys_sett_obj->sys_message(system_settings::ERROR_MSG, 
												string("Fail connect to pulse channel"));
		return 0;
	}
	channel_id = channel;	

	return 1;
}

int metro_escalator::disconnect_from_channel()
{
	return ConnectDetach(connection_id);
}

int metro_escalator::start()
{
	if (enabled)
	{
		if (pthread_create(&tid, NULL, &Run, this) != EOK)
		{
	 		sys_sett_obj->sys_message(system_settings::ERROR_MSG, 
												string("Fail to create escalator's thread"));
			return 0;
		}
	}
	
	return 1;
}
