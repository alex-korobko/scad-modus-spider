#ifndef _EXTREAMDB_DATABASE_WRAPPER_
#define _EXTREAMDB_DATABASE_WRAPPER_

class database_wrapper {
  friend class friend_class_of_database_wrapper_only_for_compiler_happening;
   private:
      database_wrapper() throw (objects_storage_exception);
      database_wrapper(const database_wrapper&){};
      database_wrapper& operator=(const database_wrapper& dbase_inst){ return const_cast<database_wrapper&>(dbase_inst);};
      ~database_wrapper() {};
// add specific for extreame db variables here
     int counter;
     static int upo_count, ie1_count, e1_count, channelb_count, max_channel_number;
    public:

// call control metods
bytes call_control_occupate_channel_side_a_only(byte upo_number, 
                                                byte ie1_number,
                                                byte e1_number,
                                                byte channel_interval,
                                                word channelb_number,
                                                word source_uid,
                                                word source_tid);

bytes call_control_occupate_channel_side_a_any(byte upo_number, 
                                                byte ie1_number,
                                                byte e1_number,
                                                byte channel_interval,
                                                word channelb_number,
                                                word source_uid,
                                                word source_tid);

bytes call_control_occupate_channel_side_b(int destination,
                                                byte mg,
                                                byte priority,
                                                byte isdn,
                                                byte channels_count,
                                                word source_uid,
                                                word source_tid);

bytes call_control_occupate_call_pointer(byte upo_number, 
                                                byte ie1_number,
                                                byte e1_number,
                                                byte interval);

bool call_control_free_channel(byte upo_number, 
                                     byte ie1_number,
                                     byte e1_number,
                                     byte interval);

bool call_control_free_call_pointer(byte upo_number, 
                                     byte ie1_number,
                                     byte e1_number,
                                     byte interval,
                                     word call_pointer);

//proc control metods
bytes  proc_control_check_state_b(byte upo_number,
                                     byte ie1_number,
                                     byte e1_number,
                                     byte interval );

//static metods
       static database_wrapper& get_instance() throw (objects_storage_exception);
};

#endif
