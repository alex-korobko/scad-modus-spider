#include <string>
#include <vector>
#include <hash_map>
#include <numeric>
#include <algorithm>
#include <sstream>
#include <stdexcept>

#include <iostream.h>

#include <inttypes.h>
#include <math.h>
#include <sys/neutrino.h>
#include <sys/syspage.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>

using namespace std;

#include "generator_common.h"
#include "generator_logger.h"
#include "mock_generator.h"

bytes mock_generator::A0_automata::command_to_storage(0);
bytes mock_generator::A1_automata::command_to_storage(0);

mock_generator::channel_b mock_generator::current_ch_b_side_a;
mock_generator::channel_b mock_generator::current_ch_b_side_b;
mock_generator::call_pointer mock_generator::current_call_pointer;

int mock_generator::channel_id(0);
int mock_generator::upo_count(0);
int mock_generator::ie1_count(0);
int mock_generator::e1_count(0);
int mock_generator::channelb_count(0);
int mock_generator::max_channel_number(0);
int mock_generator::max_destination(2);

static char state_random[32];

mock_generator::mock_generator() throw (runtime_error) :
request_cycle_begin(0),
request_cycle_end(0) {

if (ConnectAttach( 0,
               0,
               channel_id,
               0,
               0)<0) {
              string message ("mock_generator::mock_generator ConnectAttach(...) error : ");
              message+=strerror(errno);

              throw runtime_error(message);
   } ;

   initstate( time(NULL), state_random, sizeof(state_random));
   setstate(state_random);
};


mock_generator& mock_generator::get_instance() throw (runtime_error) {
   static mock_generator mock_generator_instance;
   return mock_generator_instance;
}

double mock_generator::
    calculate_sample_mean(results sample)throw (runtime_error){
if (sample.empty())  throw runtime_error("mock_generator::
    calculate_sample_mean Can`t calculate sample mean of empty sample");
float sample_mean=0;
sample_mean=accumulate(sample.begin(), sample.end(), sample_mean);
sample_mean = sample_mean/sample.size();
return sample_mean;
};

double mock_generator::
    calculate_standart_deviation(results sample) throw (runtime_error){
if (sample.empty())  
  throw runtime_error("mock_generator::
    calculate_sample_mean Can`t calculate sample deviation of empty sample");
if (sample.size()==1)
   throw runtime_error("mock_generator::
    calculate_sample_mean Can`t calculate sample deviation of sample with one element");
double sample_mean=0;
double standart_deviation=0;

sample_mean=accumulate(sample.begin(), sample.end(), sample_mean);
results::size_type sample_size = sample.size();
sample_mean = sample_mean/sample_size;
results::iterator iter_sample;

iter_sample =  sample.begin();
while (iter_sample != sample.end()) {
    standart_deviation+=pow(*iter_sample-sample_mean,2);
    iter_sample++;
};

standart_deviation /=(sample_size-1);
return sqrt(standart_deviation);
};


void  mock_generator::
        html_format_results_outputter() throw (runtime_error){
 mock_generator& gen_instance=mock_generator::get_instance();
	double 
          sample_mean_requests_to_occupate_channel_b_side_a, 
          sample_deviation_requests_to_occupate_channel_b_side_a, 

          sample_mean_requests_to_occupate_channel_b_side_b,
          sample_deviation_requests_to_occupate_channel_b_side_b, 

          sample_mean_request_to_free_channel_b, 
          sample_deviation_request_to_free_channel_b,

          sample_mean_error_to_occupation_channel_b,
          sample_deviation_error_to_occupation_channel_b,

          sample_mean_error_to_free_channel_b,
          sample_deviation_error_to_free_channel_b,

          sample_mean_requests_to_occupate_call_pointer,
          sample_deviation_requests_to_occupate_call_pointer,

          sample_mean_requests_to_free_call_pointer,
          sample_deviation_requests_to_free_call_pointer,

          sample_mean_error_to_occupate_call_pointer,
          sample_deviation_error_to_occupate_call_pointer,

          sample_mean_error_to_free_call_pointer,
          sample_deviation_error_to_free_call_pointer,

          sample_mean_request_of_channel_b_state_checking,
          sample_deviation_request_of_channel_b_state_checking,

          sample_mean_error_of_channel_b_state_checking,
          sample_deviation_error_of_channel_b_state_checking,

          simulation_time;

	uint64_t cycles_per_second=SYSPAGE_ENTRY(qtime)->cycles_per_sec,

          max_request_to_occupate_channel_b_side_a,
          min_request_to_occupate_channel_b_side_a, 

          max_request_to_occupate_channel_b_side_b,
          min_request_to_occupate_channel_b_side_b,

          max_request_to_free_channel_b, 
          min_request_to_free_channel_b, 

          max_error_to_occupation_channel_b,
          min_error_to_occupation_channel_b,

          max_error_to_free_channel_b,
          min_error_to_free_channel_b,

          max_request_to_occupate_call_pointer,
          min_request_to_occupate_call_pointer,

          max_request_to_free_call_pointer,
          min_request_to_free_call_pointer,

          max_error_to_occupate_call_pointer,
          min_error_to_occupate_call_pointer,

          max_error_to_free_call_pointer,
          min_error_to_free_call_pointer,

          max_request_of_channel_b_state_checking,
          min_request_of_channel_b_state_checking,

          max_error_of_channel_b_state_checking,
          min_error_of_channel_b_state_checking;

    ostringstream return_string;
    if (   !gen_instance.sample_requests_to_occupate_channel_b_side_a_times.empty() ||
            !gen_instance.sample_requests_to_occupate_channel_b_side_b_times.empty() ||
            !gen_instance.sample_request_to_free_channel_b_times.empty() ||
            !gen_instance.sample_error_to_occupation_channel_b_times.empty() ||
            !gen_instance.sample_error_to_free_channel_b_times.empty() ||
            !gen_instance.sample_requests_to_occupate_call_pointer_times.empty() ||
            !gen_instance.sample_requests_to_free_call_pointer_times.empty() ||
            !gen_instance.sample_error_to_occupate_call_pointer_times.empty()||
            !gen_instance.sample_error_to_free_call_pointer_times.empty()||

            !gen_instance.sample_request_of_channel_b_state_checking_times.empty()||
            !gen_instance.sample_error_of_channel_b_state_checking_times.empty()) {
                simulation_time=(gen_instance.request_cycle_end-
                                    gen_instance.request_cycle_begin)*1000/cycles_per_second;
             } else {
              	simulation_time=0;
             };

   // processing sample_requests_to_occupate_channel_b_side_a_times
    if (!gen_instance.sample_requests_to_occupate_channel_b_side_a_times.empty() &&
        gen_instance.sample_requests_to_occupate_channel_b_side_a_times.size()>1) {
	sample_mean_requests_to_occupate_channel_b_side_a=calculate_sample_mean(gen_instance.sample_requests_to_occupate_channel_b_side_a_times);
	sample_deviation_requests_to_occupate_channel_b_side_a=calculate_standart_deviation(gen_instance.sample_requests_to_occupate_channel_b_side_a_times);
    max_request_to_occupate_channel_b_side_a=*(max_element(gen_instance.sample_requests_to_occupate_channel_b_side_a_times.begin(),
                           gen_instance.sample_requests_to_occupate_channel_b_side_a_times.end()
                           ));    
     min_request_to_occupate_channel_b_side_a=*(min_element(gen_instance.sample_requests_to_occupate_channel_b_side_a_times.begin(),
                           gen_instance.sample_requests_to_occupate_channel_b_side_a_times.end()
                           ));

    } else if (gen_instance.sample_requests_to_occupate_channel_b_side_a_times.size()==1){
          sample_mean_requests_to_occupate_channel_b_side_a=gen_instance.sample_requests_to_occupate_channel_b_side_a_times[0];
          sample_deviation_requests_to_occupate_channel_b_side_a=0;
          max_request_to_occupate_channel_b_side_a=gen_instance.sample_requests_to_occupate_channel_b_side_a_times[0];
          min_request_to_occupate_channel_b_side_a=gen_instance.sample_requests_to_occupate_channel_b_side_a_times[0];
    } else {
	sample_mean_requests_to_occupate_channel_b_side_a=0;
	sample_deviation_requests_to_occupate_channel_b_side_a=0;
    max_request_to_occupate_channel_b_side_a=0;
    min_request_to_occupate_channel_b_side_a=0;
    };

   // processing sample_requests_to_occupate_channel_b_side_b_times
    if (!gen_instance.sample_requests_to_occupate_channel_b_side_b_times.empty() &&
        gen_instance.sample_requests_to_occupate_channel_b_side_b_times.size()>1) {
	sample_mean_requests_to_occupate_channel_b_side_b=calculate_sample_mean(gen_instance.sample_requests_to_occupate_channel_b_side_b_times);
	sample_deviation_requests_to_occupate_channel_b_side_b=calculate_standart_deviation(gen_instance.sample_requests_to_occupate_channel_b_side_b_times);
    max_request_to_occupate_channel_b_side_b=*(max_element(gen_instance.sample_requests_to_occupate_channel_b_side_b_times.begin(),
                           gen_instance.sample_requests_to_occupate_channel_b_side_b_times.end()
                           ));    
     min_request_to_occupate_channel_b_side_b=*(min_element(gen_instance.sample_requests_to_occupate_channel_b_side_b_times.begin(),
                           gen_instance.sample_requests_to_occupate_channel_b_side_b_times.end()
                           ));

    } else if (gen_instance.sample_requests_to_occupate_channel_b_side_b_times.size()==1){
          sample_mean_requests_to_occupate_channel_b_side_b=gen_instance.sample_requests_to_occupate_channel_b_side_b_times[0];
          sample_deviation_requests_to_occupate_channel_b_side_b=0;
          max_request_to_occupate_channel_b_side_b=gen_instance.sample_requests_to_occupate_channel_b_side_b_times[0];
          min_request_to_occupate_channel_b_side_b=gen_instance.sample_requests_to_occupate_channel_b_side_b_times[0];
    } else {
	sample_mean_requests_to_occupate_channel_b_side_b=0;
	sample_deviation_requests_to_occupate_channel_b_side_b=0;
    max_request_to_occupate_channel_b_side_b=0;
    min_request_to_occupate_channel_b_side_b=0;
    };

   // processing sample_request_to_free_channel_b_times
    if (!gen_instance.sample_request_to_free_channel_b_times.empty() &&
        gen_instance.sample_request_to_free_channel_b_times.size()>1) {
	sample_mean_request_to_free_channel_b=calculate_sample_mean(gen_instance.sample_request_to_free_channel_b_times);
	sample_deviation_request_to_free_channel_b=calculate_standart_deviation(gen_instance.sample_request_to_free_channel_b_times);
    max_request_to_free_channel_b=*(max_element(gen_instance.sample_request_to_free_channel_b_times.begin(),
                           gen_instance.sample_request_to_free_channel_b_times.end()
                           ));    
     min_request_to_free_channel_b=*(min_element(gen_instance.sample_request_to_free_channel_b_times.begin(),
                           gen_instance.sample_request_to_free_channel_b_times.end()
                           ));

    } else if (gen_instance.sample_request_to_free_channel_b_times.size()==1){
          sample_mean_request_to_free_channel_b=gen_instance.sample_request_to_free_channel_b_times[0];
          sample_deviation_request_to_free_channel_b=0;
          max_request_to_free_channel_b=gen_instance.sample_request_to_free_channel_b_times[0];
          min_request_to_free_channel_b=gen_instance.sample_request_to_free_channel_b_times[0];
    } else {
	sample_mean_request_to_free_channel_b=0;
	sample_deviation_request_to_free_channel_b=0;
    max_request_to_free_channel_b=0;
    min_request_to_free_channel_b=0;
    };

   // processing sample_error_to_occupation_channel_b_times;
    if (!gen_instance.sample_error_to_occupation_channel_b_times.empty() &&
        gen_instance.sample_error_to_occupation_channel_b_times.size()>1) {
	sample_mean_error_to_occupation_channel_b=calculate_sample_mean(gen_instance.sample_error_to_occupation_channel_b_times);
	sample_deviation_error_to_occupation_channel_b=calculate_standart_deviation(gen_instance.sample_error_to_occupation_channel_b_times);
    max_error_to_occupation_channel_b=*(max_element(gen_instance.sample_error_to_occupation_channel_b_times.begin(),
                           gen_instance.sample_error_to_occupation_channel_b_times.end()
                           ));    
     min_error_to_occupation_channel_b=*(min_element(gen_instance.sample_error_to_occupation_channel_b_times.begin(),
                           gen_instance.sample_error_to_occupation_channel_b_times.end()
                           ));

    } else if (gen_instance.sample_error_to_occupation_channel_b_times.size()==1){
          sample_mean_error_to_occupation_channel_b=gen_instance.sample_error_to_occupation_channel_b_times[0];
          sample_deviation_error_to_occupation_channel_b=0;
          max_error_to_occupation_channel_b=gen_instance.sample_error_to_occupation_channel_b_times[0];
          min_error_to_occupation_channel_b=gen_instance.sample_error_to_occupation_channel_b_times[0];
    } else {
	sample_mean_error_to_occupation_channel_b=0;
	sample_deviation_error_to_occupation_channel_b=0;
    max_error_to_occupation_channel_b=0;
    min_error_to_occupation_channel_b=0;
    };

   // processing sample_error_to_free_channel_b_times;
    if (!gen_instance.sample_error_to_free_channel_b_times.empty() &&
        gen_instance.sample_error_to_free_channel_b_times.size()>1) {
	sample_mean_error_to_free_channel_b=calculate_sample_mean(gen_instance.sample_error_to_free_channel_b_times);
	sample_deviation_error_to_free_channel_b=calculate_standart_deviation(gen_instance.sample_error_to_free_channel_b_times);
    max_error_to_free_channel_b=*(max_element(gen_instance.sample_error_to_free_channel_b_times.begin(),
                           gen_instance.sample_error_to_free_channel_b_times.end()
                           ));    
     min_error_to_free_channel_b=*(min_element(gen_instance.sample_error_to_free_channel_b_times.begin(),
                           gen_instance.sample_error_to_free_channel_b_times.end()
                           ));

    } else if (gen_instance.sample_error_to_free_channel_b_times.size()==1){
          sample_mean_error_to_free_channel_b=gen_instance.sample_error_to_free_channel_b_times[0];
          sample_deviation_error_to_free_channel_b=0;
          max_error_to_free_channel_b=gen_instance.sample_error_to_free_channel_b_times[0];
          min_error_to_free_channel_b=gen_instance.sample_error_to_free_channel_b_times[0];
    } else {
	sample_mean_error_to_free_channel_b=0;
	sample_deviation_error_to_free_channel_b=0;
    max_error_to_free_channel_b=0;
    min_error_to_free_channel_b=0;
    };

   // processing sample_requests_to_occupate_call_pointer_times;
    if (!gen_instance.sample_requests_to_occupate_call_pointer_times.empty() &&
        gen_instance.sample_requests_to_occupate_call_pointer_times.size()>1) {
	sample_mean_requests_to_occupate_call_pointer=calculate_sample_mean(gen_instance.sample_requests_to_occupate_call_pointer_times);
	sample_deviation_requests_to_occupate_call_pointer=calculate_standart_deviation(gen_instance.sample_requests_to_occupate_call_pointer_times);
    max_request_to_occupate_call_pointer=*(max_element(gen_instance.sample_requests_to_occupate_call_pointer_times.begin(),
                           gen_instance.sample_requests_to_occupate_call_pointer_times.end()
                           ));    
     min_request_to_occupate_call_pointer=*(min_element(gen_instance.sample_requests_to_occupate_call_pointer_times.begin(),
                           gen_instance.sample_requests_to_occupate_call_pointer_times.end()
                           ));

    } else if (gen_instance.sample_requests_to_occupate_call_pointer_times.size()==1){
          sample_mean_requests_to_occupate_call_pointer=gen_instance.sample_requests_to_occupate_call_pointer_times[0];
          sample_deviation_requests_to_occupate_call_pointer=0;
          max_request_to_occupate_call_pointer=gen_instance.sample_requests_to_occupate_call_pointer_times[0];
          min_request_to_occupate_call_pointer=gen_instance.sample_requests_to_occupate_call_pointer_times[0];
    } else {
	sample_mean_requests_to_occupate_call_pointer=0;
	sample_deviation_requests_to_occupate_call_pointer=0;
    max_request_to_occupate_call_pointer=0;
    min_request_to_occupate_call_pointer=0;
    };

   // processing sample_requests_to_free_call_pointer_times;
    if (!gen_instance.sample_requests_to_free_call_pointer_times.empty() &&
        gen_instance.sample_requests_to_free_call_pointer_times.size()>1) {
	sample_mean_requests_to_free_call_pointer=calculate_sample_mean(gen_instance.sample_requests_to_free_call_pointer_times);
	sample_deviation_requests_to_free_call_pointer=calculate_standart_deviation(gen_instance.sample_requests_to_free_call_pointer_times);
    max_request_to_free_call_pointer=*(max_element(gen_instance.sample_requests_to_free_call_pointer_times.begin(),
                           gen_instance.sample_requests_to_free_call_pointer_times.end()
                           ));    
     min_request_to_free_call_pointer=*(min_element(gen_instance.sample_requests_to_free_call_pointer_times.begin(),
                           gen_instance.sample_requests_to_free_call_pointer_times.end()
                           ));

    } else if (gen_instance.sample_requests_to_free_call_pointer_times.size()==1){
          sample_mean_requests_to_free_call_pointer=gen_instance.sample_requests_to_free_call_pointer_times[0];
          sample_deviation_requests_to_free_call_pointer=0;
          max_request_to_free_call_pointer=gen_instance.sample_requests_to_free_call_pointer_times[0];
          min_request_to_free_call_pointer=gen_instance.sample_requests_to_free_call_pointer_times[0];
    } else {
	sample_mean_requests_to_free_call_pointer=0;
	sample_deviation_requests_to_free_call_pointer=0;
    max_request_to_free_call_pointer=0;
    min_request_to_free_call_pointer=0;
    };

   // processing sample_error_to_occupate_call_pointer_times;
    if (!gen_instance.sample_error_to_occupate_call_pointer_times.empty() &&
        gen_instance.sample_error_to_occupate_call_pointer_times.size()>1) {
	sample_mean_error_to_occupate_call_pointer=calculate_sample_mean(gen_instance.sample_error_to_occupate_call_pointer_times);
	sample_deviation_error_to_occupate_call_pointer=calculate_standart_deviation(gen_instance.sample_error_to_occupate_call_pointer_times);
    max_error_to_occupate_call_pointer=*(max_element(gen_instance.sample_error_to_occupate_call_pointer_times.begin(),
                           gen_instance.sample_error_to_occupate_call_pointer_times.end()
                           ));    
     min_error_to_occupate_call_pointer=*(min_element(gen_instance.sample_error_to_occupate_call_pointer_times.begin(),
                           gen_instance.sample_error_to_occupate_call_pointer_times.end()
                           ));

    } else if (gen_instance.sample_error_to_occupate_call_pointer_times.size()==1){
          sample_mean_error_to_occupate_call_pointer=gen_instance.sample_error_to_occupate_call_pointer_times[0];
          sample_deviation_error_to_occupate_call_pointer=0;
          max_error_to_occupate_call_pointer=gen_instance.sample_error_to_occupate_call_pointer_times[0];
          min_error_to_occupate_call_pointer=gen_instance.sample_error_to_occupate_call_pointer_times[0];
    } else {
	sample_mean_error_to_occupate_call_pointer=0;
	sample_deviation_error_to_occupate_call_pointer=0;
    max_error_to_occupate_call_pointer=0;
    min_error_to_occupate_call_pointer=0;
    };

   // processing sample_error_to_free_call_pointer_times;
    if (!gen_instance.sample_error_to_free_call_pointer_times.empty() &&
        gen_instance.sample_error_to_free_call_pointer_times.size()>1) {
	sample_mean_error_to_free_call_pointer=calculate_sample_mean(gen_instance.sample_error_to_free_call_pointer_times);
	sample_deviation_error_to_free_call_pointer=calculate_standart_deviation(gen_instance.sample_error_to_free_call_pointer_times);
    max_error_to_free_call_pointer=*(max_element(gen_instance.sample_error_to_free_call_pointer_times.begin(),
                           gen_instance.sample_error_to_free_call_pointer_times.end()
                           ));    
     min_error_to_free_call_pointer=*(min_element(gen_instance.sample_error_to_free_call_pointer_times.begin(),
                           gen_instance.sample_error_to_free_call_pointer_times.end()
                           ));

    } else if (gen_instance.sample_error_to_free_call_pointer_times.size()==1){
          sample_mean_error_to_free_call_pointer=gen_instance.sample_error_to_free_call_pointer_times[0];
          sample_deviation_error_to_free_call_pointer=0;
          max_error_to_free_call_pointer=gen_instance.sample_error_to_free_call_pointer_times[0];
          min_error_to_free_call_pointer=gen_instance.sample_error_to_free_call_pointer_times[0];
    } else {
	sample_mean_error_to_free_call_pointer=0;
	sample_deviation_error_to_free_call_pointer=0;
    max_error_to_free_call_pointer=0;
    min_error_to_free_call_pointer=0;
    };

   // processing sample_request_of_channel_b_state_checking_times;
    if (!gen_instance.sample_request_of_channel_b_state_checking_times.empty() &&
        gen_instance.sample_request_of_channel_b_state_checking_times.size()>1) {
	sample_mean_request_of_channel_b_state_checking=calculate_sample_mean(gen_instance.sample_request_of_channel_b_state_checking_times);
	sample_deviation_request_of_channel_b_state_checking=calculate_standart_deviation(gen_instance.sample_request_of_channel_b_state_checking_times);
    max_request_of_channel_b_state_checking=*(max_element(gen_instance.sample_request_of_channel_b_state_checking_times.begin(),
                           gen_instance.sample_request_of_channel_b_state_checking_times.end()
                           ));    
     min_request_of_channel_b_state_checking=*(min_element(gen_instance.sample_request_of_channel_b_state_checking_times.begin(),
                           gen_instance.sample_request_of_channel_b_state_checking_times.end()
                           ));

    } else if (gen_instance.sample_request_of_channel_b_state_checking_times.size()==1){
          sample_mean_request_of_channel_b_state_checking=gen_instance.sample_request_of_channel_b_state_checking_times[0];
          sample_deviation_request_of_channel_b_state_checking=0;
          max_request_of_channel_b_state_checking=gen_instance.sample_request_of_channel_b_state_checking_times[0];
          min_request_of_channel_b_state_checking=gen_instance.sample_request_of_channel_b_state_checking_times[0];
    } else {
	sample_mean_request_of_channel_b_state_checking=0;
	sample_deviation_request_of_channel_b_state_checking=0;
    max_request_of_channel_b_state_checking=0;
    min_request_of_channel_b_state_checking=0;
    };

   // processing sample_error_of_channel_b_state_checking_times;
    if (!gen_instance.sample_error_of_channel_b_state_checking_times.empty() &&
        gen_instance.sample_error_of_channel_b_state_checking_times.size()>1) {
	sample_mean_error_of_channel_b_state_checking=calculate_sample_mean(gen_instance.sample_error_of_channel_b_state_checking_times);
	sample_deviation_error_of_channel_b_state_checking=calculate_standart_deviation(gen_instance.sample_error_of_channel_b_state_checking_times);
    max_error_of_channel_b_state_checking=*(max_element(gen_instance.sample_error_of_channel_b_state_checking_times.begin(),
                           gen_instance.sample_error_of_channel_b_state_checking_times.end()
                           ));    
     min_error_of_channel_b_state_checking=*(min_element(gen_instance.sample_error_of_channel_b_state_checking_times.begin(),
                           gen_instance.sample_error_of_channel_b_state_checking_times.end()
                           ));

    } else if (gen_instance.sample_error_of_channel_b_state_checking_times.size()==1){
          sample_mean_error_of_channel_b_state_checking=gen_instance.sample_error_of_channel_b_state_checking_times[0];
          sample_deviation_error_of_channel_b_state_checking=0;
          max_error_of_channel_b_state_checking=gen_instance.sample_error_of_channel_b_state_checking_times[0];
          min_error_of_channel_b_state_checking=gen_instance.sample_error_of_channel_b_state_checking_times[0];
    } else {
    sample_mean_error_of_channel_b_state_checking=0;
	   sample_deviation_error_of_channel_b_state_checking=0;
    max_error_of_channel_b_state_checking=0;
    min_error_of_channel_b_state_checking=0;
    };

    return_string.flags(ios_base::fixed|ios_base::dec);
	
	return_string<<"<center>Simulation statistic:</center><br>\n";
	return_string<<
	  "<table width=80% border=1 cellspacing=2 cellpadding=0 align=center>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nSuccess requests to occupate channel B by side A count</td>\n<td>"<<
	   gen_instance.sample_requests_to_occupate_channel_b_side_a_times.size()<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nSuccess requests to occupate channel B by side B count</td>\n<td>"<<
	   gen_instance.sample_requests_to_occupate_channel_b_side_b_times.size()<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nErrors to occupate channel B count</td>\n<td>"<<
        gen_instance.sample_error_to_occupation_channel_b_times.size()<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nSuccess requests to free channel B count</td>\n<td>"<<
	   gen_instance.sample_request_to_free_channel_b_times.size()<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nErrors to free  channel B count</td>\n<td>"<<
        gen_instance.sample_error_to_free_channel_b_times.size()<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nSuccess requests to occupate call pointers count</td>\n<td>"<<
        gen_instance.sample_requests_to_occupate_call_pointer_times.size()<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nSuccess requests to free call pointers count</td>\n<td>"<<
        gen_instance.sample_requests_to_free_call_pointer_times.size()<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nErrors to occupate call pointers count</td>\n<td>"<<
        gen_instance.sample_error_to_occupate_call_pointer_times.size()<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nErrors to free call pointers count</td>\n<td>"<<
        gen_instance.sample_error_to_free_call_pointer_times.size()<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nSuccess requests to channel b state checking count</td>\n<td>"<<
        gen_instance.sample_request_of_channel_b_state_checking_times.size()<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nErrors of channel b state checking count</td>\n<td>"<<
        gen_instance.sample_error_of_channel_b_state_checking_times.size()<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	  "<td width=80%>\nSimulation time<br>(mS)</td>\n<td>"<<
     simulation_time<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nRequests to occupate channel B by side A sample mean<br>(mS)</td>\n<td>"<<
	   (sample_mean_requests_to_occupate_channel_b_side_a*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nRequests to occupate channel B by side A sample deviation<br>(mS)</td>\n<td>"<<
	   (sample_deviation_requests_to_occupate_channel_b_side_a*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nMax request to occupate channel B by side A<br>(mS)</td>\n<td>"<<
	   (max_request_to_occupate_channel_b_side_a*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nMin request to occupate channel B by side A<br>(mS)</td>\n<td>"<<
	   (min_request_to_occupate_channel_b_side_a*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nRequests to occupate channel B by side B sample mean<br>(mS)</td>\n<td>"<<
	   (sample_mean_requests_to_occupate_channel_b_side_b*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nRequests to occupate channel B by side B sample deviation<br>(mS)</td>\n<td>"<<
	   (sample_deviation_requests_to_occupate_channel_b_side_b*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nMax request to occupate channel B by side B<br>(mS)</td>\n<td>"<<
	   (max_request_to_occupate_channel_b_side_b*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nMin request to occupate channel B by side B<br>(mS)</td>\n<td>"<<
	   (min_request_to_occupate_channel_b_side_b*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nRequests to free channel B sample mean<br>(mS)</td>\n<td>"<<
	   (sample_mean_request_to_free_channel_b*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nRequests to free channel B sample deviation<br>(mS)</td>\n<td>"<<
	   (sample_deviation_request_to_free_channel_b*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nMax request to free channel B<br>(mS)</td>\n<td>"<<
	   (max_request_to_free_channel_b*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nMin request to free channel B<br>(mS)</td>\n<td>"<<
	   (min_request_to_free_channel_b*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nError of channel B occupation sample mean<br>(mS)</td>\n<td>"<<
	   (	sample_mean_error_to_occupation_channel_b*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nError of channel B occupation sample deviation<br>(mS)</td>\n<td>"<<
	   (sample_deviation_error_to_occupation_channel_b*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nMax error of  channel B occupation<br>(mS)</td>\n<td>"<<
	   (max_error_to_occupation_channel_b*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nMin error of  channel B occupation<br>(mS)</td>\n<td>"<<
	   (min_error_to_occupation_channel_b*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nRequests to occupate call pointer sample mean<br>(mS)</td>\n<td>"<<
	   (sample_mean_requests_to_occupate_call_pointer*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nRequests to occupate call pointer sample deviation<br>(mS)</td>\n<td>"<<
	   (sample_deviation_requests_to_occupate_call_pointer*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nMax request to occupate call pointer<br>(mS)</td>\n<td>"<<
	   (max_request_to_occupate_call_pointer*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nMin request to occupate call pointer<br>(mS)</td>\n<td>"<<
	   (min_request_to_occupate_call_pointer*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nRequests to free call pointer sample mean<br>(mS)</td>\n<td>"<<
	   (sample_mean_requests_to_free_call_pointer*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nRequests to free call pointer sample deviation<br>(mS)</td>\n<td>"<<
	   (sample_deviation_requests_to_free_call_pointer*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nMax request to free call pointer<br>(mS)</td>\n<td>"<<
	   (max_request_to_free_call_pointer*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nMin request to free call pointer<br>(mS)</td>\n<td>"<<
	   (min_request_to_free_call_pointer*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nError of call pointer occupation sample mean<br>(mS)</td>\n<td>"<<
	   (sample_mean_error_to_occupate_call_pointer*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nError of call pointer occupation sample deviation<br>(mS)</td>\n<td>"<<
	   (sample_deviation_error_to_occupate_call_pointer*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nMax error of call pointer occupation<br>(mS)</td>\n<td>"<<
	   (max_error_to_occupate_call_pointer*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nMin error of call pointer occupation<br>(mS)</td>\n<td>"<<
	   (min_error_to_occupate_call_pointer*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nError of call pointer free sample mean<br>(mS)</td>\n<td>"<<
	   (sample_mean_error_to_free_call_pointer*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nError of call pointer free sample deviation<br>(mS)</td>\n<td>"<<
	   (sample_deviation_error_to_free_call_pointer*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nMax error of call pointer free<br>(mS)</td>\n<td>"<<
	   (max_error_to_free_call_pointer*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nMin error of call pointer free<br>(mS)</td>\n<td>"<<
	   (min_error_to_free_call_pointer*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nRequest of channel b state checking sample mean<br>(mS)</td>\n<td>"<<
	   (sample_mean_request_of_channel_b_state_checking*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nRequest of channel b state checking sample deviation<br>(mS)</td>\n<td>"<<
	   (sample_deviation_request_of_channel_b_state_checking*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nMax request of channel b state checking<br>(mS)</td>\n<td>"<<
	   (max_request_of_channel_b_state_checking*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nMin request of channel b state checking<br>(mS)</td>\n<td>"<<
	   (min_request_of_channel_b_state_checking*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nError of channel b state checking sample mean<br>(mS)</td>\n<td>"<<
	   (sample_mean_error_of_channel_b_state_checking*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nError of channel b state checking sample deviation<br>(mS)</td>\n<td>"<<
	   (sample_deviation_error_of_channel_b_state_checking*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nMax error of channel b state checking<br>(mS)</td>\n<td>"<<
	   (max_error_of_channel_b_state_checking*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nMin error of channel b state checking<br>(mS)</td>\n<td>"<<
	   (min_error_of_channel_b_state_checking*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

 return_string<<"</table>\n\n";

	cout<<return_string.str()<<endl;
};

void mock_generator::run(int requests_count)
         throw (runtime_error){
int counter;
bytes message_buffer;
bytes::iterator msg_buffer_iter1, msg_buffer_iter2;
int recv_id , failures_count;
word message_destination_uid;
uint64_t request_begin_cycles;
uint64_t request_end_cycles;
struct _msg_info recv_msg_info;


message_buffer.reserve(program_settings::RECEIVE_BUFFER_SIZE);
mock_generator& gen_instance=mock_generator::get_instance();

A0_automata& A0=A0_automata::get_instance();
A1_automata& A1=A1_automata::get_instance();

A0.get_state(); //only for compiler happening
A1.get_state(); //only for compiler happening

gen_instance.sample_requests_to_occupate_channel_b_side_a_times.clear();
gen_instance.sample_requests_to_occupate_channel_b_side_a_times.reserve(requests_count);

gen_instance.sample_requests_to_occupate_channel_b_side_b_times.clear();
gen_instance.sample_requests_to_occupate_channel_b_side_b_times.reserve(requests_count);

gen_instance.sample_request_to_free_channel_b_times.clear();
gen_instance.sample_request_to_free_channel_b_times.reserve(requests_count);

gen_instance.sample_error_to_occupation_channel_b_times.clear();
gen_instance.sample_error_to_occupation_channel_b_times.reserve(requests_count);

gen_instance.sample_error_to_free_channel_b_times.clear();
gen_instance.sample_error_to_free_channel_b_times.reserve(requests_count);

gen_instance.sample_requests_to_occupate_call_pointer_times.clear();
gen_instance.sample_requests_to_occupate_call_pointer_times.reserve(requests_count);

gen_instance.sample_requests_to_free_call_pointer_times.clear();
gen_instance.sample_requests_to_free_call_pointer_times.reserve(requests_count);

gen_instance.sample_error_to_occupate_call_pointer_times.clear();
gen_instance.sample_error_to_occupate_call_pointer_times.reserve(requests_count);

gen_instance.sample_error_to_free_call_pointer_times.clear();
gen_instance.sample_error_to_free_call_pointer_times.reserve(requests_count);

gen_instance.sample_request_of_channel_b_state_checking_times.clear();
gen_instance.sample_request_of_channel_b_state_checking_times.reserve(requests_count);

gen_instance.sample_error_of_channel_b_state_checking_times.clear();
gen_instance.sample_error_of_channel_b_state_checking_times.reserve(requests_count);

//reply dirven request from client
message_buffer.resize(program_settings::RECEIVE_BUFFER_SIZE);
recv_id=MsgReceive(channel_id,
                                    &message_buffer[0],
                                    message_buffer.size(),
                                    &recv_msg_info);

if (recv_id==-1) {
       string message ("mock_generator::run MsgReceive to empty buffer error : ");
       message+=strerror(errno);

      generator_logger& logger_inst=
                    generator_logger::get_instance();

      logger_inst.log_message
                      (generator_logger::ERROR_MSG,
                        message);
};

recv_id=MsgReply(recv_id,
                   EOK,
                    &message_buffer[0],
                    message_buffer.size());

if (recv_id==-1) {
       string message ("mock_generator::run MsgReply_that_channeleceive to empty buffer error : ");
       message+=strerror(errno);

      generator_logger& logger_inst=
                    generator_logger::get_instance();

      logger_inst.log_message
                      (generator_logger::ERROR_MSG,
                        message);
};

gen_instance.request_cycle_begin=0;
failures_count=0;
counter=1;
request_begin_cycles=ClockCycles();
while( counter<=requests_count &&
          failures_count<program_settings::MAX_FAILURES_COUNT) {

message_buffer.resize(program_settings::RECEIVE_BUFFER_SIZE);
recv_id=MsgReceive(channel_id,
                               &message_buffer[0],
                               message_buffer.size(),
                               &recv_msg_info);

request_end_cycles=ClockCycles();

if (gen_instance.request_cycle_begin==0)
                 gen_instance.request_cycle_begin=request_end_cycles;

if (recv_id==-1) {
       string message ("mock_generator::run MsgReceive error : ");
       message+=strerror(errno);

      generator_logger& logger_inst=
                    generator_logger::get_instance();

      logger_inst.log_message
                      (generator_logger::ERROR_MSG,
                        message);
    failures_count++;
     continue;
};

message_buffer.resize(recv_msg_info.msglen);

if (message_buffer.empty()||
     message_buffer.size()<program_settings::HEADER_SIZE_IN_BYTES) {
       ostringstream message;
       message<<"mock_generator::run too small message_buffer returned  message size "
                     <<message_buffer.size();
    if (!message_buffer.empty())
     for(vector<byte>::size_type i=0; i<message_buffer.size() && i<5; i++)
           message<<" ["<<i<<"]="<<static_cast<int>(message_buffer[i]);

      generator_logger& logger_inst=
                    generator_logger::get_instance();

      logger_inst.log_message
                      (generator_logger::ERROR_MSG,
                        message.str());
    failures_count++;
     continue;
};

failures_count=0;

//save message destination uid
msg_buffer_iter1=message_buffer.begin();
advance(msg_buffer_iter1, message_header_bytes::INDEX_OF_MESSAGE_DESTINATION_UID);
msg_buffer_iter2=msg_buffer_iter1;
advance(msg_buffer_iter2, message_header_bytes::INDEX_OF_MESSAGE_DESTINATION_UID+2);
message_destination_uid=type_from_bytes<word>(bytes(msg_buffer_iter1, msg_buffer_iter2));

if (message_buffer.size()==program_settings::HEADER_SIZE_IN_BYTES) {
     message_buffer.clear();
 } else {
     msg_buffer_iter1=message_buffer.begin();
     advance(msg_buffer_iter1, program_settings::HEADER_SIZE_IN_BYTES);
     message_buffer.erase(message_buffer.begin(), msg_buffer_iter1);
 };

if (message_buffer.empty()) { //if received only header, it`s initial request or error -  generate reqest to occupate channel
                 A0(A0_automata::EVENT_DATA_CHANGED);
                 message_buffer=A0_automata::get_command_to_storage();

                 //before data insert message header
                 message_buffer.insert(message_buffer.begin(), 1, 0); //reserved
                 message_buffer.insert(message_buffer.begin(), 1, 0); //message class

                 message_buffer.insert(message_buffer.begin(), 1, 0); //source tid high byte
                 message_buffer.insert(message_buffer.begin(), 1, 1); //source tid low byte

                 message_buffer.insert(message_buffer.begin(), 1, 0); //source uid high byte
                 message_buffer.insert(message_buffer.begin(), 1, message_header_uid::UID_CALL_CONTROL); //source uid low byte

                 message_buffer.insert(message_buffer.begin(), 1, 0); //destination tid high byte
                 message_buffer.insert(message_buffer.begin(), 1, 1); //destination tid low byte

                 message_buffer.insert(message_buffer.begin(), 1, 0); //destination uid high byte
                 message_buffer.insert(message_buffer.begin(), 1, message_header_uid::UID_DEPOSITORY); //destination uid low byte

                 message_buffer.insert(message_buffer.begin(), 1, message_header_description_flag::WAIT_MSG_FLAG | //message description
                                                  message_header_description_flag::SRC_TID_STATIC);
                 message_buffer.insert(message_buffer.begin(), 1, message_header_type::DATA_QUERY); //message type
 } else { //if (message_buffer.empty())
     request_end_cycles-=request_begin_cycles;
     switch (message_destination_uid) {
          case message_header_uid::UID_CALL_CONTROL:
              switch (message_buffer[message_content_bytes::INDEX_OF_MESSAGE_CODE]){
                 case call_control_commands::ANSWER_TO_OCCUPATE_CHANNEL_B_SIDE_A:
                      if (message_buffer.size()!=call_control_commands_sizes::ANSWER_TO_OCCUPATE_CHANNEL_B_SIDE_A)
                          throw runtime_error("message_buffer.size()!=call_control_commands_sizes::ANSWER_TO_OCCUPATE_CHANNEL_B_SIDE_A");

                                A0.x1=true;
                                //store time of occupation
                                gen_instance.sample_requests_to_occupate_channel_b_side_a_times.push_back(request_end_cycles);

                                current_ch_b_side_a.upo=message_buffer[1];
                                current_ch_b_side_a.ie1=message_buffer[2];
                                current_ch_b_side_a.e1=message_buffer[3];
                                current_ch_b_side_a.ki=message_buffer[4];

                      break; //case call_control_commands::ANSWER_TO_OCCUPATE_CHANNEL_B_SIDE_A
                 case call_control_commands::ANSWER_TO_OCCUPATE_CHANNEL_B_SIDE_B:
                      if (message_buffer.size()!=call_control_commands_sizes::ANSWER_TO_OCCUPATE_CHANNEL_B_SIDE_B)
                          throw runtime_error("message_buffer.size()!=call_control_commands_sizes::ANSWER_TO_OCCUPATE_CHANNEL_B_SIDE_B");

                                A0.x2=true;
                                //store time of occupation
                                gen_instance.sample_requests_to_occupate_channel_b_side_b_times.push_back(request_end_cycles);

                                current_ch_b_side_b.upo=message_buffer[1];
                                current_ch_b_side_b.ie1=message_buffer[2];
                                current_ch_b_side_b.e1=message_buffer[3];
                                current_ch_b_side_b.ki=message_buffer[4];

                       break; //case call_control_commands::ANSWER_TO_OCCUPATE_CHANNEL_B_SIDE_B
                     case call_control_commands::ANSWER_TO_FREE_CHANNEL_B:
                      if (message_buffer.size()!=call_control_commands_sizes::ANSWER_TO_FREE_CHANNEL_B)
                          throw runtime_error("message_buffer.size()!=call_control_commands_sizes::ANSWER_TO_FREE_CHANNEL_B");
                       //store time of free
                       gen_instance.sample_request_to_free_channel_b_times.push_back(request_end_cycles);
                        if (A0.x2){
                               A0.x2=false;
                            } else  if (A0.x1){
                               A0.x1=false;
                            };
                       break;//case call_control_commands::ANSWER_TO_FREE_CHANNEL_B:
                     case call_control_commands::ERROR_IN_CHANNEL_B_OCCUPATION_SIDE_A:
                       if (message_buffer.size()!=call_control_commands_sizes::ERROR_IN_CHANNEL_B_OCCUPATION_SIDE_A)
                            throw runtime_error("message_buffer.size()!=call_control_commands_sizes::ERROR_IN_CHANNEL_B_OCCUPATION_SIDE_A");
                      //store time of error in occupation
                         gen_instance.sample_error_to_occupation_channel_b_times.push_back(request_end_cycles);
                       break;//case call_control_commands::ERROR_IN_OCCUPATION_SIDE_A:
                     case call_control_commands::ERROR_IN_CHANNEL_B_OCCUPATION_SIDE_B:
                       if (message_buffer.size()!=call_control_commands_sizes::ERROR_IN_CHANNEL_B_OCCUPATION_SIDE_B)
                            throw runtime_error("message_buffer.size()!=call_control_commands_sizes::ERROR_IN_CHANNEL_B_OCCUPATION_SIDE_B");
                      //store time of error in occupation
                         gen_instance.sample_error_to_occupation_channel_b_times.push_back(request_end_cycles);
                       break;//case call_control_commands::ERROR_IN_OCCUPATION_SIDE_B:
                   case call_control_commands::ERROR_IN_CHANNEL_B_FREE:
                     if (message_buffer.size()!=call_control_commands_sizes::ERROR_IN_CHANNEL_B_FREE)
                           throw runtime_error("message_buffer.size()!=call_control_commands_sizes::ERROR_IN_CHANNEL_B_FREE");
                     //store time of error in free
                      gen_instance.sample_error_to_free_channel_b_times.push_back(request_end_cycles);
                      break;//case call_control_commands::ERROR_IN_CHANNEL_B_FREE

                   case call_control_commands::ANSWER_TO_OCCUPATE_CALL_POINTER:
                     if (message_buffer.size()!=call_control_commands_sizes::ANSWER_TO_OCCUPATE_CALL_POINTER)
                           throw runtime_error("message_buffer.size()!=call_control_commands_sizes::ANSWER_TO_OCCUPATE_CALL_POINTER");
                     //store time of occupation call pointer
                      gen_instance.sample_requests_to_occupate_call_pointer_times.push_back(request_end_cycles);

                      msg_buffer_iter1=message_buffer.begin();
                      msg_buffer_iter2=message_buffer.end();
                      advance(msg_buffer_iter1, 1);
                      current_call_pointer.call_pointer_id=type_from_bytes<word>(bytes(msg_buffer_iter1, msg_buffer_iter2));

                      A0.x3=true;
                      break;//case call_control_commands::ANSWER_TO_OCCUPATE_CALL_POINTER
                   case call_control_commands::ANSWER_TO_FREE_CALL_POINTER:
                     if (message_buffer.size()!=call_control_commands_sizes::ANSWER_TO_FREE_CALL_POINTER)
                           throw runtime_error("message_buffer.size()!=call_control_commands_sizes::ANSWER_TO_FREE_CALL_POINTER");
                     //store time of free call pointer
                      gen_instance.sample_requests_to_free_call_pointer_times.push_back(request_end_cycles);
                       A0.x3=false;
                      break;//case call_control_commands::ANSWER_TO_FREE_CALL_POINTER
                   case call_control_commands::ERROR_IN_CALL_POINTER_OCCUPATE:
                     if (message_buffer.size()!=call_control_commands_sizes::ERROR_IN_CALL_POINTER_OCCUPATE)
                           throw runtime_error("message_buffer.size()!=call_control_commands_sizes::ERROR_IN_CALL_POINTER_OCCUPATE");
                     //store time of error in occupation
                      gen_instance.sample_error_to_occupate_call_pointer_times.push_back(request_end_cycles);
                      break;//case call_control_commands::ERROR_IN_CALL_POINTER_OCCUPATE
                   case call_control_commands::ERROR_IN_CALL_POINTER_FREE:
                     if (message_buffer.size()!=call_control_commands_sizes::ERROR_IN_CALL_POINTER_FREE)
                           throw runtime_error("message_buffer.size()!=call_control_commands_sizes::ERROR_IN_CALL_POINTER_FREE");
                     //store time of error in free
                      gen_instance.sample_error_to_free_call_pointer_times.push_back(request_end_cycles);
                      break;//case call_control_commands::ERROR_IN_CALL_POINTER_FREE
                      default:
                        vector<char> tmp_chars(32);
                         string exception_description;

                         exception_description+=" ten or less bytes of unknown message ";
                        for (bytes::size_type i=0; (i<message_buffer.size() && i<10); i++) {
                             itoa(message_buffer[i], &tmp_chars[0], 16);
                             exception_description+=" 0x";
                             exception_description+=&tmp_chars[0];
                            };

                         itoa(counter, &tmp_chars[0], 10);
                         exception_description+=" count ";
                         exception_description+=&tmp_chars[0];

                        throw runtime_error(exception_description);
                   }; //switch (message_buffer[message_header_bytes::INDEX_OF_MESSAGE_CODE])

                 A0(A0_automata::EVENT_DATA_CHANGED);
                 //change message source or prepare new request from that type
                 if (A0.get_state()==A0_automata::CALL_CONTINUE && 
                     (counter%3)==0 ) {
                             A1(A1_automata::EVENT_DATA_CHANGED);
                             message_buffer=A1_automata::get_command_to_storage();
                               //before data insert message header
                              message_buffer.insert(message_buffer.begin(), 1, 0); //reserved
                              message_buffer.insert(message_buffer.begin(), 1, 0); //message class

                              message_buffer.insert(message_buffer.begin(), 1, 0); //source tid high byte
                              message_buffer.insert(message_buffer.begin(), 1, 1); //source tid low byte

                              message_buffer.insert(message_buffer.begin(), 1, 0); //source uid high byte
                              message_buffer.insert(message_buffer.begin(), 1, message_header_uid::UID_PROC_CONTROL); //source uid low byte

                              message_buffer.insert(message_buffer.begin(), 1, 0); //destination tid high byte
                              message_buffer.insert(message_buffer.begin(), 1, 1); //destination tid low byte

                              message_buffer.insert(message_buffer.begin(), 1, 0); //destination uid high byte
                              message_buffer.insert(message_buffer.begin(), 1, message_header_uid::UID_DEPOSITORY); //destination uid low byte

                              message_buffer.insert(message_buffer.begin(), 1, message_header_description_flag::WAIT_MSG_FLAG | //message description
                                                                                                                 message_header_description_flag::SRC_TID_STATIC);
                              message_buffer.insert(message_buffer.begin(), 1, message_header_type::DATA_QUERY); //message type
                         } else if (A0.get_state()==A0_automata::CALL_CONTINUE) {
                              if ((counter%2)==0) {
                                    A0(A0_automata::EVENT_CALL_END_SIDE_B);
                                 }else {
                                    A0(A0_automata::EVENT_CALL_END_SIDE_A);
                                 };
                              message_buffer=A0_automata::get_command_to_storage();
                               //before data insert message header
                              message_buffer.insert(message_buffer.begin(), 1, 0); //reserved
                              message_buffer.insert(message_buffer.begin(), 1, 0); //message class

                              message_buffer.insert(message_buffer.begin(), 1, 0); //source tid high byte
                              message_buffer.insert(message_buffer.begin(), 1, 1); //source tid low byte

                              message_buffer.insert(message_buffer.begin(), 1, 0); //source uid high byte
                              message_buffer.insert(message_buffer.begin(), 1, message_header_uid::UID_CALL_CONTROL); //source uid low byte

                              message_buffer.insert(message_buffer.begin(), 1, 0); //destination tid high byte
                              message_buffer.insert(message_buffer.begin(), 1, 1); //destination tid low byte

                              message_buffer.insert(message_buffer.begin(), 1, 0); //destination uid high byte
                              message_buffer.insert(message_buffer.begin(), 1, message_header_uid::UID_DEPOSITORY); //destination uid low byte

                              message_buffer.insert(message_buffer.begin(), 1, message_header_description_flag::WAIT_MSG_FLAG | //message description
                                                                                                                 message_header_description_flag::SRC_TID_STATIC);
                              message_buffer.insert(message_buffer.begin(), 1, message_header_type::DATA_QUERY); //message type
                         } else {
                               message_buffer=A0_automata::get_command_to_storage();
                               //before data insert message header
                              message_buffer.insert(message_buffer.begin(), 1, 0); //reserved
                              message_buffer.insert(message_buffer.begin(), 1, 0); //message class

                              message_buffer.insert(message_buffer.begin(), 1, 0); //source tid high byte
                              message_buffer.insert(message_buffer.begin(), 1, 1); //source tid low byte

                              message_buffer.insert(message_buffer.begin(), 1, 0); //source uid high byte
                              message_buffer.insert(message_buffer.begin(), 1, message_header_uid::UID_CALL_CONTROL); //source uid low byte

                              message_buffer.insert(message_buffer.begin(), 1, 0); //destination tid high byte
                              message_buffer.insert(message_buffer.begin(), 1, 1); //destination tid low byte

                              message_buffer.insert(message_buffer.begin(), 1, 0); //destination uid high byte
                              message_buffer.insert(message_buffer.begin(), 1, message_header_uid::UID_DEPOSITORY); //destination uid low byte

                              message_buffer.insert(message_buffer.begin(), 1, message_header_description_flag::WAIT_MSG_FLAG | //message description
                                                                                                                 message_header_description_flag::SRC_TID_STATIC);
                              message_buffer.insert(message_buffer.begin(), 1, message_header_type::DATA_QUERY); //message type
                         };
             break; //case message_header_uid::UID_CALL_CONTROL
          case message_header_uid::UID_PROC_CONTROL:
                switch (message_buffer[message_content_bytes::INDEX_OF_MESSAGE_CODE]){
                     case proc_control_commands::ANSWER_STATE_OF_CHANNEL_B:
                     if (message_buffer.size()!=proc_control_commands_sizes::ANSWER_STATE_OF_CHANNEL_B)
                           throw runtime_error("message_buffer.size()!=proc_control_commands_sizes::ANSWER_STATE_OF_CHANNEL_B");
                         //store time of error in occupation
                         gen_instance.sample_request_of_channel_b_state_checking_times.push_back(request_end_cycles);
                      break; //case proc_control_commands::ANSWER_STATE_OF_CHANNEL_B
                     case proc_control_commands::ERROR_IN_STATE_OF_CHANNEL_B:
                     if (message_buffer.size()!=proc_control_commands_sizes::ERROR_IN_STATE_OF_CHANNEL_B)
                           throw runtime_error("message_buffer.size()!=proc_control_commands_sizes::ERROR_IN_STATE_OF_CHANNEL_B");
                         //store time of error in occupation
                         gen_instance.sample_error_of_channel_b_state_checking_times.push_back(request_end_cycles);
                      break; //case proc_control_commands::ERROR_IN_STATE_OF_CHANNEL_B
                      default:
                        vector<char> tmp_chars(32);
                         string exception_description;
          
                         exception_description+=" ten or less bytes of unknown message ";
                         for (bytes::size_type i=0; (i<message_buffer.size() && i<10); i++) {
                             itoa(message_buffer[i], &tmp_chars[0], 16);
                             exception_description+=" 0x";
                             exception_description+=&tmp_chars[0];
                            };

                          itoa(counter, &tmp_chars[0], 10);
                          exception_description+=" count ";
                          exception_description+=&tmp_chars[0];

                         throw runtime_error(exception_description);
                 }; //switch (message_buffer[message_header_bytes::INDEX_OF_MESSAGE_CODE])

                 //change message source or prepare new request from that type
                 if ((counter%2)==0) {
                             A1(A1_automata::EVENT_DATA_CHANGED);
                             message_buffer=A1_automata::get_command_to_storage();
                               //before data insert message header
                              message_buffer.insert(message_buffer.begin(), 1, 0); //reserved
                              message_buffer.insert(message_buffer.begin(), 1, 0); //message class

                              message_buffer.insert(message_buffer.begin(), 1, 0); //source tid high byte
                              message_buffer.insert(message_buffer.begin(), 1, 1); //source tid low byte

                              message_buffer.insert(message_buffer.begin(), 1, 0); //source uid high byte
                              message_buffer.insert(message_buffer.begin(), 1, message_header_uid::UID_PROC_CONTROL); //source uid low byte

                              message_buffer.insert(message_buffer.begin(), 1, 0); //destination tid high byte
                              message_buffer.insert(message_buffer.begin(), 1, 1); //destination tid low byte

                              message_buffer.insert(message_buffer.begin(), 1, 0); //destination uid high byte
                              message_buffer.insert(message_buffer.begin(), 1, message_header_uid::UID_DEPOSITORY); //destination uid low byte

                              message_buffer.insert(message_buffer.begin(), 1, message_header_description_flag::WAIT_MSG_FLAG | //message description
                                                                                                                 message_header_description_flag::SRC_TID_STATIC);
                              message_buffer.insert(message_buffer.begin(), 1, message_header_type::DATA_QUERY); //message type
                         } else if (A0.get_state()==A0_automata::CALL_CONTINUE) {
                              if ((counter%2)==0) {
                                    A0(A0_automata::EVENT_CALL_END_SIDE_B);
                                 }else {
                                    A0(A0_automata::EVENT_CALL_END_SIDE_A);
                                 };
                              message_buffer=A0_automata::get_command_to_storage();
                               //before data insert message header
                              message_buffer.insert(message_buffer.begin(), 1, 0); //reserved
                              message_buffer.insert(message_buffer.begin(), 1, 0); //message class

                              message_buffer.insert(message_buffer.begin(), 1, 0); //source tid high byte
                              message_buffer.insert(message_buffer.begin(), 1, 1); //source tid low byte

                              message_buffer.insert(message_buffer.begin(), 1, 0); //source uid high byte
                              message_buffer.insert(message_buffer.begin(), 1, message_header_uid::UID_CALL_CONTROL); //source uid low byte

                              message_buffer.insert(message_buffer.begin(), 1, 0); //destination tid high byte
                              message_buffer.insert(message_buffer.begin(), 1, 1); //destination tid low byte

                              message_buffer.insert(message_buffer.begin(), 1, 0); //destination uid high byte
                              message_buffer.insert(message_buffer.begin(), 1, message_header_uid::UID_DEPOSITORY); //destination uid low byte

                              message_buffer.insert(message_buffer.begin(), 1, message_header_description_flag::WAIT_MSG_FLAG | //message description
                                                                                                                 message_header_description_flag::SRC_TID_STATIC);
                              message_buffer.insert(message_buffer.begin(), 1, message_header_type::DATA_QUERY); //message type
                         } else {
                              A0(A0_automata::EVENT_DATA_CHANGED);
                              message_buffer=A0_automata::get_command_to_storage();
                               //before data insert message header
                              message_buffer.insert(message_buffer.begin(), 1, 0); //reserved
                              message_buffer.insert(message_buffer.begin(), 1, 0); //message class

                              message_buffer.insert(message_buffer.begin(), 1, 0); //source tid high byte
                              message_buffer.insert(message_buffer.begin(), 1, 1); //source tid low byte

                              message_buffer.insert(message_buffer.begin(), 1, 0); //source uid high byte
                              message_buffer.insert(message_buffer.begin(), 1, message_header_uid::UID_CALL_CONTROL); //source uid low byte

                              message_buffer.insert(message_buffer.begin(), 1, 0); //destination tid high byte
                              message_buffer.insert(message_buffer.begin(), 1, 1); //destination tid low byte

                              message_buffer.insert(message_buffer.begin(), 1, 0); //destination uid high byte
                              message_buffer.insert(message_buffer.begin(), 1, message_header_uid::UID_DEPOSITORY); //destination uid low byte

                              message_buffer.insert(message_buffer.begin(), 1, message_header_description_flag::WAIT_MSG_FLAG | //message description
                                                                                                                 message_header_description_flag::SRC_TID_STATIC);
                              message_buffer.insert(message_buffer.begin(), 1, message_header_type::DATA_QUERY); //message type

                         };

             break; //case message_header_uid::UID_PROC_CONTROL
          default :
              string excepiton_description ("Uknown destination uid in message replay 0x");
              vector<char> tmp_chars(32);
              itoa(message_buffer[0], &tmp_chars[0], 16);
              throw runtime_error(excepiton_description+&tmp_chars[0]);
      };//switch (message_buffer[message_header_bytes::INDEX_OF_MESSAGE_DESTINATION_UID])
 }; //if (message_buffer.empty())

request_begin_cycles=ClockCycles();
recv_id=MsgReply(recv_id,
//                   EOK,
                    message_buffer.size(),
                    &message_buffer[0],
                    message_buffer.size());

if (recv_id==-1) {
       string message ("mock_generator::run MsgReply error : ");
       message+=strerror(errno);

      generator_logger& logger_inst=
                    generator_logger::get_instance();

      logger_inst.log_message
                      (generator_logger::ERROR_MSG,
                        message);
       failures_count++;
        continue;
  } else {  //if (recv_id==-1)
     failures_count=0;
  };

counter++;
}; //while(counter<requests_count)

gen_instance.request_cycle_end=ClockCycles();
}; 

void mock_generator::A0_automata::operator()(int event) throw (runtime_error){
    int old_state=state;

   switch (state) {
       case OCCUPATE_CH_B_SIDE_A:
            z1();
            if (x1) state=OCCUPATE_CH_B_SIDE_B;
           break;
       case OCCUPATE_CH_B_SIDE_B:
            if (x2) state=OCCUPATE_CALL_POINTER;
            if (!x2) state=FREE_CH_B_SIDE_A;
           break;
       case OCCUPATE_CALL_POINTER:
            if (x3) state=CALL_CONTINUE;
            if (!x3) state=FREE_CH_B_SIDE_A;
           break;
       case CALL_CONTINUE:
            if (event==EVENT_CALL_END_SIDE_A) state=FREE_CH_B_SIDE_A;
            if (event==EVENT_CALL_END_SIDE_B) state=FREE_CH_B_SIDE_B;
           break;
       case FREE_CH_B_SIDE_A:
            if (!x1 && !x2 && !x3) state=OCCUPATE_CH_B_SIDE_A;
            if (!x1 && x2) state=FREE_CH_B_SIDE_B;
            if (!x1 && !x2 && x3) state=FREE_CALL_POINTER;
           break;
       case FREE_CH_B_SIDE_B:
            if (!x2 && x1) state=FREE_CH_B_SIDE_A;
            if (!x1 && !x2 && x3) state=FREE_CALL_POINTER;
            if (!x1 && !x2 && !x3) state=OCCUPATE_CH_B_SIDE_A;
           break;
       case FREE_CALL_POINTER:
            if (event==EVENT_DATA_CHANGED) state=OCCUPATE_CH_B_SIDE_A;
           break;
        default:
            ostringstream err_descr;
            err_descr<<"mock_generator::A0_automata::operator() unknown state "<<state;
            throw runtime_error(err_descr.str());
    }; //switch state

   if (old_state==state) return;
//=========delete it===========
/*
{
                        ostringstream msg;
                        msg<<"Changed automata A0 state from "<<old_state<<" to "<<state;
                        generator_logger& logger_inst=
                                generator_logger::get_instance();
                        logger_inst.log_message
                                (generator_logger::INFO_MSG,
                                 msg.str());
} */
//=========================
    switch (state) { //new state
       case OCCUPATE_CH_B_SIDE_A:
             z1();
           break;
       case OCCUPATE_CH_B_SIDE_B:
             z2();
           break;
       case OCCUPATE_CALL_POINTER:
             z3();
           break;
       case CALL_CONTINUE:
           break;
       case FREE_CH_B_SIDE_A:
             z4();
           break;
       case FREE_CH_B_SIDE_B:
             z5();
           break;
       case FREE_CALL_POINTER:
             z6();
           break;
        default:
            ostringstream err_descr;
            err_descr<<"mock_generator::A0_automata::operator() unknown new state "<<state;
            throw runtime_error(err_descr.str());
    }; //switch state

};

void mock_generator::A0_automata::z1(){
 byte upo, ie1, e1, ki, pe;
 bytes tmp_buffer;
 word channel_b_number;

 //generation data of request
 upo=random();
 if (upo>=upo_count) upo=upo%upo_count;
 ie1=random();
 if (ie1>=ie1_count) ie1=ie1%ie1_count;
 e1=random();
 if (e1>=e1_count) e1=e1%e1_count;
  do {
       ki=random(); 
      if (ki>=channelb_count) ki=ki%channelb_count;
 } while ( ki == 0 || ki == 16 );
channel_b_number=1; //always 1 now
// channel_b_number=random();
// if (channel_b_number>= max_channel_number) channel_b_number=channel_b_number%max_channel_number;
 pe=random();
 pe=pe%2;

 command_to_storage.clear();
 command_to_storage.push_back(call_control_commands::REQUEST_TO_OCCUPATE_CHANNEL_B_SIDE_A);
 command_to_storage.push_back(upo);
 command_to_storage.push_back(ie1);
 command_to_storage.push_back(e1);
 command_to_storage.push_back(ki);
 tmp_buffer=bytes_of_type<word> (channel_b_number);
 command_to_storage.push_back(tmp_buffer[0]);
 command_to_storage.push_back(tmp_buffer[1]);
 command_to_storage.push_back(pe);
};

void mock_generator::A0_automata::z2(){
 int destination;
 byte mg, pr,isdn;
 bytes tmp_buffer;

 //generation data of request
 destination=random();
if (destination>max_destination) destination=destination%max_destination;
if (destination==0) destination=1;
 mg=random();
 mg=mg%2;
 pr=random();
 isdn=random();


 command_to_storage.clear();
 command_to_storage.push_back(call_control_commands::REQUEST_TO_OCCUPATE_CHANNEL_B_SIDE_B);
 tmp_buffer=bytes_of_type<int> (destination);
 command_to_storage.push_back(tmp_buffer[0]);
 command_to_storage.push_back(tmp_buffer[1]);
 command_to_storage.push_back(tmp_buffer[2]);
 command_to_storage.push_back(tmp_buffer[3]);

 command_to_storage.push_back(mg);
 command_to_storage.push_back(pr);
 command_to_storage.push_back(isdn);
 command_to_storage.push_back(1); //count of channels  - always 1
};

void mock_generator::A0_automata::z3(){
 byte upo, ie1, e1, ki;
 bytes tmp_buffer;

 //generation data of request
 upo=random();
 if (upo>=upo_count) upo=upo%upo_count;
 ie1=random();
 if (ie1>=ie1_count) ie1=ie1%ie1_count;
 e1=random();
 if (e1>=e1_count) e1=e1%e1_count;
 do {
       ki=random(); 
      if (ki>=channelb_count) ki=ki%channelb_count;
 } while ( ki == 0 || ki == 16 );


 current_call_pointer.upo=upo;
 current_call_pointer.ie1=ie1;
 current_call_pointer.e1=e1;
 current_call_pointer.ki=ki;

 command_to_storage.clear();
 command_to_storage.push_back(call_control_commands::REQUEST_TO_OCCUPATE_CALL_POINTER);
 command_to_storage.push_back(upo);
 command_to_storage.push_back(ie1);
 command_to_storage.push_back(e1);
 command_to_storage.push_back(ki);
};

void mock_generator::A0_automata::z4(){
 command_to_storage.clear();
 command_to_storage.push_back(call_control_commands::REQUEST_TO_FREE_CHANNEL_B);
 command_to_storage.push_back(current_ch_b_side_a.upo);
 command_to_storage.push_back(current_ch_b_side_a.ie1);
 command_to_storage.push_back(current_ch_b_side_a.e1);
 command_to_storage.push_back(current_ch_b_side_a.ki);
};

void mock_generator::A0_automata::z5(){
 command_to_storage.clear();
 command_to_storage.push_back(call_control_commands::REQUEST_TO_FREE_CHANNEL_B);
 command_to_storage.push_back(current_ch_b_side_b.upo);
 command_to_storage.push_back(current_ch_b_side_b.ie1);
 command_to_storage.push_back(current_ch_b_side_b.e1);
 command_to_storage.push_back(current_ch_b_side_b.ki);

};

void mock_generator::A0_automata::z6(){
bytes tmp_buffer;
tmp_buffer=bytes_of_type<word> (current_call_pointer.call_pointer_id);
command_to_storage.clear();
command_to_storage.push_back(call_control_commands::REQUEST_TO_FREE_CALL_POINTER);
command_to_storage.push_back(current_call_pointer.upo);
command_to_storage.push_back(current_call_pointer.ie1);
command_to_storage.push_back(current_call_pointer.e1);
command_to_storage.push_back(current_call_pointer.ki);

command_to_storage.push_back(tmp_buffer[0]);
command_to_storage.push_back(tmp_buffer[1]);
};

void mock_generator::A1_automata::operator()(int event) throw (runtime_error){
    int old_state=state;

   switch (state) {
       case CHECK_STATE_OF_CHANNEL_B:
             if (EVENT_DATA_CHANGED)
                 z1();
           break;
        default:
            ostringstream err_descr;
            err_descr<<"mock_generator::A1_automata::operator() unknown state "<<state;
            throw runtime_error(err_descr.str());
    }; //switch state

   if (old_state==state) return;
//=========delete it===========
/*
{
                        ostringstream msg;
                        msg<<"Changed automata A1 state from "<<old_state<<" to "<<state;
                        generator_logger& logger_inst=
                                generator_logger::get_instance();
                        logger_inst.log_message
                                (generator_logger::INFO_MSG,
                                 msg.str());
}
*/
//=========================
    switch (state) { //new state
       case CHECK_STATE_OF_CHANNEL_B:
             z1();
           break;
        default:
            ostringstream err_descr;
            err_descr<<"mock_generator::A1_automata::operator() unknown new state "<<state;
            throw runtime_error(err_descr.str());
    }; //switch state

};

void mock_generator::A1_automata::z1(){
 byte upo, ie1, e1, ki;

 //generation data of request
 upo=random();
 if (upo>=upo_count) upo=upo%upo_count;
 ie1=random();
 if (ie1>=ie1_count) ie1=ie1%ie1_count;
 e1=random();
 if (e1>=e1_count) e1=e1%e1_count;
  do {
       ki=random(); 
      if (ki>=channelb_count) ki=ki%channelb_count;
 } while ( ki == 0 || ki == 16 );

 current_call_pointer.upo=upo;
 current_call_pointer.ie1=ie1;
 current_call_pointer.e1=e1;
 current_call_pointer.ki=ki;

 command_to_storage.clear();
 command_to_storage.push_back(proc_control_commands::QUERY_STATE_OF_CHANNEL_B);
 command_to_storage.push_back(upo);
 command_to_storage.push_back(ie1);
 command_to_storage.push_back(e1);
 command_to_storage.push_back(ki);
};


