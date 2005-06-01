using namespace std;
#include <string>
#include <vector>
#include <hash_map>
#include <numeric>
#include <algorithm>
#include <sstream>

#include <iostream.h>

#include <inttypes.h>
#include <math.h>
#include <sys/neutrino.h>
#include <sys/syspage.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>

#include "generator_exception.h"
#include "generator_logger.h"
#include "mock_generator.h"


 mock_generator* mock_generator::mock_generator_instance=NULL;

int mock_generator::channel_id=0;
mock_generator::byte mock_generator::upo_count=0;
mock_generator::byte mock_generator::ie1_count=0;
mock_generator::byte mock_generator::e1_count=0;
mock_generator::byte mock_generator::channelb_count=0;
mock_generator::word mock_generator::max_channel_number=0;

mock_generator::mock_generator() throw (generator_exception) :
request_cycle_begin(0),
request_cycle_end(0)
 {

vector<char> random_state(32);
initstate( time(NULL), 
               &random_state[0],
              random_state.size());
setstate(&random_state[0]);

if (ConnectAttach( 0,
               0,
               channel_id,
               0,
               0)<0) {
              string message ("mock_generator::mock_generator ConnectAttach(...) error : ");
              message+=strerror(errno);

              throw generator_exception(message);
   } ;

pthread_t pthread_id=5;
pthreads_info.find(pthread_id);

pthread_id=7;
pthreads_info.find(pthread_id);
};


mock_generator* mock_generator::get_instance() {
   if (mock_generator_instance==NULL) {
              try {
              mock_generator_instance=new mock_generator();
               } catch (generator_exception gen_exc) {
                      string message ("mock_generator::get_instance() catched exception: ");
                       message+=gen_exc.get_description();

                        generator_logger* logger_inst=
                                  generator_logger::get_instance();

                       logger_inst->log_message
                                 (generator_logger::ERROR_MSG,
                                   message);

               };
          };
   return mock_generator_instance;
}

double mock_generator::
    calculate_sample_mean(vector<uint64_t> sample)throw (generator_exception){
if (sample.empty())  throw generator_exception("mock_generator::
    calculate_sample_mean Can`t calculate sample mean of empty sample");
float sample_mean=0;
sample_mean=accumulate(sample.begin(), sample.end(), sample_mean);
sample_mean = sample_mean/sample.size();
return sample_mean;
};

double mock_generator::
    calculate_standart_deviation(vector<uint64_t> sample) throw (generator_exception){
if (sample.empty())  
  throw generator_exception("mock_generator::
    calculate_sample_mean Can`t calculate sample deviation of empty sample");
if (sample.size()==1)
   throw generator_exception("mock_generator::
    calculate_sample_mean Can`t calculate sample deviation of sample with one element");
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


void  mock_generator::
        html_format_results_outputter() throw (generator_exception){
   mock_generator* gen_instance=mock_generator::get_instance();
   if (gen_instance==NULL) return;
	double 
          sample_mean_request_to_occupate, 
           sample_deviation_requests_to_occupate,

          sample_mean_request_to_free, 
          sample_deviation_requests_to_free,

          sample_mean_error_to_occupation, 
          sample_deviation_error_to_occupation,

          sample_mean_error_to_free, 
          sample_deviation_error_to_free,

	       semulation_time;
	       
	uint64_t cycles_per_second=SYSPAGE_ENTRY(qtime)->cycles_per_sec,

                   max_request_to_occupate,
                   min_request_to_occupate,

                   max_request_to_free,
                   min_request_to_free,

                   max_error_to_occupation,
                   min_error_to_occupation,

                   max_error_to_free,
                   min_error_to_free;

    ostringstream return_string;
    if ( !gen_instance->sample_requests_to_occupate_times.empty() ||
          !gen_instance->sample_requests_to_free_times.empty() ||
          !gen_instance->sample_error_to_occupation_times.empty() ||
          !gen_instance->sample_error_to_free_times.empty()
         ) {
            semulation_time=(gen_instance->request_cycle_end-
                                            gen_instance->request_cycle_begin)*1000/cycles_per_second;
        } else {
          	semulation_time=0;
        };

    if (! gen_instance->sample_requests_to_occupate_times.empty() &&
        gen_instance->sample_requests_to_occupate_times.size()>1) {
	sample_mean_request_to_occupate=calculate_sample_mean(gen_instance->sample_requests_to_occupate_times);
	sample_deviation_requests_to_occupate=calculate_standart_deviation(gen_instance->sample_requests_to_occupate_times);
    max_request_to_occupate=*(max_element(gen_instance->sample_requests_to_occupate_times.begin(),
                           gen_instance->sample_requests_to_occupate_times.end()
                           ));    
     min_request_to_occupate=*(min_element(gen_instance->sample_requests_to_occupate_times.begin(),
                           gen_instance->sample_requests_to_occupate_times.end()
                           ));

    } else if (gen_instance->sample_requests_to_occupate_times.size()==1){
          sample_mean_request_to_occupate=gen_instance->sample_requests_to_occupate_times[0];
          sample_deviation_requests_to_occupate=0;
          max_request_to_occupate=gen_instance->sample_requests_to_occupate_times[0];
          min_request_to_occupate=gen_instance->sample_requests_to_occupate_times[0];
    } else {
	sample_mean_request_to_occupate=0;
	sample_deviation_requests_to_occupate=0;
    max_request_to_occupate=0;
    min_request_to_occupate=0;
    };

    if (! gen_instance->sample_requests_to_free_times.empty() &&
            gen_instance->sample_requests_to_free_times.size()>1) {
	sample_mean_request_to_free=calculate_sample_mean(gen_instance->sample_requests_to_free_times);
	sample_deviation_requests_to_free=calculate_standart_deviation(gen_instance->sample_requests_to_free_times);
    max_request_to_free=*(max_element(gen_instance->sample_requests_to_free_times.begin(),
                           gen_instance->sample_requests_to_free_times.end()
                           ));    
     min_request_to_free=*(min_element(gen_instance->sample_requests_to_free_times.begin(),
                           gen_instance->sample_requests_to_free_times.end()
                           ));

    } else if (gen_instance->sample_requests_to_free_times.size()==1) {
            sample_mean_request_to_free=gen_instance->sample_requests_to_free_times[0];
            sample_deviation_requests_to_free=0;
            max_request_to_free=gen_instance->sample_requests_to_free_times[0];
            min_request_to_free=gen_instance->sample_requests_to_free_times[0];
    }else {
	sample_mean_request_to_free=0;
	sample_deviation_requests_to_free=0;
    max_request_to_free=0;
    min_request_to_free=0;
    };

    if (! gen_instance->sample_error_to_occupation_times.empty() &&
           gen_instance->sample_error_to_occupation_times.size()>1) {

	sample_mean_error_to_occupation=calculate_sample_mean(gen_instance->sample_error_to_occupation_times);
	sample_deviation_error_to_occupation=calculate_standart_deviation(gen_instance->sample_error_to_occupation_times);
    max_error_to_occupation=*(max_element(gen_instance->sample_error_to_occupation_times.begin(),
                           gen_instance->sample_error_to_occupation_times.end()
                           ));    
     min_error_to_occupation=*(min_element(gen_instance->sample_error_to_occupation_times.begin(),
                           gen_instance->sample_error_to_occupation_times.end()
                           ));
    } else if (gen_instance->sample_error_to_occupation_times.size()==1) {
         sample_mean_error_to_occupation=gen_instance->sample_error_to_occupation_times[0];
         sample_deviation_error_to_occupation=0;
         max_error_to_occupation=gen_instance->sample_error_to_occupation_times[0];
         min_error_to_occupation=gen_instance->sample_error_to_occupation_times[0];
    } else {
       sample_mean_error_to_occupation=0;
       sample_deviation_error_to_occupation=0;
       max_error_to_occupation=0;
       min_error_to_occupation=0;
    };

    if (! gen_instance->sample_error_to_free_times.empty() &&
           gen_instance->sample_error_to_free_times.size()>1) {
	sample_mean_error_to_free=calculate_sample_mean(gen_instance->sample_error_to_free_times);
	sample_deviation_error_to_free=calculate_standart_deviation(gen_instance->sample_error_to_free_times);
    max_error_to_free=*(max_element(gen_instance->sample_error_to_free_times.begin(),
                           gen_instance->sample_error_to_free_times.end()
                           ));    
     min_error_to_free=*(min_element(gen_instance->sample_error_to_free_times.begin(),
                           gen_instance->sample_error_to_free_times.end()
                           ));

    } else if (gen_instance->sample_error_to_free_times.size()==1) {
            sample_mean_error_to_free=gen_instance->sample_error_to_free_times[0];
            sample_deviation_error_to_free=0;
            max_error_to_free=gen_instance->sample_error_to_free_times[0];
            min_error_to_free=gen_instance->sample_error_to_free_times[0];
    } else {
	sample_mean_error_to_free=0;
	sample_deviation_error_to_free=0;
    max_error_to_free=0;
    min_error_to_free=0;
    };

    return_string.flags(ios_base::fixed|ios_base::dec);
	
	return_string<<"<center>Simulation statistic:</center><br>\n";
	return_string<<
	  "<table width=80% border=1 cellspacing=2 cellpadding=0 align=center>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nSuccess requests to occupate count</td>\n<td>"<<
	   gen_instance->sample_requests_to_occupate_times.size()<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nErrors to occupate count</td>\n<td>"<<
        gen_instance->sample_error_to_occupation_times.size()<<"</td>\n";
	return_string<<"</tr>\n";


	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nSuccess requests to free count</td>\n<td>"<<
	   gen_instance->sample_requests_to_free_times.size()<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nErrors to free count</td>\n<td>"<<
        gen_instance->sample_error_to_free_times.size()<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	  "<td width=80%>\nSimulation time<br>(mS)</td>\n<td>"<<
     semulation_time<<"</td>\n";
	return_string<<"</tr>\n";


	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nRequests to occupate sample mean<br>(mS)</td>\n<td>"<<
	   (sample_mean_request_to_occupate*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nRequests  to occupate sample deviation<br>(mS)</td>\n<td>"<<
	   (sample_deviation_requests_to_occupate*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nMax request to occupate <br>(mS)</td>\n<td>"<<
	   (max_request_to_occupate*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	  "<td width=80%>\nMin request to occupate <br>(mS)</td>\n<td>"<<
	  (min_request_to_occupate*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";


	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nRequests to free sample mean<br>(mS)</td>\n<td>"<<
	   (sample_mean_request_to_free*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nRequests  to free sample deviation<br>(mS)</td>\n<td>"<<
	   (sample_deviation_requests_to_free*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nMax request to free <br>(mS)</td>\n<td>"<<
	   (max_request_to_free*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	  "<td width=80%>\nMin request to free<br>(mS)</td>\n<td>"<<
	  (min_request_to_free*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";


	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nError to occupation sample mean<br>(mS)</td>\n<td>"<<
	   (sample_mean_error_to_occupation*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nError  to occupation sample deviation<br>(mS)</td>\n<td>"<<
	   (sample_deviation_error_to_occupation*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nMax error to occupation <br>(mS)</td>\n<td>"<<
	   (max_error_to_occupation*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	  "<td width=80%>\nMin error to occupation <br>(mS)</td>\n<td>"<<
	  (min_error_to_occupation*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";


	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nError to free sample mean<br>(mS)</td>\n<td>"<<
	   (sample_mean_error_to_free*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nError  to free sample deviation<br>(mS)</td>\n<td>"<<
	   (sample_deviation_error_to_free*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	   "<td width=80%>\nMax error to free <br>(mS)</td>\n<td>"<<
	   (max_error_to_free*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";

	return_string<<"<tr>\n";
	return_string<<
	  "<td width=80%>\nMin error to free <br>(mS)</td>\n<td>"<<
	  (min_error_to_free*1000.0/cycles_per_second)<<"</td>\n";
	return_string<<"</tr>\n";


    return_string<<"</table>\n\n";

    return_string<<endl;
	cout<<return_string.str()<<endl;
};

void mock_generator::run(int requests_count)
         throw (generator_exception){
int counter=1;
vector<byte> message_buffer(512);
vector<byte>::iterator msg_buffer_iter;
int recv_id , failures_count;
pthread_t pthread_id;
bool only_that_channel;
uint64_t request_begin_cycles;
uint64_t request_end_cycles;
mock_generator* gen_instance=mock_generator::get_instance();
if (gen_instance==NULL) return;

gen_instance->sample_requests_to_occupate_times.clear();
gen_instance->sample_requests_to_occupate_times.reserve(requests_count);

gen_instance->sample_requests_to_free_times.clear();
gen_instance->sample_requests_to_free_times.reserve(requests_count);

gen_instance->sample_error_to_occupation_times.clear();
gen_instance->sample_error_to_occupation_times.reserve(requests_count);

gen_instance->sample_error_to_free_times.clear();
gen_instance->sample_error_to_free_times.reserve(requests_count);

//reply dirven request from client
recv_id=MsgReceive(channel_id,
                                    &message_buffer[0],
                                    message_buffer.size(),
                                    NULL);

if (recv_id==-1) {
       string message ("mock_generator::run MsgReceive to empty buffer error : ");
       message+=strerror(errno);

      generator_logger* logger_inst=
                    generator_logger::get_instance();

      logger_inst->log_message
                      (generator_logger::ERROR_MSG,
                        message);
};


recv_id=MsgReply(recv_id,
                   EOK,
                    &message_buffer[0],
                    message_buffer.size());

if (recv_id==-1) {
       string message ("mock_generator::run MsgReceive to empty buffer error : ");
       message+=strerror(errno);

      generator_logger* logger_inst=
                    generator_logger::get_instance();

      logger_inst->log_message
                      (generator_logger::ERROR_MSG,
                        message);
};

gen_instance->request_cycle_begin=0;
failures_count=0;
counter=1;
while( (counter<=requests_count ||
           !gen_instance->pthreads_info.empty()) &&
          failures_count<MAX_FAILURES_COUNT) {


recv_id=MsgReceive(channel_id,
                                    &message_buffer[0],
                                    message_buffer.size(),
                                    NULL);

if (gen_instance->request_cycle_begin==0)
                 gen_instance->request_cycle_begin=ClockCycles();

request_end_cycles=ClockCycles();

if (recv_id==-1) {
       string message ("mock_generator::run MsgReceive error : ");
       message+=strerror(errno);

      generator_logger* logger_inst=
                    generator_logger::get_instance();

      logger_inst->log_message
                      (generator_logger::ERROR_MSG,
                        message);
    failures_count++;
     continue;
};

if (message_buffer.empty()||
     message_buffer.size()<DESCRIPTOR_SIZE) {
       string message ("mock_generator::run too small message_buffer returned ");

      generator_logger* logger_inst=
                    generator_logger::get_instance();

      logger_inst->log_message
                      (generator_logger::ERROR_MSG,
                        message);
    failures_count++;
     continue;
};

failures_count=0;

msg_buffer_iter=message_buffer.begin();
advance(msg_buffer_iter, DESCRIPTOR_SIZE);
message_buffer.erase(message_buffer.begin(), msg_buffer_iter);

switch (message_buffer[COMMAND_DISCRIMINATOR_INDEX]) {
   case INITIAL_RECEIVE: {

                    pthread_id = counter;
                   (random()%2)==0?only_that_channel=false: only_that_channel=true;
         
                  message_buffer=gen_instance->generate_pri_command_request_to_occupate
                                                                                                                          (pthread_id,
                                                                                                                          only_that_channel);
         break;
      };
   case DISCRIMINATOR_PRI :{
         switch (message_buffer[COMMAND_CODE_INDEX]) {
            case PRI_COMMAND_ANSWER_TO_OCCUPATE_CODE: {
                   //store time of occupation
                   request_end_cycles-=request_begin_cycles;
                   gen_instance->sample_requests_to_occupate_times.push_back(request_end_cycles);
                   //store occupation parameters
                   vector<byte>::reverse_iterator beg_iter=message_buffer.rend(), end_iter=beg_iter;
                   advance (beg_iter, -2); 
                   advance (end_iter, -(2+sizeof(pthread_t)));
                   pthread_id=type_from_bytes<pthread_t>(vector<byte>(end_iter, beg_iter));

                   call_parameters current_call;
                   current_call.thread_id=pthread_id;
                   current_call.upo_number=message_buffer[2+sizeof(pthread_t)];
                   current_call.ie1_number=message_buffer[2+sizeof(pthread_t)+1];
                   current_call.e1_number=message_buffer[2+sizeof(pthread_t)+2];
                   current_call.channel_interval=message_buffer[2+sizeof(pthread_t)+3];

                   beg_iter=message_buffer.rend();
                   end_iter=beg_iter;
                   advance (beg_iter, -2+sizeof(pthread_t)+4); 
                   advance (end_iter, -(2+sizeof(pthread_t)+4+sizeof(word)));
                   current_call.channelb_number=type_from_bytes<word>(vector<byte>(end_iter, beg_iter));

                   gen_instance->pthreads_info.insert( pthreads_infos_container::value_type (pthread_id, current_call));
                   //generate next request
                   if (! gen_instance->pthreads_info.empty() &&
                       counter<=requests_count)  {
                            int random_number=random();
                            if (random_number%2==0) {
                                           pthread_id = counter;
                                           (random()%2)==0?only_that_channel=false: only_that_channel=true;

                                            message_buffer=gen_instance->generate_pri_command_request_to_occupate
                                                                                                                                     (pthread_id,
                                                                                                                                      only_that_channel);
                             } else {
                                           message_buffer=gen_instance->generate_pri_command_request_to_free 
                                                                                       (gen_instance->pthreads_info.begin()->second);
                             }; // if (random_number%2==0)
                     } else if (counter<=requests_count){// if (! gen_instance->pthreads_info.empty())

                            pthread_id = counter;
                            (random()%2)==0?only_that_channel=false: only_that_channel=true;
         
                            message_buffer=gen_instance->generate_pri_command_request_to_occupate
                                                                                                        (pthread_id,
                                                                                                         only_that_channel);
                     } else if (! gen_instance->pthreads_info.empty()) { // if (! gen_instance->pthreads_info.empty())
                                 message_buffer=gen_instance->generate_pri_command_request_to_free 
                                                                        (gen_instance->pthreads_info.begin()->second);
                     } else {
                          failures_count=MAX_FAILURES_COUNT;
                     };
               break;
             }; // case PRI_COMMAND_ANSWER_TO_OCCUPATE_CODE
            case PRI_COMMAND_ANSWER_TO_FREE_CODE: {
                   //store time of free
                   request_end_cycles-=request_begin_cycles;
                   gen_instance->sample_requests_to_free_times.push_back(request_end_cycles);

                   //erase thread info record
                   vector<byte>::reverse_iterator beg_iter=message_buffer.rend(), end_iter=beg_iter;
                   advance (beg_iter, -2); 
                   advance (end_iter, -(2 +sizeof(pthread_t)));
                   pthread_id=type_from_bytes<pthread_t>(vector<byte>(end_iter, beg_iter));

                   gen_instance->pthreads_info.erase(pthread_id); 

                   //generate next request
                   if (! gen_instance->pthreads_info.empty() &&
                       counter<=requests_count)  {
                            int random_number=random();
                            if (random_number%2==0) {
                                           pthread_id = counter;
                                           (random()%2)==0?only_that_channel=false: only_that_channel=true;

                                            message_buffer=gen_instance->generate_pri_command_request_to_occupate
                                                                                                                                     (pthread_id,
                                                                                                                                      only_that_channel);
                             } else {
                                           message_buffer=gen_instance->generate_pri_command_request_to_free 
                                                                                       (gen_instance->pthreads_info.begin()->second);
                             }; // if (random_number%2==0)
                     } else if (counter<=requests_count){// if (! gen_instance->pthreads_info.empty())

                            pthread_id = counter;
                            (random()%2)==0?only_that_channel=false: only_that_channel=true;
         
                            message_buffer=gen_instance->generate_pri_command_request_to_occupate
                                                                                                        (pthread_id,
                                                                                                         only_that_channel);
                     } else if (! gen_instance->pthreads_info.empty()) { // if (! gen_instance->pthreads_info.empty())
                                 message_buffer=gen_instance->generate_pri_command_request_to_free 
                                                                        (gen_instance->pthreads_info.begin()->second);
                     } else {
                          failures_count=MAX_FAILURES_COUNT;
                     };

               break;
             }; // PRI_COMMAND_ANSWER_TO_FREE_CODE

            case PRI_COMMAND_ERROR_IN_OCCUPATION: {
                   //store time of error in occupation
                   request_end_cycles-=request_begin_cycles;
                   gen_instance->sample_error_to_occupation_times.push_back(request_end_cycles);

                   //generate next request
                   if (! gen_instance->pthreads_info.empty() &&
                       counter<=requests_count)  {
                            int random_number=random();
                            if (random_number%2==0) {
                                           pthread_id = counter;
                                           (random()%2)==0?only_that_channel=false: only_that_channel=true;

                                            message_buffer=gen_instance->generate_pri_command_request_to_occupate
                                                                                                                                     (pthread_id,
                                                                                                                                      only_that_channel);
                             } else {
                                           message_buffer=gen_instance->generate_pri_command_request_to_free 
                                                                                       (gen_instance->pthreads_info.begin()->second);
                             }; // if (random_number%2==0)
                     } else if (counter<=requests_count){// if (! gen_instance->pthreads_info.empty())

                            pthread_id = counter;
                            (random()%2)==0?only_that_channel=false: only_that_channel=true;
         
                            message_buffer=gen_instance->generate_pri_command_request_to_occupate
                                                                                                        (pthread_id,
                                                                                                         only_that_channel);
                     } else if (! gen_instance->pthreads_info.empty()) { // if (! gen_instance->pthreads_info.empty())
                                 message_buffer=gen_instance->generate_pri_command_request_to_free 
                                                                        (gen_instance->pthreads_info.begin()->second);
                     } else {
                          failures_count=MAX_FAILURES_COUNT;
                     };

               break;
             }; // PRI_COMMAND_ERROR_IN_OCCUPATION

            case PRI_COMMAND_ERROR_IN_FREE: {
                   //store time of error in occupation
                   request_end_cycles-=request_begin_cycles;
                   gen_instance->sample_error_to_free_times.push_back(request_end_cycles);

                   //erase thread info record
                   vector<byte>::reverse_iterator beg_iter=message_buffer.rend(), end_iter=beg_iter;
                   advance (beg_iter, -2); 
                   advance (end_iter, -(2+sizeof(pthread_t)));
                   pthread_id=type_from_bytes<pthread_t>(vector<byte>(end_iter, beg_iter));


                   gen_instance->pthreads_info.erase(pthread_id); 

                   //generate next request

                   //generate next request
                   if (! gen_instance->pthreads_info.empty() &&
                       counter<=requests_count)  {
                            int random_number=random();
                            if (random_number%2==0) {
                                           pthread_id = counter;
                                           (random()%2)==0?only_that_channel=false: only_that_channel=true;

                                            message_buffer=gen_instance->generate_pri_command_request_to_occupate
                                                                                                                                     (pthread_id,
                                                                                                                                      only_that_channel);
                             } else {
                                           message_buffer=gen_instance->generate_pri_command_request_to_free 
                                                                                       (gen_instance->pthreads_info.begin()->second);
                             }; // if (random_number%2==0)
                     } else if (counter<=requests_count){// if (! gen_instance->pthreads_info.empty())

                            pthread_id = counter;
                            (random()%2)==0?only_that_channel=false: only_that_channel=true;
         
                            message_buffer=gen_instance->generate_pri_command_request_to_occupate
                                                                                                        (pthread_id,
                                                                                                         only_that_channel);
                     } else if (! gen_instance->pthreads_info.empty()) { // if (! gen_instance->pthreads_info.empty())
                                 message_buffer=gen_instance->generate_pri_command_request_to_free 
                                                                        (gen_instance->pthreads_info.begin()->second);
                     } else {
                          failures_count=MAX_FAILURES_COUNT;
                     };

               break;
             }; // PRI_COMMAND_ERROR_IN_FREE

            default : {
//                  string exception_description ("Uknown command for DISCRIMINATOR_PRI in message replay  command code 0x");
                  vector<char> tmp_chars(32);
//                  itoa(message_buffer[1], &tmp_chars[0], 16);
//                  exception_description+=&tmp_chars[0];
                 string exception_description;
          
                 exception_description+="message  ";
                  for (vector<byte>::size_type i=0; i<message_buffer.size(); i++) {
                         itoa(message_buffer[i], &tmp_chars[0], 16);
                         exception_description+=" 0x";
                         exception_description+=&tmp_chars[0];
                   };

                  itoa(counter, &tmp_chars[0], 10);
                  exception_description+=" count ";
                  exception_description+=&tmp_chars[0];

                  throw generator_exception(exception_description);
            }; //default
         }; // switch (message_buffer[1])
      break;
   }; // case DISCRIMINATOR_PRI
  default : {
      string excepiton_description ("Uknown discriminator in message replay 0x");
      vector<char> tmp_chars(32);
      itoa(message_buffer[0], &tmp_chars[0], 16);
      throw generator_exception(excepiton_description+&tmp_chars[0]);
  }; //default 
}; //switch (message_buffer[0])

message_buffer.insert (message_buffer.begin(),
                                       DESCRIPTOR_SIZE,
                                       0);

request_begin_cycles=ClockCycles();
recv_id=MsgReply(recv_id,
                   EOK,
                    &message_buffer[0],
                    message_buffer.size());

if (recv_id==-1) {
       string message ("mock_generator::run MsgReply error : ");
       message+=strerror(errno);

      generator_logger* logger_inst=
                    generator_logger::get_instance();

      logger_inst->log_message
                      (generator_logger::ERROR_MSG,
                        message);
       failures_count++;
        continue;
  } else {  //if (recv_id==-1)
     failures_count=0;
  };

counter++;
}; //while(counter<requests_count)

gen_instance->request_cycle_end=ClockCycles();

}; 


vector<mock_generator::byte> mock_generator::
                generate_pri_command_request_to_occupate(
                                                                 pthread_t pthread_id,
                                                                 bool only_that_channel){
vector<byte> message_to_return(0);
vector<byte> data_type_bytes;
byte upo_number,
        ie1_number,
        e1_number,
        channel_interval;
//word channelb_number;
//int channel_number_max;

message_to_return.push_back(DISCRIMINATOR_PRI);
message_to_return.push_back(PRI_COMMAND_REQUEST_TO_OCCUPATE_CODE);
data_type_bytes=bytes_of_type<pthread_t>(pthread_id);
message_to_return.insert(message_to_return.end(),
                                           data_type_bytes.rbegin(),
                                           data_type_bytes.rend());

//setting upo_number
upo_number=random()%upo_count;
if (upo_number==0) upo_number=1;
message_to_return.push_back(upo_number);

//setting ie1_number
ie1_number=random()%ie1_count;
if (ie1_number==0) ie1_number=1;
message_to_return.push_back(ie1_number);

//setting e1_number
e1_number=random()%e1_count;
if (e1_number==0) e1_number=1;
message_to_return.push_back(e1_number);

//channel_interval=30;

//channel_interval
channel_interval=random()%31;
if (channel_interval<1|| channel_interval==16) channel_interval=1;
message_to_return.push_back(channel_interval);

//adding channelb_number - for voice in channel_b_d channel number always 1
message_to_return.push_back(0);
message_to_return.push_back(1);

only_that_channel?
                      message_to_return.push_back(1):
                      message_to_return.push_back(0);

//=================delete it ===============
/*
                              ostringstream message;
                              message<<"In mock_generator::generate_pri_command_request_to_occupate(...): "
                                        <<" upo "<<static_cast<int>(upo_number)
                                        <<" ie1 "<<static_cast<int>(ie1_number)
                                      <<" e1 "<<static_cast<int>(e1_number)
                                       <<" channel_interval "<<static_cast<int>(channel_interval)
                                        <<" pthread_t "<<pthread_id
                                     ;

                            generator_logger* logger_inst=
                                generator_logger::get_instance();
                            logger_inst->log_message
                                (generator_logger::INFO_MSG,
                                 message.str());

message.str("");
message<<"In mock_generator::generate_pri_command_request_to_occupate(...): ";
vector<char> tmp_chars(32);
for(int i=0; i<message_to_return.size();i++) {
       itoa(message_to_return[i], &tmp_chars[0], 16);
       message<<" 0x"<<&tmp_chars[0];
};

                            logger_inst->log_message
                                (generator_logger::INFO_MSG,
                                 message.str());
*/
//=====================================



return message_to_return;

};

vector<mock_generator::byte> mock_generator::
 generate_pri_command_request_to_free (call_parameters parameter_of_call) {
vector<byte> message_to_return(0);
vector<byte> tmp_bytes;

message_to_return.push_back(DISCRIMINATOR_PRI);
message_to_return.push_back(PRI_COMMAND_REQUEST_TO_FREE_CODE);
tmp_bytes=bytes_of_type<pthread_t>(parameter_of_call.thread_id);
message_to_return.insert(message_to_return.end(),
                                           tmp_bytes.rbegin(),
                                           tmp_bytes.rend()
                                          );

message_to_return.push_back(parameter_of_call.upo_number);
message_to_return.push_back(parameter_of_call.ie1_number);
message_to_return.push_back(parameter_of_call.e1_number);
message_to_return.push_back(parameter_of_call.channel_interval);

//channel number in channel_b_d always 1 for voice
tmp_bytes=bytes_of_type<word>(parameter_of_call.channelb_number);
message_to_return.insert(message_to_return.end(),
                                           tmp_bytes.rbegin(),
                                           tmp_bytes.rend()
                                          );

return message_to_return;
};
