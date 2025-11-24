using namespace std;

#include <cppunit/ui/text/TestRunner.h> 
#include <cppunit/TestFixture.h> 
#include <cppunit/TestSuite.h> 

#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <string>
#include <map>
#include <functional>
#include <vector>

#include "../system_settings_classes/defines.h"
#include "../com_port_class/comport_exception.h"
#include "../com_port_class/comport.h"

#include "test_comport.h"
#include "all_tests.h"

all_tests::all_tests()
 {
    runner.addTest(test_comport::suite());

 }
    
void all_tests::run_tests() 
    {
    all_tests suite_of_all_tests ;
    suite_of_all_tests.runner.run("", true);
    }
