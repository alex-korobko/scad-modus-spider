/* Y o u r   D e s c r i p t i o n                       */
/*                            AppBuilder Photon Code Lib */
/*                                         Version 2.01  */

using namespace std;

#include "global.h"

#include "system_settings.h"
#include "spider_exception.h"

#include "alert.h"

#include "contain_msg_types.h"
#include "contain_msg_dict.h"

#include "devices_types.h"
#include "contain_metro_devices_types.h"

#include "contain_morning_start.h"

#include "line.h"
#include "contain_metro_lines.h"

#include "station.h"
#include "contain_metro_stations.h"

#include "metro_device.h"

#include "data_block.h"

#include "escalator.h"
#include "contain_metro_escalators.h"

#include "contain_log_rec.h"

#include "command.h"
#include "contain_cmd_pool.h"

#include "sound.h"
#include "router.h"

system_settings g_system_settings;
alerts_container g_alerts;
metro_stations_container g_stations;
metro_escalators_container g_escalators;
cmd_pool_container g_command_pool;
metro_devices_types_container g_metro_devices_types;
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

void SigpipeHandler (int SignalNumber){
++SigpipeCount;
};


void* Run(void* arg){
	int		readNum;
	int		sock;
	metro_escalator*		curEscalator;		
	metro_escalator::buffer_data_type output(128);

	int			reconnect;
//	int 		time_correct_int=0;	
	
	curEscalator = (metro_escalator*) arg;

	if (curEscalator==NULL)
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

	// если эскалатор не включен - выхд
	if (curEscalator->get_enabled()!=system_settings::ENABLED)
				return 0;

	reconnect = 0;		

	do{		
		sock = connect_to_server(curEscalator->get_ip());
		
		// timeout before store data
		usleep(1000);

		if (!sock)
		{
			// sleep before reconnect
			sleep(1);
			continue;
		}

		curEscalator->get_data();		
		do{
		       {
			      metro_escalator::lock_mutex data_mutex(curEscalator->get_out_mutex());
			      curEscalator->swap_out_buffer(output);
			   }

			metro_escalator::buffer_data_type input(128);
			readNum = send_buffer(sock, output, input);			
						 			
 			printf ("Readnum %d\n", readNum);
 			
			if (readNum==0){
				if (curEscalator->get_online() == system_settings::ONLINE){
					if (reconnect> 4){
							MsgSendPulse(curEscalator->get_connection_id(), SIGEV_PULSE_PRIO_INHERIT, 0, curEscalator->get_id());
							reconnect=0;
						};
					reconnect++;
				}
			} else {			
				if (curEscalator->get_online() <= system_settings::OFFLINE){
					curEscalator->set_online(1);
					MsgSendPulse(curEscalator->get_connection_id(), SIGEV_PULSE_PRIO_INHERIT, 1, curEscalator->get_id());
					// после коннекта - запрос данных
					curEscalator->get_data();
				}
/*
				// DEBUG dump modbus packet
				printf("Packet [%d] tid %d  socket %d\n", curEscalator->get_id(), pthread_self(), sock);
				for(int i=0; i<readNum; i++)
					printf( "%0x ", input[i]);
				printf("\n\n"); 
				// DEBUG
*/
				// проверяем команду
				switch(input[1]){
					case 1:
					 	// смена статуса
						if (input[3] & 1)		
                            curEscalator->get_data();
                            break;
					case 4:
						// пришел блок
						printf("In block %d esc %d\n", input[0], curEscalator->get_number());
						if ((input[0] == curEscalator->get_number()) && input[2] == 76){
							metro_escalator::buffer_data_iterator iter_first, iter_last;

							printf("\nin that\n");
				
							iter_first=iter_last=input.begin();
							//determination appends range
							advance(iter_first, 3); //third element
							advance(iter_last, readNum-3); //readNum-3 element

							input.erase(input.begin(), iter_first);
							input.erase(iter_last, input.end());

							metro_escalator::lock_mutex data_mutex(curEscalator->get_in_mutex());
							curEscalator->swap_in_buffer(input);
							delete(&data_mutex);
							
							MsgSendPulse(curEscalator->get_connection_id(), SIGEV_PULSE_PRIO_INHERIT, 3, curEscalator->get_id());								
							
						} //if ((input[0
						break;
					default:				
						break;
				} // switch (input[1])
			}// else if (readNum==0) 

		} while(reconnect==0);
			
		close(sock);
		usleep(3000);
	} while(1);
	
	return 0; 
}
	
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
	 				
	 		}
	 		iter_esc++;			
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

void load_map(string file_name) throw (spider_exception) {
 	enum {LINE=0, STATION, ESCALATOR, ENTRIES_COUNT};
	ostringstream exception_description;
	string	section_name;
	const char *section_name_c_str;
	vector<string> sections_names(ENTRIES_COUNT);

	sections_names[LINE]="line";
	sections_names[STATION]="station";
 	sections_names[ESCALATOR]="escalator";
 	
	if (PxConfigOpen( file_name.c_str(), PXCONFIG_READ)!=Pt_TRUE){
		exception_description<<"Can`t open config file "<<file_name<<" : "<<strerror(errno);
		throw spider_exception(exception_description.str());		
	};
	
	g_escalators.erase(g_escalators.begin(),
									g_escalators.end());
	
	g_stations.erase(g_stations.begin(),
								g_stations.end());
	
	g_lines.erase(g_lines.begin(),
						g_lines.end());

	try {
	    section_name_c_str=PxConfigNextSection();
          while (section_name_c_str!=NULL) {
              section_name=section_name_c_str;
	
              if (section_name.compare(sections_names[LINE])==0) 	{
                     g_lines.load_line_parameters ();
                  } else if (section_name.compare(sections_names[STATION])==0) {
                     g_stations.load_station_parameters (&g_lines);
                  } else if (section_name.compare(sections_names[ESCALATOR])==0) {
                     g_escalators.load_escalator_parameters (&g_stations,
                                                                                    &g_metro_devices_types,
                     																&g_system_settings);
                  } else {
					exception_description<<"Unrecognized config section "<<section_name;
					throw spider_exception(exception_description.str());
                 };
				section_name_c_str=PxConfigNextSection();
           };
	} catch (spider_exception (spr_exc)) {
		exception_description<<spr_exc.get_description();
		if  (PxConfigClose()!=Pt_TRUE) {
				exception_description<<"\nCan`t close config file "<<file_name;
		};
		throw spider_exception(exception_description.str());
	};
 
	g_lines.set_current_line(g_lines.end());
	g_stations.set_current_station(g_stations.end());
	g_escalators.set_current_escalator(g_escalators.end());

	if  (PxConfigClose()!=Pt_TRUE)	{
		exception_description<<"Can`t close config file "<<file_name;
		throw spider_exception(exception_description.str());
	};
}

// PtInputCallbackProcF_t PulseReceiver;

int PulseReceiver(void *data, int rcvid, void *message, size_t mbsize )
{
	struct _pulse* pulse;

	pulse = (struct _pulse*)message;	
	printf("Code %d, value %d\n", pulse->code, pulse->value.sival_int);
	
	metro_escalators_container::iterator_metro_escalators temp_escalators_iterator=g_escalators.find(pulse->value.sival_int);
	if(temp_escalators_iterator!=g_escalators.end())
		{
			printf("Code %d, value %d ESCALATOR %d on %d\n", pulse->code, pulse->value.sival_int, temp_escalators_iterator->second.get_number(), temp_escalators_iterator->second.get_online());
			switch(pulse->code)
				{
					case 0:
					case 1:
					case 2:
						printf("In PulseReceiver\n");
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


#ifdef __UNIT_TESTING__

int Initialize( int argc, char *argv[] )
{

	unit_test u_test;
	u_test.run_tests();
	PtInputId_t *InputRes=NULL;
	ApDBase_t *widgets_dbase;
	
	struct sigaction sa_sig_pipe;
	memset (&sa_sig_pipe, 0, sizeof(sa_sig_pipe));
	sa_sig_pipe.sa_handler=&SigpipeHandler;
	
	sigaction(SIGPIPE,&sa_sig_pipe,NULL);
   
	// загружаем библиотеку виджето

	widgets_dbase=ApOpenDBase(ABM_interface);
  	g_system_settings.prepare_visualization(widgets_dbase);	    
	
	g_system_settings.sys_message(system_settings::INFO_MSG, 
														string("Spider: startup"));
 	
	// описание роутинга
	if (!g_router.load_routing(g_system_settings.get_routing_name()))
    	{
		string mess("Файл когурац оутинга\n");
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
 

	time_t time_now=time(NULL);
	g_main_log.filter.set_start_time(time_now);
	g_main_log.filter.set_stop_time(time_now);

	g_archive_log.filter.set_start_time(time_now);
	g_archive_log.filter.set_stop_time(time_now);

	g_msg_types.load(&g_system_settings, 
								g_system_settings.get_messages_types_name() );

	g_msg_dictionary.load(&g_system_settings, 
										&g_msg_types,
										g_system_settings.get_global_messages_name() );

	g_metro_devices_types.load( &g_system_settings,
			  										&g_msg_types,	
													g_system_settings.get_devices_types_name() );

	load_map(g_system_settings.get_devices_config_name());
 	
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
																   "pthread_create(&g_pingTID, NULL, &PingThread, NULL) != EOK");

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
		
	g_sound.play(g_system_settings.get_start_wav_file_name());
	
	return( Pt_CONTINUE );

/*
	time_t time_now=time(NULL);
	g_main_log.filter.set_start_time(time_now);
	g_main_log.filter.set_stop_time(time_now);

	g_archive_log.filter.set_start_time(time_now);
	g_archive_log.filter.set_stop_time(time_now);

	g_msg_types.load(&g_system_settings, 
								g_system_settings.get_messages_types_name() );

	g_msg_dictionary.load(&g_system_settings, 
										&g_msg_types,
										g_system_settings.get_global_messages_name() );

	g_metro_devices_types.load( &g_system_settings,
			  										&g_msg_types,	
													g_system_settings.get_devices_types_name() );

	load_map(g_system_settings.get_devices_config_name());


	return( Pt_CONTINUE );
*/
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
   
	// загружаем библиотеку виджето

	widgets_dbase=ApOpenDBase(ABM_interface);
  	g_system_settings.prepare_visualization(widgets_dbase);	    
	
	g_system_settings.sys_message(system_settings::INFO_MSG, 
														string("Spider: startup"));
 	
	// описание роутинга
	if (!g_router.load_routing(g_system_settings.get_routing_name()))
    	{
		string mess("routing file");
		mess+=g_system_settings.get_routing_name();
		mess+=" not found";

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
 

	time_t time_now=time(NULL);
	g_main_log.filter.set_start_time(time_now);
	g_main_log.filter.set_stop_time(time_now);

	g_archive_log.filter.set_start_time(time_now);
	g_archive_log.filter.set_stop_time(time_now);

	try {
	g_msg_types.load(g_system_settings.get_messages_types_name() );

	g_msg_dictionary.load(g_system_settings.get_global_messages_name(),
										&g_msg_types);

	g_metro_devices_types.load(	g_system_settings.get_devices_types_name(),
                                                 &g_msg_types);

	load_map(g_system_settings.get_devices_config_name());
	} catch (spider_exception spr_exc) {
		g_system_settings.message_window (system_settings::ERROR_MSG,
                                     spr_exc.get_description());
	};
	 	
 	metro_escalators_container::iterator_metro_escalators iter_esc = g_escalators.begin();
	while (iter_esc != g_escalators.end())
	{
		iter_esc->second.start();
		iter_esc++;
	}

	// запуск адачи опроса устройств
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
																   string("Оибк запуска системного таймера"));

		PtExit(EXIT_FAILURE);
	} 	else {
		g_system_settings.sys_message(system_settings::INFO_MSG, 
															string("Start system timer thread"));
	};

	g_sound.play(g_system_settings.get_start_wav_file_name());
		
	return( Pt_CONTINUE );
}

#endif // #ifdef __UNIT_TESTING__

