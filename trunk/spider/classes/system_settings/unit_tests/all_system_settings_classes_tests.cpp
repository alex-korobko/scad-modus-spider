using namespace std;

#include <cppunit/ui/text/TestRunner.h> 
#include <cppunit/TestFixture.h> 
#include <cppunit/TestSuite.h> 
#include <cppunit/CompilerOutputter.h> 

#include <sys/types.h>
#include <sys/neutrino.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>

#include <string>
#include <vector>
#include <sstream>

#include <Pt.h>
#include <Ph.h>
#include <Ap.h>

#include "../system_settings_classes/defines.h"
#include "../system_settings_classes/system_settings.h"
#include "../system_settings_classes/spider_exception.h"
#include "../system_settings_classes/system_settings_spider.h"

#include "system_settings_classes_unit_tests/system_settings_test.h"
#include "system_settings_classes_unit_tests/system_settings_spider_test.h"
#include "all_system_settings_classes_tests.h"

 all_system_settings_tests::all_system_settings_tests(){
    runner.addTest(system_settings_test::suite());
    runner.addTest(system_settings_spider_test::suite());
 }
    
string  all_system_settings_tests::run_tests() {
    ostringstream report_of_results;
    all_system_settings_tests suite_of_all_tests ;
    
	suite_of_all_tests.runner.setOutputter( CppUnit::CompilerOutputter::defaultOutputter( 
                          &suite_of_all_tests.runner.result(),
                          report_of_results) );
                          
 suite_of_all_tests.runner.run("", false, true, true);

 return report_of_results.str();
}
