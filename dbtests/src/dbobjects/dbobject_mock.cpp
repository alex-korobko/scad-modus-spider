using namespace std;

#include <inttypes.h>

#include <vector>
#include <string>

#include "../defines.h"

#include "dbobject_exception.h"
#include "dbobject_interface.h"
#include "dbobject_mock.h"

dbobject_mock::dbobject_mock(string dbobject_description, 
		vector<string> connection_parameters) throw (dbobject_exception) :
	dbobject_interface(dbobject_description){

	//to real dbobjects code of connection to DB here 
	string right_username("USERNAME=right_user");
	string right_password("PASSWD=right_password");

	if (connection_parameters.empty() ||
 	   connection_parameters.size()!=2)
     throw dbobject_exception("Connection failed: bad connection parameters");

	if (connection_parameters[0].compare(right_username)!=0 ||
 	   connection_parameters[1].compare(right_password)!=0) 
     throw dbobject_exception("Connection failed: bad connection parameters");
	};


uint64_t dbobject_mock::test_1_insert_blob(vector<byte> buffer) throw (dbobject_exception) {
    if (buffer.empty()) throw dbobject_exception("Buffer to insertion is empty");
    return 50;
};

uint64_t dbobject_mock::test_1_turncate_table() throw (dbobject_exception){
    return 40;
};



