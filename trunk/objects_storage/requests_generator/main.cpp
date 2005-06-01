#include <string>
#include <vector>
#include <hash_map>
#include <numeric>
#include <algorithm>
#include <sstream>

#include <iostream.h>

#include <inttypes.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <process.h>
#include <sys/neutrino.h>

using namespace std;

#include "test_cond.h"
#include "generator_common.h"
#include "generator_logger.h"
#include "mock_generator.h"

#ifdef __UNIT_TESTING__
#include "mock_generator_test.h"
#endif

void usage(string program_name) {
   if (program_name.empty()) program_name="mock_generator";
   cout<<" Usage :\n"
           <<"    "<<program_name<<" clock_period requests_count objects_storage_program_name\n"
           <<"    where :\n"
           <<"       clock_period  - new clock period of realtime clock in nanoseconds (-1 if not used; 0 is wrong value)\n"
           <<"       requests_count  - count of requests to objects storage program\n"
           <<"       objects_storage_program_name  - name (with full path or\n"
           <<"             in directory of PATH) of objects storage program\n"
           <<endl;
};

int main (int argc, const char* argv[]) {
#ifdef __UNIT_TESTING__
  mock_generator_test gen_test("simply test" ,1);
  cout<<"Run tests for "<<gen_test.get_name()<<endl;
  gen_test.run_tests();

#else

  int channel_id=ChannelCreate(0);
  int requests_count=0;
  int new_clock_period_nsec;
  string full_program_name,
             program_name,
             node_id_string("-n"),
             pid_t_string("-p"),
             channel_id_string("-c");
vector<char> tmp_chars(32);
struct _clockperiod old_clock_period, new_clock_period;


  if (argc<4) {
       usage(argv[0]);
       return 1;
   };

  new_clock_period_nsec=atoi(argv[1]);
  if (new_clock_period_nsec==0) {
       usage(argv[0]);
       return 1;
   };

  requests_count=atoi(argv[2]);
  if (requests_count==0) {
       usage(argv[0]);
       return 1;
   };

  full_program_name=argv[3];
  if (full_program_name.empty()) {
       usage(argv[0]);
       return 1;
   };

if (new_clock_period_nsec!=-1) {

 new_clock_period.nsec=new_clock_period_nsec;
 new_clock_period.fract=0;
 if (ClockPeriod(CLOCK_REALTIME,
                 &new_clock_period,
                 &old_clock_period,
                 0)==-1) {
          generator_logger& logger_inst=
          generator_logger::get_instance();
          logger_inst.log_message
                   (generator_logger::ERROR_MSG,
                    string("In requests_generator main(...) can`t set new ClockPeriod : ")+
                    strerror(errno));
          return 1;
       };
 cout<<"old_clock_period.nsec "<<old_clock_period.nsec<<endl;
};

program_name=full_program_name;
int pos=program_name.find_last_of("/");
if (pos>=0) 
    program_name.assign(program_name, pos+1, program_name.size()-pos+1);  

node_id_string+="0";
itoa( getpid(), &tmp_chars[0], 10);
pid_t_string+=&tmp_chars[0];
itoa( channel_id, &tmp_chars[0], 10);
channel_id_string+=&tmp_chars[0];

mock_generator::channel_id=channel_id;
mock_generator::upo_count=4;
mock_generator::ie1_count=16;
mock_generator::e1_count=16;
mock_generator::channelb_count=31;
mock_generator::max_channel_number=1;
mock_generator::max_destination=2;
spawnl(P_NOWAIT,
            full_program_name.c_str(),
            program_name.c_str(),
            node_id_string.c_str(),
            pid_t_string.c_str(),
            channel_id_string.c_str(),
            NULL);

  try {
      mock_generator::run(requests_count);
      mock_generator::html_format_results_outputter();
  } catch (runtime_error gen_exc) {
          generator_logger& logger_inst=
          generator_logger::get_instance();
          logger_inst.log_message
                   (generator_logger::ERROR_MSG,
                    string("In requests_generator main(...) catched exception : ")+
                    gen_exc.what());
  };
  
if (new_clock_period_nsec!=-1) {
 if (ClockPeriod(CLOCK_REALTIME,
                 &old_clock_period,
                 &new_clock_period,
                 0)==-1) {
          generator_logger& logger_inst=
          generator_logger::get_instance();
          logger_inst.log_message
                   (generator_logger::ERROR_MSG,
                    string("In requests_generator main(...) can`t set old ClockPeriod : ")+
                    strerror(errno));
       };
};


 return 0;
#endif
};
