using namespace std;

#include <string>
#include <vector>
#include <map>
#include <functional>


#include <netinet/in.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream.h>

#include <cppunit/TestFixture.h> 
#include <cppunit/TestSuite.h> 
#include <cppunit/TestCaller.h>

#include "../../system_settings_classes/defines.h"
#include "../../socket_classes/socket_exception.h"
#include "../../socket_classes/socket.h"
#include "../../socket_classes/server_socket.h"
#include "../../socket_classes/client_socket.h"

#include "client_server_sockets_test.h"

void client_server_sockets_test::setUp() {
port = 6000;
host  = inet_addr("127.0.0.1");
backlog = 15;

connect_timeout.tv_sec=0;
connect_timeout.tv_usec=500;

recv_send_timeout.tv_sec=0;
recv_send_timeout.tv_usec=100;

socket_client=NULL;
socket_server=NULL;
accepted_socket_server=NULL;
}

void  client_server_sockets_test::tearDown() {
	if (socket_client!=NULL) delete (socket_client);
	if (socket_server!=NULL) delete (socket_server);
	if (accepted_socket_server!=NULL) delete (accepted_socket_server);
};

void client_server_sockets_test::compare_recived_and_sended_messages(vector<byte> recived_message, 
																						  vector<byte> sended_message) {
	vector<char> tmp_chars(10);
	string error_message;
    port++;
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Recived message size does not equal to sended message size", 
												recived_message.size(), 
												sended_message.size());

	for (int i=0 ; i<recived_message.size() ; i++) {
	
	error_message="Recived message does not equal to sended\nin ";
	itoa(i, &tmp_chars[0], 10);
	error_message+=&tmp_chars[0];
	error_message+=" byte ( ";
	itoa(recived_message[i], &tmp_chars[0], 10);
	error_message+=&tmp_chars[0];
	error_message+=" != ";
	itoa(sended_message[i], &tmp_chars[0], 10);
	error_message+=&tmp_chars[0];
	error_message+=" )";
	
	CPPUNIT_ASSERT_EQUAL_MESSAGE(error_message, 
												recived_message[i], 
												sended_message[i]);
	};

					
}

void client_server_sockets_test::test_client_server_sockets(){
	vector<byte> message_socket_client, message_socket_server, recieved_message_header(1);
    port++;
	if (socket_client!=NULL) {
	      delete (socket_client);
	      socket_client=NULL;
	 };
	if (socket_server!=NULL) {
	    delete (socket_server);
	     socket_server=NULL;
	 };
	if (accepted_socket_server!=NULL) {
	      delete (accepted_socket_server);
	      accepted_socket_server=NULL;
	 };


	socket_client = new client_socket(
                                             host,
                                             port,
                                            connect_timeout,
                                            recv_send_timeout);

	socket_server = new server_socket(recv_send_timeout,
                                              backlog,
                                              port);

	CPPUNIT_ASSERT(recv_send_timeout.tv_sec == socket_server->get_recv_send_timeout().tv_sec);
	CPPUNIT_ASSERT(recv_send_timeout.tv_usec == socket_server->get_recv_send_timeout().tv_usec);

	CPPUNIT_ASSERT(port == socket_server->get_port());
	CPPUNIT_ASSERT(backlog == socket_server->get_backlog());

	CPPUNIT_ASSERT(connect_timeout.tv_sec == socket_client->get_connect_timeout().tv_sec);
	CPPUNIT_ASSERT(connect_timeout.tv_sec == socket_client->get_connect_timeout().tv_sec);
	
	CPPUNIT_ASSERT(recv_send_timeout.tv_sec == socket_client->get_recv_send_timeout().tv_sec);
   CPPUNIT_ASSERT(recv_send_timeout.tv_usec == socket_client->get_recv_send_timeout().tv_usec);

	try {
       socket_server->initialize();
	}
	catch (socket_exception e){
				CPPUNIT_FAIL("\nsocket_server.initialize() raise exception:\n" + e.get_description());
	};

	try {
        socket_client->initialize();
	} catch (socket_exception e){
				CPPUNIT_FAIL("\nsocket_client.initialize() raise exception:\n" + e.get_description());
	};

	message_socket_client.push_back(4);
	message_socket_client.push_back(21);
	message_socket_client.push_back(22);
	message_socket_client.push_back(23);

	try {
		socket_client->send(message_socket_client);
	}
	catch (socket_exception e){
			CPPUNIT_FAIL("\nsocket_client.send(message_socket_client) raise exception:\n" + e.get_description());
	};

	try {
	    accepted_socket_server=socket_server->accept();
	}
	catch (socket_exception e){
			CPPUNIT_FAIL("\nsocket_server.accept(accepted_socket_server) raise exception:\n" + e.get_description());
	};

	try {
       accepted_socket_server->recv_peek(recieved_message_header);
	}
	catch (socket_exception e){
				CPPUNIT_FAIL("\naccepted_socket_server.recv_peek(recieved_message_header) raise exception:\n" + e.get_description());
	};

	message_socket_server.resize(recieved_message_header[0]);
	
	try {
         accepted_socket_server->recv(message_socket_server);
	}
	catch (socket_exception e){
				CPPUNIT_FAIL("\naccepted_socket_server.recv(message_socket_server) raise exception:\n" + e.get_description());
	};
	
	this->compare_recived_and_sended_messages(message_socket_server, 
																				message_socket_client);

	message_socket_server.clear();
	message_socket_server.push_back(3);
	message_socket_server.push_back(31);
	message_socket_server.push_back(32);

	try {
        accepted_socket_server->send(message_socket_server);
	}
	catch (socket_exception e){
				CPPUNIT_FAIL("\naccepted_socket_server.send(message_socket_server) raise exception:\n" + e.get_description());
	};

	try {
	socket_client->recv_peek(recieved_message_header);
	}	catch (socket_exception e){
				CPPUNIT_FAIL("\nsocket_client.recv_peek(recieved_message_header) raise exception:\n" + e.get_description());
	};

	message_socket_client.resize(recieved_message_header[0]);

	try {	
	socket_client->recv(message_socket_client);
	}
	catch (socket_exception e){
		CPPUNIT_FAIL("\nsocket_client.recv(message_socket_client) raise exception:\n" + e.get_description());
	};

	this->compare_recived_and_sended_messages(message_socket_client, 
																				message_socket_server);

};

void client_server_sockets_test::test_fail_to_connect(){
	bool exception_raized=false;
    port++;
	if (socket_client!=NULL) {
	 delete (socket_client);
	 socket_client=NULL;
	 };
	if (socket_server!=NULL) {
	 delete (socket_server);
	 socket_server=NULL;
	 };

	if (accepted_socket_server!=NULL) {
	      delete (accepted_socket_server);
	      accepted_socket_server=NULL;
	 };

	socket_client = new client_socket(
                                             host,
                                             port,
                                            connect_timeout,
                                            recv_send_timeout);

	try {
        socket_client->initialize();
	}
	catch (socket_exception sock_exc){
		exception_raized=true;
		CPPUNIT_ASSERT_EQUAL(sock_exc.get_description(),
		string("client_socket::initialize() connection error : Connection refused"));
	};

	if (!exception_raized) {
		CPPUNIT_FAIL("\nNot raised exception on connect");	
	};

};

void client_server_sockets_test::test_recieve_server_exit(){
	vector<byte> message_socket_client, message_socket_server, recieved_message_header(1);
    port++;
	if (socket_client!=NULL) {
	 delete (socket_client);
	 socket_client=NULL;
	 };
	if (socket_server!=NULL) {
	 delete (socket_server);
	 socket_server=NULL;
	 };
	if (accepted_socket_server!=NULL) {
	      delete (accepted_socket_server);
	      accepted_socket_server=NULL;
	 };

	socket_client = new client_socket(
                                             host,
                                             port,
                                            connect_timeout,
                                            recv_send_timeout);

	socket_server = new server_socket(
                                              recv_send_timeout,
                                              backlog,
                                              port);

	try {
       socket_server->initialize();
	}
	catch (socket_exception e){
				CPPUNIT_FAIL("\nsocket_server.initialize() raise exception:\n" + e.get_description());
	};

	try {
        socket_client->initialize();
	}
	catch (socket_exception e){
				CPPUNIT_FAIL("\nsocket_client.initialize() raise exception:\n" + e.get_description());
	};

	message_socket_client.push_back(4);
	message_socket_client.push_back(21);
	message_socket_client.push_back(22);
	message_socket_client.push_back(23);

	try {
		socket_client->send(message_socket_client);
	}
	catch (socket_exception e){
		CPPUNIT_FAIL("\nsocket_client.send(message_socket_client) raise exception:\n" + e.get_description());
	};

	try {
	    accepted_socket_server=socket_server->accept();
	}
	catch (socket_exception e){
 				CPPUNIT_FAIL("\nsocket_server.accept(accepted_socket_server) raise exception:\n" + e.get_description());
	};

	try {
       accepted_socket_server->recv_peek(recieved_message_header);
	}
	catch (socket_exception e){
			CPPUNIT_FAIL("\naccepted_socket_server.recv_peek(recieved_message_header) raise exception:\n" + e.get_description());
	};

	message_socket_server.resize(recieved_message_header[0]);
	
	try {
         accepted_socket_server->recv(message_socket_server);
	}
	catch (socket_exception e){
				CPPUNIT_FAIL("\naccepted_socket_server.recv(message_socket_server) raise exception:\n" + e.get_description());
	};
	
	
	this->compare_recived_and_sended_messages(message_socket_server, 
																				message_socket_client);

	delete (socket_server);
	socket_server=NULL;
	bool exception_raised=false;
	
	try {
        socket_client->recv_peek(recieved_message_header);
	}
	catch (socket_exception e){
				exception_raised=true;
				CPPUNIT_ASSERT_EQUAL(e.get_description(),
				string("socket::recv_peek(...) select socket timeout before recv"));
	};
    
    if (!exception_raised) {
        CPPUNIT_FAIL("Exception not raised !");
       };
};

void client_server_sockets_test::test_recieve_client_exit(){
	vector<byte> message_socket_client, message_socket_server, recieved_message_header(1);
    port++;
	if (socket_client!=NULL) {
	 delete (socket_client);
	 socket_client=NULL;
	 };
	if (socket_server!=NULL) {
	 delete (socket_server);
	 socket_server=NULL;
	 };
	if (accepted_socket_server!=NULL) {
	      delete (accepted_socket_server);
	      accepted_socket_server=NULL;
	 };

	socket_client = new client_socket(
                                             host,
                                             port,
                                            connect_timeout,
                                            recv_send_timeout);

	socket_server = new server_socket(
                                              recv_send_timeout,
                                              backlog,
                                              port);
	try {
       socket_server->initialize();
	}
	catch (socket_exception e){
				CPPUNIT_FAIL("\nsocket_server.initialize() raise exception:\n" + e.get_description());
	};

	try {
        socket_client->initialize();
	}
	catch (socket_exception e){
				CPPUNIT_FAIL("\nsocket_client.initialize() raise exception:\n" + e.get_description());
	};

	message_socket_client.push_back(4);
	message_socket_client.push_back(21);
	message_socket_client.push_back(22);
	message_socket_client.push_back(23);

	try {
		socket_client->send(message_socket_client);
	}
	catch (socket_exception e){
				CPPUNIT_FAIL("\nsocket_client.send(message_socket_client) raise exception:\n" + e.get_description());
	};

	try {
	    accepted_socket_server=socket_server->accept();
	}
	catch (socket_exception e){
 				CPPUNIT_FAIL("\nsocket_server.accept(accepted_socket_server) raise exception:\n" + e.get_description());
	};

	try {
       accepted_socket_server->recv_peek(recieved_message_header);
	}
	catch (socket_exception e){
				CPPUNIT_FAIL("\naccepted_socket_server.recv_peek(recieved_message_header) raise exception:\n" + e.get_description());
	};

	message_socket_server.resize(recieved_message_header[0]);


	try {
         accepted_socket_server->recv(message_socket_server);
	}
	catch (socket_exception e){
				CPPUNIT_FAIL("\naccepted_socket_server.recv(message_socket_server) raise exception:\n" + e.get_description());	
	};
    

	this->compare_recived_and_sended_messages(message_socket_server, 
																				message_socket_client);

	delete (socket_client);
	socket_client=NULL;

	bool exception_raised=false;
	try {
         accepted_socket_server->recv(message_socket_server);
	} 	catch (socket_exception e){
				exception_raised=true;
				CPPUNIT_ASSERT_EQUAL(e.get_description(),
				string("socket::recv (...) : (recived bytes count == 0 ) != (buffer.size() == 4 )"));
	};

    if (!exception_raised) {
        CPPUNIT_FAIL("Exception not raised !");
       };

};

void client_server_sockets_test::test_send_server_exit(){
	vector<byte> message_socket_client, message_socket_server, recieved_message_header(1);
    port++;
	if (socket_client!=NULL) {
	 delete (socket_client);
	 socket_client=NULL;
	 };
	if (socket_server!=NULL) {
	 delete (socket_server);
	 socket_server=NULL;
	 };
	if (accepted_socket_server!=NULL) {
     delete (accepted_socket_server);
     accepted_socket_server=NULL;
	 };

	socket_client = new client_socket(
                                             host,
                                             port,
                                            connect_timeout,
                                            recv_send_timeout);

	socket_server = new server_socket(
                                              recv_send_timeout,
                                              backlog,
                                              port);

	try {
       socket_server->initialize();
	}
	catch (socket_exception e){
				CPPUNIT_FAIL("\nsocket_server.initialize() raise exception:\n" + e.get_description());
	};

	try {
        socket_client->initialize();
	}
	catch (socket_exception e){
				CPPUNIT_FAIL("\nsocket_client.initialize() raise exception:\n" + e.get_description());
	};

	message_socket_client.push_back(4);
	message_socket_client.push_back(21);
	message_socket_client.push_back(22);
	message_socket_client.push_back(23);

	try {
		socket_client->send(message_socket_client);
	}
	catch (socket_exception e){
			CPPUNIT_FAIL("\nsocket_client.send(message_socket_client) raise exception:\n" + e.get_description());
	};

    try {
             accepted_socket_server=socket_server->accept();
     } 	catch (socket_exception e){
             CPPUNIT_FAIL("\nsocket_server.accept(accepted_socket_server) raise exception:\n" + e.get_description());
     };

      try {
          accepted_socket_server->recv_peek(recieved_message_header);
       }	catch (socket_exception e) {
           CPPUNIT_FAIL("\naccepted_socket_server.recv_peek(recieved_message_header) raise exception:\n" + e.get_description());
       };

       message_socket_server.resize(recieved_message_header[0]);

       try {
          accepted_socket_server->recv(message_socket_server);
      }	catch (socket_exception e) {
		CPPUNIT_FAIL("\naccepted_socket_server.recv(message_socket_server) raise exception:\n" + e.get_description());
      };
	
	
      this->compare_recived_and_sended_messages(message_socket_server, 
                                                                                  message_socket_client);

	
		delete(accepted_socket_server);
		accepted_socket_server=NULL;

       if (socket_server!=NULL) {
             delete (socket_server);
              socket_server=NULL;
       };


    try {
		socket_client->send(message_socket_client);
	} catch (socket_exception e){
		CPPUNIT_FAIL("\naccepted_socket_server.recv(message_socket_server) raise exception:\n" + e.get_description());
	};

};

void client_server_sockets_test::test_send_client_exit(){
	vector<byte> message_socket_client, message_socket_server, recieved_message_header(1);
    port++;
	if (socket_client!=NULL) {
	 delete (socket_client);
	 socket_client=NULL;
	 };
	if (socket_server!=NULL) {
	 delete (socket_server);
	 socket_server=NULL;
	 };
	if (accepted_socket_server!=NULL) {
	      delete (accepted_socket_server);
	      accepted_socket_server=NULL;
	 };

	socket_client = new client_socket(
                                             host,
                                             port,
                                            connect_timeout,
                                            recv_send_timeout);

	socket_server = new server_socket(
                                              recv_send_timeout,
                                              backlog,
                                              port);

	try {
       socket_server->initialize();
	}
	catch (socket_exception e){
				CPPUNIT_FAIL("\nsocket_server.initialize() raise exception:\n" + e.get_description());
	};

	try {
        socket_client->initialize();
	}
	catch (socket_exception e){
				CPPUNIT_FAIL("\nsocket_client.initialize() raise exception:\n" + e.get_description());
	};

	message_socket_client.push_back(4);
	message_socket_client.push_back(21);
	message_socket_client.push_back(22);
	message_socket_client.push_back(23);

	try {
		socket_client->send(message_socket_client);
	}
	catch (socket_exception e){
			CPPUNIT_FAIL("\nsocket_client.send(message_socket_client) raise exception:\n" + e.get_description());
	};

     try {
        accepted_socket_server=socket_server->accept();
     } 	catch (socket_exception e){
             CPPUNIT_FAIL("\nsocket_server.accept(accepted_socket_server) raise exception:\n" + e.get_description());
      };

     try {
         accepted_socket_server->recv_peek(recieved_message_header);
      }	catch (socket_exception e) {
            CPPUNIT_FAIL("\naccepted_socket_server.recv_peek(recieved_message_header) raise exception:\n" + e.get_description());
      };

      message_socket_server.resize(recieved_message_header[0]);
	
      try {
          accepted_socket_server->recv(message_socket_server);
      }	catch (socket_exception e) {
		CPPUNIT_FAIL("\naccepted_socket_server.recv(message_socket_server) raise exception:\n" + e.get_description());
      };

      this->compare_recived_and_sended_messages(message_socket_server, 
                                                                                message_socket_client);

	if (socket_client!=NULL) {
	 delete (socket_client);
	 socket_client=NULL;
	 };

	message_socket_server.clear();
	message_socket_server.push_back(3);
	message_socket_server.push_back(31);
	message_socket_server.push_back(32);

	try {
        accepted_socket_server->send(message_socket_server);
	}
	catch (socket_exception e){
		CPPUNIT_FAIL("\naccepted_socket_server.recv(message_socket_server) raise exception:\n" + e.get_description());
	};

};



CppUnit::TestSuite* client_server_sockets_test::suite(){
CppUnit::TestSuite *suite_of_tests=new CppUnit::TestSuite( "Client and Server Sockets Objects Tests");

suite_of_tests->addTest(new CppUnit::TestCaller<client_server_sockets_test> 
												( "test of client and server sockets",
													&client_server_sockets_test::test_client_server_sockets)
										);

suite_of_tests->addTest(new CppUnit::TestCaller<client_server_sockets_test> 
												( "test of abnormal server exit on send",
													&client_server_sockets_test::test_send_server_exit)
										);

suite_of_tests->addTest(new CppUnit::TestCaller<client_server_sockets_test> 
												( "test of abnormal client exit on send",
													&client_server_sockets_test::test_send_client_exit)
										);

suite_of_tests->addTest(new CppUnit::TestCaller<client_server_sockets_test> 
												( "test of client and server sockets",
													&client_server_sockets_test::test_client_server_sockets)
										);

suite_of_tests->addTest(new CppUnit::TestCaller<client_server_sockets_test> 
												( "test of abnormal server exit on recieve",
													&client_server_sockets_test::test_recieve_server_exit)
										);

suite_of_tests->addTest(new CppUnit::TestCaller<client_server_sockets_test> 
												( "test of abnormal client exit on recieve",
													&client_server_sockets_test::test_recieve_client_exit)
										);

suite_of_tests->addTest(new CppUnit::TestCaller<client_server_sockets_test> 
												( "test of fail connect to server",
													&client_server_sockets_test::test_fail_to_connect)
										);

return suite_of_tests;
};
 