using namespace std;

#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/neutrino.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <net/route.h>
#include <sys/ioctl.h> 

#include <signal.h>
#include <inttypes.h>

#include <photon/PxProto.h>
#include <iostream.h>

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <sstream>


#include "defines.h"
#include "program_settings.h"

#include "comport_exception.h"
#include "comport.h"

#include "socket_exception.h"
#include "generic_socket.h"
#include "server_socket.h"
#include "client_socket.h"

#include "fatal_program_exception.h"
#include "metro_device.h"
#include "metro_escalator.h"
#include "metro_shavr.h"
#include "metro_udku.h"

#include "router.h"
#include "watchdog.h"

typedef map<int, metro_device*, less<int> > devices_container;

comport  *comport_device;
devices_container metro_devices;
string welcome_message("");
double comport_delay_koeff=1;
int baud_rate=comport::BAUD_19200;

enum section_type {ESCALATOR=0, SHAVR, UDKU, ROUTE,
                 ROUTING, WATCHDOG_TIMER,
                 SETTINGS, SECTIONS_COUNT};


void* watchdog_thread_function (void* arg) {
program_settings* sett_obj=program_settings::get_instance();
watchdog_timer& w_timer=watchdog_timer::get_instance();	
try {
    w_timer.loop();
 } catch (metro_device::metro_device_exception exc) {
     sett_obj->sys_message(program_settings::ERROR_MSG, exc.get_description());
 };
return 0; 
}

void* socket_thread_function(void* socket_ptr){
	if (socket_ptr==NULL) return 0;
    program_settings* sett_obj=program_settings::get_instance();
	generic_socket *socket_device=static_cast<server_socket*>(socket_ptr);

   metro_device::command_data telemetr_request;
   metro_device::command_data 
            telemetr_answer;

   program_settings::bytes telemetr_crc;
   word crc_value;
   devices_container::iterator dev_iter;

  int  failures_count=0;
  while(failures_count<program_settings::SOCKET_FAILURES_COUNT) {
      try {
        telemetr_request.clear();
	    socket_device->recv(telemetr_request,
                                            program_settings::MODBUS_REQUEST_HEADER_SIZE);

        switch (telemetr_request[program_settings::MODBUS_FUNCTION_CODE_INDEX]) {
             case 4: {
  	          socket_device->recv(telemetr_request,
                                            program_settings::MODBUS_FUNCTION_4_REQUEST_PARTIAL_SIZE);
             break; };
             case 5: {
  	          socket_device->recv(telemetr_request,
                                            program_settings::MODBUS_FUNCTION_5_REQUEST_PARTIAL_SIZE);
             break; };
             case 6: {
  	          socket_device->recv(telemetr_request,
                                            program_settings::MODBUS_FUNCTION_6_REQUEST_PARTIAL_SIZE);
             break; };
             case 16: {
  	          socket_device->recv(telemetr_request,
                                            program_settings::MODBUS_FUNCTION_16_REQUEST_PARTIAL_SIZE);
              byte data_bytes_count=telemetr_request[telemetr_request.size()-1];
  	          socket_device->recv(telemetr_request,
                                            data_bytes_count);
             break; } ;
             default: 
              string message="In request to device ";
              vector<char> tmp_chars(20);
              itoa(telemetr_request[program_settings::MODBUS_DEVICE_NUMBER_INDEX], &tmp_chars[0], 10);
              message+=&tmp_chars[0];
              message+=" not supported function ";
              itoa(telemetr_request[program_settings::MODBUS_FUNCTION_CODE_INDEX], &tmp_chars[0], 10);
              message+=&tmp_chars[0];
              sett_obj->sys_message(program_settings::ERROR_MSG, message);
              failures_count++;
              telemetr_answer=metro_device::generate_error_answer_for_function ( 
                                             telemetr_request[program_settings::MODBUS_DEVICE_NUMBER_INDEX],
                                             telemetr_request[program_settings::MODBUS_FUNCTION_CODE_INDEX],
                                             program_settings::MODBUS_EXCEPTION_ILLEGAL_FUNCTION);
  		      socket_device->send(telemetr_answer);
              usleep(program_settings::SOCKET_USLEEP_BETWEEN_FAILURES);
              continue;
         };
	     socket_device->recv(telemetr_request, program_settings::MODBUS_CRC_SIZE);
         //CRC check
		 crc_value=sett_obj->crc(metro_device::command_data(
		                                                                   &telemetr_request[0],
		                                                                   &telemetr_request[telemetr_request.size()-program_settings::MODBUS_CRC_SIZE] ));

        telemetr_crc=program_settings::bytes_of_type<word>(crc_value);
		   if (telemetr_crc[1]!=telemetr_request[telemetr_request.size()-2] ||
		       telemetr_crc[0]!=telemetr_request[telemetr_request.size()-1] ) { 
		         throw socket_exception("Bad packet CRC");
                 };

          failures_count=0;
    } //try
	catch (socket_exception sock_exc){
	   string message("modbus_srv  socket_thread_function (... ) Fail reciving data from telemetry : ");	
	   message+=sock_exc.get_description();
       sett_obj->sys_message(program_settings::ERROR_MSG, message);
       failures_count++;
       usleep(program_settings::SOCKET_USLEEP_BETWEEN_FAILURES);
       continue;
	} //catch (socket_exception

    dev_iter = metro_devices.find(telemetr_request[program_settings::MODBUS_DEVICE_NUMBER_INDEX]);
	if (dev_iter==metro_devices.end()) {
	   ostringstream exception_descripton;
	   exception_descripton<<"modbus_srv socket_thread_function (... ) Not found device with number "<<
                                      static_cast<int>(telemetr_request[program_settings::MODBUS_DEVICE_NUMBER_INDEX]);
       sett_obj->sys_message(program_settings::ERROR_MSG, exception_descripton.str());

        //if device not found - generate error message for function
        telemetr_answer=metro_device::generate_error_answer_for_function ( 
                                             telemetr_request[program_settings::MODBUS_DEVICE_NUMBER_INDEX],
                                             telemetr_request[program_settings::MODBUS_FUNCTION_CODE_INDEX],
                                             program_settings::MODBUS_EXCEPTION_FAILURE_IN_ASSOCIATED_DEVICE);

	} else { //if (dev_iter==metro_devices
            pthread_mutex_lock(dev_iter->second->get_sockets_to_device_queue_mutex());
            dev_iter->second->put_command_request_from_socket(telemetr_request);
                if (dev_iter->second->get_command_request_to_comport_buffer()!=
                    dev_iter->second->get_default_command_request_to_comport()) {
	                         pthread_mutex_lock(dev_iter->second->get_data_transfer_process_mutex());
                             pthread_cond_wait(dev_iter->second->get_data_transfer_process_cond_var(),
                                                            dev_iter->second->get_data_transfer_process_mutex());
                             pthread_mutex_unlock(dev_iter->second->get_data_transfer_process_mutex());
                        } else {
                               dev_iter->second->set_command_answer_to_socket_buffer(
                                                     dev_iter->second->get_default_answer_to_socket_buffer());
                        };
            telemetr_answer=dev_iter->second->get_command_answer_to_socket_buffer();
	        pthread_mutex_unlock(dev_iter->second->get_sockets_to_device_queue_mutex());
      }; //else for if (dev_iter==metro_devices

	try {

//=========telemetr_answer===========================
/*
if (dev_iter->second->get_number()==2) {
if (telemetr_answer.empty()) {
      cout<<"telemetr_answer is EMPTY"<<endl;
 } else { //if (telemetr_answer.empty())
 cout<<"telemetr_answer tid "<<pthread_self()<<" :"<<endl;
vector<char> tmp_buffer(32);
for (int i=0;
       i<telemetr_answer.size() && i<6;
//       i<4; //only header
       i++) {
      itoa(telemetr_answer[i],
              &tmp_buffer[0], 16);
      if (i==3 ||
          (i-3)%8==0) cout<<endl;
      cout<<"\t0x"<<&tmp_buffer[0];
    }; //for (int  i=0;
    cout<<endl;
    cout<<endl;
};//if (telemetr_answer.empty())
};
*/
//============================================

		 socket_device->send(telemetr_answer);
         failures_count=0;
	}	catch (socket_exception sock_exc){
	   string message("modbus_srv  socket_thread_function (... ) Fail reciving data from telemetry : ");	
	   message+=sock_exc.get_description();
       sett_obj->sys_message(program_settings::ERROR_MSG, message);
       failures_count++;
       usleep(program_settings::SOCKET_USLEEP_BETWEEN_FAILURES);
       continue;
	}; //catch (socket_exception
	
	}; //while(failures_count<program_settings::SOCKET_FAILURES_COUNT)

    delete(socket_device);
    return 0;
};

void* comport_thread_function(void* comport_ptr){
	if (comport_ptr==NULL) return 0;

	comport *comport_device=static_cast<comport*>(comport_ptr);
    program_settings *sett_obj=program_settings::get_instance();

    devices_container::iterator dev_iter = metro_devices.begin();
    metro_device::command_data_container
                data_to_comport,
                data_from_comport;

	metro_device::command_data device_answer;

    metro_device::command_data::iterator tmp_iter;
    byte data_bytes_count;
    word crc_value;
    program_settings::bytes device_crc;
    if (metro_devices.empty()) return 0;

    while (true) {
         if (dev_iter == metro_devices.end())  
                  dev_iter = metro_devices.begin();

        data_from_comport.clear();
        pthread_mutex_lock(dev_iter->second->get_request_from_socket_mutex());
        data_to_comport=dev_iter->second->get_command_request_to_comport();
		for (metro_device::command_data_container::size_type
                                               i=0;i<data_to_comport.size();i++) {
			try {

//=========device_request===========================
//{
// cout<<"data_to_comport :"<<endl;
//vector<char> tmp_buffer(32);
//for (int j=0;
////       j<device_answer_header.size();
//       j<9; //only header
//       j++) {
//      itoa(data_to_comport[i][j],
//             &tmp_buffer[0], 16);
//      if (j==3 ||
//          (j-3)%8==0) cout<<endl;
//      cout<<"\t0x"<<&tmp_buffer[0];
//    }; //for (int  i=0;
//    cout<<endl;
//    cout<<endl;
//}
//============================================

              comport_device->set_rts_state(comport::RTS_SET_1);	
              usleep(50);
              comport_device->send(data_to_comport[i]);
              comport_device->set_rts_state(comport::RTS_SET_0);	
              usleep(15);

                device_answer.clear();
   				comport_device->recv(device_answer, program_settings::MODBUS_REQUEST_HEADER_SIZE, true);

   				if ((device_answer[program_settings::MODBUS_FUNCTION_CODE_INDEX]&0x80)
   				     !=0) {
   				          //answer from device  -  error code
         				comport_device->recv(device_answer, program_settings::MODBUS_CRC_SIZE);
                   } else {
                       switch (device_answer[program_settings::MODBUS_FUNCTION_CODE_INDEX]) {
                           case 4:
                   				comport_device->recv(device_answer, program_settings::MODBUS_FUNCTION_4_ANSWER_PARTIAL_SIZE);
                                data_bytes_count=device_answer[program_settings::MODBUS_DATA_BYTES_COUNT_INDEX]+program_settings::MODBUS_CRC_SIZE;
                                comport_device->recv(device_answer, data_bytes_count);
                                break;
                           case 5:
                   				comport_device->recv(device_answer, program_settings::MODBUS_FUNCTION_5_ANSWER_PARTIAL_SIZE+program_settings::MODBUS_CRC_SIZE);
                                break;
                           case 6:
                   				comport_device->recv(device_answer, program_settings::MODBUS_FUNCTION_6_ANSWER_PARTIAL_SIZE+program_settings::MODBUS_CRC_SIZE);
                                break;
                           case 16:
                   				comport_device->recv(device_answer, program_settings::MODBUS_FUNCTION_16_ANSWER_PARTIAL_SIZE+program_settings::MODBUS_CRC_SIZE);
                                break;
                             default:
                                throw comport_exception("Unsupported command from comport");
                        }; //switch (device_answer[program_settings::MODBUS_FUNCTION_CODE_INDEX])
                   }; // if ((device_answer_header[program_settings::MODBUS_FUNCTION_CODE_INDEX]&0x80)

                      //CRC check
                      tmp_iter=device_answer.begin();
                      advance(tmp_iter, device_answer.size()-program_settings::MODBUS_CRC_SIZE);   
                      crc_value=sett_obj->crc(metro_device::command_data(
		                                                                   device_answer.begin(),
		                                                                   tmp_iter));
                       device_crc=program_settings::bytes_of_type<word>(crc_value);
                       if (device_crc[1]!=*(tmp_iter) ||
                            device_crc[0]!=*(++tmp_iter) ) {
                               cout<<"Packet with bad CRC for device "<<dev_iter->second->get_number()<<endl;
                                vector<char> tmp_chars(32);
                               for (metro_device::command_data::size_type
                                                                i =0; i<device_answer.size();i++) {
                                  itoa(device_answer[i], &tmp_chars[0], 16);
                                  cout<<" 0x"<<&tmp_chars[0];
                               };
                                cout<<endl;
                                throw comport_exception("Bad packet CRC");
                             }
//=========device_answer===========================
//{
// cout<<"device_answer after crc :"<<endl;
//vector<char> tmp_buffer(32);
//for (int i=0;
//       i<device_answer.size();
////       i<9; //only header
//       i++) {
//      itoa(device_answer[i],
//              &tmp_buffer[0], 16);
//      if (i==3 ||
//          (i-3)%8==0) cout<<endl;
//      cout<<"\t0x"<<&tmp_buffer[0];
//    }; //for (int  i=0;
//    cout<<endl;
//    cout<<endl;
//}
//============================================
                    data_from_comport.push_back(device_answer);
		    } catch (comport_exception cmp_exc) {
		          ostringstream exceptioin_message;
		          exceptioin_message<<"modbus_srv  comport_thread_function (... ) Fail reciving data from device ";
		          exceptioin_message<<dev_iter->second->get_number()<<" : "<<cmp_exc.get_description();
                  sett_obj->sys_message(program_settings::ERROR_MSG, exceptioin_message.str());
		    }; //catch (comport_exception cmp_exc
		   usleep(program_settings::COMPORT_USLEEP_BETWEEN_COMMANDS);
		}; //for (int i=0;i<data_to_compo

         dev_iter->second->put_command_answer_from_comport(data_from_comport);
       if (dev_iter->second->get_command_request_to_comport_buffer()!=
	        dev_iter->second->get_default_command_request_to_comport()) {
                   pthread_mutex_lock(dev_iter->second->get_data_transfer_process_mutex());
                         pthread_cond_signal(dev_iter->second->get_data_transfer_process_cond_var());
                    pthread_mutex_unlock(dev_iter->second->get_data_transfer_process_mutex());
              };

         pthread_mutex_unlock(dev_iter->second->get_request_from_socket_mutex());

		 usleep(program_settings::COMPORT_USLEEP_BETWEEN_COMMANDS);
         dev_iter++; 
    }; //while(true)

      return 0;
};

void* routing_thread_function
          (void* arg) {
    router& router_inst=router::get_instance();

    router::routes_iterator routes_iter;
    router::gateways_iterator gateways_iter;

    bool connect_to_test_host_established;
    cout<<"router_inst.size_gateway()  "<<static_cast<int>(router_inst.size_gateway())<<endl;
    if (router_inst.size_gateway()<2) {
            program_settings *sys_sett=program_settings::get_instance();
            if (sys_sett==NULL) 
                    cout<<"routing_thread: router_inst.gateways_size()<2"<<endl;
                   else 
                     sys_sett->sys_message(program_settings::ERROR_MSG, "routing_thread: router_inst.gateways_size()<2");
             return NULL;
           };

    if (router_inst.empty_routes()) {
            program_settings *sys_sett=program_settings::get_instance();
	            if (sys_sett==NULL) 
                    cout<<"routing_thread: routes is empty"<<endl;
                   else 
                     sys_sett->sys_message(program_settings::ERROR_MSG, "routing_thread: routes is empty");
              return NULL;
           };

	while (true) {
         routes_iter=router_inst.begin_routes();
         while (routes_iter!=router_inst.end_routes()){
                 cout<<"route"<<endl;
                 gateways_iter=router_inst.begin_gateway();
                while(gateways_iter!=router_inst.end_gateway() ) {
                           cout<<"gateway"<<endl;
                            //test all connections in previos gateway
                            connect_to_test_host_established=false;
                             router::route::test_hosts_iterator test_hosts_iter=routes_iter->test_hosts.begin();
                             while (test_hosts_iter!=routes_iter->test_hosts.end()) {
                                  cout<<"test hosts 1"<<endl;
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
                                    cout<<"test hosts 2"<<endl;
                                  if (router_inst.test_connection_to_test_host(*test_hosts_iter)) {
                                          connect_to_test_host_established=true;
                                           break;
                                       };
                                  test_hosts_iter++;
                             }; //while (test_hosts_iter!=routes_iter->test_hosts.end())

                           if (connect_to_test_host_established) break;
                      gateways_iter++;
                  }; //while(gateways_iter!=router_inst.end_gateways() &&
                routes_iter++;
                sleep(5);
          };//while (routes_iter!=router_inst.end_routes()
     }; //while (true)
};

void load_settings() throw (metro_device::metro_device_exception) {
 	enum {WELCOME=0, COMPORT_KOEFF, BAUD_RATE, ENTRIES_COUNT};

ostringstream exception_description;
string entry_name;
const char *entry_name_c_str;
vector<char> temp_str(512);
vector<string> entries_names(ENTRIES_COUNT);

entries_names[WELCOME]="welcome";
entries_names[COMPORT_KOEFF]="comport koeff";
entries_names[BAUD_RATE]="baud rate";

entry_name_c_str=PxConfigNextString(&temp_str[0], 
                                                                   temp_str.size() );

while(entry_name_c_str!=NULL){
entry_name=entry_name_c_str;
   if (entry_name.compare(entries_names[WELCOME])==0) {
   			welcome_message+="\n";
			welcome_message+=&temp_str[0];
  } else  if (entry_name.compare(entries_names[COMPORT_KOEFF])==0) {
          comport_delay_koeff=strtod(&temp_str[0], NULL); 
          if (comport_delay_koeff==0) {
                   exception_description<<"Uncorrect value  "<<&temp_str[0]<<" comport_koeff setted to 0";
                   throw metro_device::metro_device_exception(exception_description.str());
               };
  } else  if (entry_name.compare(entries_names[BAUD_RATE])==0) {
          baud_rate=atoi(&temp_str[0]);
          if (baud_rate!=comport::BAUD_9600 &&
               baud_rate!=comport::BAUD_19200 &&
               baud_rate!=comport::BAUD_115200){
                   exception_description<<"Uncorrect baud rate value  "<<&temp_str[0]<<" now supported only 9600, 19200 and 115200";
                   throw metro_device::metro_device_exception(exception_description.str());
               };
 	} else {
           exception_description<<"Unrecognized config entry  "<<entry_name;
           throw metro_device::metro_device_exception(exception_description.str());
	};
	
	entry_name_c_str=PxConfigNextString(&temp_str[0], 
							                        temp_str.size()
							                      );
	};
	
};


void load_metro_devices( section_type dev_type) throw (metro_device::metro_device_exception) {
 	enum {NUMBER=0, ENTRIES_COUNT};
ostringstream exception_description;
string entry_name;
vector<char> temp_str(512);
const char *entry_name_c_str;
vector<string> entries_names(ENTRIES_COUNT);
int number(-1);

entries_names[NUMBER]="number";

entry_name_c_str=PxConfigNextString(&temp_str[0], 
							                        temp_str.size()
							                      );

while(entry_name_c_str!=NULL){
entry_name=entry_name_c_str;
if (entry_name.compare(entries_names[NUMBER])==0) {
			int temp_int = atoi(&temp_str[0]);
			if (temp_int<=0){
						exception_description<<"Wrong device number "<<temp_int;
						throw metro_device::metro_device_exception(exception_description.str());
					};
              number=temp_int;
	} else {
           exception_description<<"Unrecognized config entry  "<<entry_name;
           throw metro_device::metro_device_exception(exception_description.str());
	};
	
	entry_name_c_str=PxConfigNextString(&temp_str[0], 
							                        temp_str.size()
							                      );
	};

	if (number>0 ) {
           switch (dev_type) {
                  case ESCALATOR : {
                     metro_devices.insert ( metro_devices.upper_bound(number),
                                                    devices_container::value_type 
                                                                         (number,
                                                                         new metro_escalator(number)  )
                                                                );
                     break; 
                  };
                  case SHAVR : {
                     metro_devices.insert ( metro_devices.upper_bound(number),
                                                    devices_container::value_type 
                                                                         (number,
                                                                         new metro_shavr(number)  )
                                                                );

                     break; 
                  };
                  case UDKU : {
                     metro_devices.insert ( metro_devices.upper_bound(number),
                                                    devices_container::value_type 
                                                                         (number,
                                                                         new metro_udku(number)  )
                                                                );
                     break;
                  };
                 default: {
                       exception_description<<"Unrecognized metro devices type   "<<dev_type;
                       throw metro_device::metro_device_exception(exception_description.str());
                 };
            };// switch (dev_type)
	
	} else {
       exception_description<<"Not found all of required entries for metro device adding";
       throw metro_device::metro_device_exception(exception_description.str());
	};
};

void load_configuration  (string file_name) 
                  throw (metro_device::metro_device_exception) {

	ostringstream exception_description;
	string	section_name;
	const char *section_name_c_str;
	vector<string> sections_names(SECTIONS_COUNT);
    router& router_inst=router::get_instance();
    watchdog_timer& w_timer_inst=watchdog_timer::get_instance();

	sections_names[ESCALATOR]="escalator";
	sections_names[SHAVR]="shavr";
	sections_names[UDKU]="udku";
	sections_names[ROUTE]="route";
	sections_names[ROUTING]="routing";
	sections_names[WATCHDOG_TIMER]="watchdog timer";
	sections_names[SETTINGS]="settings";	
	if (PxConfigOpen( file_name.c_str(), PXCONFIG_READ)!=Pt_TRUE ){
        exception_description<<"Can`t open config file "<<file_name;
		throw metro_device::metro_device_exception(exception_description.str());
	};

     try {
                    section_name_c_str=PxConfigNextSection();
                    while (section_name_c_str!=NULL) {
                       section_name=section_name_c_str;

                       if (section_name.compare(sections_names[ESCALATOR])==0) {
                               load_metro_devices(ESCALATOR);
                       } else if (section_name.compare(sections_names[SHAVR])==0) {
                               load_metro_devices(SHAVR);
                       } else if (section_name.compare(sections_names[UDKU])==0) {
                               load_metro_devices(UDKU);
                       } else if (section_name.compare(sections_names[ROUTING])==0) {
                               router_inst.load_routing();
                       } else if (section_name.compare(sections_names[ROUTE])==0) {
                               router_inst.load_route();
                       } else if (section_name.compare(sections_names[WATCHDOG_TIMER])==0) {
                               w_timer_inst.load_watchdog();
                       } else if (section_name.compare(sections_names[SETTINGS])==0) {
                               load_settings();
                        } else {
                           exception_description<<"Unrecognized config section "<<section_name;
                           throw metro_device::metro_device_exception(exception_description.str());
                        };
                      section_name_c_str=PxConfigNextSection();
                    };
	}catch  (metro_device::metro_device_exception dev_exc) {
           exception_description<<dev_exc.get_description();
           if  (PxConfigClose()!=Pt_TRUE) {
               exception_description<<"\nCan`t close file "<<file_name;
           };
           throw metro_device::metro_device_exception(exception_description.str());
 	};

     if  (PxConfigClose()!=Pt_TRUE) {
        exception_description<<"Can`t close file "<<file_name;
        throw metro_device::metro_device_exception(exception_description.str());
      };

};

string usage(string program_name) {
ostringstream usage_descr;
 usage_descr<<"Usage :\n"
         <<program_name<<" configuration_file\n"
         <<"\twhere :\n"
         <<"\t\tconfiguration_file file with configuration settings \n"
         <<"\t\t\tby example /etc/modbus_srv.conf\n"
         <<endl;

return usage_descr.str();
};


int main(int argc, const char *argv[]){
    	server_socket *socket_device;
        generic_socket *accepted_socket=NULL; //make compiler happy :)
	pthread_attr_t      attr;
	struct timeval recv_send_timeout;
    program_settings* sett_obj=program_settings::get_instance();

	int pthread_creating_result;

   if (argc!=2) {
         string usage_description=usage(argv[0]);
          cout<<usage_description<<endl;
          sett_obj->sys_message(program_settings::ERROR_MSG, usage_description);
	      return 0;
      };

      router& router_inst=router::get_instance();     
      watchdog_timer&  watchdog_inst=watchdog_timer::get_instance();     

    try {
        load_configuration  (argv[1]);
	}catch  (metro_device::metro_device_exception dev_exc) {
	     sett_obj->sys_message(program_settings::ERROR_MSG, dev_exc.get_description());
	      return 0;
    };


   if (!welcome_message.empty())
	cout<<"\n================================================================="
           <<welcome_message
           <<"\n================================================================="<<endl;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

   recv_send_timeout.tv_sec=program_settings::RECV_SEND_TIMEOUT;
   recv_send_timeout.tv_usec=0;

   signal( SIGPIPE, SIG_IGN );


	try {
	comport_device=new comport(program_settings::MODBUS_COM_PORT,
                                       baud_rate,
                                       comport::PARITY_DISABLE,
                                       comport::DATA_BITS_8,
                                       comport::STOP_BITS_1,
                                       0,
                                       comport_delay_koeff);

	} catch (comport_exception cmp_exc) {
	     sett_obj->sys_message(program_settings::ERROR_MSG, cmp_exc.get_description());
	      return 0;
   };

	try {
		pthread_creating_result=pthread_create(NULL, &attr, &comport_thread_function, comport_device);
		if ( pthread_creating_result!= EOK){
			   string message("fail to create device thread : ");	
			   message+=strerror(pthread_creating_result);
  			   throw comport_exception(message);
		};
	} catch (comport_exception cmp_exc) {
	     sett_obj->sys_message(program_settings::ERROR_MSG, cmp_exc.get_description());
	     delete(comport_device);
	      return 0;
   };


  if (router_inst.is_routing_enabled()) {
		     pthread_creating_result=pthread_create(NULL, &attr, &routing_thread_function, NULL);
 		     if ( pthread_creating_result!= EOK){
	           sett_obj->sys_message(program_settings::ERROR_MSG, "Fail to create routing_thread ");
	           delete(comport_device);
	           return 0;
	       	};
    };

  if (watchdog_inst.is_watchdog_enabled()) {
		     pthread_creating_result=pthread_create(NULL, &attr, &watchdog_thread_function, NULL);
 		     if ( pthread_creating_result!= EOK){
	           sett_obj->sys_message(program_settings::ERROR_MSG, "Fail to create watchdog_thread ");
	           delete(comport_device);
	           return 0;
	       	};
    };


	try {
		socket_device = new  server_socket(recv_send_timeout,
                                                        program_settings::BACKLOG,
                                                        program_settings::MODBUS_TCP_PORT);
        socket_device->initialize();
	} catch (socket_exception sock_exc){
	     sett_obj->sys_message(program_settings::ERROR_MSG, sock_exc.get_description());
	     delete (comport_device);
	     return 0;
	};

	while(true){
		try {
		    accepted_socket=socket_device->accept();
		} catch (socket_exception sock_exc){
              sett_obj->sys_message(program_settings::ERROR_MSG, sock_exc.get_description());
	    };

        pthread_creating_result=pthread_create(NULL, &attr, &socket_thread_function, accepted_socket);
		if ( pthread_creating_result != EOK){
			   string message("fail to create device thread : ");	
			   message+=strerror(pthread_creating_result);
               sett_obj->sys_message(program_settings::ERROR_MSG, message);
		};

	}; // while(true)

	return 0;
}

