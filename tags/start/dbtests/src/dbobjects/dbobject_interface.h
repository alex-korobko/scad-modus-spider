#ifndef __DBOBJECT_INTERFACE__
#define __DBOBJECT_INTERFACE__

class dbobject_interface {
private:
    string dbobject_description;

public:
    dbobject_interface(string dbobject_description) : dbobject_description(dbobject_description) {};

    string get_description() {
	return dbobject_description;
    };
   
    virtual uint64_t test_1_insert_blob(vector<byte> buffer) throw (dbobject_exception)=0;  
    virtual uint64_t test_1_turncate_table() throw (dbobject_exception)=0;  
    
    virtual ~dbobject_interface(){};
};

#endif