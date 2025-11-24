#ifndef __DBOBJECT_BERKELEY_DB__
#define __DBOBJECT_BERKELEY_DB__

class dbobject_berkeley_db : public dbobject_interface {
private:
	DB *database_handler;
    DBTYPE database_type;
    string file_name;
public:
  dbobject_berkeley_db(string dbobject_description, 
		vector<string> connection_parameters) throw (dbobject_exception);
     
  virtual uint64_t test_1_insert_blob(vector<byte> buffer) throw (dbobject_exception);
  virtual uint64_t test_1_turncate_table() throw (dbobject_exception);

  virtual ~dbobject_berkeley_db();
};

#endif