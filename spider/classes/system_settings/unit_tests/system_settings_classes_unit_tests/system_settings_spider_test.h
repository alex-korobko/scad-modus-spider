#ifndef __SYSTEM_SETTINGS_SPIDER_TEST__
#define __SYSTEM_SETTINGS_SPIDER_TEST__

class system_settings_spider_test : public CppUnit::TestFixture {
private:
system_settings_spider *tested_system_settings;

void test_simplest_metods();
void test_messages_windows();
public:
void setUp()
{
tested_system_settings = system_settings_spider::get_instance();
}
	
void tearDown(){
}

static CppUnit::TestSuite *suite();
};

#endif
