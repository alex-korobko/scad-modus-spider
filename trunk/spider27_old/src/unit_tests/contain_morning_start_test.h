#ifndef __CONTAIN_MORNING_START_TEST__
#define __CONTAIN_MORNING_START_TEST__

class contain_morning_start_test : public CppUnit::TestFixture {
private:
 
void test_simply_metods()
{
	int escalator_id=1, 
	escalator_pref_direction=system_settings::DIRECTION_REVERSE,
	escalator_direction=system_settings::DIRECTION_DOWN,
	start_hour=5, 
	start_minute=17,
	start_mode=system_settings::START_DAY_MODE_EVERYDAY;
	bool start_enabled=true;

	contain_morning_start::size_type_morining_start size_morning_start=g_morning_start.size();
	contain_morning_start::iterator_morining_start iter_morn_start;
	
    CPPUNIT_ASSERT(size_morning_start==0);
    	CPPUNIT_ASSERT(g_morning_start.empty());
 	CPPUNIT_ASSERT(! g_morning_start.get_was_created_in_this_day());

	g_morning_start.insert(g_morning_start.end(),
										escalator_start(escalator_id,
																escalator_pref_direction,
																escalator_direction,
																start_hour,
																start_minute,
																start_mode,
																start_enabled
																)
										);

    CPPUNIT_ASSERT(size_morning_start+1==g_morning_start.size());
    	CPPUNIT_ASSERT(!g_morning_start.empty());

	g_morning_start.set_was_created_in_this_day(true);
 	CPPUNIT_ASSERT(g_morning_start.get_was_created_in_this_day());

 	iter_morn_start=g_morning_start.begin();
    	CPPUNIT_ASSERT(iter_morn_start->get_start_enabled()==start_enabled);
    	CPPUNIT_ASSERT(iter_morn_start->get_start_mode()==start_mode);		
    	CPPUNIT_ASSERT(iter_morn_start->get_start_hour()==start_hour);		
    	CPPUNIT_ASSERT(iter_morn_start->get_start_minute()==start_minute);		    	
    	CPPUNIT_ASSERT(iter_morn_start->get_escalator_id()==escalator_id);	
    	CPPUNIT_ASSERT(iter_morn_start->get_escalator_direction()==escalator_direction);	 
    	CPPUNIT_ASSERT(iter_morn_start->get_escalator_pref_direction()==escalator_pref_direction);	 
 
	g_morning_start.erase(iter_morn_start);
    CPPUNIT_ASSERT(g_morning_start.size()==0);
    	CPPUNIT_ASSERT(g_morning_start.empty());

	g_morning_start.insert(g_morning_start.end(),
										escalator_start(escalator_id,
																escalator_pref_direction,
																escalator_direction,
																start_hour,
																start_minute,
																start_mode,
																start_enabled
																)
										);

	int second_escalator_id=2, 
		second_escalator_pref_direction=system_settings::DIRECTION_UP,
		second_escalator_direction=system_settings::DIRECTION_UP,
		second_start_hour=6,
		second_start_minute=38,
		second_start_mode=system_settings::START_DAY_MODE_NEVER;
		bool second_start_enabled=false;

	g_morning_start.insert(g_morning_start.end(),
										escalator_start(second_escalator_id,
																second_escalator_pref_direction,
																second_escalator_direction,
																second_start_hour,
																second_start_minute,
																second_start_mode,
																second_start_enabled
																)
										);

    CPPUNIT_ASSERT(g_morning_start.size()==2);
    	CPPUNIT_ASSERT(!g_morning_start.empty());

	g_morning_start.erase_all();	

    CPPUNIT_ASSERT(g_morning_start.size()==0);
    	CPPUNIT_ASSERT(g_morning_start.empty());
};


void test_equale_operator()
{
	int escalator_id=1, 
		escalator_pref_direction=system_settings::DIRECTION_UP,
		escalator_direction=system_settings::DIRECTION_DOWN,
		start_hour=5, 
		start_minute=17,
		start_mode=system_settings::START_DAY_MODE_EVERYDAY;
		bool start_enabled=true;

	contain_morning_start::size_type_morining_start size_morning_start=g_morning_start.size();
	
    CPPUNIT_ASSERT(size_morning_start==0);
    	CPPUNIT_ASSERT(g_morning_start.empty());

	g_morning_start.insert(g_morning_start.end(),
										escalator_start(escalator_id,
																escalator_pref_direction,
																escalator_direction,
																start_hour,
																start_minute,
																start_mode,
																start_enabled
																)
										);

	int second_escalator_id=2, 
		second_escalator_pref_direction=system_settings::DIRECTION_DOWN,
		second_escalator_direction=system_settings::DIRECTION_UP,
		second_start_hour=6,
		second_start_minute=38,
		second_start_mode=system_settings::START_DAY_MODE_NEVER;
		bool second_start_enabled=false;

	g_morning_start.insert(g_morning_start.end(),
										escalator_start(second_escalator_id,
																second_escalator_pref_direction,
																second_escalator_direction,
																second_start_hour,
																second_start_minute,
																second_start_mode,
																second_start_enabled
																)
										);


	contain_morning_start temp_morning_start(g_morning_start);
	CPPUNIT_ASSERT(temp_morning_start==g_morning_start);	

	temp_morning_start.erase_all();
	CPPUNIT_ASSERT(!(temp_morning_start==g_morning_start));	
	temp_morning_start=g_morning_start;
	CPPUNIT_ASSERT(temp_morning_start==g_morning_start);	

	temp_morning_start.only_starting();
	CPPUNIT_ASSERT(!(temp_morning_start==g_morning_start));	
	temp_morning_start.sort_all();
	CPPUNIT_ASSERT(temp_morning_start==g_morning_start);	

	contain_morning_start::iterator_morining_start tmp_iter=temp_morning_start.end();
	temp_morning_start.erase(--tmp_iter);
	CPPUNIT_ASSERT(!(temp_morning_start==g_morning_start));		

	tmp_iter=temp_morning_start.insert(temp_morning_start.end(),
															escalator_start(second_escalator_id,
																					second_escalator_pref_direction,
																					second_escalator_direction,
																					second_start_hour,
																					second_start_minute,
																					second_start_mode,
																					second_start_enabled
																					)
														);

	CPPUNIT_ASSERT(temp_morning_start==g_morning_start);		
	temp_morning_start.erase(tmp_iter);

	int tmp_escalator_id=second_escalator_id+10;
	tmp_iter=temp_morning_start.insert(temp_morning_start.end(),
															escalator_start(tmp_escalator_id,
																					second_escalator_pref_direction,
																					second_escalator_direction,
																					second_start_hour,
																					second_start_minute,
																					second_start_mode,
																					second_start_enabled
																					)
														);

	CPPUNIT_ASSERT(!(temp_morning_start==g_morning_start));	
	temp_morning_start.erase(tmp_iter);
	tmp_iter=temp_morning_start.insert(temp_morning_start.end(),
															escalator_start(second_escalator_id,
																					second_escalator_pref_direction,
																					second_escalator_direction,
																					second_start_hour,
																					second_start_minute,
																					second_start_mode,
																					second_start_enabled
																					)
															);

	CPPUNIT_ASSERT(temp_morning_start==g_morning_start);	

	temp_morning_start.erase(tmp_iter);
	int tmp_escalator_pref_direction=system_settings::DIRECTION_REVERSE;
	tmp_iter=temp_morning_start.insert(temp_morning_start.end(),
															escalator_start(second_escalator_id,
																					tmp_escalator_pref_direction,
																					second_escalator_direction,
																					second_start_hour,
																					second_start_minute,
																					second_start_mode,
																					second_start_enabled
																					)
															);

	CPPUNIT_ASSERT(!(temp_morning_start==g_morning_start));	
	temp_morning_start.erase(tmp_iter);
	tmp_iter=temp_morning_start.insert(temp_morning_start.end(),
															escalator_start(second_escalator_id,
																					second_escalator_pref_direction,
																					second_escalator_direction,
																					second_start_hour,
																					second_start_minute,
																					second_start_mode,
																					second_start_enabled
																					)
															);

	CPPUNIT_ASSERT(temp_morning_start==g_morning_start);	

	temp_morning_start.erase(tmp_iter);
	int tmp_escalator_direction=system_settings::DIRECTION_REVERSE;
	tmp_iter=temp_morning_start.insert(temp_morning_start.end(),
															escalator_start(second_escalator_id,
																					second_escalator_pref_direction,
																					tmp_escalator_direction,
																					second_start_hour,
																					second_start_minute,
																					second_start_mode,
																					second_start_enabled
																					)
															);

	CPPUNIT_ASSERT(!(temp_morning_start==g_morning_start));	

	temp_morning_start.erase(tmp_iter);
	tmp_iter=temp_morning_start.insert(temp_morning_start.end(),
															escalator_start(second_escalator_id,
																					second_escalator_pref_direction,
																					second_escalator_direction,
																					second_start_hour,
																					second_start_minute,
																					second_start_mode,
																					second_start_enabled
																					)
															);

	CPPUNIT_ASSERT(temp_morning_start==g_morning_start);	
	
	int tmp_start_hour=second_start_hour+1;
	temp_morning_start.erase(tmp_iter);
	tmp_iter=temp_morning_start.insert(temp_morning_start.end(),
															escalator_start(second_escalator_id,
																					second_escalator_pref_direction,
																					second_escalator_direction,
																					tmp_start_hour,
																					second_start_minute,
																					second_start_mode,
																					second_start_enabled
																					)
															);

	CPPUNIT_ASSERT(!(temp_morning_start==g_morning_start));	

	temp_morning_start.erase(tmp_iter);
	tmp_iter=temp_morning_start.insert(temp_morning_start.end(),
															escalator_start(second_escalator_id,
																					second_escalator_pref_direction,
																					second_escalator_direction,
																					second_start_hour,
																					second_start_minute,
																					second_start_mode,
																					second_start_enabled
																					)
															);

	CPPUNIT_ASSERT(temp_morning_start==g_morning_start);	

	int tmp_start_minute=second_start_minute+1;
	temp_morning_start.erase(tmp_iter);
	tmp_iter=temp_morning_start.insert(temp_morning_start.end(),
															escalator_start(second_escalator_id,
																					second_escalator_pref_direction,
																					second_escalator_direction,
																					second_start_hour,
																					tmp_start_minute,
																					second_start_mode,
																					second_start_enabled
																					)
															);

	CPPUNIT_ASSERT(!(temp_morning_start==g_morning_start));	

	temp_morning_start.erase(tmp_iter);
	tmp_iter=temp_morning_start.insert(temp_morning_start.end(),
															escalator_start(second_escalator_id,
																					second_escalator_pref_direction,
																					second_escalator_direction,
																					second_start_hour,
																					second_start_minute,
																					second_start_mode,
																					second_start_enabled
																					)
															);

	CPPUNIT_ASSERT(temp_morning_start==g_morning_start);	

	int tmp_start_mode=system_settings::START_DAY_MODE_EVEN;
	temp_morning_start.erase(tmp_iter);
	tmp_iter=temp_morning_start.insert(temp_morning_start.end(),
															escalator_start(second_escalator_id,
																					second_escalator_pref_direction,
																					second_escalator_direction,
																					second_start_hour,
																					second_start_minute,
																					tmp_start_mode,
																					second_start_enabled
																					)
															);

	CPPUNIT_ASSERT(!(temp_morning_start==g_morning_start));	

	temp_morning_start.erase(tmp_iter);
	tmp_iter=temp_morning_start.insert(temp_morning_start.end(),
															escalator_start(second_escalator_id,
																					second_escalator_pref_direction,
																					second_escalator_direction,
																					second_start_hour,
																					second_start_minute,
																					second_start_mode,
																					second_start_enabled
																					)
															);

	CPPUNIT_ASSERT(temp_morning_start==g_morning_start);	

	bool tmp_start_enabled=!second_start_enabled;
	temp_morning_start.erase(tmp_iter);
	tmp_iter=temp_morning_start.insert(temp_morning_start.end(),
															escalator_start(second_escalator_id,
																					second_escalator_pref_direction,
																					second_escalator_direction,
																					second_start_hour,
																					second_start_minute,
																					second_start_mode,
																					tmp_start_enabled
																					)
															);

	CPPUNIT_ASSERT(!(temp_morning_start==g_morning_start));	

	temp_morning_start.erase(tmp_iter);
	tmp_iter=temp_morning_start.insert(temp_morning_start.end(),
															escalator_start(second_escalator_id,
																					second_escalator_pref_direction,
																					second_escalator_direction,
																					second_start_hour,
																					second_start_minute,
																					second_start_mode,
																					second_start_enabled
																					)
															);

	CPPUNIT_ASSERT(temp_morning_start==g_morning_start);	

	temp_morning_start.set_was_created_in_this_day(!temp_morning_start.get_was_created_in_this_day());
	CPPUNIT_ASSERT(!(temp_morning_start==g_morning_start));	
	temp_morning_start.set_was_created_in_this_day(!temp_morning_start.get_was_created_in_this_day());
	CPPUNIT_ASSERT(temp_morning_start==g_morning_start);	
	
	temp_morning_start.erase_all();
	g_morning_start.erase_all();

	CPPUNIT_ASSERT(temp_morning_start==g_morning_start);
};


void test_creating_morning_start()
{
time_t time_t_now=time(NULL);
struct tm time_tm_now;

localtime_r(&time_t_now, &time_tm_now);
										
contain_morning_start tmp_morn_start;
contain_morning_start::iterator_morining_start iter_morn_start;
int line_id=1;
metro_lines_container::iterator_metro_lines line_iter=g_lines.find(line_id);

CPPUNIT_ASSERT(line_iter!=g_lines.end()); 

metro_line::station_id_container stations_in_current_line(line_iter->second.begin_stations_id(),
																							line_iter->second.end_stations_id());

metro_escalators_container tmp_esc_container=g_escalators.get_escalators_by_line_id (stations_in_current_line,
  																														  &g_stations);
tmp_morn_start.prepare_morning_start( tmp_esc_container);

CPPUNIT_ASSERT(tmp_morn_start.max_size()==3);
CPPUNIT_ASSERT(tmp_morn_start.size()==3);

iter_morn_start=tmp_morn_start.begin();

CPPUNIT_ASSERT(iter_morn_start->get_escalator_id()==2);
CPPUNIT_ASSERT(iter_morn_start->get_escalator_direction()==system_settings::DIRECTION_UP);
CPPUNIT_ASSERT(iter_morn_start->get_escalator_pref_direction()==system_settings::DIRECTION_DOWN);
CPPUNIT_ASSERT(iter_morn_start->get_start_mode()==system_settings::START_DAY_MODE_EVERYDAY);
CPPUNIT_ASSERT(iter_morn_start->get_start_hour()==4);
CPPUNIT_ASSERT(iter_morn_start->get_start_minute()==52);

iter_morn_start++;
CPPUNIT_ASSERT(iter_morn_start->get_escalator_id()==4);
CPPUNIT_ASSERT(iter_morn_start->get_escalator_direction()==system_settings::DIRECTION_DOWN);
CPPUNIT_ASSERT(iter_morn_start->get_escalator_pref_direction()==system_settings::DIRECTION_UP);
CPPUNIT_ASSERT(iter_morn_start->get_start_mode()==system_settings::START_DAY_MODE_ODD);
CPPUNIT_ASSERT(iter_morn_start->get_start_hour()==5);
CPPUNIT_ASSERT(iter_morn_start->get_start_minute()==05);

iter_morn_start++;
CPPUNIT_ASSERT(iter_morn_start->get_escalator_id()==3);
CPPUNIT_ASSERT(iter_morn_start->get_escalator_direction()==system_settings::DIRECTION_DOWN);
CPPUNIT_ASSERT(iter_morn_start->get_escalator_pref_direction()==system_settings::DIRECTION_REVERSE);
CPPUNIT_ASSERT(iter_morn_start->get_start_mode()==system_settings::START_DAY_MODE_NEVER);
CPPUNIT_ASSERT(iter_morn_start->get_start_hour()==5);
CPPUNIT_ASSERT(iter_morn_start->get_start_minute()==12);

tmp_morn_start.only_starting();
CPPUNIT_ASSERT(tmp_morn_start.max_size()==3);

iter_morn_start=tmp_morn_start.begin();

CPPUNIT_ASSERT(iter_morn_start->get_escalator_id()==2);
CPPUNIT_ASSERT(iter_morn_start->get_escalator_direction()==system_settings::DIRECTION_UP);
CPPUNIT_ASSERT(iter_morn_start->get_escalator_pref_direction()==system_settings::DIRECTION_DOWN);
CPPUNIT_ASSERT(iter_morn_start->get_start_mode()==system_settings::START_DAY_MODE_EVERYDAY);
CPPUNIT_ASSERT(iter_morn_start->get_start_hour()==4);
CPPUNIT_ASSERT(iter_morn_start->get_start_minute()==52);

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
		CPPUNIT_ASSERT(iter_morn_start->get_start_hour()==5);
		CPPUNIT_ASSERT(iter_morn_start->get_start_minute()==05);

	};

CPPUNIT_ASSERT(++iter_morn_start==tmp_morn_start.end());

tmp_morn_start.execute_morning_start(tmp_esc_container,
															time_tm_now.tm_hour,
															time_tm_now.tm_min);

};
 
public:
void setUp()
{
}
	
void tearDown()
{
}

static CppUnit::TestSuite *suite()
{
	CppUnit::TestSuite *suite_of_tests=new CppUnit::TestSuite("Tests for Container of Morning Start ");

	suite_of_tests->addTest(new CppUnit::TestCaller<contain_morning_start_test> 
												( "simply metods test",
													&contain_morning_start_test::test_simply_metods)
											);

	suite_of_tests->addTest(new CppUnit::TestCaller<contain_morning_start_test> 
												( "test of reloading operator ==",
													&contain_morning_start_test::test_equale_operator)
											);

	suite_of_tests->addTest(new CppUnit::TestCaller<contain_morning_start_test> 
												( "test of creating morning start list from escalators container and morning start",
													&contain_morning_start_test::test_creating_morning_start)
											);


	return suite_of_tests;
}

};
#endif
