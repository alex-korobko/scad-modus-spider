#ifndef __DBOBJECT_EXCEPTION__
#define __DBOBJECT_EXCEPTION__

class dbobject_exception {
private:
    string dbobj_exc_descr;

public:
    explicit dbobject_exception (string description) : 
	    dbobj_exc_descr (description) {};

    string get_description () {return dbobj_exc_descr;};
};
#endif
