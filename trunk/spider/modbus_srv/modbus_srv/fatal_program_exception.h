#ifndef __FATAL_PROGRAM_EXCEPTION__
#define __FATAL_PROGRAM_EXCEPTION__

class fatal_program_exception {
private:
    string fatal_exc_descr;
    
    public:
    explicit fatal_program_exception (string description) :
	            fatal_exc_descr (description) {};
		    
    string get_description () {return fatal_exc_descr;};
};
#endif