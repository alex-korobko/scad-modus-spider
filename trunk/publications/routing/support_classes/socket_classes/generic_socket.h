#ifndef __GENERIC_SOCKET_CLASS__
#define __GENERIC_SOCKET_CLASS__

//definition of the socket class

class generic_socket{
private:
 typedef uint8_t byte;
 typedef vector<byte> bytes;

 struct timeval recv_send_timeout;
	generic_socket();

protected:
	int m_sock;
	sockaddr_in m_addr;
	
public :
	generic_socket(struct timeval recv_send_timeout);
	virtual ~generic_socket();

	bool create() throw (runtime_error);

   void set_sock(int new_sock) {m_sock=new_sock;};
   int get_sock() {return m_sock;};

	//data transmittion
	virtual bool send (const vector<byte> ) throw (runtime_error);
 virtual int recv ( vector<byte> &received_bytes_buffer, int bytes_to_receive_count) throw (runtime_error);

	bool is_valid () const {return m_sock !=-1;}

 virtual timeval get_recv_send_timeout() {return recv_send_timeout;};
};

#endif
