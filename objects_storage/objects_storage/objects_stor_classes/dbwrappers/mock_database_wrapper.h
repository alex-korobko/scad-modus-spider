#ifndef _MOCK_DATABASE_WRAPPER_
#define _MOCK_DATABASE_WRAPPER_

class database_wrapper {
   private:
       database_wrapper() throw (objects_storage_exception);
       static database_wrapper* dbwrapper_instance;

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
       static database_wrapper* get_instance();
};

#endif
