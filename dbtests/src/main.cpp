using namespace std;


#include <inttypes.h>

#include <iostream.h>
#include <vector>
#include <string>

#include  "../dbobjects.h"
#include  "test_cond.h"
#include  "defines.h"

#include "dbobjects/dbobject_exception.h"
#include "dbobjects/dbobject_interface.h"

#ifdef __MOCK_OBJECT__
#include "dbobjects/dbobject_mock.h"
#endif

#include "tests/test_exception.h"
#include "tests/test_interface.h"
#include "tests/test_1.h"

#ifdef __UNIT_TESTING__
#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <cppunit/ui/text/TestRunner.h>

#include "../unit_tests/test_dbobject_mock.h"
#include "../unit_tests/test_test_1.h"
#include "../unit_tests/all_tests.h"
#endif

int main (int argc, char *argv[]) {
#ifdef __UNIT_TESTING__
all_tests::run_tests();
#else
vector<dbobject_interface*> dbobjects;
vector<dbobject_interface*>::iterator iterator_dbobjects;
vector<test_interface*> tests;
vector<test_interface*>::iterator iterator_tests;
bool breaked_running=false;

#ifdef __MOCK_OBJECT__
//remove all of dbobject_mock after testing
vector<string> connection_parameters;
connection_parameters.push_back("USERNAME=right_user");
connection_parameters.push_back("PASSWD=right_password");
try {
	dbobjects.push_back(new dbobject_mock("Объект-заглушка", connection_parameters));
} catch (dbobject_exception dbj_exc) {
	 cout<<"</br>In creation dbobject_mock raised exception :</br>"<<dbj_exc.get_description()<<"</br>"<<endl;
};
#endif


#ifdef __TEST_1__

tests.push_back(new test_1("Тестирование скорости вставки данных в таблицу.",
			   1024,
			   2048,
			   3,
			   5,
			   2));
#endif


cout<<"<html>\n<head>\n";
cout<<"\t<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\">\n";
cout<<"</head>\n<body>\n"<<endl;

iterator_tests=tests.begin();
while (iterator_tests!=tests.end()){
    iterator_dbobjects=dbobjects.begin();
    while(iterator_dbobjects!=dbobjects.end()){
	cout<<"\n<h1>"<<(*iterator_tests)->get_description()<<"</h1>\n"<<endl;
	try {
	   (*iterator_tests)->run(*iterator_dbobjects);
	} catch (test_exception tst_exc) {
		 cout<<tst_exc.get_description()<<endl;
         breaked_running=true;
		 break;
	};

	try {
	   cout<<(*iterator_tests)->html_table_report()<<endl;
	} catch (test_exception tst_exc) {
		 cout<<tst_exc.get_description()<<endl;
         breaked_running=true;	 
	 	break;
	};


	iterator_dbobjects++;
    };
    if (breaked_running) break;
iterator_tests++;
};

iterator_tests=tests.begin();
while (iterator_tests!=tests.end()) {
    delete(*iterator_tests);
    iterator_tests++;
};
	  
iterator_dbobjects=dbobjects.begin();
while (iterator_dbobjects!=dbobjects.end()) {
	try{
    delete(*iterator_dbobjects);
    } catch (dbobject_exception dbj_exc){
		 cout<<dbj_exc.get_description()<<endl;    		
    };
    iterator_dbobjects++;
};

cout << "\n</body>\n</html>\n";
#endif
return 1;
};