#ifndef __GENERIC_SOCKET_CLASS__
#define __GENERIC_SOCKET_CLASS__

//definition of the socket class
class generic_socket{
private:
    uint64_t recv_send_timeout_nseconds;
	generic_socket();

protected:
	int sock;
	sockaddr_in sock_addr;

public :
	generic_socket(uint64_t recv_send_timeout_nsec);
	virtual ~generic_socket();

    bool create() throw (socket_exception);

   void set_sock(int new_sock) {sock=new_sock;};
   int get_sock() {return sock;};

	//data transmittion
	bool send (const vector<byte> ) throw (socket_exception);
    int recv ( vector<byte> &received_bytes_buffer, int bytes_to_receive_count) throw (socket_exception);

	bool is_valid () const {return (sock !=-1);}
    uint64_t get_recv_send_timeout() {return recv_send_timeout_nseconds;};
};

#endif
