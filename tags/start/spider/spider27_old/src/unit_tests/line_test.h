#ifndef __LINE_TEST__
#define __LINE_TEST__

class line_test : public CppUnit::TestFixture {
private:
 
void test_simply_metods()
{
	string line_name("test");
	int line_id=1;
	PgColor_t line_color=Pg_BLUE;
	metro_line testing_line(
				line_id,
				line_name,
				line_color
				);

 CPPUNIT_ASSERT(testing_line.get_line_widget()==NULL);
 
CPPUNIT_ASSERT(testing_line.get_color()==line_color);
line_color=metro_line::DEFAULT_COLOR;
testing_line.set_color(line_color);
CPPUNIT_ASSERT(testing_line.get_color()==line_color);

CPPUNIT_ASSERT(testing_line.get_stl_name_string().compare(line_name)==0);
CPPUNIT_ASSERT(line_name.compare(testing_line.get_c_name_string())==0);
CPPUNIT_ASSERT(line_name.size() == testing_line.get_c_name_string_size());
line_name="new_test";
testing_line.set_stl_name_string(line_name);
CPPUNIT_ASSERT(testing_line.get_stl_name_string().compare(line_name)==0);

CPPUNIT_ASSERT(testing_line.get_id()==line_id);
line_id=2;
testing_line.set_id(line_id);
CPPUNIT_ASSERT(testing_line.get_id()==line_id);
};

void test_stations_id_container_metods()
{
	metro_line::iterator_stations_id stat_id_iter;
	metro_line testing_line(
				2,
				"testing line",
				Pg_RED
				);

	vector<int> station_id_container(3);
	station_id_container.push_back(1);
	station_id_container.push_back(2);
	station_id_container.push_back(3);

	CPPUNIT_ASSERT(testing_line.empty_stations_id());
	CPPUNIT_ASSERT(testing_line.size_stations_id()==0);
	for (vector<int>::size_type i=0 ; i<station_id_container.size();i++)
	{
		testing_line.push_back_stations_id(station_id_container[i]);
	};
	CPPUNIT_ASSERT(!testing_line.empty_stations_id());
	CPPUNIT_ASSERT(testing_line.size_stations_id()==station_id_container.size());
	stat_id_iter=testing_line.find_stations_id(2);
	CPPUNIT_ASSERT(stat_id_iter!=testing_line.end_stations_id());
	testing_line.remove_stations_id(stat_id_iter);
	testing_line.remove_stations_id(testing_line.begin_stations_id(), testing_line.end_stations_id());

	CPPUNIT_ASSERT(testing_line.empty_stations_id());
	CPPUNIT_ASSERT(testing_line.size_stations_id()==0);
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
CppUnit::TestSuite *suite_of_tests=new CppUnit::TestSuite( "Metro Line Object Tests");

suite_of_tests->addTest(new CppUnit::TestCaller<line_test> 
												( "Tests of simplest metods of metro_line",
													&line_test::test_simply_metods)
										);

suite_of_tests->addTest(new CppUnit::TestCaller<line_test> 
												( "Tests of stations ids container metods",
													&line_test::test_stations_id_container_metods)
										);
return suite_of_tests;
}
};

#endif