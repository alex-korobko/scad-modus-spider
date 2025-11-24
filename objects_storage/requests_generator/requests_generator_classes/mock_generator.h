#ifndef __MOCK_GENERATOR__
#define __MOCK_GENERATOR__

class mock_generator {
private :
   mock_generator::mock_generator() throw (runtime_error);
  pthread_t current_thread_id;

  class A0_automata{
    friend class friend_class_of_A0_automata_only_for_compiler_happening;
    private:
      static bytes command_to_storage;
      A0_automata():state(OCCUPATE_CH_B_SIDE_A), x1(false), x2(false), x3(false){z1();};
      A0_automata(const A0_automata&){};
      A0_automata& operator= (const A0_automata& a) {return const_cast<A0_automata&>(a);};

       void z1();
       void z2();
       void z3();
       void z4();
       void z5();
       void z6();
     int state;
    public:
     enum {
          OCCUPATE_CH_B_SIDE_A=1,
          OCCUPATE_CH_B_SIDE_B,
          OCCUPATE_CALL_POINTER,
          CALL_CONTINUE,
          FREE_CH_B_SIDE_A,
          FREE_CH_B_SIDE_B,
          FREE_CALL_POINTER
          };
      enum {
          EVENT_DATA_CHANGED=1,
          EVENT_CALL_END_SIDE_A,
          EVENT_CALL_END_SIDE_B
        };

     bool x1, x2, x3;
     int get_state() {return state;}
     void operator()(int event) throw (runtime_error);
     static A0_automata& get_instance() {static A0_automata auto_inst; return auto_inst;}
     static bytes& get_command_to_storage() {return command_to_storage;}
   };

  class A1_automata{
    friend class friend_class_of_A1_automata_only_for_compiler_happening;
    private:
      static bytes command_to_storage;
      A1_automata():state(CHECK_STATE_OF_CHANNEL_B) { z1();};
      A1_automata(const A1_automata&){};
      A1_automata& operator= (const A1_automata& a) {return const_cast<A1_automata&>(a);};
      void z1();
      int state;
    public:
     enum {
          CHECK_STATE_OF_CHANNEL_B=1
          };

     enum {
         EVENT_DATA_CHANGED=1
       };

     int get_state() {return state;}
     void operator()(int event) throw (runtime_error);
     static A1_automata& get_instance() {static A1_automata auto_inst; return auto_inst;}
     static bytes& get_command_to_storage() {return command_to_storage;}
   };

  struct channel_b {
    byte upo;
    byte ie1;
    byte e1;
    byte ki;
    channel_b():upo(0), ie1(0),e1(0),ki(0){};
  };

  struct call_pointer {
    byte upo;
    byte ie1;
    byte e1;
    byte ki;
    word call_pointer_id;
    call_pointer():upo(0), ie1(0), e1(0), ki(0),call_pointer_id(0){};
  };
/*
  class result {
   private:
    uint64_t accumulate_of_samle_mean;
    uint64_t accumulate_of_samle_deviation;
    uint64_t samples_count;
    uint64_t max_sample_member;
    uint64_t min_sample_member;

  public:
     result():
         accumulate_of_samle_mean(0),
         accumulate_of_samle_deviation(0),
         samples_count(0),
         max_sample_member(0),
         min_sample_member(0){};

     void operator() +=(uint64_t new_sample_member){
          accumulate_of_samle_mean+=new_sample_member;
          accumulate_of_samle_deviation+=new_sample_member; //ATTENTION!! It`s wrong
          if (new_sample_member>max_sample_member) max_sample_member=new_sample_member;
          if (new_sample_member<min_sample_member) min_sample_member=new_sample_member;
          samples_count++;
       };

    double sample_mean() throw (runtime_error) 
          {if (samples_count==0) 
              throw runtime_error("On result::sample_mean samples_count==0");
            return accumulate_of_samle_mean/samples_count;};

    double samle_deviation() throw (runtime_error)  //ATTENTION!! It`s wrong
          {if (samples_count<2) 
              throw runtime_error("On result::samle_deviation samples_count==0");
            return sqrt(accumulate_of_samle_deviation/(samples_count-1));};

    uint64_t count() {return samples_count;} ;
    uint64_t max() throw (runtime_error)
    {if (samples_count==0) 
              throw runtime_error("On result::max samples_count==0");
            return max_sample_member;};

    uint64_t min() throw (runtime_error)
        {if (samples_count==0) 
              throw runtime_error("On result::min samples_count==0");
            return min_sample_member;};

  };
*/

  uint64_t request_cycle_begin;
  uint64_t request_cycle_end;

  results sample_requests_to_occupate_channel_b_side_a_times;
  results sample_requests_to_occupate_channel_b_side_b_times;
  results sample_request_to_free_channel_b_times;
  results sample_error_to_occupation_channel_b_times;
  results sample_error_to_free_channel_b_times;

  results sample_requests_to_occupate_call_pointer_times;
  results sample_requests_to_free_call_pointer_times;
  results sample_error_to_occupate_call_pointer_times;
  results sample_error_to_free_call_pointer_times;

  results sample_request_of_channel_b_state_checking_times;
  results sample_error_of_channel_b_state_checking_times;

  static mock_generator& get_instance() throw (runtime_error);
  static double calculate_sample_mean(results sample) throw (runtime_error);
  static double calculate_standart_deviation(results sample) throw (runtime_error);

public :

// splitting and merging per bytes -  very dangerous templates using
	template<typename T>
	static bytes bytes_of_type (T value_to_splitting) {
		bytes ret_val(sizeof(T));
		for (bytes::size_type i=0; i<ret_val.size();i++){
			ret_val[i]=((value_to_splitting & (0x00FF<<i*8))>>i*8);
		};
		return  ret_val;
   };

	template<typename T>
	static T type_from_bytes (bytes bytes_to_merging){
			T ret_val=0;
			bytes::size_type count=__min(bytes_to_merging.size(), sizeof(T));
			for (bytes::size_type i=0; i<count; i++){
				ret_val |=(bytes_to_merging[i] << (i*8) );
            };

            return  ret_val;
    };

 static void run(int requests_count) throw (runtime_error);
 static void  html_format_results_outputter() throw (runtime_error);

 static int channel_id;
 static call_pointer current_call_pointer;
 static channel_b current_ch_b_side_a, current_ch_b_side_b;
 
 static int upo_count, ie1_count, e1_count, channelb_count, max_channel_number, max_destination;
};

#endif
