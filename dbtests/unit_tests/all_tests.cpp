using namespace std;

#include <cppunit/ui/text/TestRunner.h> 
#include <cppunit/TestFixture.h> 
#include <cppunit/TestSuite.h> 

#include <inttypes.h>

#include "../tests.h"
#include "../dbobjects.h"

#include "../src/defines.h"

#include "../src/dbobjects/dbobject_exception.h"
#include "../src/dbobjects/dbobject_interface.h"
#include "../src/dbobjects/dbobject_mock.h"

#include "../src/tests/test_exception.h"
#include "../src/tests/test_interface.h"
#include "../src/tests/test_1.h"

#ifdef __MOCK_OBJECT__
#include "test_dbobject_mock.h"
#endif

#ifdef __TEST_1__
#include "test_test_1.h"
#endif

#include "all_tests.h"

all_tests::all_tests()
 {
#ifdef __MOCK_OBJECT__
    runner.addTest(test_dbobject_mock::suite());
#endif

#ifdef __TEST_1__
    runner.addTest(test_test_1::suite());
#endif
 }
    
void all_tests::run_tests() 
    {
    all_tests suite_of_all_tests = all_tests();
    suite_of_all_tests.runner.run("", true);
    }
