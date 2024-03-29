#ifndef __MSG_DICT_CONT_TEST_H__
#define __MSG_DICT_CONT_TEST_H__

class msg_dict_container_test : public CppUnit::TestFixture {
private:
 string file_name;
 
void test_insert()
{

g_msg_dictionary.insert(g_msg_dictionary.end(), msg_dict_container::msg_dict_pair(3, 
																										dictionary_message(3,3 ,"Message number 3")
																										)
								);

g_msg_dictionary.insert(g_msg_dictionary.begin(), msg_dict_container::msg_dict_pair(1, 
																										dictionary_message(1,1 ,"Message number 1")
																										)
								);

g_msg_dictionary.insert(g_msg_dictionary.upper_bound(2), msg_dict_container::msg_dict_pair(2, 
																										dictionary_message(2,2 ,"Message number 2")
																										)
							 );

g_msg_dictionary.insert(g_msg_dictionary.upper_bound(143), msg_dict_container::msg_dict_pair(143, 
																										dictionary_message(143,2 ,"Message number 143")
																										)
							 );

g_msg_dictionary.insert(g_msg_dictionary.upper_bound(144), msg_dict_container::msg_dict_pair(144, 
																										dictionary_message(144, 2 ,"Message number 144")
																										)
							 );

};


void test_erase()
{
msg_dict_container::msg_dict_iterator tmp_iter;
msg_dict_container::msg_dict_size_type old_size;
old_size=g_msg_dictionary.size();
CPPUNIT_ASSERT(old_size==5);
CPPUNIT_ASSERT(!g_msg_dictionary.empty());

tmp_iter=g_msg_dictionary.find(1);
CPPUNIT_ASSERT( tmp_iter!=g_msg_dictionary.end());
g_msg_dictionary.erase(tmp_iter);
CPPUNIT_ASSERT(old_size-1==g_msg_dictionary.size());
CPPUNIT_ASSERT(!g_msg_dictionary.empty());

g_msg_dictionary.erase(g_msg_dictionary.begin(),
										g_msg_dictionary.end());

CPPUNIT_ASSERT(g_msg_dictionary.empty());
CPPUNIT_ASSERT(g_msg_dictionary.size()==0);
};

void test_load()
{
CPPUNIT_ASSERT(g_msg_dictionary.load(&g_system_settings, &g_msg_types , file_name));
};

 
public:
void setUp()
{
	file_name="../unit_tests/msg_dict_test.ini";
}
	
void tearDown()
{
}

static CppUnit::TestSuite *suite()
{
CppUnit::TestSuite *suite_of_tests=new CppUnit::TestSuite( "Tests for Container of Messages Dictionary");

suite_of_tests->addTest(new CppUnit::TestCaller<msg_dict_container_test> 
												( "insert metod test",
													&msg_dict_container_test::test_insert)
										);

suite_of_tests->addTest(new CppUnit::TestCaller<msg_dict_container_test> 
												( "erase metod test",
													&msg_dict_container_test::test_erase)
										);

suite_of_tests->addTest(new CppUnit::TestCaller<msg_dict_container_test> 
												( "load metod test",
													&msg_dict_container_test::test_load)
										);


return suite_of_tests;
}

};

#endif