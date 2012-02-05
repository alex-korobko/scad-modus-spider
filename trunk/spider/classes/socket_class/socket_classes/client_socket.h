#ifndef __CLIENT_SOCKET_H__
#define __CLIENT_SOCKET_H__

class client_socket : public generic_socket {
private:
	in_addr_t host;
	int port;
	uint64_t connect_timeout_nseconds;

public:
	client_socket (in_addr_t host, 
                          int port,
						  uint64_t connect_timeout_nsec,
						  uint64_t recv_send_timeout_nsec);

	virtual ~client_socket() {};

void initialize() throw (socket_exception);
 uint64_t get_connect_timeout() {return connect_timeout_nseconds;};
};

#endif
