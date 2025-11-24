#include <Ap.h>
#include <Ph.h>
#include <Pt.h>

#include <arpa/inet.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/neutrino.h>
#include <sys/syspage.h>
#include <inttypes.h>
#include <fcntl.h>
#include <signal.h>
#include <iostream>

#include <map>
#include <vector>
#include <string>
#include <list>
#include <map>
#include <functional>
#include <algorithm>
#include <sstream>

using namespace std;

#include "defines.h"
#include "spider_exception.h"
#include "system_settings.h"
#include "system_settings_spider.h"

#include "socket_exception.h"
#include "generic_socket.h"
#include "client_socket.h"

#include "command.h"
#include "data_block.h"
#include "escalator_data_block.h"
#include "shavr_data_block.h"

#include "msg_types_container.h"
#include "msg_dict_container.h"

#include "devices_types.h"
#include "timer_command_container.h"
#include "metro_devices_types_container.h"
#include "metro_device.h"

class metro_devices_container;
#include "station.h"
#include "metro_stations_container.h"
#include "metro_devices_container.h"

int pulse_reciever_catcher
                    (void *data,
                     int rcvid,
                     void *message,
                    size_t mbsize ){
	struct _pulse* pulse;
	metro_devices_container *metro_devices=	metro_devices_container::get_instance();
	
	pulse = (struct _pulse*)message;
	metro_devices_container::iterator 
	                  dev_iter=metro_devices->find(pulse->value.sival_int);

	if(dev_iter!=metro_devices->end()){
			switch(pulse->code){
					case system_settings_spider::PULSE_CODE_UPDATE_DEVICE : {
			              dev_iter->second->decode_answer_from_device_to_data_block();
                           break;
						};
					default: {
					       system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
					       ostringstream error_description;
                           error_description<<"pulse_receiver_thread(...) unrecognized pulse code : "<<pulse->code;
                           spider_sys_sett->sys_message(system_settings::ERROR_MSG,  
									                                           error_description.str());
                	return Pt_HALT;
                    };
			};
			PtFlush();
		} else {
           system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
           	ostringstream error_description;
           	error_description<<"pulse_receiver_thread(...) unrecognized device id : "<<pulse->value.sival_int;
            spider_sys_sett->sys_message(system_settings::ERROR_MSG,  
                                                          error_description.str());
		};
	return Pt_HALT;
}

void* metro_device_thread
          (void* arg) {
	client_socket  *socket_dev;
	metro_device*	metro_dev;
	int	 connect_faliures_count=0;
	metro_device::buffer_data_type
                 request_to_device,  
	             answer_from_device_header,
	             answer_from_device,
	             answer_from_device_crc;
	metro_device::buffer_data_type::iterator  answer_buffer_iter;
	word crc_value;
	struct timeval connection_timeout , recive_send_timeout;
	metro_dev = (metro_device*) arg;

system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();

	if (metro_dev==NULL) {
		spider_sys_sett->sys_message(system_settings::ERROR_MSG, "metro_device_thread(...) invalid metro device pointer");
		return 0;
	}

	if (!metro_dev->get_enabled())
						return 0;

    connection_timeout.tv_sec=system_settings_spider::CONNECT_TO_DEVICE_TIMEOUT;
    connection_timeout.tv_usec=0;

    recive_send_timeout.tv_sec=system_settings::RECIEVE_SEND_TO_DEVICE_TIMEOUT;
    	recive_send_timeout.tv_usec=0;
	
	while (true) {
          socket_dev=new client_socket(metro_dev->get_ip(),
                                                    system_settings::MODBUS_TCP_PORT,
                                                    connection_timeout,
                                                    recive_send_timeout);

		 connect_faliures_count=0;
		 try {
             socket_dev->initialize();
		 } catch (socket_exception sock_exc) {
		               	ostringstream exception_description;
                       exception_description<<"metro_device_thread(...) for dev id "<<metro_dev->get_id();
                       exception_description<<" socket_dev->initialize() : "<<sock_exc.get_description();
                       spider_sys_sett->sys_message(system_settings::ERROR_MSG,  
                                                          exception_description.str());
                       connect_faliures_count=system_settings::RECONNECTS_TO_DEVICE_COUNT;
		 };

         while (connect_faliures_count<system_settings::RECONNECTS_TO_DEVICE_COUNT) {

                   request_to_device=metro_dev->get_request_for_send_to_device();

          try{
                socket_dev->send(request_to_device);
          } catch (socket_exception sock_exc) {
               ostringstream exception_description;
               exception_description<<"metro_device_thread(...) for dev id "<<metro_dev->get_id();
               exception_description<<" socket_dev->send(...) : "<<sock_exc.get_description();
               spider_sys_sett->sys_message(system_settings::ERROR_MSG,  
                                                          exception_description.str());
               connect_faliures_count++;
               usleep(system_settings::DELAY_BETWEEN_REQUESTS_TO_DEVICE);
               continue;
           };

           try{
               answer_from_device_header.clear();
               answer_from_device_header.resize(system_settings::MODBUS_HEADER_SIZE);
               socket_dev->recv(&answer_from_device_header[0],
                                          system_settings::MODBUS_HEADER_SIZE);
           }catch (socket_exception sock_exc) {
		               ostringstream exception_description;
                       exception_description<<"metro_device_thread(...) for dev id "<<metro_dev->get_id();
                       exception_description<<" socket_dev->recv(answer_from_device_header) : "<<sock_exc.get_description();
                       spider_sys_sett->sys_message(system_settings::ERROR_MSG,  
                                                          exception_description.str());
                       connect_faliures_count++;
                       usleep(system_settings::DELAY_BETWEEN_REQUESTS_TO_DEVICE);
                        cout<<exception_description.str()<<endl;
                       continue;
              };

               vector<byte>::size_type recieved_answer_size;
   				if ((answer_from_device_header[system_settings::MODBUS_FUNCTION_CODE_INDEX]|0x80) == 
                       answer_from_device_header[system_settings::MODBUS_FUNCTION_CODE_INDEX]) {
   				          //answer from device  -  error code
                         recieved_answer_size=system_settings::MODBUS_CRC_SIZE;
                   } else if (answer_from_device_header[system_settings::MODBUS_FUNCTION_CODE_INDEX]==4 ||
                                 answer_from_device_header[system_settings::MODBUS_FUNCTION_CODE_INDEX]==3) {
                      		recieved_answer_size=answer_from_device_header[system_settings::MODBUS_DATA_BYTES_COUNT_INDEX]+
                   		                                   system_settings::MODBUS_CRC_SIZE;
                   } else {
                            recieved_answer_size=system_settings::MODBUS_CRC_SIZE+
                                                              system_settings::MODBUS_WRITE_COMMAND_ANSWER_FRAGMENT;
                   };

                    try{
                          answer_from_device.clear();
                          answer_from_device.resize(recieved_answer_size);
                          socket_dev->recv(&answer_from_device[0],
                                                     recieved_answer_size);
                    } catch (socket_exception sock_exc) {
		               ostringstream exception_description;
                       exception_description<<"metro_device_thread(...) for dev id "<<metro_dev->get_id();
                       exception_description<<" socket_dev->recv(answer_from_device) : "<<sock_exc.get_description();
                       spider_sys_sett->sys_message(system_settings::ERROR_MSG,  
                                                          exception_description.str());
                       connect_faliures_count++;
                        cout<<exception_description.str()<<endl;
                        usleep(system_settings::DELAY_BETWEEN_REQUESTS_TO_DEVICE);
                       continue;
                    };

                   answer_from_device.insert(answer_from_device.begin(),
                                                             answer_from_device_header.begin(),
                                                             answer_from_device_header.end());

                   //CRC check
                    answer_buffer_iter=answer_from_device.begin();
                    advance(answer_buffer_iter, 
                                  answer_from_device.size()-system_settings::MODBUS_CRC_SIZE);   
                    crc_value=spider_sys_sett->crc(metro_device::buffer_data_type(
		                                                             answer_from_device.begin(),
		                                                              answer_buffer_iter));

                   answer_from_device_crc=spider_sys_sett->bytes_of_type<word>(crc_value);
                   if (answer_from_device_crc[1]!=
                       answer_from_device[answer_from_device.size()-system_settings::MODBUS_CRC_SIZE] ||
                       answer_from_device_crc[0]!=
                       answer_from_device[answer_from_device.size()-system_settings::MODBUS_CRC_SIZE+1] ) {
		                        ostringstream exception_description;
                                exception_description<<"metro_device_thread(...) for dev id "<<metro_dev->get_id();
                                vector<char> tmp_chars(40);
                                itoa(answer_from_device_crc[1], 
                                       &tmp_chars[0],
                                       16);
                                exception_description<<" bad recieved packet crc : calculated crc "<<&tmp_chars[0];
                                itoa(answer_from_device_crc[0], 
                                       &tmp_chars[0],
                                       16);
                                exception_description<<&tmp_chars[0];
                                exception_description<<" answer crc  ";
                                itoa(answer_from_device[answer_from_device.size()-system_settings::MODBUS_CRC_SIZE], 
                                       &tmp_chars[0],
                                       16);
                                exception_description<<&tmp_chars[0];
                                itoa(answer_from_device[answer_from_device.size()-system_settings::MODBUS_CRC_SIZE+1], 
                                       &tmp_chars[0],
                                       16);
                                exception_description<<&tmp_chars[0];

                                spider_sys_sett->sys_message(system_settings::ERROR_MSG,  
                                                                                exception_description.str());
                                 connect_faliures_count++;
                                 usleep(system_settings_spider::DELAY_BETWEEN_REQUESTS_TO_DEVICE);
                                 continue;
                       };
                  
                   connect_faliures_count=0;

                   metro_dev->set_answer_from_device_buffer(answer_from_device);

                   MsgSendPulse(metro_dev->get_connection_id(),
                                          system_settings_spider::PHOTON_THREAD_PULSE,
                                          system_settings_spider::PULSE_CODE_UPDATE_DEVICE,
                                          metro_dev->get_id());

                    usleep(system_settings::DELAY_BETWEEN_REQUESTS_TO_DEVICE);
              }; //while (connect_faliures_count

              MsgSendPulse(metro_dev->get_connection_id(),
                                     system_settings_spider::PHOTON_THREAD_PULSE,
                                     system_settings_spider::PULSE_CODE_UPDATE_DEVICE,
                                     metro_dev->get_id());
              
			delete(socket_dev); 
            sleep(system_settings_spider::CONNECT_TO_DEVICE_TIMEOUT);

			}; //while (true)

	return 0; 
}

