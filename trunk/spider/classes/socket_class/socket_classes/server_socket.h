#ifndef __SERVER_SOCKET_H__
#define __SERVER_SOCKET_H__

class server_socket : public generic_socket{
private:
	int port;
	int backlog;	

public:
	server_socket ( uint64_t recv_send_timeout,
                              int new_backlog,
							  int new_port);
	virtual ~server_socket () {
    };

	generic_socket* accept () throw (socket_exception);
	void initialize() throw (socket_exception);

	int get_port() {return port;};
	int get_backlog() {return backlog;};
};


#endif
