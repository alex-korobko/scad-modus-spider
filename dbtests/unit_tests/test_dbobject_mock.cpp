using namespace std;

#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

#include <inttypes.h>

#include <vector>
#include <string>
#include <sstream>

#include "../src/defines.h"

#include "../src/dbobjects/dbobject_exception.h"
#include "../src/dbobjects/dbobject_interface.h"
#include "../src/dbobjects/dbobject_mock.h"
#include "test_dbobject_mock.h"

void test_dbobject_mock::test_object_right() {
    uint64_t ret_val;
    vector<byte> buffer;
    buffer.push_back(1);
    	ostringstream message_to_return;
	dbobject_interface *right_dbobj;
	
    vector<string> connection_parameters;
    connection_parameters.push_back(right_username);
    connection_parameters.push_back(right_password);
	
    try {
    right_dbobj = new dbobject_mock(dbobject_description,
							connection_parameters);
    } catch (dbobject_exception db_exc) {
    		delete(right_dbobj);
        CPPUNIT_FAIL("Unexpected exception:"+db_exc.get_description());
    };
    	if (dbobject_description.compare(right_dbobj->get_description())!=0) {
    		message_to_return << "Expected:\n"<<dbobject_description<<"\nbut:\n"<<right_dbobj->get_description()<<endl;
        delete(right_dbobj);
	     CPPUNIT_FAIL(message_to_return.str());
	};

    try {
        ret_val=right_dbobj->test_1_insert_blob(buffer);
    } catch (dbobject_exception db_exc) {
    		message_to_return << "Unexpected exception:"<<db_exc.get_description()<<endl;
        delete(right_dbobj);
        CPPUNIT_FAIL(message_to_return.str());
    };
    	if (ret_val!=50) {
   			message_to_return<<"Expected : "<<50<<"\nbut\n"<<static_cast<long>(ret_val)<<endl;
   			CPPUNIT_FAIL(message_to_return.str());
    	};

    try {
        ret_val=right_dbobj->test_1_turncate_table();
    } catch (dbobject_exception db_exc) {
        delete(right_dbobj);
        CPPUNIT_FAIL("Unexpected exception:"+db_exc.get_description());
    };

    	if (ret_val!=40) {
   			message_to_return<<"Expected : "<<40<<"\nbut\n"<<static_cast<long>(ret_val)<<endl;
   			CPPUNIT_FAIL(message_to_return.str());
    	};
    
    delete(right_dbobj);
};


void test_dbobject_mock::test_object_wrong() {
    bool exception_raised;    
    uint64_t ret_val;
    vector<byte> buffer;
    buffer.push_back(1);
    vector<string> connection_parameters;
    dbobject_interface *wrong_dbobj;

    exception_raised=false;
    try {
	    wrong_dbobj = new dbobject_mock(dbobject_description,
					    connection_parameters);

    } catch (dbobject_exception db_exc) {
	CPPUNIT_ASSERT_EQUAL(db_exc.get_description(),
		    string("Connection failed: bad connection parameters"));
	exception_raised=true;
    };
    if (!exception_raised) {
	 CPPUNIT_FAIL("Exception don`t raised");
    };
	 
    connection_parameters.push_back(right_username);
    exception_raised=false;
    try {
    wrong_dbobj = new dbobject_mock(dbobject_description,
				    connection_parameters);
    } catch (dbobject_exception db_exc) {
	CPPUNIT_ASSERT_EQUAL(db_exc.get_description(),
		    string("Connection failed: bad connection parameters"));
	exception_raised=true;
    };
    if (!exception_raised) {
	 CPPUNIT_FAIL("Exception don`t raised");
    };

    connection_parameters.push_back(wrong_password);
    exception_raised=false;
    try {
    wrong_dbobj = new dbobject_mock(dbobject_description,
				    connection_parameters);
    } catch (dbobject_exception db_exc) {
	CPPUNIT_ASSERT_EQUAL(db_exc.get_description(),
		    string("Connection failed: bad connection parameters"));
	exception_raised=true;
    };
    if (!exception_raised) {
	 CPPUNIT_FAIL("Exception don`t raised");
    };

    connection_parameters[0]=wrong_username;
    connection_parameters[1]=right_password;
    exception_raised=false;
    try {
    wrong_dbobj = new dbobject_mock(dbobject_description,
				    connection_parameters);
    } catch (dbobject_exception db_exc) {
	CPPUNIT_ASSERT_EQUAL(db_exc.get_description(),
		    string("Connection failed: bad connection parameters"));
	exception_raised=true;
    };
    if (!exception_raised) {
	 CPPUNIT_FAIL("Exception don`t raised");
    };

    connection_parameters[0]=wrong_username;
    connection_parameters[1]=wrong_password;
    exception_raised=false;
    try {
	    wrong_dbobj = new dbobject_mock(dbobject_description,
				    connection_parameters);

    } catch (dbobject_exception db_exc) {
	CPPUNIT_ASSERT_EQUAL(db_exc.get_description(),
		    string("Connection failed: bad connection parameters"));
	exception_raised=true;
    };
    if (!exception_raised) {
	 CPPUNIT_FAIL("Exception don`t raised");
    };

    connection_parameters[0]=right_username;
    connection_parameters[1]=right_password;
    try {
	    wrong_dbobj = new dbobject_mock(dbobject_description,
						    connection_parameters);
    } catch (dbobject_exception db_exc) {
        CPPUNIT_FAIL("Unexpected exception:"+db_exc.get_description());
    };

    buffer.clear();
    exception_raised=false;
    try {
        ret_val=wrong_dbobj->test_1_insert_blob(buffer);
    } catch (dbobject_exception db_exc) {
	CPPUNIT_ASSERT_EQUAL(db_exc.get_description(),
		    string("Buffer to insertion is empty"));
	exception_raised=true;
    };
    if (!exception_raised) {
	 CPPUNIT_FAIL("Exception don`t raised");
    };
   
};

void test_dbobject_mock::setUp(){
right_username="USERNAME=right_user";
wrong_username="USERNAME=wrong_user";
right_password="PASSWD=right_password";
wrong_password="PASSWD=wrong_password";
dbobject_description="Dbobject mock";
};

void test_dbobject_mock::tearDown(){
};

CppUnit::TestSuite* test_dbobject_mock::suite(){
CppUnit::TestSuite *suite_of_tests=new CppUnit::TestSuite("Mock Object of databases wrapper object Tests");

suite_of_tests->addTest(new CppUnit::TestCaller<test_dbobject_mock>
    ( "test of dbobject_mock, no exceptions",
    &test_dbobject_mock::test_object_right));

suite_of_tests->addTest(new CppUnit::TestCaller<test_dbobject_mock>
    ( "test of dbobject_mock, with exceptions",
    &test_dbobject_mock::test_object_wrong));


return suite_of_tests;

};
