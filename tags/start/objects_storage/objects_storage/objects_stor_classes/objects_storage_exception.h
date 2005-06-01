#ifndef _OBJECTS_STORAGE_EXCEPTION_
#define _OBJECTS_STORAGE_EXCEPTION_

class objects_storage_exception {
private:
    string test_exc_descr;

public:
    explicit objects_storage_exception (string description) : 
	    test_exc_descr (description) {};

    string get_description () {return test_exc_descr;};
};
#endif

