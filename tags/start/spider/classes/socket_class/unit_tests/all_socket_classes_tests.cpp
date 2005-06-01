using namespace std;

#include <cppunit/ui/text/TestRunner.h> 
#include <cppunit/TestFixture.h> 
#include <cppunit/TestSuite.h> 

#include <string>
#include <vector>
#include <map>
#include <functional>

#include <netinet/in.h>
#include <sys/time.h>

#include "../system_settings_classes/defines.h"
#include "../socket_classes/socket_exception.h"
#include "../socket_classes/socket.h"
#include "../socket_classes/client_socket.h"
#include "../socket_classes/server_socket.h"

#include "socket_classes_unit_tests/socket_test.h"
#include "socket_classes_unit_tests/client_server_sockets_test.h"
#include "all_socket_classes_tests.h"

all_socket_classes_tests::all_socket_classes_tests(){
    runner.addTest(socket_test::suite());
    runner.addTest(client_server_sockets_test::suite());
}
    
void all_socket_classes_tests::run_tests() {
    all_socket_classes_tests suite_of_all_tests = all_socket_classes_tests();
    suite_of_all_tests.runner.run("", true);
}
