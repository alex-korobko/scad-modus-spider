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
#include <sys/socket.h>
#include <arpa/inet.h>
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
#include "../../metro_devices_classes/shavr_data_block.h"

#include "../../containers_classes/msg_dict_container.h"
#include "../../metro_devices_classes/devices_types.h"
#include "../../metro_devices_classes/metro_device.h"
#include "../../metro_devices_classes/shavr.h"

#include "shavr_test.h"

void shavr_test::setUp() {
 shavr_id=1;
 station_id=2;
 number=3;
 type_id=3;
 channel=32;
 enabled=true;
 ip=  inet_addr("192.168.0.200");
}

void shavr_test::tearDown(){
}


void shavr_test::test_shavr_object() {
  
}

CppUnit::TestSuite* shavr_test::suite() {
CppUnit::TestSuite *suite_of_tests=new CppUnit::TestSuite( "SHAVR Object Tests");

suite_of_tests->addTest(new CppUnit::TestCaller<shavr_test> 
		    	( "test of SHAVR object",
			   &shavr_test::test_shavr_object)
			);

return suite_of_tests;
}
