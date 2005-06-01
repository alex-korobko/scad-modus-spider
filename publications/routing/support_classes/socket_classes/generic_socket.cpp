//implemenation of socket class
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream.h>
#include <sys/neutrino.h>
#include <inttypes.h>

#include <vector>
#include <string>
#include <functional>
#include <sstream>
#include <stdexcept>

using namespace std;

#include "generic_socket.h"

generic_socket::generic_socket(struct timeval recv_send_timeout) : 
   recv_send_timeout(recv_send_timeout),
   m_sock (-1){	};

generic_socket::~generic_socket() {
	if ( is_valid()) 
		    shutdown (m_sock, 0);
}

bool generic_socket::create() throw (runtime_error){
	m_sock = ::socket (AF_INET,
								  SOCK_STREAM,
								  0);

	if  (m_sock==-1)
  throw runtime_error(string("generic_socket::create() ::socket (...) error: ")+strerror( errno));

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
	 string exception_description;
			exception_description+=(string("generic_socket::create() setsockopt (...) error: ")+strerror( errno));
			if (shutdown(m_sock, 0)!=0) 
        exception_description+=(string("\nCan`t shutdown socket, error : ")+strerror( errno));
			throw runtime_error(exception_description);
		};

	if (setsockopt_return!=0) return false;
	return true;
}


bool generic_socket::send (bytes buffer) throw (runtime_error){
   	fd_set write_fd;
   	int result, send_size, send_status, sended_size=0;
   bytes::iterator end_iter;

	if ( !is_valid() ) throw runtime_error("generic_socket::send() socket is not valid");

	if (buffer.empty())throw runtime_error("generic_socket::send() buffer to sending is empty");

send_size=buffer.size();
while(sended_size<send_size) {
   send_status = ::send( m_sock, 
			    					  &buffer[0],
							     	  buffer.size(),
      						  0);

    if (send_status == -1)
			       throw runtime_error (string("generic_socket::send() ::send(...) error : ")+strerror( errno));

    FD_ZERO(&write_fd);
    FD_SET(m_sock, &write_fd);

     result = select(m_sock + 1, NULL, &write_fd, NULL, &recv_send_timeout);
     switch (result) {
       case (-1) : throw runtime_error (string("generic_socket::send(...) can`t select socket after send : ")+strerror(errno));
       case (0) : throw runtime_error ("generic_socket::send(...) select socket timeout after send");
     }; //switch (result)

    if (send_status==0)
        throw runtime_error (string("generic_socket::send() ::send(...) sended 0 bytes "));

    sended_size+=send_status;
    end_iter=buffer.begin();
    advance(end_iter, send_status);
    buffer.erase(buffer.begin(), end_iter);
 }; //while(send_status<send_size)

	return true;
}

int generic_socket::recv (
                bytes &received_bytes_buffer,
                int bytes_to_receive_count) throw (runtime_error) {
   	fd_set  read_fd;
    int result, recv_status=0, received_bytes_count=0;
    bytes tmp_buffer(bytes_to_receive_count);
    bytes::iterator tmp_iterator;

	if ( !is_valid()) throw runtime_error ("generic_socket::recv(...)  socket not valid");

	if (bytes_to_receive_count==0) throw runtime_error ("generic_socket::recv(...) trying to recieve 0 bytes");

FD_ZERO(&read_fd);
FD_SET(m_sock,&read_fd);
	do { //while(static_cast<vector<byte>::size_type>(recived_bytes_count)<tmp_buffer.size())
        result = select(m_sock + 1, &read_fd, NULL, NULL, &recv_send_timeout);
        switch (result) {
                    case (-1) : throw runtime_error (string("generic_socket::recv(...) can`t select socket before recv : ")+strerror(errno));
                    case (0) : throw runtime_error("generic_socket::recv(...) select socket timeout before recv");
           }; //switch (result)

        recv_status = ::recv ( m_sock, 
				 						 	     &tmp_buffer[0],
  											     bytes_to_receive_count,
		  									     0);

       if (recv_status<0) throw runtime_error (string("generic_socket::recv() ::recv(...) error : ")+strerror( errno));
       if (recv_status==0) break;

       tmp_iterator=tmp_buffer.begin();
       advance(tmp_iterator, recv_status);
       received_bytes_buffer.insert(received_bytes_buffer.end(), tmp_buffer.begin(), tmp_iterator);
       received_bytes_count+=recv_status;

   } while(received_bytes_count<bytes_to_receive_count); 

	if (bytes_to_receive_count!=received_bytes_count){
	  ostringstream exception_description;
			exception_description<<"generic_socket::recv (...) : (received bytes count == "<<received_bytes_count;
			exception_description<<" ) != (bytes_to_recieve_count == "<<bytes_to_receive_count<<" )";
			throw runtime_error(exception_description.str());
		};

	return received_bytes_count;
}
