
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
#include <sys/select.h> //#define FD_SETSIZE
#include "socket_exception.h"
#include "generic_socket.h"

#include "server_socket.h"

server_socket::server_socket (
                              struct timeval recv_send_timeout,
                              int new_backlog,
							  int new_port):
			generic_socket(recv_send_timeout),
			m_port(new_port),
			m_backlog(new_backlog){};

void server_socket::initialize() throw (socket_exception){
	if (! generic_socket::create())
			throw socket_exception("server_socket::initialize() : generic_socket::create()==false");

	if ( ! generic_socket::is_valid() )
		throw socket_exception("server_socket::initialize() : generic_socket don`t valid");


	m_addr.sin_family = AF_INET;
	m_addr.sin_addr.s_addr = INADDR_ANY;
	m_addr.sin_port = htons ( m_port );

	
	int bind_return = ::bind( m_sock, 
										(sockaddr *) &m_addr,
										sizeof(m_addr)
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

	int listen_return = ::listen (m_sock,
											m_backlog);

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

/*
void server_socket::send ( const vector<byte> buffer_to_send) throw (socket_exception){
	if (! socket::send(buffer_to_send)){
			throw socket_exception("server_socket::send (...): socket::send(...)==false");
		};
}


void server_socket::recv ( vector<byte> *buffer_to_recv) throw (socket_exception){
  int result = socket::recv(buffer_to_recv);
  cout<<"server_socket::recv result " <<result<<" time "<<ClockCycles()<<endl;

  vector<char> tmp_chars(20);
   cout<<"server_socket::recv for return buffer size "<<buffer_to_recv->size()<<" time "<<ClockCycles()<<endl;
   for (int i=0; i<buffer_to_recv->size(); i++) {
   itoa(buffer_to_recv->at(i), &tmp_chars[0], 16);
   cout<<"\t0x"<<&tmp_chars[0]<<endl;
   };

}

void server_socket::recv_peek ( vector<byte> *buffer_to_recv) throw (socket_exception){
	socket::recv_peek(buffer_to_recv);
}
*/

generic_socket* server_socket::accept ()  throw (socket_exception){
	int flags;
	ostringstream exception_description;
	socklen_t addr_length = sizeof ( m_addr);

	if ( ! generic_socket::is_valid() ) 	return false;


	generic_socket *new_socket = new generic_socket(
               generic_socket::get_recv_send_timeout());

	new_socket->set_sock( ::accept (m_sock,
													(sockaddr*) &m_addr,
													&addr_length)
                                          );

	if (! new_socket->is_valid()){
			string message="server_socket::accept() ::accept(...) error : ";
			message+=strerror(errno);
			delete(new_socket);
			throw socket_exception (message);
		}

	if ((flags = fcntl(new_socket->get_sock(), F_GETFL, 0)) < 0) {
	   exception_description<<"server_socket::accept () fail to get file status flags error : "<<strerror(errno);
	   throw socket_exception(exception_description.str());
	};

	if (fcntl(new_socket->get_sock(), F_SETFL, flags | O_NONBLOCK) < 0){
	   exception_description<<"server_socket::accept () fail to set file status flags error : "<<strerror(errno);
	   throw socket_exception(exception_description.str());
	};

	return new_socket;
}

