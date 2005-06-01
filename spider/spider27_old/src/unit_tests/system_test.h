#ifndef __SYSTEM_TEST__
#define __SYSTEM_TEST__
class system_test : public CppUnit::TestFixture {
private:

void test_prepare_visualization()
{
	ApDBase_t *widgets_dbase=ApOpenDBase(ABM_interface);
	CPPUNIT_ASSERT(widgets_dbase!=NULL);
  	g_system_settings.prepare_visualization(widgets_dbase);	    
}

void test_sys_message()
{
	int msg_type=0;
	while(msg_type<system_settings::MESSAGES_COUNT)
	{
	g_system_settings.sys_message(msg_type, 
														string("System : messages types test"));
	msg_type++;
	}

}

void test_message_window()
{
	int msg_type=0;
	while(msg_type<system_settings::MESSAGES_COUNT)
	{
	g_system_settings.message_window(msg_type, 
														string("System : messages types test"));
	msg_type++;
	}

}

void test_simplest_metods()
{
	word input_word=357, output_word=0;
	system_settings::bytes tmp_bytes=g_system_settings.bytes_of_type<word> (input_word);
	output_word=g_system_settings.type_from_bytes<word>(tmp_bytes);
	CPPUNIT_ASSERT( input_word==output_word);


	time_t input_time=time(NULL), output_time=0;
	tmp_bytes=g_system_settings.bytes_of_type<time_t>(input_time);
	output_time=g_system_settings.type_from_bytes<time_t>(tmp_bytes);
	CPPUNIT_ASSERT( input_time==output_time);
}


public:
void setUp()
{
}
	
void tearDown()
{
}

static CppUnit::TestSuite *suite()
{
CppUnit::TestSuite *suite_of_tests=new CppUnit::TestSuite( "System Settings Object Tests");

suite_of_tests->addTest(new CppUnit::TestCaller<system_test> 
												( "system_settings::prepare_visualization",
													&system_test::test_prepare_visualization)
										);
suite_of_tests->addTest(new CppUnit::TestCaller<system_test> 
												( "system_settings::test_sys_message",
													&system_test::test_sys_message)
										);

suite_of_tests->addTest(new CppUnit::TestCaller<system_test> 
												( "system_settings::test_sys_message",
													&system_test::test_message_window)
										);

suite_of_tests->addTest(new CppUnit::TestCaller<system_test> 
												( "system_settings::test_simplest_metods",
													&system_test::test_simplest_metods)
										);


return suite_of_tests;
}
};

#endif