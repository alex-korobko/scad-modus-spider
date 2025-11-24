#ifndef __DBOBJECT_MOCK__
#define __DBOBJECT_MOCK__

class dbobject_mock : public dbobject_interface {
public:
  dbobject_mock(string dbobject_description, 
		vector<string> connection_parameters) throw (dbobject_exception);
     
  virtual uint64_t test_1_insert_blob(vector<byte> buffer) throw (dbobject_exception);
  virtual uint64_t test_1_turncate_table() throw (dbobject_exception);

  virtual ~dbobject_mock(){};
};

#endif