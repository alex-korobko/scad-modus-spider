#ifndef __METRO_ESCALATORS_CONT_TEST__
#define __METRO_ESCALATORS_CONT_TEST__

class metro_escalators_container_test : public CppUnit::TestFixture {
private:
 string file_name;
 
void test_insert()
{

g_escalators.insert(g_escalators.end(), 
							metro_escalators_container::pair_metro_escalators(3, 
																					metro_escalator(
																								&g_system_settings,
																								3, 
																								2,
																								1,
																								1,
																								system_settings::DIRECTION_UP,
																								system_settings::DIRECTION_UP,
																								system_settings::ENABLED,
																								inet_addr("192.168.0.3")
																								)

																										)
						);

g_escalators.insert(g_escalators.begin(), metro_escalators_container::pair_metro_escalators(1, 
																					metro_escalator(
																								&g_system_settings,
																								1, 
																								1,
																								2,
																								1,
																								system_settings::DIRECTION_UP,
																								system_settings::DIRECTION_UP,
																								system_settings::ENABLED,
																								inet_addr("192.168.0.3")
																								)

																										)
						);

g_escalators.insert(g_escalators.upper_bound(2), metro_escalators_container::pair_metro_escalators(2, 
																					metro_escalator(
																								&g_system_settings,
																								2, 
																								1,
																								3,
																								2,
																								system_settings::DIRECTION_UP,
																								system_settings::DIRECTION_UP,
																								system_settings::ENABLED,
																								inet_addr("192.168.0.3")
																								)

																										)
						);


};


void test_erase()
{
metro_escalators_container::iterator_metro_escalators tmp_iter;

CPPUNIT_ASSERT(g_escalators.size()==3);

tmp_iter=g_escalators.find(1);
CPPUNIT_ASSERT( tmp_iter!=g_escalators.end());
g_escalators.erase(tmp_iter);

tmp_iter=g_escalators.find(2);
CPPUNIT_ASSERT( tmp_iter!=g_escalators.end());
g_escalators.erase(tmp_iter);

tmp_iter=g_escalators.find(3);
CPPUNIT_ASSERT( tmp_iter!=g_escalators.end());
g_escalators.erase(tmp_iter);

CPPUNIT_ASSERT(g_escalators.empty());
CPPUNIT_ASSERT(g_escalators.size()==0);
};

void test_load()
{

//ATTENTION  - all of that escalators must be in file_name !!!
/*
#example of right file_name file
[escalator]
id=1
stationID=1
type=1
enabled=disabled
num=1
ip=192.168.0.1
direction=stop
predef_direction=up

[escalator]
id=2
stationID=1
type=3
enabled=enabled
num=2
ip=192.168.0.2
direction=up
predef_direction=down

[escalator]
id=3
stationID=2
type=2
enabled=enabled
num=1
ip=192.168.0.3
direction=down
predef_direction=reverse
*/

metro_escalators_container tmp_esc_container(&g_system_settings);

tmp_esc_container.insert(tmp_esc_container.end(), 
							metro_escalators_container::pair_metro_escalators(3, 
																					metro_escalator(
																								&g_system_settings,
																								3, 
																								2,
																								2,
																								1,
																								system_settings::DIRECTION_REVERSE,
																								system_settings::DIRECTION_DOWN,
																								system_settings::ENABLED,
																								inet_addr("192.168.0.3")
																								)

																										)
						);

tmp_esc_container.insert(tmp_esc_container.begin(), metro_escalators_container::pair_metro_escalators(1, 
																					metro_escalator(
																								&g_system_settings,
																								1, 
																								1,
																								1,
																								1,
																								system_settings::DIRECTION_UP,
																								system_settings::DIRECTION_STOP,
																								system_settings::DISABLED,
																								inet_addr("192.168.0.1")
																								)

																										)
						);

tmp_esc_container.insert(tmp_esc_container.upper_bound(2), metro_escalators_container::pair_metro_escalators(2, 
																					metro_escalator(
																								&g_system_settings,
																								2, 
																								1,
																								3,
																								2,
																								system_settings::DIRECTION_DOWN,
																								system_settings::DIRECTION_UP,
																								system_settings::ENABLED,
																								inet_addr("192.168.0.2")
																								)

																										)
						);

	CPPUNIT_ASSERT(g_escalators.load(&g_stations, file_name));
	CPPUNIT_ASSERT(tmp_esc_container.size()==g_escalators.size());
	metro_escalators_container::iterator_metro_escalators tmp_iter1=g_escalators.begin(), tmp_iter2;
	while (tmp_iter1!=g_escalators.end())
	{
		tmp_iter2=tmp_esc_container.find (tmp_iter1->second.get_id());
		CPPUNIT_ASSERT(tmp_iter2!=tmp_esc_container.end());

		CPPUNIT_ASSERT(tmp_iter2->second.get_id()==tmp_iter1->second.get_id());
		CPPUNIT_ASSERT(tmp_iter2->second.get_station_id()==tmp_iter1->second.get_station_id());
		CPPUNIT_ASSERT(tmp_iter2->second.get_type()==tmp_iter1->second.get_type());
		CPPUNIT_ASSERT(tmp_iter2->second.get_number()==tmp_iter1->second.get_number());
		CPPUNIT_ASSERT(tmp_iter2->second.get_pref_direction()==tmp_iter1->second.get_pref_direction());
		CPPUNIT_ASSERT(tmp_iter2->second.get_direction()==tmp_iter1->second.get_direction());
		CPPUNIT_ASSERT(tmp_iter2->second.get_enabled()==tmp_iter1->second.get_enabled());
		CPPUNIT_ASSERT(tmp_iter2->second.get_ip()==tmp_iter1->second.get_ip());

		tmp_iter1++;
	}

};

 
public:
void setUp()
{
	file_name="../unit_tests/escalators_test.ini";
}
	
void tearDown()
{
}

static CppUnit::TestSuite *suite()
{
CppUnit::TestSuite *suite_of_tests=new CppUnit::TestSuite( "Tests for Container of Metro Escalators");

suite_of_tests->addTest(new CppUnit::TestCaller<metro_escalators_container_test> 
												( "insert metod test",
													&metro_escalators_container_test::test_insert)
										);

suite_of_tests->addTest(new CppUnit::TestCaller<metro_escalators_container_test> 
												( "erase metod test",
													&metro_escalators_container_test::test_erase)
										);

suite_of_tests->addTest(new CppUnit::TestCaller<metro_escalators_container_test> 
												( "load metod test",
													&metro_escalators_container_test::test_load)
										);


return suite_of_tests;
}
};

#endif