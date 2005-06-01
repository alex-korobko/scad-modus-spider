#ifndef __COMPORT_EXCEPTION__
#define __COMPORT_EXCEPTION__

class comport_exception {
private:
    string comport_exc_descr;

public:
    explicit comport_exception (string description) : 
	    comport_exc_descr (description) {};

    string get_description () {return comport_exc_descr;};
};
#endif
