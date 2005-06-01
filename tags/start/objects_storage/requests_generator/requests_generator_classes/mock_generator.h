class mock_generator {
private :
    typedef uint8_t byte;
    typedef uint16_t word;

  enum {MAX_FAILURES_COUNT=20};

  enum { COMMAND_DISCRIMINATOR_INDEX=0,
               COMMAND_CODE_INDEX=1
               };

   enum {INITIAL_RECEIVE=0,
               INITIAL_STATE=1,
               DISCRIMINATOR_PRI=2};

   enum {DESCRIPTOR_SIZE=10};

//PRI COMMANDS
   enum {PRI_COMMAND_REQUEST_TO_OCCUPATE_CODE=1,
                PRI_COMMAND_REQUEST_TO_FREE_CODE=2,
                PRI_COMMAND_ANSWER_TO_OCCUPATE_CODE=11,
                PRI_COMMAND_ANSWER_TO_FREE_CODE=12,
                PRI_COMMAND_ERROR_IN_OCCUPATION=101,
                PRI_COMMAND_ERROR_IN_FREE=102,
                PRI_UNKNOWN_COMMAND=201,
                PRI_BAD_COMMAND=202};

    struct  call_parameters {
          pthread_t thread_id;
          byte upo_number;
          byte ie1_number;
          byte e1_number;
          byte channel_interval;
          word channelb_number;
    };


  typedef   hash_map<pthread_t, call_parameters > pthreads_infos_container;


   mock_generator::mock_generator() throw (generator_exception);

    vector<byte> generate_pri_command_request_to_occupate(
                                                                 pthread_t pthread_id,
                                                                 bool only_that_channel);

     vector<byte> generate_pri_command_request_to_free (call_parameters parameter_of_call);


// splitting and merging per bytes -  very dangerous templates using
	template<typename T>
	static vector<byte> bytes_of_type (T value_to_splitting) {
		vector<byte> ret_val(sizeof(T));
		for (vector<byte>::size_type i=0; i<ret_val.size();i++){
			ret_val[i]=((value_to_splitting & (0x00FF<<i*8))>>i*8);
		};
		return  ret_val;
   };

	template<typename T>
	static T type_from_bytes (vector<byte> bytes_to_merging){
			T ret_val=0;
			vector<byte>::size_type count=__min(bytes_to_merging.size(), sizeof(T));
			for (vector<byte>::size_type i=0; i<count; i++){
				ret_val |=(bytes_to_merging[i] << (i*8) );
            };

            return  ret_val;
    };

    static mock_generator* mock_generator_instance;

 	uint64_t request_cycle_begin;
 	uint64_t request_cycle_end;

 	vector<uint64_t> sample_requests_to_occupate_times;
 	vector<uint64_t> sample_requests_to_free_times;   
 	vector<uint64_t> sample_error_to_occupation_times;   
 	vector<uint64_t> sample_error_to_free_times;   
    pthreads_infos_container  pthreads_info;

    static mock_generator* get_instance();
    static double calculate_sample_mean(vector<uint64_t> sample) throw (generator_exception);
    static double calculate_standart_deviation(vector<uint64_t> sample) throw (generator_exception);

public :

 static void run(int requests_count) throw (generator_exception);
 static void  html_format_results_outputter() throw (generator_exception);


 static int channel_id;
 static byte upo_count;
 static byte ie1_count;
 static byte e1_count;
 static byte channelb_count;
 static word max_channel_number;
};
