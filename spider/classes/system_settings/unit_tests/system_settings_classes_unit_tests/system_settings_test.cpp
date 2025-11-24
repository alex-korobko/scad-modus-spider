using namespace std;

#include <cppunit/ui/text/TestRunner.h> 
#include <cppunit/TestCaller.h> 
#include <cppunit/TestFixture.h> 
#include <cppunit/TestSuite.h> 
#include <cppunit/TestAssert.h>

#include <Ap.h>
#include <Pt.h>
#include <Ph.h>

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

#include "system_settings_test.h"

void system_settings_test::test_sys_message()
{
	int msg_type=0;
	while(msg_type<system_settings::MESSAGES_COUNT)
	{
	tested_system_setttings->sys_message(msg_type, 
														string("System : messages types test"));
	msg_type++;
	}

}

void system_settings_test::test_simplest_metods()
{
	byte input_byte=255, output_byte=0;
	system_settings::bytes tmp_bytes=tested_system_setttings->bytes_of_type<byte> (input_byte);
	CPPUNIT_ASSERT( tmp_bytes.size()==1);
	CPPUNIT_ASSERT( tmp_bytes[0]==0xFF);
	output_byte=tested_system_setttings->type_from_bytes<byte>(tmp_bytes);
	CPPUNIT_ASSERT( input_byte==output_byte);

	input_byte=0;
	output_byte=255;
	tmp_bytes=tested_system_setttings->bytes_of_type<byte> (input_byte);
	CPPUNIT_ASSERT( tmp_bytes.size()==1);
	CPPUNIT_ASSERT( tmp_bytes[0]== 0);
	output_byte=tested_system_setttings->type_from_bytes<byte>(tmp_bytes);
	CPPUNIT_ASSERT( input_byte==output_byte);

	word input_word=0x00dc, output_word=0;
	tmp_bytes=tested_system_setttings->bytes_of_type<word> (input_word);
	CPPUNIT_ASSERT( tmp_bytes.size()==2);
	CPPUNIT_ASSERT( tmp_bytes[0]==(input_word & 0x00FF));
	CPPUNIT_ASSERT( tmp_bytes[1]==(input_word & 0xFF00));
	output_word=tested_system_setttings->type_from_bytes<word>(tmp_bytes);
	CPPUNIT_ASSERT( input_word==output_word);

	input_word=0xefdc;
	output_word=0;
	tmp_bytes=tested_system_setttings->bytes_of_type<word> (input_word);
	CPPUNIT_ASSERT( tmp_bytes.size()==2);
	CPPUNIT_ASSERT( tmp_bytes[0]==(input_word & 0x00FF));
	CPPUNIT_ASSERT( tmp_bytes[1]==((input_word & 0xFF00)>>8));
	output_word=tested_system_setttings->type_from_bytes<word>(tmp_bytes);
	CPPUNIT_ASSERT( input_word==output_word);

	time_t input_time=time(NULL), output_time=0;
	tmp_bytes=tested_system_setttings->bytes_of_type<time_t>(input_time);
	output_time=tested_system_setttings->type_from_bytes<time_t>(tmp_bytes);
	CPPUNIT_ASSERT( input_time==output_time);
}


CppUnit::TestSuite* system_settings_test::suite() {
CppUnit::TestSuite *suite_of_tests=new CppUnit::TestSuite( "System Settings Object Tests");

suite_of_tests->addTest(new CppUnit::TestCaller<system_settings_test> 
												( "system_settings_test::test_sys_message",
													&system_settings_test::test_sys_message)
										);

suite_of_tests->addTest(new CppUnit::TestCaller<system_settings_test> 
												( "system_settings_test::test_simplest_metods",
													&system_settings_test::test_simplest_metods)
										);


return suite_of_tests;
}
