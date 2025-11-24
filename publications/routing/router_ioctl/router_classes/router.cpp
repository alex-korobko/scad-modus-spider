#include <sys/types.h>
#include <inttypes.h>
#include <sys/neutrino.h>

#include <sys/select.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/route.h>
#include <string.h>

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>

#include <Ap.h>
#include <Pt.h>

using namespace std;

#include "generic_socket.h"
#include "client_socket.h"

#include "router.h"

router::router() throw (runtime_error)
    :routing_enabled(false){

  	if ((sock=socket(AF_INET, SOCK_DGRAM, 0))==-1) {
        throw runtime_error(string("In router::router() socket(AF_INET, SOCK_DGRAM,...) error: ")+
                                      strerror(errno));
   };
};

router::~router(){
  	shutdown (sock, 0);
};


bool router::test_connection_to_test_host(struct in_addr test_host)
 {
     struct timeval connection_timeout, recive_send_timeout;

     generic_socket::bytes request_to_server(1);
     generic_socket::bytes answer_from_server;
     answer_from_server.reserve(request_to_server.size());

    connection_timeout.tv_sec=router::CONNECT_TO_DEVICE_TIMEOUT;
    connection_timeout.tv_usec=0;

   recive_send_timeout.tv_sec=router::RECIEVE_SEND_TO_DEVICE_TIMEOUT;
   	recive_send_timeout.tv_usec=0;
    request_to_server[0]=0xff;

      try{
       client_socket  socket_dev(test_host.s_addr,
                                                  router::ECHO_TCP_PORT,
                                                  connection_timeout,
                                                  recive_send_timeout);
         socket_dev.initialize();
         socket_dev.send(request_to_server);
         answer_from_server.clear();
         socket_dev.recv(answer_from_server, request_to_server.size());
         if (answer_from_server!=request_to_server) return false;
      } catch (runtime_error sock_exc) {
            cout<<"In router::test_connection_to_test_host raised exception: "<<sock_exc.what()<<endl;
            return false;
      };
        return true;
};

void router::add_route (struct in_addr destination, 
                               struct in_addr mask,
                               struct in_addr gateway,
                               int flags) {
struct ortentry route_entry;
struct sockaddr_in dest_addr, gate_addr, mask_addr;

memset(&route_entry, 0, sizeof(route_entry));

memset(&dest_addr, 0, sizeof(dest_addr));
dest_addr.sin_family = AF_INET;
dest_addr.sin_len = sizeof (struct sockaddr_in);
dest_addr.sin_addr = destination;

memset(&gate_addr, 0, sizeof(gate_addr));
gate_addr.sin_family = AF_INET;
gate_addr.sin_len = sizeof (struct sockaddr_in);
gate_addr.sin_addr = gateway;

memset(&mask_addr, 0, sizeof(mask_addr));
mask_addr.sin_family = AF_INET;
mask_addr.sin_len = sizeof (struct sockaddr_in);
mask_addr.sin_addr = mask;

memcpy (&route_entry.rt_dst, &dest_addr, sizeof(dest_addr));
memcpy (&route_entry.rt_gateway, &gate_addr, sizeof(gate_addr));
memcpy (&route_entry.rt_netmask, &mask_addr, sizeof(mask_addr));

route_entry.rt_flags |= (RTF_GATEWAY | RTF_UP | RTF_STATIC);
route_entry.rt_flags |= flags;

if (ioctl(sock,SIOCADDRT, &route_entry)==-1)	
   cout<<"In router::add_route :\n"<<strerror(errno)<<endl;
};


void router::delete_route (struct in_addr destination, 
                               struct in_addr mask,
                               struct in_addr gateway,
                               int flags){
struct ortentry route_entry;
struct sockaddr_in dest_addr, gate_addr, mask_addr;

memset(&route_entry, 0, sizeof(route_entry));

memset(&dest_addr, 0, sizeof(dest_addr));
dest_addr.sin_family = AF_INET;
dest_addr.sin_len = sizeof (struct sockaddr_in);
dest_addr.sin_addr = destination;

memset(&gate_addr, 0, sizeof(gate_addr));
gate_addr.sin_family = AF_INET;
gate_addr.sin_len = sizeof (struct sockaddr_in);
gate_addr.sin_addr = gateway;

memset(&mask_addr, 0, sizeof(mask_addr));
mask_addr.sin_family = AF_INET;
mask_addr.sin_len = sizeof (struct sockaddr_in);
mask_addr.sin_addr = mask;

memcpy (&route_entry.rt_dst, &dest_addr, sizeof(dest_addr));
memcpy (&route_entry.rt_gateway, &gate_addr, sizeof(gate_addr));
memcpy (&route_entry.rt_netmask, &mask_addr, sizeof(mask_addr));

route_entry.rt_flags |= (RTF_GATEWAY | RTF_UP | RTF_STATIC);
route_entry.rt_flags |= flags;

if (ioctl(sock,SIOCDELRT, &route_entry)==-1)	
   cout<<"In router::delete_route :\n"<<strerror(errno)<<endl;
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

