#ifndef __SOUND_TEST__
#define __SOUND_TEST__

class sound_test : public CppUnit::TestFixture {
private:
 string file_name;
 
void test_play()
{
CPPUNIT_ASSERT( g_sound.play(file_name));
};

void test_player()
{
char* file_name_c_str = new char[file_name.size()+1];
strcpy(file_name_c_str, file_name.c_str());

Player(file_name_c_str);
};

 
public:
void setUp()
{
	file_name="../unit_tests/alert.wav";
}
	
void tearDown()
{
}

static CppUnit::TestSuite *suite()
{
CppUnit::TestSuite *suite_of_tests=new CppUnit::TestSuite( "Sound Object Tests");

suite_of_tests->addTest(new CppUnit::TestCaller<sound_test> 
												( "thread function Player (for sounds playing) test",
													&sound_test::test_player)
										);

suite_of_tests->addTest(new CppUnit::TestCaller<sound_test> 
												( "play metod test",
													&sound_test::test_play)
										);
return suite_of_tests;
}
};

#endif