using namespace std;

#include <inttypes.h>

#include <iostream.h>
#include <vector>
#include <string>

#include  "../tests.h"
#include  "../dbobjects.h"
#include  "test_cond.h"
#include  "defines.h"

#include "dbobjects/dbobject_exception.h"
#include "dbobjects/dbobject_interface.h"

#ifdef __MOCK_OBJECT__
#include "dbobjects/dbobject_mock.h"
#endif

#ifdef __BERKELEY_DB_OBJECT__
#include <inttypes.h>
#include <db.h>
#include "dbobjects/dbobject_berkeley_db.h"
#endif

#include "tests/test_exception.h"
#include "tests/test_interface.h"

#ifdef __TEST_1__
#include "tests/test_1.h"
#endif

#ifdef __UNIT_TESTING__
#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <cppunit/ui/text/TestRunner.h>

#ifdef __MOCK_OBJECT__
#include "../unit_tests/test_dbobject_mock.h"
#endif

#ifdef __BERKELEY_DB_OBJECT__
#include "../unit_tests/test_dbobject_berkeley_db.h"
#endif

#ifdef __TEST_1__
#include "../unit_tests/test_test_1.h"
#endif

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
vector<string> connection_parameters;

//data for test 1
string test_1_description("Тестирование скорости вставки данных в таблицу.");
int test_1_min_data_block=1024; //kbytes
int test_1_max_data_block=2048; //kbytes
int test_1_data_block_split_count=3;
int test_1_buffer_split_count=5;
int test_1_repeats_count=2;



#ifdef __MOCK_OBJECT__
//remove all of dbobject_mock after testing
connection_parameters.clear();
connection_parameters.push_back("USERNAME=right_user");
connection_parameters.push_back("PASSWD=right_password");
try {
	dbobjects.push_back(new dbobject_mock("Объект-заглушка", connection_parameters));
} catch (dbobject_exception dbj_exc) {
	 cout<<"</br>In creation dbobject_mock raised exception :</br>"<<dbj_exc.get_description()<<"</br>"<<endl;
};
#endif


#ifdef __BERKELEY_DB_OBJECT__

connection_parameters.clear();
connection_parameters.push_back("DB_HASH");
connection_parameters.push_back("berkeley.db");
connection_parameters.push_back("DB_CREATE");
try {
	dbobjects.push_back(new dbobject_berkeley_db("berkeleyDB object<br> type DB_HASH on hard disk", connection_parameters));
} catch (dbobject_exception dbj_exc) {
	 cout<<"</br>In creation berkeleyDB object raised exception :</br>"<<dbj_exc.get_description()<<"</br>"<<endl;
};

connection_parameters.clear();
connection_parameters.push_back("DB_RECNO");
connection_parameters.push_back("");
connection_parameters.push_back("DB_CREATE");
connection_parameters.push_back("2097151");
try {
	dbobjects.push_back(new dbobject_berkeley_db("berkeleyDB object<br> type DB_RECNO in memory", connection_parameters));
} catch (dbobject_exception dbj_exc) {
	 cout<<"</br>In creation berkeleyDB object raised exception :</br>"<<dbj_exc.get_description()<<"</br>"<<endl;
};

#endif

#ifdef __TEST_1__
tests.push_back(new test_1( test_1_description,
                                      test_1_min_data_block,
                                      test_1_max_data_block,
                                      test_1_data_block_split_count,
                                      test_1_buffer_split_count,
                                      test_1_repeats_count
                        ));
#endif

cout<<"<html>\n<head>\n";
cout<<"\t<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\">\n";
cout<<"</head>\n<body>\n"<<endl;

iterator_tests=tests.begin();
while (iterator_tests!=tests.end()){
	cout<<"\n<center><h1>"<<(*iterator_tests)->get_description()<<"</h1></center>\n"<<endl;


    iterator_dbobjects=dbobjects.begin();
    while(iterator_dbobjects!=dbobjects.end()){
	try {
	   (*iterator_tests)->run(*iterator_dbobjects);
	} catch (test_exception tst_exc) {
		 cout<<tst_exc.get_description()<<endl;
	};

	iterator_dbobjects++;
    }; //while(iterator_dbobjects!=dbobjects.end())

	try {
	   cout<<(*iterator_tests)->html_table_report()<<endl;
	} catch (test_exception tst_exc) {
		 cout<<tst_exc.get_description()<<endl;
	};

iterator_tests++;
}; //while (iterator_tests!=tests.end())

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