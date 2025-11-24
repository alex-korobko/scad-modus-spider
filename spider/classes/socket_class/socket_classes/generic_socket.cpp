//implemenation of socket class
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <sys/neutrino.h>
#include <inttypes.h>
#include <errno.h>

#include <vector>
#include <string>
#include <map>
#include <functional>
#include <sstream>

using namespace std;

#include "defines.h"
#include "socket_exception.h"

#include "generic_socket.h"

generic_socket::generic_socket(uint64_t recv_send_timeout_nsec): 
   recv_send_timeout_nseconds(recv_send_timeout_nsec),
   sock (-1){	};

generic_socket::~generic_socket() {
	if ( is_valid()) 
		    shutdown (sock, 2);
     close(sock);
}

bool generic_socket::create() throw (socket_exception){
	sock = ::socket (AF_INET,
							  SOCK_STREAM,
							  0);

	if  (sock==-1){
	        ostringstream exception_description;
			exception_description<<"generic_socket::create() ::socket (...) error : "<<strerror( errno);
			throw socket_exception (exception_description.str());
		};

	if ( ! is_valid())	return false;

	int on=1, setsockopt_return;
	setsockopt_return=setsockopt (sock,
							SOL_SOCKET,
							SO_REUSEADDR,
							&on,
							sizeof(on)
							);

	if (setsockopt_return==-1){
	        ostringstream exception_description;
			exception_description<<"generic_socket::create() setsockopt (...) error : "<<strerror( errno);
			if (shutdown(sock, 1)!=0) {
               exception_description<<"\nCan`t shutdown socket.  Error : "<<strerror( errno);
			};
			throw socket_exception (exception_description.str());
		};

	if (setsockopt_return!=0) return false;
	return true;
}


bool generic_socket::send (const vector<byte> buffer) throw (socket_exception){
   	ostringstream exception_description;

	if ( ! is_valid() )
       throw socket_exception ("generic_socket::send() socket not valid");

	if (buffer.empty())
        throw socket_exception ("generic_socket::send() buffer to sending is empty");

    TimerTimeout(CLOCK_REALTIME, _NTO_TIMEOUT_REPLY,
                            NULL, &recv_send_timeout_nseconds, NULL);

	int send_status = ::send( sock, 
								  &buffer[0],
								  buffer.size(),
								  0);

	if (send_status == -1){
			string message="generic_socket::send() ::send(...) error : ";
			message+=strerror( errno);
			throw socket_exception (message);
		}

	if (static_cast<vector<byte>::size_type>(send_status)<buffer.size()){
			throw socket_exception ("generic_socket::send() ::send(...) error : send_status<buffer.size()");
		}
	return true;
}

int generic_socket::recv (
                vector<byte> &received_bytes_buffer,
                int bytes_to_receive_count) throw (socket_exception) {
    int  recv_status=0, received_bytes_count=0;
    vector<byte> tmp_buffer(bytes_to_receive_count);
    vector<byte>::iterator tmp_iterator;
   	ostringstream exception_description;

	if ( ! is_valid())
       throw socket_exception ("generic_socket::recv(...)  socket not valid");

	if (bytes_to_receive_count==0)
       throw socket_exception ("generic_socket::recv(...) trying to receive 0 bytes");

      do {
                TimerTimeout(CLOCK_REALTIME, _NTO_TIMEOUT_REPLY,
                            NULL, &recv_send_timeout_nseconds, NULL);

              recv_status = ::recv ( sock, 
										 	     &tmp_buffer[0],
											     bytes_to_receive_count,
											     0);

                if (recv_status<0){
			            exception_description<<"generic_socket::recv() ::recv(...) error : "<<strerror( errno);
                        throw socket_exception (exception_description.str());
                 }

               if (recv_status==0)
                              break;
               
               if (recv_status>bytes_to_receive_count) recv_status=bytes_to_receive_count;

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
