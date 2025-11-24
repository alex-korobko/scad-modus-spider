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
#include "../../metro_devices_classes/escalator_data_block.h"

#include "../../containers_classes/msg_dict_container.h"
#include "../../metro_devices_classes/devices_types.h"
#include "../../metro_devices_classes/metro_device.h"
#include "../../metro_devices_classes/escalator.h"

#include "escalator_test.h"

void escalator_test::setUp() {
 escalator_id=1;
 station_id=2;
 number=3;
 type_id=3;
 pref_direction=system_settings::DIRECTION_UP;
 direction=system_settings::DIRECTION_DOWN;
 start_day_mode=system_settings::START_DAY_MODE_EVEN;
 start_hour=05;
 start_minute=10;
 channel=32;
  enabled=true;
  ip=  inet_addr("192.168.0.200");
  
}

void escalator_test::tearDown(){
	if (tested_escalator!=NULL) 	delete (tested_escalator);
}


void escalator_test::test_escalator_object() {
	ostringstream description;
	try {
   	tested_escalator = new metro_escalator(
                                         escalator_id,
                                         station_id,
                                         number,
                                         type_id,
                                         pref_direction,
                                         direction,
                                         start_day_mode,
                                         start_hour,
                                         start_minute,
                                         enabled,
                                         ip,
                                         channel);
 	} catch (spider_exception spr_exc) {
		description<<"Exception raised : "<<spr_exc.get_description();
        CPPUNIT_FAIL(description.str());
 	};
  
}


CppUnit::TestSuite* escalator_test::suite()
{
CppUnit::TestSuite *suite_of_tests=new CppUnit::TestSuite( "Escalator Object Tests");

suite_of_tests->addTest(new CppUnit::TestCaller<escalator_test> 
		    	( "test of escalator object",
			   &escalator_test::test_escalator_object)
			);

return suite_of_tests;
}
