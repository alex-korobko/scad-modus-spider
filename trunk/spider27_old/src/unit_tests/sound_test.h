class sound_test : public CppUnit::TestFixture {
private:
 sound snd_var;
 string file_name;
 
void test_play()
{
snd_var.play(file_name);
};

 
public:
void setUp()
{
	file_name="../unit_tests/alert.wav";
	snd_var.init();
}
	
void tearDown()
{
}

static CppUnit::TestSuite *suite()
{
CppUnit::TestSuite *suite_of_tests=new CppUnit::TestSuite( "Sound Object Tests");
suite_of_tests->addTest(new CppUnit::TestCaller<sound_test> 
												( "play metod test",
													&sound_test::test_play)
										);
return suite_of_tests;
}
};