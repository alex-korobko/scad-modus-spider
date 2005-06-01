#ifndef __SERVER_SOCKET_H__
#define __SERVER_SOCKET_H__

class server_socket : public generic_socket{
private:
	int m_port;
	int m_backlog;	

public:
	server_socket (  struct timeval recv_send_timeout,
                              int new_backlog,
							  int new_port);
	virtual ~server_socket () {
    };

//	void send ( const vector<byte> buffer_to_send)  throw (socket_exception);
//	void recv ( vector<byte> *buffer_to_recv)  throw (socket_exception);
//	void recv_peek ( vector<byte> *buffer_to_recv)  throw (socket_exception);

	generic_socket* accept () throw (socket_exception);
	void initialize() throw (socket_exception);

	int get_port() {return m_port;};
	int get_backlog() {return m_backlog;};
//	timeval get_recv_send_timeout() {return genericsocket::get_recv_send_timeout();};
};


#endif
