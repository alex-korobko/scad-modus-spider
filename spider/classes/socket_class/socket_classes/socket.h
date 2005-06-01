#ifndef __SOCKET_CLASS__
#define __SOCKET_CLASS__

//definition of the socket class

class socket{
private:
    struct timeval recv_send_timeout;
	socket();

protected:
	int m_sock;
	sockaddr_in m_addr;
	
public :
	socket(struct timeval recv_send_timeout);

	virtual ~socket();

	bool create() throw (socket_exception);
	
	//data transmittion
	bool send (const vector<byte> ) throw (socket_exception);
	int recv (vector<byte> *buffer ) throw (socket_exception);
	int recv_peek (vector<byte> *buffer)  throw (socket_exception);
		
	bool is_valid () const {return m_sock !=-1;}

    timeval get_recv_send_timeout() {return recv_send_timeout;};
};

#endif
