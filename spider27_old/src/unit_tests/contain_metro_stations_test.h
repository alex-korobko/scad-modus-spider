#ifndef __METRO_STATIONS_CONT_TEST__
#define __METRO_STATIONS_CONT_TEST__

class metro_stations_container_test : public CppUnit::TestFixture {
private:
 string file_name;
 
void test_insert()
{

g_stations.insert(g_stations.end(), metro_stations_container::pair_metro_stations(3, 
																										metro_station 
																													(
																													3,
																													2,
																													100,
																													100,
																													string("third station")
																													)
																										)
						);

g_stations.insert(g_stations.begin(), metro_stations_container::pair_metro_stations(1, 
																										metro_station 
																													(
																													1,
																													1,
																													200,
																													200,
																													string("first station")
																													)

																										)
						);

g_stations.insert(g_stations.upper_bound(2), metro_stations_container::pair_metro_stations(2, 
																										metro_station 
																													(
																													2,
																													2,
																													300,
																													300,
																													"second station"
																													)
																										)
						);


};


void test_erase()
{
metro_stations_container::iterator_metro_stations tmp_iter;

CPPUNIT_ASSERT(g_stations.size()==3);

tmp_iter=g_stations.find(1);
CPPUNIT_ASSERT( tmp_iter!=g_stations.end());
g_stations.erase(tmp_iter);

tmp_iter=g_stations.find(2);
CPPUNIT_ASSERT( tmp_iter!=g_stations.end());
g_stations.erase(tmp_iter);

tmp_iter=g_stations.find(3);
CPPUNIT_ASSERT( tmp_iter!=g_stations.end());
g_stations.erase(tmp_iter);

CPPUNIT_ASSERT(g_stations.empty());
CPPUNIT_ASSERT(g_stations.size()==0);
};

void test_load()
{
//ATTENTION  - all of that metro stations must be in file_name !!!
/*
#example of right file_name file
[station]
id=1
name=first station
lineID=1
x=13
y=15

[station]
id=2
name=second station
lineID=1
x=21
y=52

[station]
id=3
description=third station
lineID=2
x=150
y=130
*/
metro_stations_container tmp_stations;

tmp_stations.insert(tmp_stations.end(), metro_stations_container::pair_metro_stations(3, 
																										metro_station 
																													(
																													3,
																													2,
																													150,
																													130,
																													string("third station")
																													)
																										)
						);

tmp_stations.insert(tmp_stations.begin(), metro_stations_container::pair_metro_stations(1, 
																										metro_station 
																													(
																													1,
																													1,
																													13,
																													15,
																													string("first station")
																													)

																										)
						);

tmp_stations.insert(tmp_stations.upper_bound(2), metro_stations_container::pair_metro_stations(2, 
																										metro_station 
																													(
																													2,
																													1,
																													21,
																													52,
																													"second station"
																													)
																										)
						);


	CPPUNIT_ASSERT(g_stations.load(&g_system_settings, &g_lines, file_name));
	CPPUNIT_ASSERT(g_stations.size()==tmp_stations.size());

	metro_stations_container::iterator_metro_stations tmp_iter1=g_stations.begin(), tmp_iter2;
	while (tmp_iter1!=g_stations.end())
	{
		tmp_iter2=tmp_stations.find(tmp_iter1->second.get_id());
		CPPUNIT_ASSERT(tmp_iter2!=tmp_stations.end());
		
		CPPUNIT_ASSERT(tmp_iter2->second.get_id()==tmp_iter1->second.get_id());
		CPPUNIT_ASSERT(tmp_iter2->second.get_line_id()==tmp_iter1->second.get_line_id());
		CPPUNIT_ASSERT(tmp_iter2->second.get_x_coord()==tmp_iter1->second.get_x_coord());
		CPPUNIT_ASSERT(tmp_iter2->second.get_y_coord()==tmp_iter1->second.get_y_coord());
		CPPUNIT_ASSERT(tmp_iter2->second.get_stl_name_string()==tmp_iter1->second.get_stl_name_string());
		
		tmp_iter1++;
	}

};

 
public:
void setUp()
{
	file_name="../unit_tests/stations_test.ini";
}
	
void tearDown()
{
}

static CppUnit::TestSuite *suite()
{
CppUnit::TestSuite *suite_of_tests=new CppUnit::TestSuite( "Tests for Container of Metro Stations");

suite_of_tests->addTest(new CppUnit::TestCaller<metro_stations_container_test> 
												( "insert metod test",
													&metro_stations_container_test::test_insert)
										);

suite_of_tests->addTest(new CppUnit::TestCaller<metro_stations_container_test> 
												( "erase metod test",
													&metro_stations_container_test::test_erase)
										);

suite_of_tests->addTest(new CppUnit::TestCaller<metro_stations_container_test> 
												( "load metod test",
													&metro_stations_container_test::test_load)
										);


return suite_of_tests;
}
};

#endif