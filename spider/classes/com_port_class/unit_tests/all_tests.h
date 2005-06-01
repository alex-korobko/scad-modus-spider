#ifndef __ALL_TESTS__
#define __ALL_TESTS__

class all_tests { 
private:
 CppUnit::TextUi::TestRunner runner;
public: 
 all_tests();
 static void run_tests();
 };
 
 #endif