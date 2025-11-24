#ifndef __SYSTEM_SETTINGS_TEST__
#define __SYSTEM_SETTINGS_TEST__

class system_settings_test : public CppUnit::TestFixture {
private:
system_settings *tested_system_setttings;

void test_sys_message();
void test_simplest_metods();

public:
void setUp()
{
tested_system_setttings = system_settings_spider::get_instance();
}
	
void tearDown(){
}

static CppUnit::TestSuite *suite();
};

#endif
