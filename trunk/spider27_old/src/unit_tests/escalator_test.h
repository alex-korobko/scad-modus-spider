#ifndef __ESCALATOR_TEST__
#define __ESCALATOR_TEST__

class escalator_test : public CppUnit::TestFixture {
private:

 
void test_simply_metods()
{

string escalator_name("test");
int station_id=3, type_id=10, escalator_number=11,
	 escalator_pref_direction=system_settings::DIRECTION_UP, 	
	 escalator_direction=system_settings::DIRECTION_UP,
	 escalator_enabled=system_settings::ENABLED,
	 escalator_start_day_mode=system_settings::START_DAY_MODE_EVEN,
	 escalator_start_hour=system_settings::START_HOUR_MAX,
	 escalator_start_minute=59,
	 tmp=0;
	 
dword escalator_id=1;
in_addr_t escalator_ip=inet_addr("192.168.0.1");
	
metro_escalator testing_escalator(
		&g_system_settings,
		escalator_id, 
		station_id,
		type_id,
		escalator_number,
		escalator_pref_direction,
		escalator_direction,
		escalator_start_day_mode,
		escalator_start_hour,
		escalator_start_minute,
		escalator_enabled,
		escalator_ip
		);

CPPUNIT_ASSERT(testing_escalator.get_connection_id()==0);

CPPUNIT_ASSERT(testing_escalator.get_station_widget()==NULL);
CPPUNIT_ASSERT(testing_escalator.get_block_label_widget()==NULL);
CPPUNIT_ASSERT(testing_escalator.get_arrow_label_widget()==NULL);

CPPUNIT_ASSERT(testing_escalator.get_id()==escalator_id);
escalator_id=2;
testing_escalator.set_id(escalator_id);
CPPUNIT_ASSERT(testing_escalator.get_id()==escalator_id);

CPPUNIT_ASSERT(testing_escalator.get_number()==escalator_number);
escalator_number=3;
testing_escalator.set_number(escalator_number);
CPPUNIT_ASSERT(testing_escalator.get_number()==escalator_number);

CPPUNIT_ASSERT(testing_escalator.get_type()==type_id);
type_id=5;
testing_escalator.set_type(type_id);
CPPUNIT_ASSERT(testing_escalator.get_type()==type_id);

CPPUNIT_ASSERT(testing_escalator.get_direction()==escalator_direction);
escalator_direction=system_settings::DIRECTION_DOWN;
testing_escalator.set_direction(escalator_direction);
CPPUNIT_ASSERT(testing_escalator.get_direction()==escalator_direction);

CPPUNIT_ASSERT(testing_escalator.get_start_day_mode()==escalator_start_day_mode);
CPPUNIT_ASSERT(testing_escalator.get_start_hour()==escalator_start_hour);
CPPUNIT_ASSERT(testing_escalator.get_start_minute()==escalator_start_minute);

CPPUNIT_ASSERT(testing_escalator.get_pref_direction()==escalator_pref_direction);
escalator_pref_direction=system_settings::DIRECTION_REVERSE;
testing_escalator.set_pref_direction(escalator_pref_direction);
CPPUNIT_ASSERT(testing_escalator.get_pref_direction()==escalator_pref_direction);

tmp=msg_dict_container::MSG_PROGRAM_STARTED;
testing_escalator.set_last_message_remote_id(tmp);
CPPUNIT_ASSERT(testing_escalator.get_last_message_remote_id()==tmp);

tmp=100;
testing_escalator.set_sleepticks(tmp);
testing_escalator.increment_sleepticks();
CPPUNIT_ASSERT(testing_escalator.get_sleepticks()==++tmp);

CPPUNIT_ASSERT(testing_escalator.get_station_id()==station_id);
station_id=11;
testing_escalator.set_station_id(station_id);
CPPUNIT_ASSERT(testing_escalator.get_station_id()==station_id);

CPPUNIT_ASSERT(testing_escalator.get_pref_direction()==escalator_pref_direction);
escalator_pref_direction=system_settings::DIRECTION_REVERSE;
testing_escalator.set_pref_direction(escalator_pref_direction);
CPPUNIT_ASSERT(testing_escalator.get_pref_direction()==escalator_pref_direction);

CPPUNIT_ASSERT(testing_escalator.get_enabled()==escalator_enabled);
escalator_enabled=system_settings::DISABLED;
testing_escalator.set_enabled(escalator_enabled);
CPPUNIT_ASSERT(testing_escalator.get_enabled()==escalator_enabled);

tmp=system_settings::ONLINE;
testing_escalator.set_online(tmp);
CPPUNIT_ASSERT(testing_escalator.get_online()==tmp);

CPPUNIT_ASSERT(testing_escalator.get_ip()==escalator_ip);
escalator_ip=inet_addr("192.168.0.3");
testing_escalator.set_ip(escalator_ip);
CPPUNIT_ASSERT(testing_escalator.get_ip()==escalator_ip);
};



void test_data_processing_metods()
{
metro_escalator::buffer_data_type buffer(system_settings::MODBUS_BUFFER_SIZE);
word tmp_data;
dword tmp_data_dword;
data_block block_of_data;
system_settings::bytes tmp_bytes;

string escalator_name("test");
int station_id=3, type_id=10, escalator_number=11,
	escalator_pref_direction=system_settings::DIRECTION_UP, 	
	escalator_direction=system_settings::DIRECTION_UP,
	escalator_enabled=system_settings::ENABLED,
	 escalator_start_day_mode=system_settings::START_DAY_MODE_EVEN,
	 escalator_start_hour=system_settings::START_HOUR_MAX,
	 escalator_start_minute=59;
	
in_addr_t escalator_ip=inet_addr("192.168.0.1");
dword escalator_id=1;

metro_escalator testing_escalator(
		&g_system_settings,
		escalator_id, 
		station_id,
		type_id,
		escalator_number,
		escalator_pref_direction,
		escalator_direction,
		escalator_start_day_mode,
		escalator_start_hour,
		escalator_start_minute,
		escalator_enabled,
		escalator_ip
		);

testing_escalator.get_data();

block_of_data.status=system_settings::STATE_UP_MAIN_DRIVE;
block_of_data.mode=system_settings::MODE_MAIN_DRIVE_AT_PANEL;
block_of_data.ready=system_settings::STATE_TEST_READY;
block_of_data.block_circuts_status=system_settings::BLOCK_NORMA;
block_of_data.breaking_path_status=system_settings::BREAKING_PATH_NORMA;
block_of_data.channels=8;
block_of_data.message_number=4;
block_of_data.count_of_messages=2;
block_of_data.reserved1=16;
block_of_data.breaking_path_value=32;
block_of_data.running_path_value=64;
block_of_data.reserved2=128;
block_of_data.reserved3=512;

int tmp_size = block_of_data.signals.size();
for(int i=0; i<tmp_size; i++)
		block_of_data.signals[i] = i;

tmp_size = block_of_data.messages.size();
for(int i=0; i<tmp_size; i++)
	block_of_data.messages[i] = i*2;

tmp_data=htons(block_of_data.status);
tmp_bytes=g_system_settings.bytes_of_type<word>(tmp_data);
buffer[0]=tmp_bytes[0];
buffer[1]=tmp_bytes[1];

tmp_data=htons(block_of_data.mode);
tmp_bytes=g_system_settings.bytes_of_type<word>(tmp_data);
buffer[2]=tmp_bytes[0];
buffer[3]=tmp_bytes[1];

tmp_data=htons(block_of_data.ready);
tmp_bytes=g_system_settings.bytes_of_type<word>(tmp_data);
buffer[4]=tmp_bytes[0];
buffer[5]=tmp_bytes[1];

tmp_data=htons(block_of_data.block_circuts_status);
tmp_bytes=g_system_settings.bytes_of_type<word>(tmp_data);
buffer[6]=tmp_bytes[0];
buffer[7]=tmp_bytes[1];

tmp_data=htons(block_of_data.breaking_path_status);
tmp_bytes=g_system_settings.bytes_of_type<word>(tmp_data);
buffer[8]=tmp_bytes[0];
buffer[9]=tmp_bytes[1];

tmp_data=htons(block_of_data.channels);
tmp_bytes=g_system_settings.bytes_of_type<word>(tmp_data);
buffer[10]=tmp_bytes[0];
buffer[11]=tmp_bytes[1];

tmp_data=htons(block_of_data.message_number);
tmp_bytes=g_system_settings.bytes_of_type<word>(tmp_data);
buffer[12]=tmp_bytes[0];
buffer[13]=tmp_bytes[1];

tmp_data=htons(block_of_data.count_of_messages);
tmp_bytes=g_system_settings.bytes_of_type<word>(tmp_data);
buffer[14]=tmp_bytes[0];
buffer[15]=tmp_bytes[1];

tmp_data=htons(block_of_data.reserved1);
tmp_bytes=g_system_settings.bytes_of_type<word>(tmp_data);
buffer[16]=tmp_bytes[0];
buffer[17]=tmp_bytes[1];

tmp_data_dword=htonl(block_of_data.breaking_path_value);
tmp_bytes=g_system_settings.bytes_of_type<dword>(tmp_data_dword);
buffer[18]=tmp_bytes[0];
buffer[19]=tmp_bytes[1];
buffer[20]=tmp_bytes[2];
buffer[21]=tmp_bytes[3];

tmp_data_dword=htonl(block_of_data.running_path_value);
tmp_bytes=g_system_settings.bytes_of_type<dword>(tmp_data_dword);
buffer[22]=tmp_bytes[0];
buffer[23]=tmp_bytes[1];
buffer[24]=tmp_bytes[2];
buffer[25]=tmp_bytes[3];

tmp_data=htons(block_of_data.reserved2);
tmp_bytes=g_system_settings.bytes_of_type<word>(tmp_data);
buffer[26]=tmp_bytes[0];
buffer[27]=tmp_bytes[1];

tmp_data=htons(block_of_data.reserved3);
tmp_bytes=g_system_settings.bytes_of_type<word>(tmp_data);
buffer[28]=tmp_bytes[0];
buffer[29]=tmp_bytes[1];

tmp_size = block_of_data.signals.size();
for(int i=0; i<tmp_size; i++)
	{
		tmp_data=htons(block_of_data.signals[i]);
		tmp_bytes=g_system_settings.bytes_of_type<word>(tmp_data);
		buffer[30+i*2]=tmp_bytes[0];
		buffer[30+i*2 +1]=tmp_bytes[1];
	}

tmp_size = block_of_data.messages.size();
for(int i=0; i<tmp_size; i++)
	{
		tmp_data=htons(block_of_data.messages[i]);
		tmp_bytes=g_system_settings.bytes_of_type<word>(tmp_data);
		buffer[60+i*2]=tmp_bytes[0];
		buffer[60+i*2 +1]=tmp_bytes[1];
	}


testing_escalator.swap_in_buffer(buffer);
testing_escalator.set_data();

CPPUNIT_ASSERT(testing_escalator.get_status()==block_of_data.status);
CPPUNIT_ASSERT(testing_escalator.get_mode()==block_of_data.mode);
CPPUNIT_ASSERT(testing_escalator.get_ready()==block_of_data.ready);
CPPUNIT_ASSERT(testing_escalator.get_blocked_status()==block_of_data.block_circuts_status);
CPPUNIT_ASSERT(testing_escalator.get_breaking_status()==block_of_data.breaking_path_status);

CPPUNIT_ASSERT(testing_escalator.get_breaking_path_value()==block_of_data.breaking_path_value);
CPPUNIT_ASSERT(testing_escalator.get_running_path_value()==block_of_data.running_path_value);

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
CppUnit::TestSuite *suite_of_tests=new CppUnit::TestSuite( "Metro Escalator Object Tests");

suite_of_tests->addTest(new CppUnit::TestCaller<escalator_test> 
												( "Tests of simplest metods of metro_escalator",
													&escalator_test::test_simply_metods)
										);

suite_of_tests->addTest(new CppUnit::TestCaller<escalator_test> 
												( "Tests of data processing metods of metro_escalator",
													&escalator_test::test_data_processing_metods)
										);


return suite_of_tests;
}
};

#endif