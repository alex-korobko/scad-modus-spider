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
																													string("third station"),
																													string("sounds/station3.wav")
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
																													string("first station"),
																													string("sounds/station1.wav")
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
																													"second station",
																													"sounds/station2.wav"
																													)
																										)
						);


};


void test_erase()
{
metro_stations_container::iterator_metro_stations tmp_iter;
metro_stations_container::size_type_metro_stations old_size=g_stations.size();
CPPUNIT_ASSERT(old_size==3);
CPPUNIT_ASSERT(!g_stations.empty());

tmp_iter=g_stations.find(1);
CPPUNIT_ASSERT( tmp_iter!=g_stations.end());
g_stations.erase(tmp_iter);
CPPUNIT_ASSERT(!g_stations.empty());
CPPUNIT_ASSERT(old_size==g_stations.size()+1);

g_stations.erase(g_stations.begin(),
							g_stations.end());

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
wav file=sounds/station1.wav
lineID=1
x=13
y=15

[station]
id=2
name=second station
wav file=sounds/station2.wav
lineID=1
x=21
y=52

[station]
id=3
name=third station
wav file=sounds/station3.wav
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
																													string("third station"),
																													string("sounds/station3.wav")
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
																													string("first station"),
																													string("sounds/station1.wav")
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
																													"second station",
																													"sounds/station2.wav"
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
		CPPUNIT_ASSERT(tmp_iter2->second.get_wav_file_name()==tmp_iter1->second.get_wav_file_name());

		tmp_iter1++;
	}

	//test stations id in lines container
	
	//test line with id 1 it may contain two stations id  - with id 1 and 2
	metro_lines_container::iterator_metro_lines iter_line=g_lines.find(1);
	CPPUNIT_ASSERT(iter_line!=g_lines.end());
	CPPUNIT_ASSERT(iter_line->second.size_stations_id()==2);
	metro_line::iterator_stations_id iter_station_id=iter_line->second.begin_stations_id();
	CPPUNIT_ASSERT(*iter_station_id==1);
	iter_station_id++;
	CPPUNIT_ASSERT(*iter_station_id==2);

	iter_line=g_lines.find(2);
	CPPUNIT_ASSERT(iter_line!=g_lines.end());
	CPPUNIT_ASSERT(iter_line->second.size_stations_id()==1);
	iter_station_id=iter_line->second.begin_stations_id();
	CPPUNIT_ASSERT(*iter_station_id==3);

	iter_line=g_lines.find(3);
	CPPUNIT_ASSERT(iter_line!=g_lines.end());
	CPPUNIT_ASSERT(iter_line->second.size_stations_id()==0);
	CPPUNIT_ASSERT(iter_line->second.empty_stations_id());

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