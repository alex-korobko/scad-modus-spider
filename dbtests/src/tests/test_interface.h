#ifndef __TEST_INTERFACE__
#define __TEST_INTERFACE__

class test_interface {
private:
string test_description;

public:
    test_interface(string test_description);

    string get_description();

    virtual void run(dbobject_interface *dbobject) throw (test_exception)=0;
    virtual string html_table_report() throw (test_exception)=0;
  
    virtual ~test_interface(){};
    
    static double calculate_sample_mean(vector<uint64_t> sample) throw (test_exception);
    static double calculate_standart_deviation(vector<uint64_t> sample) throw (test_exception);
	static uint64_t cycles_per_second;
};

#endif