//implemenation of socket class
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream.h>
#include <sys/neutrino.h>
#include <inttypes.h>

#include <vector>
#include <string>
#include <map>
#include <functional>
#include <sstream>

using namespace std;

#include "defines.h"
#include <sys/select.h> //#define FD_SETSIZE
#include "socket_exception.h"

#include "generic_socket.h"

generic_socket::generic_socket(struct timeval recv_send_timeout) : 
   recv_send_timeout(recv_send_timeout),
   m_sock (-1){	};

generic_socket::~generic_socket() {
	if ( is_valid()) 
//		    shutdown (m_sock, 1);
             close (m_sock);

}

bool generic_socket::create() throw (socket_exception){
	m_sock = ::socket (AF_INET,
								  SOCK_STREAM,
								  0);

	if  (m_sock==-1){
	        ostringstream exception_description;
			exception_description<<"generic_socket::create() ::socket (...) error : "<<strerror( errno);
			throw socket_exception (exception_description.str());
		};

	if ( ! is_valid())	return false;

	//TIME WAIT - argh
	int on=1, setsockopt_return;
	setsockopt_return=setsockopt (m_sock,
							SOL_SOCKET,
							SO_REUSEADDR,
							&on,
							sizeof(on)
							);

	if (setsockopt_return==-1){
	        ostringstream exception_description;
			exception_description<<"generic_socket::create() setsockopt (...) error : "<<strerror( errno);
			if (shutdown(m_sock, 1)!=0) {
               exception_description<<"\nCan`t shutdown socket.  Error : "<<strerror( errno);
			};
			throw socket_exception (exception_description.str());
		};

	if (setsockopt_return!=0) return false;
	return true;
}


bool generic_socket::send (const vector<byte> buffer) throw (socket_exception){
   	fd_set write_fd;
   	ostringstream exception_description;
	int result;

	if ( ! is_valid() )
       throw socket_exception ("generic_socket::send() socket not valid");

	if (buffer.empty())
        throw socket_exception ("generic_socket::send() buffer to sending is empty");

	int send_status = ::send( m_sock, 
								  &buffer[0],
								  buffer.size(),
								  0);

	if (send_status == -1){
			string message="generic_socket::send() ::send(...) error : ";
			message+=strerror( errno);
			throw socket_exception (message);
		}

   FD_ZERO(&write_fd);
   FD_SET(m_sock, &write_fd);

   result = select(m_sock + 1, NULL, &write_fd, NULL, &recv_send_timeout);
   switch (result) {
     case (-1) : {
      exception_description<<"generic_socket::send(...) can`t select socket after send : "<<strerror(errno);
      throw socket_exception (exception_description.str());
     }; //case -1
     case (0) : {
      exception_description<<"generic_socket::send(...) select socket timeout after send ";
      throw socket_exception (exception_description.str());
     }; //case 0
   }; //switch (result)

	if (static_cast<vector<byte>::size_type>(send_status)<buffer.size()){
			throw socket_exception ("generic_socket::send() ::send(...) error : send_status<buffer.size()");
		}
	return true;
}

int generic_socket::recv (
                vector<byte> &received_bytes_buffer,
                int bytes_to_receive_count) throw (socket_exception) {
   	fd_set  read_fd;
    int result, recv_status=0, received_bytes_count=0;
    vector<byte> tmp_buffer(bytes_to_receive_count);
    vector<byte>::iterator tmp_iterator;
   	ostringstream exception_description;

	if ( ! is_valid())
       throw socket_exception ("generic_socket::recv(...)  socket not valid");

	if (bytes_to_receive_count==0)
       throw socket_exception ("generic_socket::recv(...) trying to recieve 0 bytes");

   FD_ZERO(&read_fd);
   FD_SET(m_sock,&read_fd);
	do { //while(static_cast<vector<byte>::size_type>(recived_bytes_count)<tmp_buffer.size())
               result = select(m_sock + 1, &read_fd, NULL, NULL, &recv_send_timeout);
               switch (result) {
                    case (-1) : {
                        exception_description<<"generic_socket::recv(...) can`t select socket before recv : "<<strerror(errno);
                        throw socket_exception (exception_description.str());
                     }; //case -1
                    case (0) : {
                        exception_description<<"generic_socket::recv(...) select socket timeout before recv";
                        throw socket_exception (exception_description.str());
                    }; //case 0
              }; //switch (result)

               recv_status = ::recv ( m_sock, 
										 	     &tmp_buffer[0],
											     bytes_to_receive_count,
											     0);

                if (recv_status<0){
			            exception_description<<"generic_socket::recv() ::recv(...) error : "<<strerror( errno);
                        throw socket_exception (exception_description.str());
                 }

               if (recv_status==0)
                              break;

                tmp_iterator=tmp_buffer.begin();
                advance(tmp_iterator, recv_status);
                received_bytes_buffer.insert(received_bytes_buffer.end(), tmp_buffer.begin(), tmp_iterator);
                received_bytes_count+=recv_status;

     } while(received_bytes_count<bytes_to_receive_count); 

	if (bytes_to_receive_count!=received_bytes_count){
			exception_description<<"generic_socket::recv (...) : (received bytes count == "<<received_bytes_count;
			exception_description<<" ) != (bytes_to_recieve_count == "<<bytes_to_receive_count<<" )";
			throw socket_exception(exception_description.str());
		};

	return received_bytes_count;
}
