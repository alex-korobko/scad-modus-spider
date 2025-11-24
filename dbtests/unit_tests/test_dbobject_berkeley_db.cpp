using namespace std;

#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

#include <inttypes.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <vector>
#include <string>
#include <sstream>

#include <db_cxx.h>

#include "../src/defines.h"

#include "../src/dbobjects/dbobject_exception.h"
#include "../src/dbobjects/dbobject_interface.h"
#include "../src/dbobjects/dbobject_berkeley_db.h"
#include "test_dbobject_berkeley_db.h"

void test_dbobject_berkeley_db::test_object_right() {
    vector<byte> buffer;
    buffer.push_back(1);
	dbobject_interface *right_dbobj;
	ostringstream message_to_return;
	struct stat file_info;
	int file_desc, ret_val;
	
    vector<string> connection_parameters;

     file_desc=open( database_file_name.c_str() , O_RDONLY);
     if (file_desc>0) {
           if (fstat(file_desc, &file_info)<0) {
                string error_message(strerror(errno));
                if (close (file_desc)<0) {
                         CPPUNIT_FAIL("Can`t stat&close database file "+database_file_name+" : "+error_message+" and "+strerror(errno));
                        };      
                      CPPUNIT_FAIL("Can`t stat database file "+database_file_name+" : "+error_message);
                   } else {
                      if (S_ISREG(file_info.st_mode)!=1)
                          CPPUNIT_FAIL("Database file "+database_file_name+" must be simply file");
                   };
                if (close (file_desc)<0) {
                      CPPUNIT_FAIL("Can`t close database file "+database_file_name+" : "+strerror(errno));
                   };
                if (unlink(database_file_name.c_str())<0)  {
                       CPPUNIT_FAIL("Can`t unlink database file "+database_file_name+" : "+strerror(errno));
                   };
            };


     for(vector<string>::size_type i = 0 ; i<supported_database_types.size(); i++) {
     supported_dbobject_flags.clear();
     supported_dbobject_flags.push_back("DB_CREATE");
     supported_dbobject_flags.push_back("DB_CREATE|DB_EXCL");

     if (supported_database_types[i].compare("DB_HASH")==0 ||
         supported_database_types[i].compare("DB_BTREE")==0 ) {
        supported_dbobject_flags.push_back("DB_CREATE|DB_THREAD"); 
     } ;
     
        for(vector<string>::size_type j = 0 ; j<supported_dbobject_flags.size(); j++) {

        connection_parameters.clear();
        connection_parameters.push_back(supported_database_types[i]);
        connection_parameters.push_back(database_file_name);
        connection_parameters.push_back(supported_dbobject_flags[j]);

        if (supported_database_types[i].compare("DB_QUEUE")==0)
                                      connection_parameters.push_back("10");
        try {
                right_dbobj = new dbobject_berkeley_db(dbobject_description,
							connection_parameters);
              } catch (dbobject_exception db_exc) {
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

           try {
              ret_val=right_dbobj->test_1_turncate_table();
            } catch (dbobject_exception db_exc) {
              delete(right_dbobj);
              CPPUNIT_FAIL("Unexpected exception:"+db_exc.get_description());
            };

           try {
              delete(right_dbobj);
           } catch (dbobject_exception db_exc) {
              delete(right_dbobj);
              CPPUNIT_FAIL("Unexpected exception:"+db_exc.get_description());
           }; 

        file_desc=open( database_file_name.c_str() , O_RDONLY);
         if (file_desc>0) {
                if (fstat(file_desc, &file_info)<0) {
                    string error_message(strerror(errno));
                    if (close (file_desc)<0) {
                         CPPUNIT_FAIL("Can`t stat&close database file "+database_file_name+" : "+error_message+" and "+strerror(errno));
                        };      
                      CPPUNIT_FAIL("Can`t stat database file "+database_file_name+" : "+error_message);
                   } else {
                      if (S_ISREG(file_info.st_mode)!=1)
                          CPPUNIT_FAIL("Database file "+database_file_name+" must be simply file");
                   };
                if (close (file_desc)<0) {
                      CPPUNIT_FAIL("Can`t close database file "+database_file_name+" : "+strerror(errno));
                   };
                if (unlink(database_file_name.c_str())<0)  {
                       CPPUNIT_FAIL("Can`t unlink database file "+database_file_name+" : "+strerror(errno));
                   };
            };

             }; //for(vector<string>::size_type j = 0 ; j<supported_dbobject_flags
       }; //for(vector<string>::size_type i = 0 ; i<supported_database_types


     for(vector<string>::size_type i = 0 ; i<supported_database_types.size(); i++) {
        supported_dbobject_flags.clear();
        supported_dbobject_flags.push_back("DB_CREATE");
        supported_dbobject_flags.push_back("DB_CREATE|DB_EXCL");

        if (supported_database_types[i].compare("DB_HASH")==0 ||
           supported_database_types[i].compare("DB_BTREE")==0 ) {
           supported_dbobject_flags.push_back("DB_CREATE|DB_THREAD"); 
         } ;

        for(vector<string>::size_type j = 0 ; j<supported_dbobject_flags.size(); j++) {
        connection_parameters.clear();
        connection_parameters.push_back(supported_database_types[i]);
        connection_parameters.push_back("");
        connection_parameters.push_back(supported_dbobject_flags[j]);

        if (supported_database_types[i].compare("DB_QUEUE")==0)
                                      connection_parameters.push_back("10");

        try {
                right_dbobj = new dbobject_berkeley_db(dbobject_description,
							connection_parameters);
              } catch (dbobject_exception db_exc) {
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

           try {
              ret_val=right_dbobj->test_1_turncate_table();
            } catch (dbobject_exception db_exc) {
              delete(right_dbobj);
              CPPUNIT_FAIL("Unexpected exception:"+db_exc.get_description());
            };

           try {
              delete(right_dbobj);
           } catch (dbobject_exception db_exc) {
              CPPUNIT_FAIL("Unexpected exception:"+db_exc.get_description());
           }; 

       }; // for(vector<string>::size_type j = 0 ; j<supported_dbobject_flags
    }; // for(vector<string>::size_type i = 0 ; i<supported_database_types
};


void test_dbobject_berkeley_db::test_object_wrong() {
    bool exception_raised;
    vector<byte> buffer;
    buffer.push_back(1);
    vector<string> connection_parameters;
    dbobject_interface *wrong_dbobj;
	struct stat file_info;
	int file_desc;

     file_desc=open( database_file_name.c_str() , O_RDONLY);
     if (file_desc>0) {
            if (fstat(file_desc, &file_info)<0) {
                    string error_message(strerror(errno));
                    if (close (file_desc)<0) {
                         CPPUNIT_FAIL("Can`t stat&close database file "+database_file_name+" : "+error_message+" and "+strerror(errno));
                        };      
                      CPPUNIT_FAIL("Can`t stat database file "+database_file_name+" : "+error_message);
                   } else {
                      if (S_ISREG(file_info.st_mode)!=1)
                          CPPUNIT_FAIL("Database file "+database_file_name+" must be simply file");
                   };
                if (close (file_desc)<0) {
                      CPPUNIT_FAIL("Can`t close database file "+database_file_name+" : "+strerror(errno));
                   };
                if (unlink(database_file_name.c_str())<0)  {
                       CPPUNIT_FAIL("Can`t unlink database file "+database_file_name+" : "+strerror(errno));
                   };
        };

    exception_raised=false;
    try {
	    wrong_dbobj = new dbobject_berkeley_db(dbobject_description,
					    connection_parameters);

    } catch (dbobject_exception db_exc) {
	CPPUNIT_ASSERT_EQUAL(db_exc.get_description(),
		    string("in connection_parameters must be database type"));
	exception_raised=true;
    };
    if (!exception_raised) {
	 CPPUNIT_FAIL("Exception don`t raised");
    };
 
    connection_parameters.clear();
    connection_parameters.push_back("ERROR_DB_TYPE");
    exception_raised=false;
    try {
    wrong_dbobj = new dbobject_berkeley_db(dbobject_description,
				    connection_parameters);
    } catch (dbobject_exception db_exc) {
	CPPUNIT_ASSERT_EQUAL(db_exc.get_description(),
		    string("unknown database type ERROR_DB_TYPE"));
	exception_raised=true;
    };
    if (!exception_raised) {
	 CPPUNIT_FAIL("Exception don`t raised");
    };

    connection_parameters.clear();
    connection_parameters.push_back("DB_QUEUE");
    connection_parameters.push_back(database_file_name);
    connection_parameters.push_back("DB_CREATE");
    exception_raised=false;
    try {
    wrong_dbobj = new dbobject_berkeley_db(dbobject_description,
				    connection_parameters);
    } catch (dbobject_exception db_exc) {
	CPPUNIT_ASSERT_EQUAL(db_exc.get_description(),
		    string("for database type DB_QUEUE must be specifed connection_parameters[3] record size"));
	exception_raised=true;
    };
    if (!exception_raised) {
	 CPPUNIT_FAIL("Exception don`t raised");
    };

    connection_parameters.clear();
    connection_parameters.push_back("DB_QUEUE");
    connection_parameters.push_back(database_file_name);
    connection_parameters.push_back("DB_CREATE");
    connection_parameters.push_back("wrong record length value");
    exception_raised=false;
    try {
    wrong_dbobj = new dbobject_berkeley_db(dbobject_description,
				    connection_parameters);
    } catch (dbobject_exception db_exc) {
	CPPUNIT_ASSERT_EQUAL(db_exc.get_description(),
		    string("wrong record length : wrong record length value"));
	exception_raised=true;
    };
    if (!exception_raised) {
	 CPPUNIT_FAIL("Exception don`t raised");
    };

    connection_parameters.clear();
    connection_parameters.push_back("DB_QUEUE");
    connection_parameters.push_back(database_file_name);
    connection_parameters.push_back("DB_CREATE");
    connection_parameters.push_back("0");
    exception_raised=false;
    try {
    wrong_dbobj = new dbobject_berkeley_db(dbobject_description,
				    connection_parameters);
    } catch (dbobject_exception db_exc) {
	CPPUNIT_ASSERT_EQUAL(db_exc.get_description(),
		    string("wrong record length : 0"));
	exception_raised=true;
    };
    if (!exception_raised) {
	 CPPUNIT_FAIL("Exception don`t raised");
    };

    connection_parameters.clear();
    connection_parameters.push_back("DB_HASH");
    connection_parameters.push_back(database_file_name);
    connection_parameters.push_back("DB_CREATE|DB_EXCL");
     try {
         wrong_dbobj = new dbobject_berkeley_db(dbobject_description,
					connection_parameters);
     } catch (dbobject_exception db_exc) {
         CPPUNIT_FAIL("Unexpected exception:"+db_exc.get_description());
     };

     try {
           delete(wrong_dbobj);
     } catch (dbobject_exception db_exc) {
           CPPUNIT_FAIL("Unexpected exception:"+db_exc.get_description());
     }; 

    exception_raised=false;
    connection_parameters[2]="DB_CREATE|DB_EXCL";
     try {
         wrong_dbobj = new dbobject_berkeley_db(dbobject_description,
					connection_parameters);
    } catch (dbobject_exception db_exc) {
	CPPUNIT_ASSERT_EQUAL(db_exc.get_description(),
		    string("File exists"));
	exception_raised=true;
    };
    if (!exception_raised) {
	 CPPUNIT_FAIL("Exception don`t raised");
    };

    exception_raised=false;
    connection_parameters[2]="DB_CREATE|DB_RDONLY";
     try {
         wrong_dbobj = new dbobject_berkeley_db(dbobject_description,
					connection_parameters);
    } catch (dbobject_exception db_exc) {
	CPPUNIT_ASSERT_EQUAL(db_exc.get_description(),
		    string("Invalid argument"));
	exception_raised=true;
    };
    if (!exception_raised) {
	 CPPUNIT_FAIL("Exception don`t raised");
    };

    file_desc=open( database_file_name.c_str() , O_RDONLY);
      if (file_desc>0) {
          if (fstat(file_desc, &file_info)<0) {
                    string error_message(strerror(errno));
                    if (close (file_desc)<0) {
                         CPPUNIT_FAIL("Can`t stat&close database file "+database_file_name+" : "+error_message+" and "+strerror(errno));
                        };      
                      CPPUNIT_FAIL("Can`t stat database file "+database_file_name+" : "+error_message);
                   } else {
                      if (S_ISREG(file_info.st_mode)!=1)
                          CPPUNIT_FAIL("Database file "+database_file_name+" must be simply file");
                   };
                if (close (file_desc)<0) {
                      CPPUNIT_FAIL("Can`t close database file "+database_file_name+" : "+strerror(errno));
                   };
                if (unlink(database_file_name.c_str())<0)  {
                       CPPUNIT_FAIL("Can`t unlink database file "+database_file_name+" : "+strerror(errno));
                   };
       };
   
};

void test_dbobject_berkeley_db::setUp(){
   dbobject_description="berkeleyDB object";
   database_file_name="test_berkeley.db";
   
   supported_database_types.push_back("DB_HASH");
   supported_database_types.push_back("DB_BTREE");
   supported_database_types.push_back("DB_QUEUE");
   supported_database_types.push_back("DB_RECNO");

};

void test_dbobject_berkeley_db::tearDown(){
};

CppUnit::TestSuite* test_dbobject_berkeley_db::suite(){
CppUnit::TestSuite *suite_of_tests=new CppUnit::TestSuite("Berkeley DB Object of databases wrapper object Tests");

suite_of_tests->addTest(new CppUnit::TestCaller<test_dbobject_berkeley_db>
    ( "test of dbobject_berkeley_db, with exceptions",
    &test_dbobject_berkeley_db::test_object_wrong));

suite_of_tests->addTest(new CppUnit::TestCaller<test_dbobject_berkeley_db>
    ( "test of dbobject_berkeley_db, no exceptions",
    &test_dbobject_berkeley_db::test_object_right));

return suite_of_tests;
};
