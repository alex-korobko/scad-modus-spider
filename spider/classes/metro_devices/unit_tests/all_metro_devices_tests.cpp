using namespace std;
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <sstream>

#include <Ap.h>
#include <Pt.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/neutrino.h>
#include <sys/syspage.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <inttypes.h>

#include <cppunit/ui/text/TestRunner.h> 
#include <cppunit/TestFixture.h> 
#include <cppunit/TestSuite.h> 
#include <cppunit/CompilerOutputter.h> 

#include "../system_settings_classes/defines.h"
#include "../system_settings_classes/spider_exception.h"
#include "../system_settings_classes/system_settings.h"
#include "../system_settings_classes/system_settings_spider.h"
#include "../metro_devices_classes/command.h"
#include "../metro_devices_classes/data_block.h"
#include "../metro_devices_classes/escalator_data_block.h"
#include "../metro_devices_classes/shavr_data_block.h"

#include "../containers_classes/msg_dict_container.h"
#include "../metro_devices_classes/devices_types.h"
#include "../metro_devices_classes/metro_device.h"
#include "../metro_devices_classes/escalator.h"
#include "../metro_devices_classes/shavr.h"

#include "metro_devices_classes_unit_tests/command_test.h"
#include "metro_devices_classes_unit_tests/metro_device_test.h"
#include "metro_devices_classes_unit_tests/escalator_test.h"
#include "metro_devices_classes_unit_tests/shavr_test.h"

#include "all_metro_devices_tests.h"

 all_tests_metro_devices::all_tests_metro_devices(){
    runner.addTest(command_test::suite());
    runner.addTest(metro_device_test::suite());
    runner.addTest(escalator_test::suite());
    runner.addTest(shavr_test::suite());
 }

string  all_tests_metro_devices::run_tests()  {
    ostringstream report_of_results;
    all_tests_metro_devices suite_of_all_tests = all_tests_metro_devices();
    
	suite_of_all_tests.runner.setOutputter( CppUnit::CompilerOutputter::defaultOutputter( 
                          &suite_of_all_tests.runner.result(),
                          report_of_results) );
                     
 suite_of_all_tests.runner.run("", false, true, true);

 return report_of_results.str();
 }
   


