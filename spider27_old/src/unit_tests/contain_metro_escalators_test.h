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
																								system_settings::START_DAY_MODE_EVEN,
																								system_settings::START_HOUR_MAX,
																								59,
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
																								system_settings::START_DAY_MODE_ODD,
																								system_settings::START_HOUR_MIN,
																								58,
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
																								system_settings::START_DAY_MODE_ODD,
																								system_settings::START_HOUR_MIN,
																								58,
																								system_settings::ENABLED,
																								inet_addr("192.168.0.3")
																								)

																										)
						);


};


void test_erase()
{
metro_escalators_container::iterator_metro_escalators tmp_iter;
metro_escalators_container::size_type_metro_escalators old_size=g_escalators.size();

CPPUNIT_ASSERT(old_size==3);
CPPUNIT_ASSERT(!g_escalators.empty());
tmp_iter=g_escalators.find(1);
CPPUNIT_ASSERT( tmp_iter!=g_escalators.end());
g_escalators.erase(tmp_iter);
CPPUNIT_ASSERT(old_size==g_escalators.size()+1);
CPPUNIT_ASSERT(!g_escalators.empty());

g_escalators.erase(g_escalators.begin(),
								g_escalators.end());

CPPUNIT_ASSERT(g_escalators.empty());
CPPUNIT_ASSERT(g_escalators.size()==0);
};

void test_save_load()
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
direction=up
predef_direction=up
start day mode=even
start hour=5
start minute=0


[escalator]
id=2
stationID=1
type=3
enabled=enabled
num=2
ip=192.168.0.2
direction=up
predef_direction=down
start day mode=everyday
start hour=4
start minute=52

[escalator]
id=3
stationID=2
type=2
enabled=enabled
num=1
ip=192.168.0.3
direction=down
predef_direction=reverse
start day mode=never
start hour=5
start minute=12

[escalator]
id=4
stationID=2
type=2
enabled=enabled
num=1
ip=192.168.0.4
direction=down
predef_direction=up
start day mode=odd
start hour=5
start minute=05

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
																								system_settings::START_DAY_MODE_NEVER,
																								5,
																								12,
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
																								system_settings::DIRECTION_UP,
																								system_settings::START_DAY_MODE_EVEN,
																								5,
																								0,
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
																								system_settings::START_DAY_MODE_EVERYDAY,
																								4,
																								52,
																								system_settings::ENABLED,
																								inet_addr("192.168.0.2")
																								)

																										)
						);

tmp_esc_container.insert(tmp_esc_container.end(), 
							metro_escalators_container::pair_metro_escalators(4, 
																					metro_escalator(
																								&g_system_settings,
																								4, 
																								2,
																								2,
																								1,
																								system_settings::DIRECTION_UP,
																								system_settings::DIRECTION_DOWN,
																								system_settings::START_DAY_MODE_ODD,
																								5,
																								05,
																								system_settings::ENABLED,
																								inet_addr("192.168.0.4")
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

	//test escalators id in stations container
	
	//test station with id 1 it may contain two escalators id  - with id 1 and 2
	metro_stations_container::iterator_metro_stations iter_stat=g_stations.find(1);
	CPPUNIT_ASSERT(iter_stat!=g_stations.end());
	CPPUNIT_ASSERT(iter_stat->second.size_escalators_id()==2);
	metro_station::iterator_escalators_id iter_esc_id=iter_stat->second.begin_escalators_id();
	CPPUNIT_ASSERT(*iter_esc_id==1);
	iter_esc_id++;
	CPPUNIT_ASSERT(*iter_esc_id==2);

	iter_stat=g_stations.find(2);
	CPPUNIT_ASSERT(iter_stat!=g_stations.end());
	CPPUNIT_ASSERT(iter_stat->second.size_escalators_id()==2);
	iter_esc_id=iter_stat->second.begin_escalators_id();
	CPPUNIT_ASSERT(*iter_esc_id==3);
	iter_esc_id++;
	CPPUNIT_ASSERT(iter_esc_id!=iter_stat->second.end_escalators_id());
	CPPUNIT_ASSERT(*iter_esc_id==4);

	iter_stat=g_stations.find(3);
	CPPUNIT_ASSERT(iter_stat!=g_stations.end());
	CPPUNIT_ASSERT(iter_stat->second.size_escalators_id()==0);
	CPPUNIT_ASSERT(iter_stat->second.empty_escalators_id());

/*
save metod testing 
*/
	int first_start_hour, second_start_hour, 
		 first_start_minute, second_start_minute,
		 first_start_day_mode, second_start_day_mode, 
		 first_direction, second_direction,
		 first_pref_direction, second_pref_direction;

	tmp_iter1=g_escalators.begin();
	CPPUNIT_ASSERT(tmp_iter1!=tmp_esc_container.end());
	
	first_start_hour=tmp_iter1->second.get_start_hour();
	second_start_hour=first_start_hour+1;
	tmp_iter1->second.set_start_hour(second_start_hour);

	first_start_minute=tmp_iter1->second.get_start_minute();
	second_start_minute=first_start_minute+1;
	tmp_iter1->second.set_start_minute(second_start_minute);

	first_start_day_mode=tmp_iter1->second.get_start_day_mode();
	second_start_day_mode=system_settings::START_DAY_MODE_NEVER;
	if (first_start_day_mode==second_start_day_mode) 
				second_start_day_mode=system_settings::START_DAY_MODE_EVERYDAY;
	tmp_iter1->second.set_start_day_mode(second_start_day_mode);

	first_direction=tmp_iter1->second.get_direction();
	second_direction=system_settings::DIRECTION_DOWN;
	if (first_direction==second_direction) 
				second_direction=system_settings::DIRECTION_UP;
	tmp_iter1->second.set_direction(second_direction);

	first_pref_direction=tmp_iter1->second.get_pref_direction();
	second_pref_direction=system_settings::DIRECTION_DOWN;
	if (first_pref_direction==second_pref_direction) 
				second_pref_direction=system_settings::DIRECTION_UP;
	tmp_iter1->second.set_pref_direction(second_pref_direction);
	
	CPPUNIT_ASSERT(g_escalators.save( file_name));
	CPPUNIT_ASSERT(g_escalators.load(&g_stations, file_name));	
	tmp_iter1=g_escalators.begin();	
	CPPUNIT_ASSERT(tmp_iter1!=tmp_esc_container.end());

	CPPUNIT_ASSERT(second_start_hour==tmp_iter1->second.get_start_hour());	
	CPPUNIT_ASSERT(second_start_minute==tmp_iter1->second.get_start_minute());	
	CPPUNIT_ASSERT(second_start_day_mode==tmp_iter1->second.get_start_day_mode());	
	CPPUNIT_ASSERT(second_direction==tmp_iter1->second.get_direction());	
	CPPUNIT_ASSERT(second_pref_direction==tmp_iter1->second.get_pref_direction());	

	tmp_iter1->second.set_start_hour(first_start_hour);
	tmp_iter1->second.set_start_minute(first_start_minute);
	tmp_iter1->second.set_start_day_mode(first_start_day_mode);
	tmp_iter1->second.set_direction(first_direction);
	tmp_iter1->second.set_pref_direction(first_pref_direction);
	CPPUNIT_ASSERT(g_escalators.save( file_name));		
};

void test_creating_morning_start()
{
time_t time_t_now=time(NULL);
struct tm time_tm_now;

localtime_r(&time_t_now, &time_tm_now);
										
contain_morning_start tmp_morn_start;
contain_morning_start::iterator_morining_start iter_morn_start;

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
																								system_settings::START_DAY_MODE_NEVER,
																								time_tm_now.tm_hour,
																								time_tm_now.tm_min,
																								system_settings::ENABLED,
																								inet_addr("192.168.40.3")
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
																								system_settings::DIRECTION_UP,
																								system_settings::START_DAY_MODE_EVEN,
																								time_tm_now.tm_hour,
																								time_tm_now.tm_min,
																								system_settings::DISABLED,
																								inet_addr("192.168.40.1")
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
																								system_settings::START_DAY_MODE_EVERYDAY,
																								time_tm_now.tm_hour,
																								time_tm_now.tm_min,
																								system_settings::ENABLED,
																								inet_addr("192.168.40.2")
																								)

																										)
						);

tmp_esc_container.insert(tmp_esc_container.end(), 
							metro_escalators_container::pair_metro_escalators(4, 
																					metro_escalator(
																								&g_system_settings,
																								4, 
																								2,
																								2,
																								1,
																								system_settings::DIRECTION_UP,
																								system_settings::DIRECTION_DOWN,
																								system_settings::START_DAY_MODE_ODD,
																								time_tm_now.tm_hour,
																								time_tm_now.tm_min,
																								system_settings::ENABLED,
																								inet_addr("192.168.40.4")
																								)

																										)
						);

tmp_esc_container.insert(tmp_esc_container.end(), 
							metro_escalators_container::pair_metro_escalators(5, 
																					metro_escalator(
																								&g_system_settings,
																								5, 
																								2,
																								2,
																								1,
																								system_settings::DIRECTION_DOWN,
																								system_settings::DIRECTION_DOWN,
																								system_settings::START_DAY_MODE_EVERYDAY,
																								time_tm_now.tm_hour,
																								time_tm_now.tm_min,
																								system_settings::ENABLED,
																								inet_addr("192.168.40.5")
																								)

																										)
						);


metro_station::escalators_id_container escalators_for_morning_start(4);
escalators_for_morning_start[0]=1;
escalators_for_morning_start[1]=2;
escalators_for_morning_start[2]=3;
escalators_for_morning_start[3]=4;

tmp_esc_container.prepare_morning_start(&tmp_morn_start,
																	&escalators_for_morning_start,	
																	&g_system_settings);

CPPUNIT_ASSERT(tmp_morn_start.max_size()==3);
CPPUNIT_ASSERT(tmp_morn_start.size()==3);

iter_morn_start=tmp_morn_start.begin();

CPPUNIT_ASSERT(iter_morn_start->get_escalator_id()==2);
CPPUNIT_ASSERT(iter_morn_start->get_escalator_direction()==system_settings::DIRECTION_UP);
CPPUNIT_ASSERT(iter_morn_start->get_escalator_pref_direction()==system_settings::DIRECTION_DOWN);
CPPUNIT_ASSERT(iter_morn_start->get_start_mode()==system_settings::START_DAY_MODE_EVERYDAY);
CPPUNIT_ASSERT(iter_morn_start->get_start_hour()==time_tm_now.tm_hour);
CPPUNIT_ASSERT(iter_morn_start->get_start_minute()==time_tm_now.tm_min);

iter_morn_start++;
CPPUNIT_ASSERT(iter_morn_start->get_escalator_id()==3);
CPPUNIT_ASSERT(iter_morn_start->get_escalator_direction()==system_settings::DIRECTION_DOWN);
CPPUNIT_ASSERT(iter_morn_start->get_escalator_pref_direction()==system_settings::DIRECTION_REVERSE);
CPPUNIT_ASSERT(iter_morn_start->get_start_mode()==system_settings::START_DAY_MODE_NEVER);
CPPUNIT_ASSERT(iter_morn_start->get_start_hour()==time_tm_now.tm_hour);
CPPUNIT_ASSERT(iter_morn_start->get_start_minute()==time_tm_now.tm_min);

iter_morn_start++;
CPPUNIT_ASSERT(iter_morn_start->get_escalator_id()==4);
CPPUNIT_ASSERT(iter_morn_start->get_escalator_direction()==system_settings::DIRECTION_DOWN);
CPPUNIT_ASSERT(iter_morn_start->get_escalator_pref_direction()==system_settings::DIRECTION_UP);
CPPUNIT_ASSERT(iter_morn_start->get_start_mode()==system_settings::START_DAY_MODE_ODD);
CPPUNIT_ASSERT(iter_morn_start->get_start_hour()==time_tm_now.tm_hour);
CPPUNIT_ASSERT(iter_morn_start->get_start_minute()==time_tm_now.tm_min);

tmp_morn_start.only_starting();
CPPUNIT_ASSERT(tmp_morn_start.max_size()==3);

iter_morn_start=tmp_morn_start.begin();

CPPUNIT_ASSERT(iter_morn_start->get_escalator_id()==2);
CPPUNIT_ASSERT(iter_morn_start->get_escalator_direction()==system_settings::DIRECTION_UP);
CPPUNIT_ASSERT(iter_morn_start->get_escalator_pref_direction()==system_settings::DIRECTION_DOWN);
CPPUNIT_ASSERT(iter_morn_start->get_start_mode()==system_settings::START_DAY_MODE_EVERYDAY);
CPPUNIT_ASSERT(iter_morn_start->get_start_hour()==time_tm_now.tm_hour);
CPPUNIT_ASSERT(iter_morn_start->get_start_minute()==time_tm_now.tm_min);

if (time_tm_now.tm_mday%2==0)  //even day
	{
		CPPUNIT_ASSERT(tmp_morn_start.size()==1);
	} else {	
		CPPUNIT_ASSERT(tmp_morn_start.size()==2);
		
		iter_morn_start++;
		CPPUNIT_ASSERT(iter_morn_start->get_escalator_id()==4);
		CPPUNIT_ASSERT(iter_morn_start->get_escalator_direction()==system_settings::DIRECTION_DOWN);
		CPPUNIT_ASSERT(iter_morn_start->get_escalator_pref_direction()==system_settings::DIRECTION_UP);
		CPPUNIT_ASSERT(iter_morn_start->get_start_mode()==system_settings::START_DAY_MODE_ODD);
		CPPUNIT_ASSERT(iter_morn_start->get_start_hour()==time_tm_now.tm_hour);
		CPPUNIT_ASSERT(iter_morn_start->get_start_minute()==time_tm_now.tm_min);

	};

CPPUNIT_ASSERT(++iter_morn_start==tmp_morn_start.end());

tmp_esc_container.execute_morning_start(&tmp_morn_start,
															time_tm_now.tm_hour,
															time_tm_now.tm_min);

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
												( "save and load metod test",
													&metro_escalators_container_test::test_save_load)
										);

suite_of_tests->addTest(new CppUnit::TestCaller<metro_escalators_container_test> 
												( "test of creating morning start list from escalators container and morning start",
													&metro_escalators_container_test::test_creating_morning_start)
											);


return suite_of_tests;
}
};

#endif