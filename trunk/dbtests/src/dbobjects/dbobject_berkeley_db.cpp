using namespace std;

#include <sys/neutrino.h>
#include <inttypes.h>
#include <sys/types.h>
#include <stdlib.h>
#include <inttypes.h>

#include <db_cxx.h>

#include <vector>
#include <string>

#include <iostream.h>

#include "../defines.h"

#include "dbobject_exception.h"
#include "dbobject_interface.h"
#include "dbobject_berkeley_db.h"

dbobject_berkeley_db::dbobject_berkeley_db(string dbobject_description, 
		vector<string> connection_parameters) throw (dbobject_exception) :
	dbobject_interface(dbobject_description){
/*
connection_parameters[0] database type
connection_parameters[1] file name
connection_parameters[2] open flags
connection_parameters[3] record length (must be specifed for DB_QUEUE and may be for DB_RECNO)
*/
	int ret_value;
    u_int32_t database_flags=0, record_length;
    const char *database_file_name;
	string exception_description;

     if (( ret_value= db_create(&database_handler, NULL, 0)) != 0)
                throw dbobject_exception(db_strerror(ret_value));

	if (connection_parameters.empty()) {
        exception_description+="in connection_parameters must be database type";
        throw dbobject_exception(exception_description); 
	};

	if (connection_parameters[0].compare("DB_HASH")==0) {
	      database_type=DB_HASH;
	 } else if (connection_parameters[0].compare("DB_BTREE")==0) {
 	      database_type=DB_BTREE;
	 } else if (connection_parameters[0].compare("DB_QUEUE")==0) {
 	      database_type=DB_QUEUE;
 	        if (connection_parameters.size()>3 &&
               !connection_parameters[3].empty()) {
                     record_length=atol(connection_parameters[3].c_str());
                          if (record_length==0) {
                             exception_description+="wrong record length : ";
                             exception_description+=connection_parameters[3];
                             throw dbobject_exception(exception_description); 
                          };

                      if((ret_value=database_handler->set_re_len(database_handler, record_length))>0) {
                             exception_description+=" database_handler->set_re_len : ";
                             exception_description+=db_strerror(ret_value);
                             throw dbobject_exception(exception_description); 
                             };
                         
	                } else {
                       throw dbobject_exception(
                             "for database type DB_QUEUE must be specifed connection_parameters[3] record size"
                             ); 
	                };

	 } else if (connection_parameters[0].compare("DB_RECNO")==0) {
 	      database_type=DB_RECNO;
          if (connection_parameters.size()>3 &&
	         !connection_parameters[3].empty()) {
                  record_length=atol(connection_parameters[3].c_str());
                  if (record_length==0) {
                         exception_description+="wrong record length : ";
                         exception_description+=connection_parameters[3];
                         throw dbobject_exception(exception_description); 
                    };
                 if((ret_value=database_handler->set_re_len(database_handler, record_length))>0) {
                         exception_description+=" database_handler->set_re_len : ";
                          exception_description+=db_strerror(ret_value);
                          throw dbobject_exception(exception_description); 
                   };

	      };
	 } else {
          exception_description+="unknown database type ";
          exception_description+=connection_parameters[0];
          throw dbobject_exception(exception_description); 
	 };

	if (connection_parameters.size()>1 &&
	     !connection_parameters[1].empty()) {
	        file_name=connection_parameters[1];
	        database_file_name=connection_parameters[1].c_str();
	        } else {
	        database_file_name=NULL;
	        };

	if (connection_parameters.size()>2) {
          if (static_cast<int>(connection_parameters[2].find("DB_CREATE"))!=-1) {
             database_flags|=DB_CREATE;
          };
          if (static_cast<int>(connection_parameters[2].find("DB_EXCL"))!=-1) {
             database_flags|=DB_EXCL;
          };
          if (static_cast<int>(connection_parameters[2].find("DB_RDONLY"))!=-1) {
             database_flags|=DB_RDONLY;
          };
          if (static_cast<int>(connection_parameters[2].find("DB_THREAD"))!=-1) {
             database_flags|=DB_THREAD;
          };
    };

	if ((ret_value = database_handler->open(
	                         database_handler,
                             NULL,  // without transactions
                             database_file_name,
                             NULL, // one database in file
                             database_type,
                             database_flags,
                             0664)) != 0)
                throw dbobject_exception(db_strerror(ret_value));      
};


uint64_t dbobject_berkeley_db::test_1_insert_blob(vector<byte> buffer) throw (dbobject_exception) {
int ret_val;
if (buffer.empty()) throw dbobject_exception("Buffer to insertion is empty");
uint64_t  begin_system_ticks;
u_int32_t rec_size;
int key_value;
DBT key, data;

if (database_type==DB_HASH ||
    database_type==DB_BTREE) {
       memset(&key, 0, sizeof(key));
       memset(&data, 0, sizeof(data));

       key_value=rand();
       key.data = &key_value;
       key.size = sizeof(int);
       data.data = &buffer[0];
       data.size = buffer.size();

      begin_system_ticks=ClockCycles();
        if ((ret_val=database_handler->put(database_handler, NULL, &key, &data, 0))!=0)
           throw dbobject_exception(db_strerror(ret_val));
         return (ClockCycles()-begin_system_ticks);
   } else  if (database_type==DB_QUEUE ||
                  database_type==DB_RECNO){

      memset(&key, 0, sizeof(key));
      memset(&data, 0, sizeof(data));
/*       
      database_handler->get_re_len(database_handler, &rec_size);
      cout<<"In insert rec_size "<<rec_size<<"<br>"<<endl;
      buffer.assign(rec_size, 0);
  */     
      data.data = &buffer[0];
      data.size = buffer.size();

        begin_system_ticks=ClockCycles();
        if ((ret_val=database_handler->put(database_handler, NULL, &key, &data, DB_APPEND))!=0)
//        if ((ret_val=database_handler->put(database_handler, NULL, &key, &data, 0))!=0)
           throw dbobject_exception(db_strerror(ret_val));
        return (ClockCycles()-begin_system_ticks);
   } else  {
       throw dbobject_exception("dbobject_berkeley_db::test_1_insert_blob(...) Urecognized database type");
   };

};

uint64_t dbobject_berkeley_db::test_1_turncate_table() throw (dbobject_exception){
u_int32_t turnc_count=0;
int ret_val;
uint64_t  begin_system_ticks=ClockCycles();
if ((ret_val=database_handler->truncate(database_handler, NULL, &turnc_count, 0))!=0) {
           throw dbobject_exception(db_strerror(ret_val));
      };
return (ClockCycles()-begin_system_ticks); 
};

dbobject_berkeley_db::~dbobject_berkeley_db() {
    database_handler->close(database_handler, 0);
};


