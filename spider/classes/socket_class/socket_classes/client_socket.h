#ifndef __CLIENT_SOCKET_H__
#define __CLIENT_SOCKET_H__

class client_socket : public generic_socket {
private:
	in_addr_t m_host;
	int m_port;
	struct timeval m_connect_timeout;

public:
	client_socket (in_addr_t host, 
                          int port,
						  struct timeval connect_timeout,
                          struct timeval recv_send_timeout);

	virtual ~client_socket() {
    };

void initialize() throw (socket_exception);

timeval get_connect_timeout() {return m_connect_timeout;};
};

#endif
