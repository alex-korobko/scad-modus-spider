#include <iostream.h>

#include <vector>
#include <string>
#include <map>
#include <functional>
#include <sstream>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

using namespace std;

#include "defines.h"
#include <sys/select.h> //#define FD_SETSIZE
#include "socket_exception.h"
#include "generic_socket.h"

#include "client_socket.h"

client_socket::client_socket (
                 in_addr_t host, 
                 int port,
                 struct timeval connect_timeout,
                 struct timeval recv_send_timeout) :
           generic_socket(recv_send_timeout),
           m_host(host),
           m_port(port),
           m_connect_timeout(connect_timeout){};

void client_socket::initialize() throw (socket_exception){
    	int flags, status;
	
	if (! generic_socket::create()){
			throw socket_exception("client_socket::initialize() : generic_socket::create()==false");
		};

	if (! generic_socket::is_valid()) {
			throw socket_exception("client_socket::initialize() : socket is not valid");
		};

	if ((flags = fcntl(m_sock, F_GETFL, 0)) < 0) {
	   ostringstream exception_description;	
	   exception_description<<"client_socket::initialize() fail to get file status flags error : "<<strerror(errno);
	   throw socket_exception(exception_description.str());
	};

	if (fcntl(m_sock, F_SETFL, flags | O_NONBLOCK) < 0){
	   ostringstream exception_description;	
	   exception_description<<"client_socket::initialize() fail to set file status flags error : "<<strerror(errno);
	   throw socket_exception(exception_description.str());
	};

    m_addr.sin_addr.s_addr = m_host;
    m_addr.sin_port = htons(m_port);
    m_addr.sin_family = AF_INET;
     
    status=::connect (m_sock,
                    (sockaddr*) &m_addr,
                    sizeof (m_addr));

	if (status!=0 &&
	    errno!=EINPROGRESS) {
           	ostringstream exception_description;	
			exception_description<<"client_socket::initialize() connect(...) error : "<<strerror(errno);
			throw socket_exception (exception_description.str());
    };

	fd_set	readFD, writeFD, exFD;

    if (FD_SETSIZE<=m_sock) {
           ostringstream exception_description;
           exception_description<<"client_socket::initialize()  : FD_SETSIZE<=m_sock FD_SETSIZE "<<FD_SETSIZE<<" m_sock "<<m_sock;
		   throw socket_exception (exception_description.str());
        };

	FD_ZERO(&readFD);
	FD_SET(m_sock, &readFD);

	FD_ZERO(&writeFD);
	FD_SET(m_sock, &writeFD);

	FD_ZERO(&exFD);
	FD_SET(m_sock, &exFD);

	int result = ::select(m_sock + 1,
	                            &readFD,
	                            &writeFD,
	                            &exFD,
                                &m_connect_timeout);

	switch (result) {
        case -1: {
            ostringstream exception_description;
            exception_description<<"client_socket::initialize() fail to select socket after connect : "<<strerror(errno);
            throw socket_exception(exception_description.str());
        };

        case 0: {
           ostringstream exception_description;	
           exception_description<<"client_socket::initialize() connect timeout expected";
           throw socket_exception(exception_description.str());
        };

        default: { //connection established, but must be checked
              	int err=0;
				socklen_t length  = sizeof(err);

                  if (getsockopt(m_sock, SOL_SOCKET, SO_ERROR, &err, &length) < 0) {
                     ostringstream exception_description;
                     exception_description<<"client_socket::initialize() can`t get socket state : "<<strerror(errno);
                     throw socket_exception(exception_description.str());
                  };

                  if (err!=0) {
                      ostringstream exception_description;	
                      exception_description<<"client_socket::initialize() connection error : "<<strerror(err);
                      throw socket_exception(exception_description.str());           
                  };

               if (FD_ISSET(m_sock, &exFD)!=0)
                      throw socket_exception("client_socket::initialize() connection returned descriptor have an exceptional condition pending.");

               if (FD_ISSET(m_sock, &readFD)==0 &&
                    FD_ISSET(m_sock, &writeFD)==0)
                      throw socket_exception("client_socket::initialize() connection return bad read and write descriptor");

        }; //default:
	}; //switch

};

