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
#include <fcntl.h>
#include <pthread.h>
#include <sys/neutrino.h>
#include <inttypes.h>

using namespace std;

#include "defines.h"
#include "socket_exception.h"
#include "generic_socket.h"

#include "server_socket.h"

server_socket::server_socket ( uint64_t recv_send_timeout_nsec,
                              int new_backlog,
							  int new_port):
			generic_socket(recv_send_timeout_nsec),
			port(new_port),
			backlog(new_backlog){};

void server_socket::initialize() throw (socket_exception){
	if (! generic_socket::create())
			throw socket_exception("server_socket::initialize() : generic_socket::create()==false");

	if ( ! generic_socket::is_valid() )
		throw socket_exception("server_socket::initialize() : generic_socket don`t valid");

	sock_addr.sin_family = AF_INET;
	sock_addr.sin_addr.s_addr = INADDR_ANY;
	sock_addr.sin_port = htons ( port );

	
	int bind_return = ::bind( sock, 
										(sockaddr *) &sock_addr,
										sizeof(sock_addr)
										);

	if ( bind_return==-1){
			string message="server_socket::initialize() ::bind(...) error : ";
			message+=strerror( errno);
			throw socket_exception (message);
		};
	
	if ( bind_return!=0) {
			string message="server_socket::initialize() ::bind(...) return !=0 ";
			throw socket_exception (message);
		};

	int listen_return = ::listen (sock,
											backlog);

	if (listen_return == -1){
			string message="socket::listen() ::listen(...) error : ";
			message+=strerror( errno);
			throw socket_exception (message);
		}

	if (listen_return != 0){
			string message="server_socket::listen() ::listen(...) return !=0 ";
			throw socket_exception (message);
		}
}

generic_socket* server_socket::accept ()  throw (socket_exception){
	ostringstream exception_description;
	socklen_t addr_length = sizeof ( sock_addr);

	if ( ! generic_socket::is_valid() ) 	return false;

	generic_socket *new_socket = new generic_socket(
               generic_socket::get_recv_send_timeout());

	new_socket->set_sock( ::accept (sock,
													(sockaddr*) &sock_addr,
													&addr_length)
                                          );

	if (! new_socket->is_valid()){
			string message="server_socket::accept() ::accept(...) error : ";
			message+=strerror(errno);
			delete(new_socket);
			throw socket_exception (message);
		}

	return new_socket;
}

