/* Y o u r   D e s c r i p t i o n                       */
/*                            AppBuilder Photon Code Lib */
/*                                         Version 2.01  */

#include "global.h"

system_settings g_system_settings;
alerts_container g_alerts;
metro_stations_container g_stations;
metro_escalators_container g_escalators(&g_system_settings);
cmd_pool_container g_command_pool;
devices_types_container g_metro_devices_types;
metro_lines_container g_lines;
msg_types_container g_msg_types;
msg_dict_container g_msg_dictionary;
router g_router;
log_records_container  g_main_log, g_archive_log;
sound g_sound;

int g_chanID;
pthread_t		g_pingTID, g_timerTID;


sig_atomic_t SigpipeCount;

/* Application Options string */

const char ApOptions[] =
	AB_OPTIONS "";

/* Add your options in the "" */

void SigpipeHandler (int SignalNumber)
{
++SigpipeCount;
};
	
void* PingThread(void* arg)
{
metro_escalators_container::iterator_metro_escalators iter_esc;

	while(1)
	{
		if (SigpipeCount>0) {
		g_system_settings.sys_message(system_settings::ERROR_MSG, string ("Recived SIGPIPE"));
		--SigpipeCount;
										};
		iter_esc=g_escalators.begin();
		while (iter_esc!=g_escalators.end())
	 	{
	 		if ((iter_esc->second.get_online() ==system_settings::ONLINE) && iter_esc->second.get_enabled())
	 		{
	 			if (iter_esc->second.get_sleepticks() >= 1)
	 			{
//	 				iter_esc->second.check_status();	
					iter_esc->second.get_data();	
	 				iter_esc->second.set_sleepticks (0);
	 			}
	 			else
	 				iter_esc->second.increment_sleepticks();
	 		}
	 		else if (iter_esc->second.get_enabled())
	 		{
				if (iter_esc->second.get_sleepticks() >= 5)
	 			{
//	 				iter_esc->second.check_status();	
					iter_esc->second.get_data();
	 				iter_esc->second.set_sleepticks (0);
	 			}
	 			else
	 				iter_esc->second.increment_sleepticks();
	 				
	 		iter_esc++;		
	 		}
			
 		}

 		usleep(system_settings::PING_TIMEOUT);
 	}
}

void* TimerThread(void *arg)
{
	struct _pulse   pulse;
	timer_t         timerID;
	struct sigevent event;
	struct itimerspec timer;
	int  conID,  timeoutChannel, recvID;
	
	if ((timeoutChannel = ChannelCreate(0)) == -1)
	{
		string message("Can't create timeout channel ");
		message+=strerror(errno);
		g_system_settings.sys_message(system_settings::ERROR_MSG,  message);

		return 0;
	}
	
	
	conID = ConnectAttach(0, 0, timeoutChannel, 0, 0);
	if (conID == -1) {
		string message("Attach channel ");
		message+=strerror(errno);
		g_system_settings.sys_message(system_settings::ERROR_MSG,  message);

		return 0;
	}
	
	
	SIGEV_PULSE_INIT(&event, conID, SIGEV_PULSE_PRIO_INHERIT, system_settings::SYSTEM_TIMER, 0);

	if (timer_create(CLOCK_REALTIME, &event, &timerID) == -1)
	{
		string message("Fail to create system timer  ");
		message+=strerror(errno);
		g_system_settings.sys_message(system_settings::ERROR_MSG,  message);
	}

	
	timer.it_value.tv_sec = 1;
	timer.it_value.tv_nsec = 0;
	timer.it_interval.tv_sec = system_settings::SYSTEM_TICK;
	timer.it_interval.tv_nsec = 0;

	timer_settime(timerID, 0, &timer, NULL);

	while (1)
	{
		recvID = MsgReceive(timeoutChannel, &pulse, sizeof(pulse), NULL);
		
		if (recvID==-1)
			{
				string message("MsgReceive ");
				message+=strerror(errno);
				g_system_settings.sys_message(system_settings::ERROR_MSG,  message);

			};
 		if (recvID == 0 && pulse.code == system_settings::SYSTEM_TIMER) g_main_log.rotate();
	}

	return 0;
}

bool load_map(string file_name)
{
 	enum {LINE=0, STATION, ESCALATOR, ENTRIES_COUNT};

	string	section_name;
	const char *section_name_c_str;
	vector<string> sections_names(ENTRIES_COUNT);

	sections_names[LINE]="line";
	sections_names[STATION]="station";
 	sections_names[ESCALATOR]="escalator";
 	
	if (PxConfigOpen( file_name.c_str(), PXCONFIG_READ)==Pt_FALSE )
	{
		string message("Can`t open config file ");
		message+=file_name.c_str();
		g_system_settings.sys_message(system_settings::ERROR_MSG,  message);

		return 0;
	};
	section_name_c_str=PxConfigNextSection();
	while (section_name_c_str!=NULL) 
	{
	
	section_name=section_name_c_str;
	
			if (section_name.compare(sections_names[LINE])==0) 
			{
				if (! g_lines.load_line_parameters (&g_system_settings)) {return(false);};
			} else if (section_name.compare(sections_names[STATION])==0) {
				if (! g_stations.load_station_parameters (&g_system_settings, &g_lines)) {return(false);};
			} else if (section_name.compare(sections_names[ESCALATOR])==0) {
				if (! g_escalators.load_escalator_parameters (&g_stations) ) {return(false);};
			} else {
				string message("Unrecognized config section ");
				message+=section_name.c_str();
				g_system_settings.sys_message(system_settings::ERROR_MSG,  message);
				return 0;
			};
			
			section_name_c_str=PxConfigNextSection();
	};	

	g_lines.set_current_line(g_lines.end());
	g_stations.set_current_station(g_stations.end());
	g_escalators.set_current_escalator(g_escalators.end());

	if  (PxConfigClose()) 
	{return (true);}
	else
	{
		string message("Can`t close config file ");
		message+=file_name.c_str();
		g_system_settings.sys_message(system_settings::ERROR_MSG,  message);

		return (false);
	};
	
	return (true);
}

// PtInputCallbackProcF_t PulseReceiver;

int PulseReceiver(void *data, int rcvid, void *message, size_t mbsize )
{
	struct _pulse* pulse;

	pulse = (struct _pulse*)message;	
//	printf("Code %d, value %d\n", pulse->code, pulse->value.sival_int);
	
	metro_escalators_container::iterator_metro_escalators temp_escalators_iterator=g_escalators.find(pulse->value.sival_int);
	if(temp_escalators_iterator!=g_escalators.end())
		{
			//	printf("Code %d, value %d ESCALATOR %p on %d\n", pulse->code, pulse->value.sival_int, temp_escalators_iterator->second.get_number(), temp_escalators_iterator->second.get_online());
			switch(pulse->code)
				{
					case 0:
					case 1:
					case 2:
//						printf("In PulseReceiver\n");
						temp_escalators_iterator->second.set_state(pulse->code);			
						break;
					case 3:
						temp_escalators_iterator->second.set_data();
						break;
					default:
						break;
				};
	
			PtFlush();

		
		} else {
			g_system_settings.sys_message(system_settings::ERROR_MSG,  
																string("Can`t find current escalator"));

			return Pt_HALT;
		};

	return Pt_HALT;
}



bool load_dictionary(string fileName)
{
/*
	FILE*		file;
	char		str[256];
	char		delims[] = "\t";
	char*	token;
	int			count = 0;
	CMsgDictContainer::MsgDictPair DictPair;
	
	file = fopen(fileName, "rt");
	if (file)
	{
		while(fgets(str, 256, file))
		{
			str[strlen(str) - 1] = 0;
				count = 0;
				token = strtok(str, delims);
			    while(token)
			    {
					 switch(count)
					 {
					 	case 0:		 		
				 			DictPair.second.number = atoi(token);
				 			DictPair.first=atoi(token);				 			
					 		break;
					 	case 1:
					 		DictPair.second.text = new char[(strlen(token) + 5) & ~3];
					 		if (DictPair.second.text)
					 		{
					 			strcpy(DictPair.second.text, token);
					 			strcat(DictPair.second.text, "\n");
					 		}
					 		else
					 		{
					 			SysMessage(ERROR_MSG, "Memory allocation");
					 			fclose(file);
					 			return 0;
					 		}
					 		break;
					 	case 2:
					 		if (!strcmp(token, "BLACK"))
					 			DictPair.second.color = system_settings::COLOR_BLACK;
					 		else if (!strcmp(token, "RED"))
					 			DictPair.second.color = system_settings::COLOR_RED;
					 		else if (!strcmp(token, "BLUE"))
					 			DictPair.second.color = system_settings::COLOR_BLUE;
					 		else if (!strcmp(token, "GREEN"))
					 			DictPair.second.color = system_settings::COLOR_GREEN;
					 		break;
					 	default:
					 		break;
					 }
      				token = strtok( NULL, delims );
      				count++;
    				}
    				g_msgDictionary.insert(DictPair);
		}
		fclose(file);
	}
	else
	{
		SysMessage(ERROR_MSG, "fail to open '%s' dictionary file", fileName);
		return 0;
	}

	SysMessage(INFO_MSG, "Dictionary loaded");
*/	
	return true;
}



bool load_escalators_types(string fileName)
{
/*
	Block*		curBlock = NULL;
	Type*		curType = NULL;
	Signal*		curSignal = NULL;
	char	buffer[256];
	char	temp[40];
	FILE*	handle;
   	char 	*val_name, *val_data;
    char  	tokens[] = "=";
	int	   	mode = 0;
	int 	   	intTemp, i;
	int		typeCount = -1, blockCount = -1, signalCount = -1;

	handle = fopen(fileName, "rt");
	if (!handle)
	{
		SysMessage(ERROR_MSG, "can't open escalators' library [%s]", strerror(errno));
		return 0;
	}
		
	while(fgets(buffer, 256, handle))
	{
		if (extract_string(buffer, temp, '[', ']'))
		{
			if (!strcmp("type", temp))
			{
				mode = 1;
				typeCount++;
				if (typeCount >= g_escTypeNum)
				{
					typeCount = -1;
					mode = 0;
					SysMessage(ERROR_MSG, "Parsing escalator library file");
					fclose(handle);
					return 0;
				}
				signalCount = -1;
				blockCount = -1;
			}
			else if (!strcmp("block", temp))
			{
				mode = 2;
				blockCount++;
				signalCount = -1;
			}
			else if (!strcmp("signal", temp))
			{
				mode = 3;
				signalCount++;
			}
		}
		else if (buffer[0] != '#')
		{
			val_name = strtok(buffer, tokens);
			val_data = strtok(NULL, tokens);
			if (val_name && val_data)
			{
				if (!strcmp("typeNum", val_name))
				{					
					g_escTypeNum = atoi(val_data);
					if (g_escTypeNum)
						g_escTypes = new Type[g_escTypeNum];
				}
				else if (!strcmp("blockNum", val_name))
				{	
					if (g_escTypes)
					{
						g_escTypes[typeCount].size = atoi(val_data);
						g_escTypes[typeCount].blocks = new Block[g_escTypes[typeCount].size];
					}
				}
				else if (!strcmp("signalNum", val_name))
				{					
					if (g_escTypes[typeCount].blocks)
					{
						g_escTypes[typeCount].blocks[blockCount].size = atoi(val_data);
						g_escTypes[typeCount].blocks[blockCount].signals = new Signal[g_escTypes[typeCount].blocks[blockCount].size];
					}
				}
				else if (!strcmp("num", val_name))
				{					
					g_escTypes[typeCount].blocks[blockCount].signals[signalCount].index = atoi(val_data);
				}
				else if (!strcmp("description", val_name))
				{					
					g_escTypes[typeCount].blocks[blockCount].signals[signalCount].hint.Set(val_data);
				}
				else if (!strcmp("name", val_name))
				{
					switch(mode)
					{
						case 1:
							g_escTypes[typeCount].name.Set(val_data);
							break;
						case 2:
							g_escTypes[typeCount].blocks[blockCount].name.Set(val_data);
							break;
						case 3:
							g_escTypes[typeCount].blocks[blockCount].signals[signalCount].name.Set(val_data);
							break;
						default:
							break;
					}
				}
			}
		}
	}

	fclose(handle);
*/	
	return true;
}

#ifdef __UNIT_TESTING__

int Initialize( int argc, char *argv[] )
{

	time_t time_now=time(NULL);
	g_main_log.filter.set_start_time(time_now);
	g_main_log.filter.set_stop_time(time_now);

	g_archive_log.filter.set_start_time(time_now);
	g_archive_log.filter.set_stop_time(time_now);

	unit_test u_test;
	u_test.run_tests();

	return( Pt_CONTINUE );
}

#else // #ifdef __UNIT_TESTING__

int Initialize( int argc, char *argv[] )
{
	PtInputId_t *InputRes=NULL;
	ApDBase_t *widgets_dbase;
	
	struct sigaction sa_sig_pipe;
	memset (&sa_sig_pipe, 0, sizeof(sa_sig_pipe));
	sa_sig_pipe.sa_handler=&SigpipeHandler;
	
	sigaction(SIGPIPE,&sa_sig_pipe,NULL);

	time_t time_now=time(NULL);
	g_main_log.filter.set_start_time(time_now);
	g_main_log.filter.set_stop_time(time_now);

	g_archive_log.filter.set_start_time(time_now);
	g_archive_log.filter.set_stop_time(time_now);

   
	// загружаем библиотеку виджето

	widgets_dbase=ApOpenDBase(ABM_interface);
  	g_system_settings.prepare_visualization(widgets_dbase);	    
	
	g_system_settings.sys_message(system_settings::INFO_MSG, 
														string("Spider: startup"));
 	
	// описание роутинга
	if (!g_router.load_routing(g_system_settings.get_routing_name()))
    	{
		string mess("Файл конгурац роутинга\n");
		mess+=g_system_settings.get_routing_name();
		mess+="\nне нйден ил поврежден";

 		g_system_settings.message_window(system_settings::ERROR_MSG,
																   mess);
    	} else {
		// запускаем поток роуте
		g_router.start();
    	};
	
	// канал для пульсов
	g_chanID = PhChannelAttach(0, -1, NULL);
	if (g_chanID < 0)
		{
 		g_system_settings.message_window(system_settings::ERROR_MSG,
																   string("Ошибка соданя канала IPC"));
	
		PtExit(EXIT_FAILURE);
		} 	else {
		string message ("Created channel. Id ");
		vector<char> tmp_chars(10);
		itoa (g_chanID, &tmp_chars[0], 10);
		message+=&tmp_chars[0];
		g_system_settings.sys_message(system_settings::INFO_MSG,
															message);
		};

	InputRes=PtAppAddInput(NULL, 0, PulseReceiver, NULL);
    	if (InputRes== NULL)
    	{
		g_system_settings.message_window(system_settings::ERROR_MSG,
																   string("Ошибка регистрации обработчика пульсов !"));

          PtExit(EXIT_FAILURE);
    	}
 
 
 	metro_escalators_container::iterator_metro_escalators iter_esc = g_escalators.begin();
	while (iter_esc != g_escalators.end())
	{
		iter_esc->second.connect_to_channel(g_chanID);	
		iter_esc->second.start();
		iter_esc++;
	}

	// запуск задачи опроса устройств
	if (pthread_create(&g_pingTID, NULL, &PingThread, NULL) != EOK)
	{
		g_system_settings.message_window(system_settings::ERROR_MSG,
																   string("Ошибка  запуска менеджера устройств!"));

		PtExit(EXIT_FAILURE);
	} 	else {
		g_system_settings.sys_message(system_settings::INFO_MSG, 
															string("Start ping thread"));

	};

	if (pthread_create(&g_timerTID, NULL, &TimerThread, NULL) != EOK)
	{
		g_system_settings.message_window(system_settings::ERROR_MSG,
																   string("Ошибка запуска системного таймера"));

		PtExit(EXIT_FAILURE);
	} 	else {
		g_system_settings.sys_message(system_settings::INFO_MSG, 
															string("Start system timer thread"));
	};
		
//	g_sound.Play("startup.wav");
//	g_sound.Play("test.wav");
	
	return( Pt_CONTINUE );
}

#endif // #ifdef __UNIT_TESTING__