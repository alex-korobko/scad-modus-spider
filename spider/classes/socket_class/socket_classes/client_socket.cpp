#include <iostream.h>

#include <vector>
#include <string>
#include <map>
#include <functional>
#include <sstream>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/syspage.h>
#include <sys/neutrino.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

using namespace std;

#include "defines.h"
#include "socket_exception.h"
#include "generic_socket.h"

#include "client_socket.h"

client_socket::client_socket (in_addr_t host, 
                          int port,
						  uint64_t connect_timeout_nsec,
						  uint64_t recv_send_timeout_nsec) :
           generic_socket(recv_send_timeout_nsec),
           host(host),
           port(port),
           connect_timeout_nseconds(connect_timeout_nsec){};

void client_socket::initialize() throw (socket_exception){
    	int status;
       	int err=0;

 		socklen_t length  = sizeof(err);

	if (! generic_socket::create()){
			throw socket_exception("client_socket::initialize() : generic_socket::create()==false");
		};

	if (! generic_socket::is_valid()) {
			throw socket_exception("client_socket::initialize() : socket is not valid");
		};

    sock_addr.sin_addr.s_addr = host;
    sock_addr.sin_port = htons(port);
    sock_addr.sin_family = AF_INET;

    TimerTimeout(CLOCK_REALTIME,_NTO_TIMEOUT_REPLY,
                            NULL, &connect_timeout_nseconds, NULL);

    status=::connect (sock,
                    (sockaddr*) &sock_addr,
                    sizeof (sock_addr));

	if (status!=0) {
           	ostringstream exception_description;	
			exception_description<<"client_socket::initialize() connect(...) error : "<<strerror(errno);
			throw socket_exception (exception_description.str());
    };

 if (getsockopt(sock, SOL_SOCKET, SO_ERROR, &err, &length) < 0) {
           ostringstream exception_description;
            exception_description<<"client_socket::initialize() can`t get socket state : "<<strerror(errno);
            throw socket_exception(exception_description.str());
      };

  if (err!=0) {
          ostringstream exception_description;	
          exception_description<<"client_socket::initialize() connection error : "<<strerror(err);
          throw socket_exception(exception_description.str());           
     };

};

