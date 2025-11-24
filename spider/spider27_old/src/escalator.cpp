#include "global.h"
#include "system_settings.h"
#include "spider_exception.h"
#include "alert.h"
#include "devices_types.h"
#include "contain_metro_devices_types.h"
#include "metro_device.h"
#include "data_block.h"
#include "escalator.h"

metro_escalator::metro_escalator(
		system_settings *sys_sett_object,
		int id, 
		int id_station,
		int number,
		int type,
		int pref_direction,
		int direction,
	   	int	 start_day_mode,
	   	int	 start_hour,
	   	int	 start_minute,
		bool enabled,
		in_addr_t	ip,
		int channel ) throw (spider_exception):

	metro_device( id,
                   id_station,
                   number,
                   type,
                   enabled,
                   ip,
                   channel),
	tid(0),	
	direction (direction),
 	start_day_mode(start_day_mode),
	start_hour(start_hour),
	start_minute(start_minute),
	pref_direction(pref_direction){
		arrow = num_label = block_label = NULL;
		sys_sett_obj=sys_sett_object;
	};


	metro_escalator::~metro_escalator() {
       if (tid!=0) {
           pthread_cancel(tid);
		};
	}

/*
Metro escalator  metods
*/
void metro_escalator::update_leds(
       metro_devices_types_container *dev_types)  throw (spider_exception){
	PtArg_t arg;
	ostringstream exception_description;
	if (sys_sett_obj->get_paneled_escalator_id()==this->get_id()
		&& !g_metro_devices_types.empty())
	{
		metro_devices_types_container::iterator_devices_types iter_type;
		iter_type=dev_types->find(type);
			
		if (iter_type!=dev_types->end())
			{
				metro_device_type::iterator_blocks iter_block;
				for (iter_block=iter_type->second.blocks_begin(); 
						iter_block!=iter_type->second.blocks_end();
						iter_block++) {

					metro_device_type::iterator_signals iter_signal;	
					device_block::iterator_signals_id iter_signals_id;
					for (iter_signals_id=iter_block->second.signals_id_begin();
							iter_signals_id!=iter_block->second.signals_id_end();
							iter_signals_id++){
							iter_signal=iter_type->second.signals_find(*iter_signals_id);
							
							if (iter_signal==iter_type->second.signals_end())  {
                                exception_description<<"Not found signal "<<iter_signal->second.get_index();
                                exception_description<<" in block "<<iter_block->get_id()<<" escalator ";
                                exception_description<<this->get_id();
                                throw spider_exception(exception_description.str());
                             };
								
							if (iter_signal->second.get_led()==NULL) {
                                exception_description<<"Not found led to signal "<<iter_signal->second.get_index();
                                exception_description<<" block "<<iter_block->get_id()<<" escalator ";
                                exception_description<<this->get_id();
                                throw spider_exception(exception_description.str());
                            } ;
	
							int temp = iter_signal->second.get_index() / 8;
							int shift = 16 - ((iter_signal->second.get_index() % 8) + 1)*2;
							int signal = (data_block.signals[temp] >> shift) & 0x3;
							switch(signal){	
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
                exception_description<<"metro_escalator::update_leds  - escalator types empty";
                throw spider_exception(exception_description.str());
			};
	} //	if (sys_sett_obj->get_paneled_escalator_id()==this->get_id()
};

void metro_escalator::update_escalator()  {
	vector<PtArg_t> args(2);
	ostrngstream exception_description;
	
	if (this->get_block_label_widget()==NULL ||
		this->get_arrow_label_widget()==NULL) return;

	if (this->get_enabled()== system_settings::ENABLED){
		if (this->get_online() == system_settings::ONLINE){
	 	PtSetArg(&args[0], Pt_ARG_TEXT_STRING, "", 0);			
	 	PtSetArg(&args[1], Pt_ARG_INLINE_COLOR, system_settings::NO_BLOCK_COLOR, 0);			
		if (this->get_blocked_status()!=system_settings::BLOCK_NORMA){
			 	PtSetArg(&args[0], Pt_ARG_TEXT_STRING, "БЛОК", 0);			
			 	PtSetArg(&args[1], Pt_ARG_INLINE_COLOR, system_settings::BLOCK_COLOR, 0);			
			};
	

 		PtSetResources(this->get_block_label_widget(), args.size(), &args[0]);			
 		
		if (this->get_mode() == system_settings::MODE_MAIN_DRIVE_AT_REMOTE_CONTROL){
				switch(this->get_status()){		
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
								if (data_block.ready!=system_settings::STATE_NOT_READY) {
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
		} else {
				PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, sys_sett_obj->get_image(system_settings::GREY_STOP), 0);
				PtSetArg(&args[1], Pt_ARG_FLAGS, Pt_TRUE, Pt_BLOCKED);
				PtSetResources(this->get_arrow_label_widget(), args.size(), &args[0]);		
		} //	if (this->get_online()...
	}	else	{
		PtSetArg(&args[0], Pt_ARG_LABEL_IMAGE, sys_sett_obj->get_image(system_settings::GREY_OFF), 0);
		PtSetArg(&args[1], Pt_ARG_FLAGS, Pt_TRUE, Pt_BLOCKED);
		PtSetResources(this->get_arrow_label_widget(), args.size(), &args[0]);		
	} // //	if (this->get_enabled()...
	};

void metro_escalator::update_panel() {
	vector<PtArg_t> args(3);

	if (sys_sett_obj->get_paneled_escalator_id()==this->get_id()){
		if (this->get_online()==system_settings::ONLINE) {
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

			if (this->get_breaking_status()!=system_settings::BREAKING_PATH_NORMA) {
					PtSetArg(&args[0], Pt_ARG_FILL_COLOR, system_settings::INDICATOR_ENABLED_NOT_NORMA_FILL, 0);
					PtSetArg(&args[1], Pt_ARG_COLOR, system_settings::INDICATOR_ENABLED_NOT_NORMA_TEXT, 0);
				};

			itoa (this->get_breaking_path_value(), &tmp_str[0], 10);
			PtSetArg(&args[2], Pt_ARG_TEXT_STRING, &tmp_str[0], 0);
			PtSetResources(ABW_BreakingPathIndicator, args.size(), &args[0]);
		
		} ;// if (online=
		}
	};

int metro_escalator::confirm_direction(int new_direction){
	string message_text;
	vector<char*> btns_txt;
	vector<char> tmp_btns(10);
	
	itoa(this->get_number(), &tmp_btns[0], 10);
	message_text="Вы действительно хотите сменить направление движения\nэскатора ";
	message_text+=&tmp_btns[0];
	message_text+=" ?\nПо умолчанию направление движения : ";
	
	switch(this->get_pref_direction()) {		
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
	
	switch(this->get_direction()) {		
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
	
	switch(new_direction){		
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
	if (!message_text.empty()){
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
	 
	return(ret_val);	  
}


void metro_escalator::set_data(
	msg_dict_container *msg_dict,
	log_records_container *log_records,
	metro_devices_types_container *dev_types) throw (spider_exception) {

    ostrinstream exception_description;
	vector<byte>	buffer;
	vector<PtArg_t> args(2);
	vector<char> 	block_circut(80);
	system_settings::bytes tmp_bytes;
	
   {
	    metro_escalator::lock_mutex data_mutex(this->get_in_mutex());
        this->swap_in_buffer(buffer);
	}
	
	if(buffer.empty() || 
        buffer.size()<system_settings::MODBUS_BUFFER_SIZE) {
		return;
	}

	tmp_bytes.resize(2);
	tmp_bytes[0]=buffer[0];
	tmp_bytes[1]=buffer[1];
	data_block.status = ntohs(sys_sett_obj->type_from_bytes<word>(tmp_bytes));
	
	tmp_bytes[0]=buffer[2];
	tmp_bytes[1]=buffer[3];
	data_block.mode = ntohs(sys_sett_obj->type_from_bytes<word>(tmp_bytes));

	tmp_bytes[0]=buffer[4];
	tmp_bytes[1]=buffer[5];
	data_block.ready = ntohs(sys_sett_obj->type_from_bytes<word>(tmp_bytes));

	tmp_bytes[0]=buffer[6];
	tmp_bytes[1]=buffer[7];
	data_block.block_circuts_status = ntohs(sys_sett_obj->type_from_bytes<word>(tmp_bytes));

	tmp_bytes[0]=buffer[8];
	tmp_bytes[1]=buffer[9];
	data_block.breaking_path_status = ntohs(sys_sett_obj->type_from_bytes<word>(tmp_bytes));

	tmp_bytes[0]=buffer[10];
	tmp_bytes[1]=buffer[11];
	data_block.channels = ntohs(sys_sett_obj->type_from_bytes<word>(tmp_bytes));

	tmp_bytes[0]=buffer[12];
	tmp_bytes[1]=buffer[13];
	data_block.message_number = ntohs(sys_sett_obj->type_from_bytes<word>(tmp_bytes));

	tmp_bytes[0]=buffer[14];
	tmp_bytes[1]=buffer[15];
	data_block.count_of_messages = ntohs(sys_sett_obj->type_from_bytes<word>(tmp_bytes));

	tmp_bytes[0]=buffer[16];
	tmp_bytes[1]=buffer[17];
	data_block.reserved1 = ntohs(sys_sett_obj->type_from_bytes<word>(tmp_bytes));
	
	tmp_bytes.resize(4);
	tmp_bytes[0]=buffer[18];
	tmp_bytes[1]=buffer[19];
	tmp_bytes[2]=buffer[20];
	tmp_bytes[3]=buffer[21];
	data_block.breaking_path_value = ntohl(sys_sett_obj->type_from_bytes<dword>(tmp_bytes));
	
	tmp_bytes[0]=buffer[22];
	tmp_bytes[1]=buffer[23];
	tmp_bytes[2]=buffer[24];
	tmp_bytes[3]=buffer[25];
	data_block.running_path_value = ntohl(sys_sett_obj->type_from_bytes<dword>(tmp_bytes));

	tmp_bytes.resize(2);	
	tmp_bytes[0]=buffer[26];
	tmp_bytes[1]=buffer[27];
	data_block.reserved2 = ntohs(sys_sett_obj->type_from_bytes<word>(tmp_bytes));

	tmp_bytes[0]=buffer[26];
	tmp_bytes[1]=buffer[27];
	data_block.reserved3 = ntohs(sys_sett_obj->type_from_bytes<word>(tmp_bytes));


	data_block::data_container::size_type tmp_size = data_block.signals.size();
	for(data_block::data_container::size_type i=0; i<tmp_size; i++){
			tmp_bytes[0]=buffer[30+i*2];
			tmp_bytes[1]=buffer[30+i*2+1];
		
			data_block.signals[i] = ntohs(sys_sett_obj->type_from_bytes<word>(tmp_bytes));
		}

	tmp_size = data_block.messages.size();
	for(data_block::data_container::size_type i=0; i<tmp_size; i++){
			tmp_bytes[0]=buffer[60+i*2];
			tmp_bytes[1]=buffer[60+i*2+1];
		
			data_block.messages[i] = ntohs(sys_sett_obj->type_from_bytes<word>(tmp_bytes));
		}

	for(int i=0; i<data_block.count_of_messages; i++){
		int msg = data_block.messages[i];
		if (msg > 0 && msg < 140){
			if (msg == 119){
				msg |=	data_block.breaking_path_value << 16;

				msg_dict_container::msg_dict_iterator tmp_msg_iter=msg_dict->find(msg);
				if(tmp_msg_iter==msg_dict->end()){
					exception_description<<"Not found message  with id "<<msg;
					throw spider_exception(exception_description.str());
					};
				int msg_type=tmp_msg_iter->second.get_type();

				int tmp_id=log_records->get_records_autoincrement();
				log_records->set_records_autoincrement(++tmp_id);
				log_records->insert(log_record(
															tmp_id, 
															msg, 
															msg_type, 
															id_station, 
															number, 
															time(NULL))); 
					// NOTICE: time(NULL) must be replaced by time from escalator
				
			} else {

				msg_dict_container::msg_dict_iterator tmp_msg_iter=msg_dict->find(msg);
				if(tmp_msg_iter==msg_dict->end())	{
					exception_description<<"Not found message  with id "<<msg;
					throw spider_exception(exception_description.str());
					};
				int msg_type=tmp_msg_iter->second.get_type();

				int tmp_id=log_records->get_records_autoincrement();
				log_records->set_records_autoincrement(++tmp_id);
				log_records->insert(log_record(tmp_id, 
															msg, 
															msg_type, 
															id_station, 
															number, 
															time(NULL))); 
						// NOTICE: time(NULL) must be replaced by time from escalator

			};
		}else if (msg & 0x8000) {

				msg_dict_container::msg_dict_iterator tmp_msg_iter=msg_dict->find(msg);
				if(tmp_msg_iter==msg_dict->end()){
					exception_description<<"Not found message  with id "<<msg;
					throw spider_exception(exception_description.str());
					};
				int msg_type=tmp_msg_iter->second.get_type();

				int tmp_id=log_records->get_records_autoincrement();
				log_records->set_records_autoincrement(++tmp_id);
				log_records->insert(log_record(tmp_id,
															msg,
															msg_type,
															id_station,
															number,
															time(NULL)));
					// NOTICE: time(NULL) must be replaced by time from escalator

		};

	}
		if (data_block.status == system_settings::STATE_STOP && !direction)
		{
			if (prefDirection != system_settings::DIRECTION_REVERSE)
				direction = pref_direction;
			else
			 	direction = system_settings::DIRECTION_STOP; //  NOTICE :	Is it work?????
			// add msg dialog
		}
		else if (data_block.status == system_settings::STATE_UP_MAIN_DRIVE || data_block.status == system_settings::STATE_UP_SMALL_DRIVE)
			direction = system_settings::DIRECTION_UP;
		else if (data_block.status == system_settings::STATE_DOWN_MAIN_DRIVE || data_block.status == system_settings::STATE_DOWN_SMALL_DRIVE)
			direction = system_settings::DIRECTION_DOWN;
			
		update_escalator();
		update_panel();
		update_leds(dev_types);
}

void metro_escalator::set_state(
		dword state,
		alerts_container *alerts_cont) {

	string	buffer;
	metro_stations_container::iterator_metro_stations station_iter;

//	printf("Set state %d\n", id);
	if (state!=system_settings::ONLINE) {
			station_iter=g_stations.find(id_station);
			if (station_iter!=g_stations.end()){

				vector<char> tmp_char(10);
				itoa(number,&tmp_char[0],10);
				
				buffer="Потеряна связь с эскалатором ";
				buffer+=&tmp_char[0];
				buffer+=" на станции \"";
				buffer+=station_iter->second.get_stl_name_string();
				buffer+="\"";

				alerts_cont->push_back(alert_record(time(NULL), buffer));
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

void metro_escalator::send_command(byte cmd)
{
	vector<byte> buffer(14);
	word		crc;
	
	printf("Send cmd %0x\n", cmd);

	buffer[0] = buffer[1] = buffer[2] = buffer[3] = buffer[4] = 0;
	buffer[5] = 8;
	buffer[6] = number;	// адрес устройства
	buffer[7] = 6;		// команда
	buffer[8] = 0;
	buffer[9] = 0;
	buffer[10] = 0;
	buffer[11] = cmd;
	vector<byte>::iterator iter_beg=buffer.begin();
	vector<byte>::iterator  iter_end=buffer.begin();
	advance(iter_beg,6);
	advance(iter_end,6+6);
	crc = sys_sett_obj->crc(vector<byte>(iter_beg, iter_end));
	
	system_settings::bytes tmp_bytes=sys_sett_obj->bytes_of_type<word>(crc);
	buffer[12] =tmp_bytes[1];
	buffer[13] =tmp_bytes[0];
	
	{						
	metro_escalator::lock_mutex data_mutex(this->get_out_mutex());
		this->swap_out_buffer(buffer);
	}
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
	vector<byte>::iterator  iter_end=buffer.begin();
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

    {
        metro_escalator::lock_mutex data_mutex(this->get_out_mutex());
		this->swap_out_buffer(buffer);
	}
	
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
	vector<byte>::iterator  iter_end=buffer.begin();
	advance(iter_beg,6);
	advance(iter_end,6+6);

	crc = sys_sett_obj->crc(vector<byte>(iter_beg, iter_end));
/*	
	buffer[12] =sys_sett_obj->second_byte(crc);
	buffer[13] =sys_sett_obj->first_byte(crc);
*/
	system_settings::bytes tmp_bytes=sys_sett_obj->bytes_of_type<word>(crc);
	buffer[12] =tmp_bytes[1];
	buffer[13] =tmp_bytes[0];

   {
 	    metro_escalator::lock_mutex data_mutex(this->get_out_mutex());
		this->swap_out_buffer(buffer);
	}

}

void metro_escalator::get_data(){
	vector<byte> buffer(14);
	word		crc;

	buffer[0] = buffer[1] = buffer[2] = buffer[3] = buffer[4] = 0;
	buffer[5] = 8;
	buffer[6] = number;	// с устройств
	buffer[7] = 4;			// команда
	buffer[8] = 0;
	buffer[9] = 0;
	buffer[10] = 0;
	buffer[11] = 38;

	vector<byte>::iterator iter_beg=buffer.begin();
	vector<byte>::iterator  iter_end=buffer.begin();
	advance(iter_beg,6);
	advance(iter_end,6+6);

	crc = sys_sett_obj->crc(vector<byte>(iter_beg, iter_end));
/*
	buffer[12] =sys_sett_obj->second_byte(crc);
	buffer[13] =sys_sett_obj->first_byte(crc);
*/

	system_settings::bytes tmp_bytes=sys_sett_obj->bytes_of_type<word>(crc);
	buffer[12] =tmp_bytes[1];
	buffer[13] =tmp_bytes[0];

    {
        metro_escalator::lock_mutex data_mutex(this->get_out_mutex());
		this->swap_out_buffer(buffer);
	}
}


void metro_escalator::start() 
         throw (spider_exception) {
	if (enabled){
		if (pthread_create(&tid, NULL, &Run, this) != EOK) {
			throw spider_exception("Fail to create escalator's thread");			
		}
	}

}
