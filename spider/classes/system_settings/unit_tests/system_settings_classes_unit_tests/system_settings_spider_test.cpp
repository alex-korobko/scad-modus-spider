using namespace std;

#include <cppunit/ui/text/TestRunner.h> 
#include <cppunit/TestCaller.h> 
#include <cppunit/TestFixture.h> 
#include <cppunit/TestSuite.h> 
#include <cppunit/TestAssert.h>

#include <Pt.h>
#include <Ph.h>
#include <Ap.h>

#include <sys/types.h>
#include <sys/neutrino.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>

#include <string>
#include <vector>

#include "../../system_settings_classes/defines.h"
#include "../../system_settings_classes/spider_exception.h"
#include "../../system_settings_classes/system_settings.h"
#include "../../system_settings_classes/system_settings_spider.h"

#include "system_settings_spider_test.h"

void system_settings_spider_test::test_simplest_metods(){
	CPPUNIT_ASSERT(tested_system_settings->get_widgets_dbase()==NULL);
	CPPUNIT_ASSERT(tested_system_settings->get_main_window()== NULL);

	CPPUNIT_ASSERT(tested_system_settings->get_start_wav_file_name().compare("sounds/wp_start.wav")==0);
};

void system_settings_spider_test::test_messages_windows(){
	ApDBase_t *widgets_db;
	ostringstream exception_description;
	string path_to_dbase("../wgt/interface.wgtp");

	try {
		 widgets_db=ApOpenDBaseFile( path_to_dbase.c_str());
		 if (widgets_db==NULL) {
		 	exception_description<<"Widgets dbase file "<<path_to_dbase<<" can`t be opened.";
		 	throw spider_exception(exception_description.str());
		 };
		tested_system_settings->prepare_visualization(widgets_db);		 
		CPPUNIT_ASSERT(tested_system_settings->get_widgets_dbase()==widgets_db);
	} catch (spider_exception spr_exc) {
		CPPUNIT_FAIL(spr_exc.get_description());
	};

			tested_system_settings->message_window(
             system_settings::DEBUG_MSG,
             "Debug message");

			tested_system_settings->message_window(
             system_settings::ERROR_MSG,
             "Error message");

			tested_system_settings->message_window(
             system_settings::INFO_MSG,
             "Info message");

			tested_system_settings->message_window(
             system_settings::WARN_MSG,
             "Warning message");

			tested_system_settings->message_window(
             system_settings::DEBUG_MSG,
             "Debug message",
             true);

			tested_system_settings->message_window(
             system_settings::ERROR_MSG,
             "Error message",
             true);

			tested_system_settings->message_window(
             system_settings::INFO_MSG,
             "Info message",
             true);

			tested_system_settings->message_window(
             system_settings::WARN_MSG,
             "Warning message",
             true);

};


CppUnit::TestSuite* system_settings_spider_test::suite() {
CppUnit::TestSuite *suite_of_tests=new CppUnit::TestSuite( "System Settings Spider Object Tests");

suite_of_tests->addTest(new CppUnit::TestCaller<system_settings_spider_test> 
		( "system_settings_spider_test::test_simplest_metods",
		&system_settings_spider_test::test_simplest_metods)
		);

suite_of_tests->addTest(new CppUnit::TestCaller<system_settings_spider_test> 
		( "system_settings_spider_test::test_messages_windows",
		&system_settings_spider_test::test_messages_windows)
		);


return suite_of_tests;
}
