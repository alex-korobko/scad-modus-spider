#ifndef _EXTREAMDB_DATABASE_WRAPPER_
#define _EXTREAMDB_DATABASE_WRAPPER_
#include <kgtsdb.h>

class database_wrapper {
  friend class friend_class_of_database_wrapper_only_for_compiler_happening;
   private:
      
     bool  find_channel_d_for_channel_b_any (  mco_trans_h t, uint4 id_channel_b, /*OUT*/ uint2* channel_b_number, /*OUT*/ CHANNEL_B* channel_b_id_for_d   )  ;
     bool  find_channel_d_for_channel_b_only (  mco_trans_h t, uint4 id_channel_b,  uint2 required_channel_b_number, /*OUT*/ CHANNEL_B* channel_b_id_for_d   )  ;
     bool  find_channel_d_for_channel_b (  mco_trans_h t, uint4 id_channel_b, uint2 required_channel_b,  /*OUT*/ uint2* channel_b_number, /*OUT*/ CHANNEL_B* channel_b_id_for_d, bool only   ) ;

      bool find_channel_b_id_by_coordinates(  mco_trans_h t, uint1 upo_number, uint1 ie1_number,  uint1 e1_number, uint1 interval, /*OUT*/ uint4 * result_channel_b_id  )  ;

   	 bytes get_channel_b_coordinates( uint4 id ); 
      database_wrapper() throw (objects_storage_exception);
      database_wrapper(const database_wrapper&){};
      database_wrapper& operator=(const database_wrapper& dbase_inst){ return const_cast<database_wrapper&>(dbase_inst);};
      ~database_wrapper() ;
// add specific for extreame db variables here
	mco_db_h        db;
     int counter;
     static int upo_count, ie1_count, e1_count, channelb_count, max_channel_number;
    public:

//common functions

int get_counter_value() {return counter;}

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
