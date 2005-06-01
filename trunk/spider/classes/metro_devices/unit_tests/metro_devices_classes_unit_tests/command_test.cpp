using namespace std;

#include <string>
#include <map>
#include <functional>
#include <vector>

#include <Ap.h>
#include <Pt.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/neutrino.h>
#include <sys/syspage.h>
#include <inttypes.h>

#include <cppunit/TestFixture.h> 
#include <cppunit/TestSuite.h> 
#include <cppunit/TestCaller.h>

#include "../../system_settings_classes/defines.h"
#include "../../system_settings_classes/spider_exception.h"
#include "../../system_settings_classes/system_settings.h"
#include "../../system_settings_classes/system_settings_spider.h"
#include "../../metro_devices_classes/command.h"

#include "command_test.h"

void command_test::test_command_object() {
}

void command_test::setUp() {
}

void command_test::tearDown(){
}

CppUnit::TestSuite* command_test::suite()
{
CppUnit::TestSuite *suite_of_tests=new CppUnit::TestSuite( "Command Object Tests");

suite_of_tests->addTest(new CppUnit::TestCaller<command_test> 
		    	( "test of metro command object",
			   &command_test::test_command_object)
			);

return suite_of_tests;
}
