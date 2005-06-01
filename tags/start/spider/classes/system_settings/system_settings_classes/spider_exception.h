#ifndef __SPIDER_EXCEPTION__
#define __SPIDER_EXCEPTION__

class spider_exception {
private:
    string test_exc_descr;

public:
    explicit spider_exception (string description) : 
	    test_exc_descr (description) {};

    string get_description () {return test_exc_descr;};
};
#endif
