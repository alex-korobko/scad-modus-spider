#ifndef _MOCK_DATABASE_WRAPPER_
#define _MOCK_DATABASE_WRAPPER_

class database_wrapper {
  friend class friend_class_of_database_wrapper_only_for_compiler_happening;
   private:
      database_wrapper() throw (objects_storage_exception);
      database_wrapper(const database_wrapper&){};
      database_wrapper& operator=(const database_wrapper& dbase_inst){ return const_cast<database_wrapper&>(dbase_inst);};
      ~database_wrapper() {};

     int counter;

    public:

// PRI SUPPORT METODS
vector<byte> pri_occupate_channel_only(byte upo_number, 
                                                byte ie1_number,
                                                byte e1_number,
                                                byte channel_interval,
                                                word channelb_number,
                                                pthread_t pthread_id);

vector<byte> pri_occupate_channel_any(byte upo_number, 
                                                byte ie1_number,
                                                byte e1_number,
                                                byte channel_interval,
                                                word channelb_number,
                                                pthread_t pthread_id);

bool pri_free_channel(byte upo_number, 
                                     byte ie1_number,
                                     byte e1_number,
                                     byte channel_interval,
                                     word channelb_number,
                                     pthread_t  pthread_id);
  
//static metods
       static database_wrapper& get_instance() throw (objects_storage_exception);
};

#endif
