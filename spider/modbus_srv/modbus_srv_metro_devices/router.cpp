#include <sys/types.h>
#include <inttypes.h>
#include <sys/neutrino.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>

#include <sys/select.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream.h>
#include <sys/socket.h>
#include <net/if.h>
#include <net/route.h>

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <sstream>

#include <Ap.h>
#include <Pt.h>

using namespace std;

#include "defines.h"
#include "program_settings.h"

#include "socket_exception.h"
#include "generic_socket.h"
#include "client_socket.h"

#include "router.h"


router::router() throw (runtime_error)
    :routing_enabled(false){
    if((sock = socket(AF_ROUTE, SOCK_RAW, AF_UNSPEC)) == -1) {
           throw runtime_error(string("In router::router() socket(PF_ROUTE, ...) error: ")+
                                               strerror(errno));
        };
};

router::~router(){
  	shutdown (sock, 2);
    close(sock);
};


bool router::test_connection_to_test_host(struct in_addr test_host) {
     program_settings::bytes request_to_server(1);
     program_settings::bytes answer_from_server;
     answer_from_server.reserve(request_to_server.size());
    request_to_server[0]=0xff;

   uint64_t connect_to_test_host_timeout_nanosec(program_settings::CONNECT_TO_DEVICE_TIMEOUT),
          recieve_send_to_test_host_timeout_nanosec(program_settings::RECIEVE_SEND_TO_DEVICE_TIMEOUT);
   connect_to_test_host_timeout_nanosec*=1000000000;
   recieve_send_to_test_host_timeout_nanosec*=1000000000;


      try{
       client_socket  socket_dev(test_host.s_addr,
                                                  program_settings::ECHO_TCP_PORT,
                                                  connect_to_test_host_timeout_nanosec,
                                                  recieve_send_to_test_host_timeout_nanosec);
         socket_dev.initialize();
         socket_dev.send(request_to_server);
         answer_from_server.clear();
         socket_dev.recv(answer_from_server, request_to_server.size());
         if (answer_from_server!=request_to_server) return false;
      } catch (socket_exception sock_exc) {
            program_settings *prog_sett=program_settings::get_instance();
            ostringstream except_descr;
            except_descr<<"In router::test_connection_to_test_host test host "<<inet_ntoa(test_host)<<
                                       " raised exception: "<<sock_exc.get_description()<<endl;
            prog_sett->sys_message(program_settings::ERROR_MSG, except_descr.str());
            return false;
      };
        return true;
};

void router::change_route (
                               struct in_addr destination,
                               struct in_addr mask,
                               struct in_addr gateway) throw (runtime_error) {
      struct rt_msghdr *rtm;
      struct sockaddr_in *dst, *gate, *msk;
      struct internal_route my_rt;

     //ATTENTION Only for test
//     return;

      program_settings *prog_sett=program_settings::get_instance();
       prog_sett->sys_message(program_settings::ERROR_MSG, "In router change route");

      memset(&my_rt, 0x00, sizeof(my_rt));

      rtm  = &my_rt.rt;
      dst  = &my_rt.dst;
      gate = &my_rt.gate;
      msk = &my_rt.mask;

      rtm->rtm_type = RTM_ADD;
      rtm->rtm_flags = RTF_UP | RTF_GATEWAY | RTF_STATIC;
      rtm->rtm_msglen = sizeof(my_rt);
      rtm->rtm_version = RTM_VERSION;
      rtm->rtm_seq = 1234;
      rtm->rtm_addrs = RTA_DST | RTA_GATEWAY | RTA_NETMASK;
      rtm->rtm_pid = getpid();

      dst->sin_len    = sizeof(sockaddr_in);
      dst->sin_family = AF_INET;
      dst->sin_addr=destination;

      msk->sin_len    = sizeof(sockaddr_in);
      msk->sin_family = AF_INET;
      msk->sin_addr=mask;

      gate->sin_len    = sizeof(sockaddr_in);
      gate->sin_family = AF_INET;
      gate->sin_addr=gateway;


      if(write(sock, rtm, rtm->rtm_msglen) < 0)
                if( errno == EEXIST) {
                       rtm->rtm_type = RTM_CHANGE;
                       if(write(sock, rtm, rtm->rtm_msglen) < 0)
                              throw runtime_error (string("In write(...) RTM_CHANGE error: ")
                                     +strerror(errno));
                  } else {
                      throw runtime_error (string("In write(...) RTM_ADD error: ")
                                     +strerror(errno));
                  };
};


void router::load_route() throw (runtime_error) {
 	enum {DESTINATION, MASK, TEST_HOST, ENTRIES_COUNT};

ostringstream exception_description;
string entry_name;
const char *entry_name_c_str;
vector<char> temp_str(512);
vector<string> entries_names(ENTRIES_COUNT);
router::route route_to_add;
in_addr net_device;
bool dest_present=false, mask_present=false, test_host_present=false;

entries_names[DESTINATION]="destination";
entries_names[MASK]="mask";
entries_names[TEST_HOST]="test host";

entry_name_c_str=PxConfigNextString(&temp_str[0],
                                                                   temp_str.size() );

while(entry_name_c_str!=NULL){
entry_name=entry_name_c_str;
 if (entry_name.compare(entries_names[DESTINATION])==0) {
          if (inet_aton( &temp_str[0],  &net_device )==1) {
                    dest_present=true;
                    route_to_add.destination=net_device;
                  } else {
                   exception_description<<"Uncorrect server address  "<<&temp_str[0]<<" now supported addresses in form xxx.xxx.xxx.xxx";
                   throw runtime_error(exception_description.str());
               };

 } else  if (entry_name.compare(entries_names[MASK])==0) {
          if (inet_aton( &temp_str[0],  &net_device )==1) {
                    mask_present=true;
                    route_to_add.mask=net_device;
                  } else {
                   exception_description<<"Uncorrect server address  "<<&temp_str[0]<<" now supported addresses in form xxx.xxx.xxx.xxx";
                   throw runtime_error(exception_description.str());
               };

 } else  if (entry_name.compare(entries_names[TEST_HOST])==0) {
          if (inet_aton( &temp_str[0],  &net_device )==1) {
                    test_host_present=true;
                    route_to_add.test_hosts.push_back(net_device);
                  } else {
                   exception_description<<"Uncorrect server address  "<<&temp_str[0]<<" now supported addresses in form xxx.xxx.xxx.xxx";
                   throw runtime_error(exception_description.str());
               };

	} else {
           exception_description<<"Unrecognized config entry  "<<entry_name;
           throw runtime_error(exception_description.str());
	};
	
	entry_name_c_str=PxConfigNextString(&temp_str[0], 
							                        temp_str.size()
							                      );
	};

if (dest_present &&
     mask_present &&
     test_host_present) {
        push_back_routes(route_to_add);
      } else  if (!dest_present){
           exception_description<<"In load_route() not present destination";
           throw runtime_error(exception_description.str());
      } else if (!mask_present) {
           exception_description<<"In load_route() not present mask";
           throw runtime_error(exception_description.str());
      } else if (!test_host_present) {
           exception_description<<"In load_route() not present test host";
           throw runtime_error(exception_description.str());
      };
};

void router::load_routing() throw (runtime_error) {
 enum {ROUTING_MANIPULATING=0, GATEWAY, ENTRIES_COUNT};

ostringstream exception_description;
string entry_name;
const char *entry_name_c_str;
vector<char> temp_str(512);
vector<string> entries_names(ENTRIES_COUNT);
in_addr net_device;

entries_names[ROUTING_MANIPULATING]="routing manipulating";
entries_names[GATEWAY]="gateway";

entry_name_c_str=PxConfigNextString(&temp_str[0], 
                                                                   temp_str.size() );

while(entry_name_c_str!=NULL){
entry_name=entry_name_c_str;
     if (entry_name.compare(entries_names[ROUTING_MANIPULATING])==0) {
          string value_example="enabled";
          if (value_example.compare(&temp_str[0])==0) {
                      routing_enabled=true;
               } else   { //if (value_example.compare(value_example)==0)
                     value_example="disabled";
                       if (value_example.compare(&temp_str[0])==0) {
                              routing_enabled=false;
                           } else { //if (value_example.compare(value_example)==0)
                              exception_description<<"Uncorrect routing mainpulaing value  "<<&temp_str[0]<<" supported only enabled, disabled";
                              throw runtime_error(exception_description.str());
                           }; //if (value_example.compare(value_example)==0)
               }; //if (value_example.compare(value_example)==0)

       } else if (entry_name.compare(entries_names[GATEWAY])==0) {
          if (inet_aton( &temp_str[0],  &net_device )==1) {
                      push_back_gateways(net_device);
                  } else { //if (inet_aton( &temp_str[0],  &net_device )
                   exception_description<<"Uncorrect gateway address  "<<&temp_str[0]<<" now supported addresses in form xxx.xxx.xxx.xxx";
                   throw runtime_error(exception_description.str());
               }; //if (inet_aton( &temp_str[0],  &net_device )

         } else {
            exception_description<<"Unrecognized config entry  "<<entry_name;
            throw runtime_error(exception_description.str());
         };

	entry_name_c_str=PxConfigNextString(&temp_str[0], 
							                        temp_str.size()
							                      );
	}; //while(entry_name_c_str!=NULL){
};


void router::load (string file_name) throw (runtime_error) {
	ostringstream exception_description;

	string	section_name;
	const char *section_name_c_str;
	vector<string> sections_names(SECTIONS_COUNT);

	sections_names[ROUTING]="routing";
	sections_names[ROUTE]="route";	
	if (PxConfigOpen( file_name.c_str(), PXCONFIG_READ)!=Pt_TRUE )
		throw runtime_error("Can`t open config file "+file_name);

     try {
                    section_name_c_str=PxConfigNextSection();
                    while (section_name_c_str!=NULL) {
                       section_name=section_name_c_str;

                      if (section_name.compare(sections_names[ROUTING])==0) {
                               load_routing();
                       } else if (section_name.compare(sections_names[ROUTE])==0) {
                               load_route();
                        } else {
                           throw runtime_error("Unrecognized config section "+section_name);
                        };
                      section_name_c_str=PxConfigNextSection();
                    };
	}catch  (runtime_error dev_exc) {
	          string message(dev_exc.what());
           if  (PxConfigClose()!=Pt_TRUE) 
               message+=("\nCan`t close file "+file_name);

           throw runtime_error(message);
 	};

     if  (PxConfigClose()!=Pt_TRUE) {
        exception_description<<"Can`t close file "<<file_name;
        throw runtime_error(exception_description.str());
      };
};

