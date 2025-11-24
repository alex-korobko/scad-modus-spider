#ifndef __CLIENT_SERVER_SOCKETS_TEST__
#define __CLIENT_SERVER_SOCKETS_TEST__

class client_server_sockets_test : public CppUnit::TestFixture {
private:
int port;
in_addr_t host;
int backlog;

timeval connect_timeout;
timeval recv_send_timeout;

client_socket *socket_client;
server_socket *socket_server;
server_socket *accepted_socket_server;


void compare_recived_and_sended_messages(vector<byte> recived_message, 
																		  vector<byte> sended_message);
void test_client_server_sockets();
void test_fail_to_connect();
void test_recieve_server_exit();
void test_recieve_client_exit();
void test_send_server_exit();
void test_send_client_exit();

public:
void setUp();
void tearDown();
static CppUnit::TestSuite* suite();
};

#endif