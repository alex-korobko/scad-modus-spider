#ifndef __CONTAIN_LOG_REC_TEST__
#define __CONTAIN_LOG_REC_TEST__
class contain_log_rec_test : public CppUnit::TestFixture {
private:
 string file_name;


void test_save_load()
{
log_records_container tmp_log_rec_cont;
int mess_id;
tmp_log_rec_cont.set_records_autoincrement(tmp_log_rec_cont.get_records_autoincrement()+1);

mess_id=msg_dict_container::MSG_PROGRAM_STARTED; //	message_id Program started
msg_dict_container::msg_dict_iterator tmp_msg_iter=g_msg_dictionary.find(mess_id);
if(tmp_msg_iter==g_msg_dictionary.end())
{
	vector<char> tmp_chars(10);
	string mess = "Not found message id ";
	itoa (mess_id, &tmp_chars[0], 10);
	mess +=&tmp_chars[0];

	g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
};

int msg_type=tmp_msg_iter->second.get_type();



tmp_log_rec_cont.insert (log_record (
												 tmp_log_rec_cont.get_records_autoincrement(),
												 mess_id,
												 msg_type, //	message_type 
												 0, // station_id -  broadcast
												 0, // device_id -  broadcast
												 time(NULL)
												)
							);
tmp_log_rec_cont.set_records_autoincrement(tmp_log_rec_cont.get_records_autoincrement()+1);
mess_id=msg_dict_container::MSG_PROGRAM_CLOSED; //	message_id Program stopped

mess_id=msg_dict_container::MSG_PROGRAM_STARTED; //	message_id Program started

tmp_msg_iter=g_msg_dictionary.find(mess_id);
if(tmp_msg_iter==g_msg_dictionary.end())
{
	vector<char> tmp_chars(10);
	string mess = " Not found message id ";
	itoa (mess_id, &tmp_chars[0], 10);
	mess +=&tmp_chars[0];

	g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
};

msg_type=tmp_msg_iter->second.get_type();

tmp_log_rec_cont.insert (log_record (
												 tmp_log_rec_cont.get_records_autoincrement(),
												 mess_id,
												 msg_type, //	message_type 
												 0, // station_id -  broadcast
												 0, // device_id -  broadcast
												 time(NULL)
												)
							);

CPPUNIT_ASSERT(tmp_log_rec_cont.save(file_name));

CPPUNIT_ASSERT(g_main_log.load(file_name));

CPPUNIT_ASSERT(g_main_log.size()==tmp_log_rec_cont.size());

log_records_container::iterator_log_rec tmp1_iter=g_main_log.begin(), 
															 tmp2_iter=tmp_log_rec_cont.begin();

while (tmp1_iter!=g_main_log.end() || 
			tmp2_iter!=tmp_log_rec_cont.end())
	{
		CPPUNIT_ASSERT(tmp2_iter->get_id()==tmp1_iter->get_id());
		CPPUNIT_ASSERT(tmp2_iter->get_msg_id()==tmp1_iter->get_msg_id());
		CPPUNIT_ASSERT(tmp2_iter->get_msg_type()==tmp1_iter->get_msg_type());
		CPPUNIT_ASSERT(tmp2_iter->get_station_id()==tmp1_iter->get_station_id());
		CPPUNIT_ASSERT(tmp2_iter->get_device_id()==tmp1_iter->get_device_id());
		CPPUNIT_ASSERT(tmp2_iter->get_record_time()==tmp1_iter->get_record_time());
		tmp1_iter++;
		tmp2_iter++;
	}


}

void test_erase()
{
log_records_container::size_type_log_rec old_size=g_main_log.size();
g_main_log.erase(g_main_log.begin());
CPPUNIT_ASSERT(!g_main_log.empty());
CPPUNIT_ASSERT(g_main_log.size()==old_size-1);
g_main_log.erase_all();
CPPUNIT_ASSERT(g_main_log.empty());
CPPUNIT_ASSERT(g_main_log.size()==0);
}

void test_insert()
{
log_filter::ids_container_size_type old_ids_container_size;

CPPUNIT_ASSERT(g_main_log.empty());
CPPUNIT_ASSERT(g_main_log.size()==0);

	// message 1 - unfiltered
	g_main_log.set_records_autoincrement(g_main_log.get_records_autoincrement()+1);
	g_main_log.insert(log_record (
												 g_main_log.get_records_autoincrement(),
												 msg_dict_container::MSG_PROGRAM_CLOSED, //	message_id Program closed
												 1, //	message_type 
												 0, // station_id -  broadcast
												 0, // device_id -  broadcast
												 time(NULL)
												)
							);

CPPUNIT_ASSERT(!g_main_log.empty());
CPPUNIT_ASSERT(g_main_log.size()==1);

	// message 2 - unfiltered
	g_main_log.set_records_autoincrement(g_main_log.get_records_autoincrement()+1);
	g_main_log.insert(log_record (
												 g_main_log.get_records_autoincrement(),
												 msg_dict_container::MSG_PROGRAM_CLOSED, //	message_id Program closed
												 1, //	message_type 
												 0, // station_id -  broadcast
												 0, // device_id -  broadcast
												 time(NULL)
												)
							);
	// message 3 - unfiltered
	g_main_log.set_records_autoincrement(g_main_log.get_records_autoincrement()+1);
	g_main_log.insert(log_record (
												 g_main_log.get_records_autoincrement(),
												 msg_dict_container::MSG_PROGRAM_CLOSED, //	message_id Program closed
												 1, //	message_type 
												 0, // station_id -  broadcast
												 0, // device_id -  broadcast
												 time(NULL)
												)
							);

//Testing filtering by stations  - set filter condition station id 1
	g_main_log.filter.reset_to_default();
	g_main_log.filter.set_all_stations(false);
	
	//test filter.stations metods
	old_ids_container_size=g_main_log.filter.stations_size();
	g_main_log.filter.insert_station (1);
	g_main_log.filter.insert_station (2);
	CPPUNIT_ASSERT(!g_main_log.filter.stations_empty());
	CPPUNIT_ASSERT(g_main_log.filter.stations_size()==old_ids_container_size+2);
	g_main_log.filter.erase_stations_range(g_main_log.filter.stations_begin(),
																g_main_log.filter.stations_end());
	CPPUNIT_ASSERT(g_main_log.filter.stations_empty());
	CPPUNIT_ASSERT(g_main_log.filter.stations_size()==0);

	
	g_main_log.filter.insert_station (1);
	g_main_log.filter.set_filter_state(true);
	g_main_log.set_filtering();

	//message 4 - filtered
	g_main_log.set_records_autoincrement(g_main_log.get_records_autoincrement()+1);
	g_main_log.insert(log_record (
												 g_main_log.get_records_autoincrement(),
												 msg_dict_container::MSG_PROGRAM_CLOSED, //	message_id Program closed
												 1, //	message_type 												 
												 1, // station_id -  to filter
												 0, // device_id -  broadcast
												 time(NULL)
												)
							);

	//message 5 - filtered
	g_main_log.set_records_autoincrement(g_main_log.get_records_autoincrement()+1);
	g_main_log.insert(log_record (
												 g_main_log.get_records_autoincrement(),
												 msg_dict_container::MSG_PROGRAM_CLOSED, //	message_id Program closed
												 1, //	message_type 
												 1, // station_id -  to filter
												 0, // device_id -  broadcast
												 time(NULL)
												)
							);

//Testing filtering by times  - set filter condition time at now up to now+10mS (??)
	time_t time_now=time(NULL);
	g_main_log.filter.set_filter_state(false);
	g_main_log.set_filtering();
	g_main_log.filter.reset_to_default();
	g_main_log.filter.set_all_times(false);
	g_main_log.filter.set_start_time(time_now);
	g_main_log.filter.set_stop_time(time_now+10);
	g_main_log.filter.set_filter_state(true);
	g_main_log.set_filtering();

	//message 6 - filtered
	g_main_log.set_records_autoincrement(g_main_log.get_records_autoincrement()+1);
	g_main_log.insert(log_record (
												 g_main_log.get_records_autoincrement(),
												 msg_dict_container::MSG_PROGRAM_CLOSED, //	message_id Program closed
												 1, //	message_type 
												 0, // station_id -  broadcast
												 0, // device_id -  broadcast
												 time(NULL)  // -  to filter
												)
							);

	//message 7 - filtered
	g_main_log.set_records_autoincrement(g_main_log.get_records_autoincrement()+1);
	g_main_log.insert(log_record (
												 g_main_log.get_records_autoincrement(),
												 msg_dict_container::MSG_PROGRAM_CLOSED, //	message_id Program closed
												 1, //	message_type 
												 0, // station_id -  broadcast
												 0, // device_id -  broadcast
												 time(NULL)  // -  to filter
												)
							);

//Testing filtering by devices  -  ATTENTION!!! g_escalators must have escalator with id 1 & 2!!!
	g_main_log.filter.set_filter_state(false);
	g_main_log.set_filtering();
	g_main_log.filter.reset_to_default();
	g_main_log.filter.set_all_devices(false);


	//test filter.devices metods
	old_ids_container_size=g_main_log.filter.devices_size();
	g_main_log.filter.insert_device (1);
	g_main_log.filter.insert_device (2);
	CPPUNIT_ASSERT(!g_main_log.filter.devices_empty());
	CPPUNIT_ASSERT(g_main_log.filter.devices_size()==old_ids_container_size+2);
	g_main_log.filter.erase_devices_range(g_main_log.filter.devices_begin(),
																g_main_log.filter.devices_end());
	CPPUNIT_ASSERT(g_main_log.filter.devices_empty());
	CPPUNIT_ASSERT(g_main_log.filter.devices_size()==0);


	g_main_log.filter.insert_device (1);
	g_main_log.filter.insert_device (2);
	g_main_log.filter.set_filter_state(true);	
	g_main_log.set_filtering();

	//message 8 - filtered but not in filter condition
	g_main_log.set_records_autoincrement(g_main_log.get_records_autoincrement()+1);
	g_main_log.insert(log_record (
												 g_main_log.get_records_autoincrement(),
												 msg_dict_container::MSG_PROGRAM_CLOSED, //	message_id Program closed
												 1, //	message_type 
												 0, // station_id -  broadcast
												 2, // device_id -  to filter
												 time(NULL)
												)
							);
	//message 9 - filtered but not in filter condition
	g_main_log.set_records_autoincrement(g_main_log.get_records_autoincrement()+1);
	g_main_log.insert(log_record (
												 g_main_log.get_records_autoincrement(),
												 msg_dict_container::MSG_PROGRAM_CLOSED, //	message_id Program closed
												 1, //	message_type
												 0, // station_id -  broadcast
												 1, // device_id -  to filter
												 time(NULL)
												)
							);

//Testing filtering by escalators  -  ATTENTION!!! g_escalators must have escalator with id 1 & 2!!!
	g_main_log.filter.set_filter_state(false);	
	g_main_log.set_filtering();
	g_main_log.filter.reset_to_default();
	g_main_log.filter.set_all_msg_types(false);
	
	//test filter.msg_types metods
	old_ids_container_size=g_main_log.filter.msg_types_size();
	g_main_log.filter.insert_msg_type (1);
	g_main_log.filter.insert_msg_type (2);
	CPPUNIT_ASSERT(!g_main_log.filter.msg_types_empty());
	CPPUNIT_ASSERT(g_main_log.filter.msg_types_size()==old_ids_container_size+2);
	g_main_log.filter.erase_msg_types_range (g_main_log.filter.msg_types_begin(),
																	 g_main_log.filter.msg_types_end());
	CPPUNIT_ASSERT(g_main_log.filter.msg_types_empty());
	CPPUNIT_ASSERT(g_main_log.filter.msg_types_size()==0);
	
	g_main_log.filter.insert_msg_type (3);
	g_main_log.filter.insert_msg_type (2);
	g_main_log.filter.set_filter_state(true);	
	g_main_log.set_filtering();

	//message 10 - filtered 
	g_main_log.set_records_autoincrement(g_main_log.get_records_autoincrement()+1);
	g_main_log.insert(log_record (
												 g_main_log.get_records_autoincrement(),
												 msg_dict_container::MSG_PROGRAM_CLOSED, //	message_id Program closed
												 3, //	message_type -  to filter
												 0, // station_id -  broadcast
												 0, // device_id 
												 time(NULL)
												)
							);
	//message 11 - filtered but not in filter condition
	g_main_log.set_records_autoincrement(g_main_log.get_records_autoincrement()+1);
	g_main_log.insert(log_record (
												 g_main_log.get_records_autoincrement(),
												 msg_dict_container::MSG_PROGRAM_CLOSED, //	message_id Program closed
												 1, //	message_type -  to filter
												 0, // station_id -  broadcast
												 0, // device_id
												 time(NULL)
												)
							);
							
	g_main_log.filter.set_filter_state(false);	
	g_main_log.set_filtering();
	g_main_log.filter.reset_to_default();
}

void log_filter_test()
{
log_filter filter1;

filter1.set_all_times(false);
filter1.set_all_msg_types(false);
filter1.set_all_stations(false);
filter1.set_all_devices(false);
filter1.set_start_time(time(NULL));
filter1.set_stop_time(time(NULL)+3600);
filter1.insert_msg_type (1);
filter1.insert_msg_type (2);
filter1.insert_station (3);
filter1.insert_station (4);
filter1.insert_device (5);
filter1.insert_device (6);

log_filter filter2(filter1);
CPPUNIT_ASSERT(filter2==filter1);

filter2.set_all_times(!filter1.get_all_times());
CPPUNIT_ASSERT(!(filter2==filter1));
filter2.set_all_times(filter1.get_all_times());
CPPUNIT_ASSERT(filter2==filter1);

filter2.set_all_msg_types(!filter1.get_all_msg_types());
CPPUNIT_ASSERT(!(filter2==filter1));
filter2.set_all_msg_types(filter1.get_all_msg_types());
CPPUNIT_ASSERT(filter2==filter1);

filter2.set_all_stations(!filter1.get_all_stations());
CPPUNIT_ASSERT(!(filter2==filter1));
filter2.set_all_stations(filter1.get_all_stations());
CPPUNIT_ASSERT(filter2==filter1);

filter2.set_all_devices(!filter1.get_all_devices());
CPPUNIT_ASSERT(!(filter2==filter1));
filter2.set_all_devices(filter1.get_all_devices());
CPPUNIT_ASSERT(filter2==filter1);


filter2.set_start_time(time(NULL)+210);
CPPUNIT_ASSERT(!(filter2==filter1));
filter2.set_start_time(filter1.get_start_time());
CPPUNIT_ASSERT(filter2==filter1);

filter2.set_stop_time(time(NULL)+333);
CPPUNIT_ASSERT(!(filter2==filter1));
filter2.set_stop_time(filter1.get_stop_time());
CPPUNIT_ASSERT(filter2==filter1);

filter2.erase_stations_range(filter2.stations_begin(),
											filter2.stations_end());
CPPUNIT_ASSERT(!(filter2==filter1));
filter2.insert_stations_range(filter1.stations_begin(),
											filter1.stations_end());
CPPUNIT_ASSERT(filter2==filter1);
filter2.insert_station (10);
CPPUNIT_ASSERT(!(filter2==filter1));
filter2.erase_station (10);
CPPUNIT_ASSERT(filter2==filter1);

filter2.erase_devices_range(filter2.devices_begin(),
											filter2.devices_end());
CPPUNIT_ASSERT(!(filter2==filter1));
filter2.insert_devices_range(filter1.devices_begin(),
											filter1.devices_end());
CPPUNIT_ASSERT(filter2==filter1);
filter2.insert_device (10);
CPPUNIT_ASSERT(!(filter2==filter1));
filter2.erase_device (10);
CPPUNIT_ASSERT(filter2==filter1);


filter2.insert_msg_type (10);
CPPUNIT_ASSERT(!(filter2==filter1));

filter2=filter1;
CPPUNIT_ASSERT(filter2==filter1);

filter2.erase_msg_types_range(filter2.msg_types_begin(),
											filter2.msg_types_end());
CPPUNIT_ASSERT(!(filter2==filter1));

}
 
public:
void setUp()
{
	file_name="../unit_tests/test_log.log";
}
	
void tearDown()
{
}

static CppUnit::TestSuite *suite()
{
CppUnit::TestSuite *suite_of_tests=new CppUnit::TestSuite( "Log Records Container Object Tests");

suite_of_tests->addTest(new CppUnit::TestCaller<contain_log_rec_test> 
												( "log_filter metods test",
													&contain_log_rec_test::log_filter_test)
										);

suite_of_tests->addTest(new CppUnit::TestCaller<contain_log_rec_test> 
												( "insert metods test",
													&contain_log_rec_test::test_insert)
										);
suite_of_tests->addTest(new CppUnit::TestCaller<contain_log_rec_test> 
												( "erase metods tests",
													&contain_log_rec_test::test_erase)
										);

suite_of_tests->addTest(new CppUnit::TestCaller<contain_log_rec_test> 
												( "Save and load metods test",
													&contain_log_rec_test::test_save_load)
										);


return suite_of_tests;
}
};

#endif