#ifndef __STATION_TEST__
#define __STATION_TEST__

class station_test : public CppUnit::TestFixture {
private:
 
void test_simply_metods()
{

string station_name("test");
int station_id=1, station_x=10, station_y=11, line_id=3;
	
metro_station testing_station (
						station_id,
						line_id,
						station_x,
						station_y,
						station_name
  						  );

CPPUNIT_ASSERT(testing_station.get_wnd()==NULL);

CPPUNIT_ASSERT(testing_station.get_id()==station_id);
station_id=2;
testing_station.set_id(station_id);
CPPUNIT_ASSERT(testing_station.get_id()==station_id);

CPPUNIT_ASSERT(testing_station.get_line_id()==line_id);
line_id=2;
testing_station.set_line_id(line_id);
CPPUNIT_ASSERT(testing_station.get_line_id()==line_id);


CPPUNIT_ASSERT(testing_station.get_x_coord()==station_x);
station_x=12;
testing_station.set_x_coord(station_x);
CPPUNIT_ASSERT(testing_station.get_x_coord()==station_x);

CPPUNIT_ASSERT(testing_station.get_y_coord()==station_y);
station_y=17;
testing_station.set_y_coord(station_y);
CPPUNIT_ASSERT(testing_station.get_y_coord()==station_y);

CPPUNIT_ASSERT(testing_station.get_stl_name_string().compare(station_name)==0);
CPPUNIT_ASSERT(station_name.compare(testing_station.get_c_name_string())==0);
CPPUNIT_ASSERT(station_name.size() == testing_station.get_c_name_string_size());
station_name="new_test";
testing_station.set_stl_name_string(station_name);
CPPUNIT_ASSERT(testing_station.get_stl_name_string().compare(station_name)==0);

};


void test_stations_id_container_metods()
{
	metro_station::iterator_escalators_id esc_id_iter;
	string station_name("test");
	int station_id=1, station_x=10, station_y=11, line_id=3;
	
	metro_station testing_station (
						station_id,
						line_id,
						station_x,
						station_y,
						station_name
  						  );

	vector<int> escalators_id_container(3);
	escalators_id_container.push_back(1);
	escalators_id_container.push_back(2);
	escalators_id_container.push_back(3);

	CPPUNIT_ASSERT(testing_station.empty_escalators_id());
	CPPUNIT_ASSERT(testing_station.size_escalators_id()==0);
	for (vector<int>::size_type i=0 ; i<escalators_id_container.size();i++)
	{
		testing_station.push_back_escalators_id(escalators_id_container[i]);
	};
	CPPUNIT_ASSERT(!testing_station.empty_escalators_id());
	CPPUNIT_ASSERT(testing_station.size_escalators_id()==escalators_id_container.size());
	esc_id_iter=testing_station.find_escalators_id(2);
	CPPUNIT_ASSERT(esc_id_iter!=testing_station.end_escalators_id());
	testing_station.remove_escalators_id(esc_id_iter);
	testing_station.remove_escalators_id(testing_station.begin_escalators_id(), testing_station.end_escalators_id());

	CPPUNIT_ASSERT(testing_station.empty_escalators_id());
	CPPUNIT_ASSERT(testing_station.size_escalators_id()==0);
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
CppUnit::TestSuite *suite_of_tests=new CppUnit::TestSuite( "Metro Station Object Tests");

suite_of_tests->addTest(new CppUnit::TestCaller<station_test> 
												( "Tests of simplest metods of metro_station",
													&station_test::test_simply_metods)
										);

suite_of_tests->addTest(new CppUnit::TestCaller<station_test> 
												( "Tests of stations ids container metods",
													&station_test::test_stations_id_container_metods)
										);
return suite_of_tests;
}
};

#endif