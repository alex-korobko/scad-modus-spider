#ifndef __TEST_INTERFACE__
#define __TEST_INTERFACE__

class test_interface {
private:
uint64_t cycles_per_second;
string test_description;

public:
    float calculate_sample_mean(vector<uint64_t> sample) throw (test_exception);
    float calculate_sample_variance(vector<uint64_t> sample) throw (test_exception);

    test_interface(string test_description);

    string get_description();
	uint64_t get_cycles_per_second();

    virtual void run(dbobject_interface *dbobject) throw (test_exception)=0;
    virtual string html_table_report() throw (test_exception)=0;
  
    virtual ~test_interface(){};
};

#endif