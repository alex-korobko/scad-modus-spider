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
#include "../../metro_devices_classes/data_block.h"

#include "../../containers_classes/msg_dict_container.h"
#include "../../metro_devices_classes/devices_types.h"
#include "../../metro_devices_classes/metro_device.h"

#include "metro_device_test.h"

void metro_device_test::test_metro_device_object() {
}

void metro_device_test::setUp() {
}

void metro_device_test::tearDown(){
}

CppUnit::TestSuite* metro_device_test::suite()
{
CppUnit::TestSuite *suite_of_tests=new CppUnit::TestSuite( "Metro Device Object Tests");

suite_of_tests->addTest(new CppUnit::TestCaller<metro_device_test> 
		    	( "test of metro device object",
			   &metro_device_test::test_metro_device_object)
			);

return suite_of_tests;
}
