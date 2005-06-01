#ifndef __MSG_TYPES_CONT_TEST__
#define __MSG_TYPES_CONT_TEST__

class msg_types_container_test : public CppUnit::TestFixture {
private:
 string file_name;
 
void test_insert()
{

g_msg_types.insert(g_msg_types.end(), msg_types_container::msg_types_pair(3, 
																										msg_type(3,"third type", Pg_GREEN)
																										)
								);

g_msg_types.insert(g_msg_types.begin(), msg_types_container::msg_types_pair(1, 
																										msg_type(1,"first type", Pg_BLACK)
																										)
								);

g_msg_types.insert(g_msg_types.upper_bound(2), msg_types_container::msg_types_pair(2, 
																										msg_type(2,"second type", Pg_RED)
																										)
							 );

};


void test_erase()
{

msg_types_container::msg_types_iterator tmp_iter;
msg_types_container::msg_types_size_type old_size=
																	g_msg_types.size();

CPPUNIT_ASSERT(old_size==3);
CPPUNIT_ASSERT(!g_msg_types.empty());

tmp_iter=g_msg_types.find(1);

CPPUNIT_ASSERT( tmp_iter!=g_msg_types.end());
g_msg_types.erase(tmp_iter);

CPPUNIT_ASSERT(old_size==g_msg_types.size()+1);

g_msg_types.erase(g_msg_types.begin(), 
								g_msg_types.end());

CPPUNIT_ASSERT(g_msg_types.empty());
CPPUNIT_ASSERT(g_msg_types.size()==0);

};

void test_load()
{
//ATTENTION  - all of that messages types must be in file_name !!!
/*
#example of right file_name file
[type]
id=1
description=first type of messages
color=0xEE1111

[type]
id=2
description=second type of messages
color=0x0011EE


[type]
id=3
description=third type of messages
color=0x55ff55
*/
msg_types_container tmp_msg_types;
tmp_msg_types.insert(tmp_msg_types.end(), msg_types_container::msg_types_pair(3, 
																										msg_type(3,
																														"third type of messages",
																														strtol("0x55ff55", NULL, 16)
																														)
																										)
								);

tmp_msg_types.insert(tmp_msg_types.begin(), msg_types_container::msg_types_pair(1, 
																										msg_type(1,
																														"first type of messages",
																														strtol("0xEE1111", NULL, 16)
																														)

																										)
								);

tmp_msg_types.insert(tmp_msg_types.upper_bound(2), msg_types_container::msg_types_pair(2, 
																										msg_type(2,
																														"second type of messages",
																														strtol("0x0011EE", NULL, 16)
																														)

																										)
							 );

CPPUNIT_ASSERT(g_msg_types.load(&g_system_settings, file_name));
msg_types_container::msg_types_iterator tmp_iter1=g_msg_types.begin(), tmp_iter2;

while (tmp_iter1!=g_msg_types.end())
	{
		tmp_iter2=tmp_msg_types.find(tmp_iter1->second.get_type_id());
		CPPUNIT_ASSERT( tmp_iter2!=tmp_msg_types.end());
		CPPUNIT_ASSERT( tmp_iter2->second.get_type_id()==tmp_iter1->second.get_type_id());
		CPPUNIT_ASSERT( tmp_iter2->second.get_type_color()==tmp_iter1->second.get_type_color());
		CPPUNIT_ASSERT( tmp_iter2->second.get_type_description().compare(
																								tmp_iter1->second.get_type_description()
																													)==0);
		tmp_iter1++;
	}

};

 
public:
void setUp()
{
	file_name="../unit_tests/msg_types_test.ini";
}
	
void tearDown()
{
}

static CppUnit::TestSuite *suite()
{
CppUnit::TestSuite *suite_of_tests=new CppUnit::TestSuite( "Tests for Container of Messages Types ");

suite_of_tests->addTest(new CppUnit::TestCaller<msg_types_container_test> 
												( "insert metod test",
													&msg_types_container_test::test_insert)
										);

suite_of_tests->addTest(new CppUnit::TestCaller<msg_types_container_test> 
												( "erase metod test",
													&msg_types_container_test::test_erase)
										);

suite_of_tests->addTest(new CppUnit::TestCaller<msg_types_container_test> 
												( "load metod test",
													&msg_types_container_test::test_load)
										);


return suite_of_tests;
}
};

#endif