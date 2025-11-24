using namespace std;

#include <sys/syspage.h>
#include <math.h>
#include <inttypes.h>
#include <iostream.h>

#include <vector>
#include <string>
#include <numeric>

#include "../defines.h"

#include "../dbobjects/dbobject_exception.h"
#include "../dbobjects/dbobject_interface.h"

#include "test_exception.h"
#include "test_interface.h"

uint64_t test_interface::cycles_per_second=SYSPAGE_ENTRY(qtime)->cycles_per_sec;

test_interface::test_interface(string test_description)
          :test_description(test_description){};

string test_interface::get_description() { return test_description;};

double test_interface::calculate_sample_mean(vector<uint64_t> sample)
	 throw (test_exception){
if (sample.empty())  throw test_exception("Can`t calculate sample mean of empty sample");
float sample_mean=0;
sample_mean=accumulate(sample.begin(), sample.end(), sample_mean);
sample_mean = sample_mean/sample.size();
return sample_mean;
};

double test_interface::calculate_standart_deviation(vector<uint64_t> sample)
	 throw (test_exception){
if (sample.empty())  throw test_exception("Can`t calculate sample dispersion of empty sample");
if (sample.size()==1) throw test_exception("Can`t calculate sample dispersion of sample with one element");
double sample_mean=0;
double standart_deviation=0;

sample_mean=accumulate(sample.begin(), sample.end(), sample_mean);
vector<uint64_t>::size_type sample_size = sample.size();
sample_mean = sample_mean/sample_size;
vector<uint64_t>::iterator iter_sample;

iter_sample =  sample.begin();
while (iter_sample != sample.end()) {
    standart_deviation+=pow(*iter_sample-sample_mean,2);
    iter_sample++;
};

standart_deviation /=(sample_size-1);

return sqrt(standart_deviation);
};
																  