#include <cppunit/ui/text/TestRunner.h> 
#include <cppunit/TestFixture.h> 
#include <cppunit/TestSuite.h> 
#include <cppunit/TestCaller.h> 

#include "sound_test.h"

class unit_test 
{ 
private:

CppUnit::TextUi::TestRunner runner;
public: 
 unit_test()
 {
 	runner.addTest(sound_test::suite());
 }
    
void run_tests() 
    {
    runner.run("", true);
    }
 };