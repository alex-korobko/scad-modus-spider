#ifndef __METRO_LINES_CONT_TEST__
#define __METRO_LINES_CONT_TEST__

class metro_lines_container_test : public CppUnit::TestFixture {
private:
 string file_name;
 
void test_insert()
{


g_lines.insert(g_lines.end(), metro_lines_container::pair_metro_lines(3, 
																										metro_line 
																													(
																													3,
																													"third line",
																													Pg_GREEN
																													)
																										)
						);

g_lines.insert(g_lines.begin(), metro_lines_container::pair_metro_lines(1, 
																										metro_line 
																													(
																													1,
																													"first line",
																													Pg_RED
																													)
																										)
						);

g_lines.insert(g_lines.upper_bound(2), metro_lines_container::pair_metro_lines(2, 
																										metro_line 
																													(
																													2,
																													"second line",
																													Pg_BLUE
																													)
																										)
						);


};


void test_erase()
{
metro_lines_container::iterator_metro_lines tmp_iter;

CPPUNIT_ASSERT(g_lines.size()==3);

tmp_iter=g_lines.find(1);
CPPUNIT_ASSERT( tmp_iter!=g_lines.end());
g_lines.erase(tmp_iter);

tmp_iter=g_lines.find(2);
CPPUNIT_ASSERT( tmp_iter!=g_lines.end());
g_lines.erase(tmp_iter);

tmp_iter=g_lines.find(3);
CPPUNIT_ASSERT( tmp_iter!=g_lines.end());
g_lines.erase(tmp_iter);

CPPUNIT_ASSERT(g_lines.empty());
CPPUNIT_ASSERT(g_lines.size()==0);
};

void test_load()
{
/*
#example of right file_name file
[line]
id=1
name=first line
color=0xEE1111

[line]
id=2
name=second line
color=0x0011EE


[line]
id=3
name=third line
color=0x55ff55
*/

metro_lines_container tmp_metro_lines;


tmp_metro_lines.insert(tmp_metro_lines.end(),
									metro_lines_container::pair_metro_lines(3, 
																								metro_line (
																													3,
																													"third line",
																													strtol("0x55ff55", NULL, 16)
																													)
																								)
									);

tmp_metro_lines.insert(tmp_metro_lines.begin(), 
									metro_lines_container::pair_metro_lines(1, 
																									metro_line (
																													1,
																													"first line",
																													strtol("0xEE1111", NULL, 16)
																													)
																									)
									);

tmp_metro_lines.insert(tmp_metro_lines.upper_bound(2),
					metro_lines_container::pair_metro_lines(2, 
																				metro_line (
																									2,
																									"second line",
																									strtol("0x0011EE", NULL, 16)
																								)
																				)
									);

	CPPUNIT_ASSERT(g_lines.load(&g_system_settings, file_name));
	CPPUNIT_ASSERT(tmp_metro_lines.size()==g_lines.size());
	metro_lines_container::iterator_metro_lines tmp1_iter=g_lines.begin(),
																		tmp2_iter;

	while (tmp1_iter!=g_lines.end())
	{
		tmp2_iter=tmp_metro_lines.find(tmp1_iter->second.get_id());

		CPPUNIT_ASSERT(tmp2_iter->second.get_id()==tmp1_iter->second.get_id());	
		CPPUNIT_ASSERT(tmp2_iter->second.get_color()==tmp1_iter->second.get_color());	
		CPPUNIT_ASSERT(tmp2_iter->second.get_stl_name_string().compare(
																		tmp1_iter->second.get_stl_name_string()
																												)==0);

		tmp1_iter++;
	}

};

 
public:
void setUp()
{
	file_name="../unit_tests/lines_test.ini";
}
	
void tearDown()
{
}

static CppUnit::TestSuite *suite()
{
CppUnit::TestSuite *suite_of_tests=new CppUnit::TestSuite( "Tests for Container of Metro Lines ");

suite_of_tests->addTest(new CppUnit::TestCaller<metro_lines_container_test> 
												( "insert metod test",
													&metro_lines_container_test::test_insert)
										);

suite_of_tests->addTest(new CppUnit::TestCaller<metro_lines_container_test> 
												( "erase metod test",
													&metro_lines_container_test::test_erase)
										);

suite_of_tests->addTest(new CppUnit::TestCaller<metro_lines_container_test> 
												( "load metod test",
													&metro_lines_container_test::test_load)
										);


return suite_of_tests;
}
};

#endif