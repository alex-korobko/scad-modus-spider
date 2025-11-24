#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

#include <vector>
#include <string>
#include <stdexcept>

using namespace std;

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

void client_socket::initialize() throw (runtime_error){
    	int flags, status;
	
	if (!generic_socket::create()) 	throw runtime_error("client_socket::initialize() : generic_socket::create()==false");
	if (!generic_socket::is_valid())			throw runtime_error("client_socket::initialize() : socket is not valid");

	if ((flags = fcntl(m_sock, F_GETFL, 0)) < 0)
	   throw runtime_error(string("client_socket::initialize() fail to get file status flags error: ")+strerror(errno));

	if (fcntl(m_sock, F_SETFL, flags | O_NONBLOCK) < 0)
		   throw runtime_error(string("client_socket::initialize() fail to set file status flags error: ")+strerror(errno));

    m_addr.sin_addr.s_addr = m_host;
    m_addr.sin_port = htons(m_port);
    m_addr.sin_family = AF_INET;
     
    status=::connect (m_sock,
                    (sockaddr*) &m_addr,
                    sizeof (m_addr));

	if (status!=0 &&
	    errno!=EINPROGRESS) 
  		   throw runtime_error(string("client_socket::initialize() connect(...) error: ")+strerror(errno));

	fd_set	readFD, writeFD, exFD;
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
        case -1:
       		   throw runtime_error(string("client_socket::initialize() fail to select socket after connect : ")+strerror(errno));
        case 0:
      		   throw runtime_error(string("client_socket::initialize() connect timeout expaired: ")+strerror(errno));
        default: //connection established, but must be checked
              int err;
              socklen_t length  = sizeof(err);
               if (!FD_ISSET(m_sock, &readFD) && 
                   !FD_ISSET(m_sock, &writeFD))
               		   throw runtime_error("client_socket::initialize() connection return bad read or write descriptor");

                  if (getsockopt(generic_socket::get_sock(), SOL_SOCKET, SO_ERROR, &err, &length) < 0)
                 		   throw runtime_error(string("client_socket::initialize() connection return bad read or write descriptor: ")+strerror(errno));

                  if (err!=0)
                		   throw runtime_error(string("client_socket::initialize() connection error: ")+strerror(errno));
	}; //switch

};

