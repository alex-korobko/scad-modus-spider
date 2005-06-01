#ifndef _LINTER_DATABASE_WRAPPER_
#define _LINTER_DATABASE_WRAPPER_

class database_wrapper {
   private:
       database_wrapper() throw (objects_storage_exception);

       WORD linter_connection_id;

       WORD linter_cursor_id;

       WORD call_control_channel_occupation_statement_1;
       WORD call_control_channel_occupation_statement_2;
       WORD call_control_channel_occupation_statement_3;
       WORD call_control_channel_occupation_statement_4;
       WORD call_control_channel_occupation_statement_5;
       WORD call_control_channel_occupation_statement_6;
       WORD call_control_channel_free_statement_1;

       struct occupation_answer  {
              LONG channel_id;
              LONG channel_right_side;
              LONG channel_interval;
              LONG channel_number; 
       };

       struct needed_id  {
              LONG channel_id;
       };


       vector<occupation_answer> occupation_answer_data;
       vector<needed_id> occupation_needed_ids;

       pthread_t tid_param;
       LONG channel_id_param,
                  channel_number_param, 
                  channel_interval_param,
                  left_side_param,
                  right_side_param,
                  size_of_param;

       static database_wrapper* dbwrapper_instance;

       static vector<byte> prepare_channel_attributes_by_right_side(LONG channel_right_side,
                                                                                                                word channel_number,
                                                                                                                pthread_t pthread_id);
       static string linter_error_decoding (LONG return_code,
                                                                 WORD connection_id,
                                                                 WORD cursor_id,
                                                                WORD statement_id);
    public:

// PRI SUPPORT METODS
vector<byte> call_control_occupate_channel_only(byte upo_number, 
                                                byte ie1_number,
                                                byte e1_number,
                                                byte channel_interval,
                                                word channelb_number,
                                                pthread_t pthread_id);

vector<byte> call_control_occupate_channel_any(byte upo_number, 
                                                byte ie1_number,
                                                byte e1_number,
                                                byte channel_interval,
                                               word channelb_number,
                                                pthread_t pthread_id);

bool call_control_free_channel(byte upo_number, 
                                     byte ie1_number,
                                     byte e1_number,
                                     byte channel_interval,
                                     word channelb_number,
                                     pthread_t  pthread_id);
  
//static metods
       static database_wrapper* get_instance();
};

#endif
