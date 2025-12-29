#include <Ap.h>
#include <Ph.h>
#include <Pt.h>

#include <arpa/inet.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/neutrino.h>
#include <sys/syspage.h>
#include <inttypes.h>
#include <fcntl.h>
#include <signal.h>
#include <iostream.h>
#include <sys/socket.h>
#include <net/if.h>
#include <net/route.h>


#include <hash_map>
#include <vector>
#include <string>
#include <list>
#include <set>
#include <hash_map>
#include <functional>
#include <algorithm>
#include <sstream>
using namespace std;

#include "defines.h"
#include <sys/select.h> //#define FD_SETSIZE
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
#include "log_records_container.h"

#include "devices_types.h"
#include "timer_command_container.h"
#include "metro_devices_types_container.h"
#include "metro_device.h"

class metro_devices_container;
#include "station.h"
#include "metro_stations_container.h"
#include "metro_devices_container.h"

#include "router.h"

extern log_records_container *main_log;
extern bool sending_commands_disabled;

int pulse_reciever_catcher
                    (void *data,
                     int rcvid,
                     void *message,
                    size_t mbsize ){
  
	struct _pulse* pulse;

     system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
      if (spider_sys_sett==NULL) {
              cout<<"In pulse_reciever_catcher can`t system_settings_spider"<<endl;
              	return Pt_HALT;
        };

	metro_devices_container *metro_devices=	metro_devices_container::get_instance();
    if (metro_devices==NULL) {
               spider_sys_sett->sys_message(system_settings::ERROR_MSG,  
									                                  "Can`t get instance of metro_devices_container");
              	return Pt_HALT;
      };

	pulse = (struct _pulse*)message;
			switch(pulse->code){
					case system_settings_spider::PULSE_CODE_UPDATE_DEVICE : {
                           	metro_devices_container::iterator dev_iter=metro_devices->find(pulse->value.sival_int);
                            if(dev_iter!=metro_devices->end()){
			                        dev_iter->second->decode_answer_from_device_to_data_block();
//                                    if (dev_iter->second->get_id()==1)
//                                            cout<<"Pulse to device with id 1"<<endl;
                                    } else {
                                     ostringstream error_description;
                                     error_description<<"pulse_receiver_catcher(...) unrecognized device id : "<<pulse->value.sival_int;
                                     spider_sys_sett->sys_message(system_settings::ERROR_MSG,  
                                                          error_description.str());
                                    return Pt_HALT;
                                   };
                           break;
						};
                    case system_settings_spider::PULSE_CODE_TIMER_INTERVAL : {

                          static int  minute_tick=0;
                          static int  hour_tick=0;
                          time_t			current_time;
                          struct	tm		local_time;

                          timer_command_container *timer_commands=timer_command_container::get_instance();
                          if (timer_commands==NULL) {
                                  system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
                                   spider_sys_sett->sys_message(system_settings::ERROR_MSG,  
									                                          "Can`t get instance of metro_devices");
                                      return Pt_HALT;
                                        };

                            current_time=time(NULL);
							localtime_r(&current_time, &local_time);

                              if ( ! sending_commands_disabled &&
                                   hour_tick!=local_time.tm_hour) {
                                    hour_tick=local_time.tm_hour;
                                    metro_devices->set_time();
                                 }; // if (hour_tick!=local_time.tm_min)

                              if (minute_tick!=local_time.tm_min) { //minute is changed
                                   minute_tick=local_time.tm_min;
                                  try {
                                      main_log->rotate();
                                  } catch (spider_exception spr_exc) {
                                    spider_sys_sett->sys_message(system_settings::ERROR_MSG,  
									                                  "In pulse_receiver_catcher(...)  main_log->rotate() : "+spr_exc.get_description());
                                 };

                                  try {
                                      main_log->save(spider_sys_sett->get_main_log_name());
                                  } catch (spider_exception spr_exc) {
                                    spider_sys_sett->sys_message(system_settings::ERROR_MSG,  
									                                  "In pulse_receiver_catcher(...)  main_log->save(spider_sys_sett->get_main_log_name()) : "+spr_exc.get_description());
                                 };

								//if first minute of day (tm_hour==0 and tm_minute==0 and commands_not_generated_today)
                               // cleaning previos day timer commands
                                try {
                                   if (local_time.tm_hour==23 &&
                                    local_time.tm_min==59 &&
                                     timer_commands->get_was_created_in_this_day()) {
                                      timer_commands->set_was_created_in_this_day(false);
                                      timer_commands->set_timer_commands_accepted(false);
                                      timer_commands->change_indicator_state();
                                      timer_commands->prepare_to_display(); //ATTENTION !!:  prepare_to_display() throw exception if raw list widget is NULL
                                     };
                                   } catch (spider_exception spr_exc) {
                                             spider_sys_sett->sys_message(system_settings::ERROR_MSG,  
                                                                                "In pulse_reciever_catcher cleaning previos day raised exception :"+spr_exc.get_description());
                                   };

                               //creating new day`s timer commands
                             try {
                               if (local_time.tm_hour==0 &&
                                    local_time.tm_min==0 &&
                                   ! timer_commands->get_was_created_in_this_day()) {
                                           metro_devices->prepare_timer_command();
                                           timer_commands->set_was_created_in_this_day(true);
                                           timer_commands->set_timer_commands_accepted(false);
                                           timer_commands->change_indicator_state();
                                           timer_commands->prepare_to_display();
                                          };
                                   } catch (spider_exception spr_exc) {
                                             spider_sys_sett->sys_message(system_settings::ERROR_MSG,  
                                                                                "In pulse_reciever_catcher creating new day raised exception :"+spr_exc.get_description());
                                   };

                                if (timer_commands->get_was_created_in_this_day() &&
                                     timer_commands->get_timer_commands_accepted())
                                         metro_devices_container::execute_timer_commands(
                                                                                                 local_time.tm_hour,
                                                                                                 local_time.tm_min);

                            };
                         break;
                        };
					default: {
			            	ostringstream error_description;
                            error_description<<"pulse_receiver_catcher(...) unrecognized pulse code : "<<static_cast<int>(pulse->code);
                            spider_sys_sett->sys_message(system_settings::ERROR_MSG,  
									                                  error_description.str());
                	return Pt_HALT;
                    };
				};

    PtFlush();
	return Pt_HALT;
}

void log_device_buffer(int device_id, metro_device::buffer_data_type buffer_to_log)
{
    system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
      ostringstream msg_description;
      msg_description<<" buffer for dev id "<<device_id<<"\n";

      vector<char> tmp_chars(20);
      for (vector<byte>::size_type i =0; i<buffer_to_log.size(); i++) {
            itoa(buffer_to_log[i], &tmp_chars[0], 16);
           msg_description<<" 0x"<<&tmp_chars[0];
            if (i>0 && (i%10)==0) msg_description<<"\n";
       };
      spider_sys_sett->sys_message(system_settings::INFO_MSG,  
                                                          msg_description.str());
};

void* metro_device_thread
          (void* arg) {
	client_socket  *socket_dev;
	metro_device*	metro_dev;
	int	 connect_faliures_count=0;
	metro_device::buffer_data_type
                 request_to_device,  
	             answer_from_device_crc,
                 answer_from_device_header,
                 answer_from_device;

	metro_device::buffer_data_type::iterator  answer_buffer_iter;
	word crc_value;
	metro_dev = (metro_device*) arg;

    system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();

   uint64_t connect_to_device_timeout_nanosec(system_settings_spider::CONNECT_TO_DEVICE_TIMEOUT),
          recieve_send_to_device_timeout_nanosec(system_settings::RECIEVE_SEND_TO_DEVICE_TIMEOUT);
   connect_to_device_timeout_nanosec*=1000000000;
   recieve_send_to_device_timeout_nanosec*=1000000000;

    answer_from_device_header.reserve(3);
    answer_from_device.reserve(255);

	if (metro_dev==NULL) {
		spider_sys_sett->sys_message(system_settings::ERROR_MSG, "metro_device_thread(...) invalid metro device pointer");
		return 0;
	}

	if (!metro_dev->get_enabled())
	{
						return 0;
	};
//door not in thread now - it reciv data from shavrs or escalators on station
   if (metro_dev->get_type_description()==metro_device_type::DEVICE_TYPE_DOOR)
						                                                                                         return 0;

	while (true) {
          socket_dev=new client_socket(metro_dev->get_ip(),
                                                    system_settings::MODBUS_TCP_PORT,
                                                    connect_to_device_timeout_nanosec,
                                                    recieve_send_to_device_timeout_nanosec);

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
                       // Set empty buffer to signal connection failure to device automata
                       metro_device::buffer_data_type empty_buffer;
                       metro_dev->set_answer_from_device_buffer(empty_buffer);
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
               int header_bytes_received = socket_dev->recv(answer_from_device_header, system_settings::MODBUS_HEADER_SIZE);
               
               // Check if we received at least the header
               if (header_bytes_received < system_settings::MODBUS_HEADER_SIZE) {
                   ostringstream exception_description;
                   exception_description<<"metro_device_thread(...) for dev id "<<metro_dev->get_id();
                   exception_description<<" received only "<<header_bytes_received
                                        <<" bytes for header, expected "<<system_settings::MODBUS_HEADER_SIZE;
                   spider_sys_sett->sys_message(system_settings::ERROR_MSG, exception_description.str());
                   connect_faliures_count++;
                   usleep(system_settings::DELAY_BETWEEN_REQUESTS_TO_DEVICE);
                   continue;
               }
               
               // Calculate how many bytes we need for the complete packet based on header
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
                   
               // Calculate total packet size (header + data)
               vector<byte>::size_type total_packet_size = system_settings::MODBUS_HEADER_SIZE + recieved_answer_size;
               
               // Initialize answer buffer with header (and any extra bytes that came with header)
               answer_from_device = answer_from_device_header;
               
               // Calculate how many more bytes we need to receive
               // If we already received more than just the header, account for that
               int bytes_already_received = header_bytes_received - system_settings::MODBUS_HEADER_SIZE;
               int bytes_still_needed = recieved_answer_size - bytes_already_received;
               
               // Read remaining bytes if needed
               if (bytes_still_needed > 0) {
                   metro_device::buffer_data_type remaining_data;
                   int remaining_bytes_received = socket_dev->recv(remaining_data, bytes_still_needed);
                   
                   // Check if we received enough bytes
                   if (remaining_bytes_received < bytes_still_needed) {
                       ostringstream exception_description;
                       exception_description<<"metro_device_thread(...) for dev id "<<metro_dev->get_id();
                       exception_description<<" received only "<<remaining_bytes_received
                                            <<" bytes for data, expected "<<bytes_still_needed;
                       spider_sys_sett->sys_message(system_settings::ERROR_MSG, exception_description.str());
                       connect_faliures_count++;
                       usleep(system_settings::DELAY_BETWEEN_REQUESTS_TO_DEVICE);
                       continue;
                   }
                   
                   // Append remaining data to answer buffer
                   answer_from_device.insert(answer_from_device.end(), remaining_data.begin(), remaining_data.end());
               }
               
               // Verify we have the complete packet
               if (answer_from_device.size() < total_packet_size) {
                   ostringstream exception_description;
                   exception_description<<"metro_device_thread(...) for dev id "<<metro_dev->get_id();
                   exception_description<<" incomplete packet: received "<<answer_from_device.size()
                                        <<" bytes, expected "<<total_packet_size;
                   spider_sys_sett->sys_message(system_settings::ERROR_MSG, exception_description.str());
                   connect_faliures_count++;
                   usleep(system_settings::DELAY_BETWEEN_REQUESTS_TO_DEVICE);
                   continue;
               }
               
           }catch (socket_exception sock_exc) {
		               ostringstream exception_description;
                       exception_description<<"metro_device_thread(...) for dev id "<<metro_dev->get_id();
                       exception_description<<" socket_dev->recv(...) : "<<sock_exc.get_description();
                       spider_sys_sett->sys_message(system_settings::ERROR_MSG,  
                                                          exception_description.str());
                       connect_faliures_count++;
                       usleep(system_settings::DELAY_BETWEEN_REQUESTS_TO_DEVICE);
                        cout<<exception_description.str()<<endl;
                       continue;
              };
                   //CRC check
                    answer_buffer_iter=answer_from_device.begin();
                    advance(answer_buffer_iter, 
                                  answer_from_device.size()-system_settings::MODBUS_CRC_SIZE);   
                    crc_value=spider_sys_sett->crc(metro_device::buffer_data_type(
		                                                             answer_from_device.begin(),
		                                                              answer_buffer_iter));

                   answer_from_device_crc=spider_sys_sett->bytes_of_type<word>(crc_value);
                   if (answer_from_device_crc[0]!=
                       answer_from_device[answer_from_device.size()-system_settings::MODBUS_CRC_SIZE] ||
                       answer_from_device_crc[1]!=
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
                                 usleep(system_settings::DELAY_BETWEEN_REQUESTS_TO_DEVICE);
                                 continue;
                       };
                  
                   connect_faliures_count=0;
                   metro_dev->set_answer_from_device_buffer(answer_from_device);
                   MsgSendPulse(metro_dev->get_connection_id(),
                                          system_settings_spider::PHOTON_THREAD_PULSE,
                                          system_settings_spider::PULSE_CODE_UPDATE_DEVICE,
                                          metro_dev->get_id());
					if (metro_dev->is_packet_logging())
						{
							log_device_buffer(metro_dev->get_id(), answer_from_device);
						}
                    usleep(system_settings::DELAY_BETWEEN_REQUESTS_TO_DEVICE);
              }; //while (connect_faliures_count

              // Set empty buffer to signal connection failure (too many failures or initialization failed)
              // This allows device automata to properly handle offline state transition
              metro_device::buffer_data_type empty_buffer;
              metro_dev->set_answer_from_device_buffer(empty_buffer);
              MsgSendPulse(metro_dev->get_connection_id(),
                                     system_settings_spider::PHOTON_THREAD_PULSE,
                                     system_settings_spider::PULSE_CODE_UPDATE_DEVICE,
                                     metro_dev->get_id());

		      ostringstream exception_description;
              exception_description<<"metro_device_thread(...) for dev id "<<metro_dev->get_id();
              exception_description<<" reconnection for the socket "<<endl;
              spider_sys_sett->sys_message(system_settings::ERROR_MSG,  
                                                          exception_description.str());
              cout<<exception_description.str()<<endl;

			delete(socket_dev); 
            sleep(system_settings_spider::CONNECT_TO_DEVICE_TIMEOUT);

			}; //while (true)

	return 0; 
}

void* routing_thread
          (void* arg) {

    system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();

    router& router_inst=router::get_instance();
    router::routes_iterator routes_iter;
    router::gateways_iterator gateways_iter;
   try {
    bool connect_to_test_host_established;
    if (router_inst.size_gateway()<2) {
             spider_sys_sett->sys_message(system_settings::ERROR_MSG,  
                                                          "routing_thread: router_inst.gateways_size()<2");
             return NULL;
           };

    if (router_inst.empty_routes()) {
             spider_sys_sett->sys_message(system_settings::ERROR_MSG,  
                                                          "routing_thread: routes is empty");
              return NULL;
           };

		ostringstream err_message;
        err_message<<"Size of routing "<<router_inst.size_routes();
         spider_sys_sett->sys_message(system_settings::ERROR_MSG,  
                                               err_message.str());

     	while (true) {
         routes_iter=router_inst.begin_routes();
         while (routes_iter!=router_inst.end_routes()){
                sleep(5);
                gateways_iter=router_inst.begin_gateway();
                while(gateways_iter!=router_inst.end_gateway() ) {
                            //test all connections in previos gateway
                             connect_to_test_host_established=false;
                             router::route::test_hosts_iterator test_hosts_iter=routes_iter->test_hosts.begin();
                             while (test_hosts_iter!=routes_iter->test_hosts.end()) {
                                  if (router_inst.test_connection_to_test_host(*test_hosts_iter)) {
                                          connect_to_test_host_established=true;
                                           break;
                                       };
                                  test_hosts_iter++;
                             }; //while (test_hosts_iter!=routes_iter->test_hosts.end())
                           if (connect_to_test_host_established) break;

                               router_inst.change_route(routes_iter->destination,
                                                                      routes_iter->mask,
                                                                      *gateways_iter);
                          
                               //test all connections in new gateway
                            connect_to_test_host_established=false;
                            test_hosts_iter= routes_iter->test_hosts.begin();
                             while (test_hosts_iter!=routes_iter->test_hosts.end()) {
                                  if (router_inst.test_connection_to_test_host(*test_hosts_iter)) {
                                          connect_to_test_host_established=true;
                                           break;
                                       };
                                  test_hosts_iter++;
                             }; //while (test_hosts_iter!=routes_iter->test_hosts.end())

                           if (connect_to_test_host_established) {
									 gateways_iter=router_inst.end_gateway();
									 break;
						    };
                      gateways_iter++;
                  }; //while(gateways_iter!=router_inst.end_gateway()
                routes_iter++;
          };//while (routes_iter!=router_inst.end_routes()
     }; //while (true)
      } catch (runtime_error run_err) {
             spider_sys_sett->sys_message(system_settings::ERROR_MSG,  
                                                                     string("routing_thread: ")+run_err.what());
      };
	return NULL;
};

