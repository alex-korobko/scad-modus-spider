#ifndef __GENERATOR_EXCEPTION__
#define __GENERATOR_EXCEPTION__

class generator_exception {
private:
   string test_exc_descr;

public:
    explicit generator_exception (string description) : 
	    test_exc_descr (description) {};

    string get_description () {return test_exc_descr;};
};


#endif // __GENERATOR_EXCEPTION__
