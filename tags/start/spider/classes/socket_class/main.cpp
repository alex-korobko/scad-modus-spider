using namespace std;

#include "test_cond.h"

#include <netinet/in.h>
#include <sys/time.h>
#include <iostream.h>
#include <signal.h>

#include <string>
#include <vector>
#include <map>
#include <functional>

#include "system_settings_classes/defines.h"
#include "socket_classes/socket_exception.h"
#include "socket_classes/socket.h"
#include "socket_classes/client_socket.h"
#include "socket_classes/server_socket.h"

#ifdef __UNIT_TESTING__

#include <cppunit/TestFixture.h> 
#include <cppunit/TestSuite.h> 
#include <cppunit/TestCaller.h>
#include <cppunit/ui/text/TestRunner.h> 

#include "unit_tests/socket_classes_unit_tests/socket_test.h"
#include "unit_tests/socket_classes_unit_tests/client_server_sockets_test.h"
#include "unit_tests/all_socket_classes_tests.h"

#endif

sig_atomic_t SigpipeCount;

void SigpipeHandler (int SignalNumber)
{
++SigpipeCount;
};


void main (int argc , char* argv[]) 
{
struct sigaction sa_sig_pipe;
memset (&sa_sig_pipe, 0, sizeof(sa_sig_pipe));
sa_sig_pipe.sa_handler=&SigpipeHandler;

sigaction(SIGPIPE,&sa_sig_pipe,NULL);


#ifdef __UNIT_TESTING__
all_socket_classes_tests::run_tests();
#endif

cout<< "\nRecieved "<<SigpipeCount<< " SIGPIPE`s"<<endl;
}
