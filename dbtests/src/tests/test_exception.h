#ifndef __TEST_EXCEPTION__
#define __TEST_EXCEPTION__

class test_exception {
private:
    string test_exc_descr;

public:
    explicit test_exception (string description) : 
	    test_exc_descr (description) {};

    string get_description () {return test_exc_descr;};
};
#endif
