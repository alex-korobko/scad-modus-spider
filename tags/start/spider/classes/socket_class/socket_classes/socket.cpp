//implemenation of socket class
using namespace std;

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
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

#include "defines.h"
#include "socket_exception.h"

#include "generic_socket.h"
#include "socket_exception.h"


generic_socket::generic_socket(struct timeval recv_send_timeout) : 
   recv_send_timeout(recv_send_timeout),
   m_sock (-1){	};

generic_socket::~generic_socket() {
	if ( is_valid()) 
		    shutdown (m_sock, 1);
}

bool generic_socket::create() throw (socket_exception){
	ostringstream exception_description;

	m_sock = ::socket (AF_INET,
								  SOCK_STREAM,
								  0);

	if  (m_sock==-1){	
			exception_description<<"socket::create() ::socket (...) error : "<<strerror( errno);
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
			exception_description<<"socket::create() setsockopt (...) error : "<<strerror( errno);
			if (shutdown(m_sock, 1)!=0) {
               exception_description<<"\nCan`t shutdown socket error : "<<strerror( errno);
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
       throw socket_exception ("socket::send() socket not valid");
	if (buffer.empty())
        throw socket_exception ("socket::send() buffer to sending is empty");
	
	int send_status = ::send( m_sock, 
								  &buffer[0],
								  buffer.size(),
								  0);

	if (send_status == -1){
			string message="socket::send() ::send(...) error : ";
			message+=strerror( errno);
			throw socket_exception (message);
		}

   FD_ZERO(&write_fd);
   FD_SET(m_sock,&write_fd);

   result = select(m_sock + 1, NULL, &write_fd, NULL, &recv_send_timeout);
   switch (result) {
     case (-1) : {
      exception_description<<"socket::send(...) can`t select socket after send : "<<strerror(errno);
      throw socket_exception (exception_description.str());
     }; //case -1
     case (0) : {
      exception_description<<"socket::send(...) select socket timeout after send ";
      throw socket_exception (exception_description.str());
     }; //case 0
   }; //switch (result)

	if (static_cast<vector<byte>::size_type>(send_status)<buffer.size()){
			throw socket_exception ("socket::send() ::send(...) error : send_status<buffer.size()");
		}
	return true;
}

int generic_socket::recv_peek (vector<byte> *buffer) throw (socket_exception){
   	fd_set read_fd;
   	ostringstream exception_description;
	int result, recv_status=0;
   	vector<byte> tmp_buffer(buffer->size());

	if ( ! is_valid() )	 
	       throw socket_exception ("socket::recv_peek()  socket not valid");

	if (buffer->empty())
        throw socket_exception ("socket::recv_peek() buffer to recieving is empty");

   FD_ZERO(&read_fd);
   FD_SET(m_sock,&read_fd);

     do {  // while(recv_status<buffer.size())
          result = select(m_sock + 1, &read_fd, NULL, NULL, &recv_send_timeout);
          switch (result) {
               case (-1) : {
                   exception_description<<"socket::recv_peek(...) can`t select socket before recv : "<<strerror(errno);
                   throw socket_exception (exception_description.str());
                }; //case -1
               case (0) : {
                  exception_description<<"socket::recv_peek(...) select socket timeout before recv";
                   throw socket_exception (exception_description.str());
                }; //case 0
           }; //switch (result)

           recv_status = ::recv ( m_sock, 
									        &tmp_buffer[0],
											tmp_buffer.size(),
											MSG_PEEK);

           if (recv_status==-1){
			   exception_description<<"socket::recv_peek() ::recv(...) error : "<<strerror( errno);
			    throw socket_exception (exception_description.str());
           };

          if (recv_status==0)
                    break;
     }while(static_cast<vector<byte>::size_type>(recv_status)<tmp_buffer.size()) ;

	*buffer=tmp_buffer;

	if (static_cast<vector<byte>::size_type>(recv_status)!=buffer->size()){
			exception_description<<"socket::recv_peek (...) : (recv == "<<recv_status;
			exception_description<<" ) != (buffer.size() == "<<buffer->size()<<" )";
			throw socket_exception(exception_description.str());
		};

  return recv_status; 
}


int generic_socket::recv (vector<byte> *buffer) throw (socket_exception){
   	fd_set  read_fd;
   	vector<byte> tmp_buffer(buffer->size());
   	vector<byte>::iterator tmp_iter;
   	ostringstream exception_description;
    int result, recv_status=0, recived_bytes_count=0;

	if ( ! is_valid() )
		       throw socket_exception ("socket::recv(...)  socket not valid");

	if (buffer->empty())
        throw socket_exception ("socket::recv(...) buffer to recieving is empty");

    FD_ZERO(&read_fd);
    FD_SET(m_sock,&read_fd);

   buffer->clear();
	do { //while(static_cast<vector<byte>::size_type>(recived_bytes_count)<tmp_buffer.size())
               result = select(m_sock + 1, &read_fd, NULL, NULL, &recv_send_timeout);
               switch (result) {
                    case (-1) : {
                        exception_description<<"socket::recv(...) can`t select socket before recv : "<<strerror(errno);
                        throw socket_exception (exception_description.str());
                     }; //case -1
                    case (0) : {
                        exception_description<<"socket::recv(...) select socket timeout before recv";
                        throw socket_exception (exception_description.str());
                    }; //case 0
              }; //switch (result)

               cout<<"before recv  tmp_buffer.size() "<<tmp_buffer.size()<<" time "<<ClockCycles()<<endl;

               recv_status = ::recv ( m_sock, 
										 	     &tmp_buffer[0],
											     tmp_buffer.size(),
											     0);
               cout<<"after recv  recv_status "<<recv_status<<" time "<<ClockCycles()<<endl;

                if (recv_status==-1){
			            exception_description<<"socket::recv() ::recv(...) error : "<<strerror( errno);
                        throw socket_exception (exception_description.str());
                 }

                 if (recv_status==0)
                              break;
                tmp_iter=tmp_buffer.begin();
                 if (recv_status>tmp_buffer.size()) {
                         cout<<"ERROR RECV_STATUS>tmp_buffer.size"<<endl;
                     };
                           
				advance(tmp_iter, recv_status);
				buffer->insert(buffer->end(),
				                     tmp_buffer.begin(),
				                     tmp_iter);
                recived_bytes_count+=recv_status;
               cout<<"after inserting  recived_bytes_count "<<recived_bytes_count<<"  tmp_buffer.size()  "<<tmp_buffer.size()<<" time "<<ClockCycles()<<endl;
     } while(static_cast<vector<byte>::size_type>(recived_bytes_count)<tmp_buffer.size()); 

	if (tmp_buffer.size()!=static_cast<vector<byte>::size_type>(recived_bytes_count)){
			exception_description<<"socket::recv (...) : (recived bytes count == "<<recived_bytes_count;
			exception_description<<" ) != (buffer.size() == "<<tmp_buffer.size()<<" )";
			throw socket_exception(exception_description.str());
		};

  vector<char> tmp_chars(20);
   cout<<"for return buffer size "<<buffer->size()<<" time "<<ClockCycles()<<endl;
   for (int i=0; i<buffer->size(); i++) {
   itoa(buffer->at(i), &tmp_chars[0], 16);
   cout<<"\t0x"<<&tmp_chars[0]<<endl;
   };
	return recived_bytes_count;
}
