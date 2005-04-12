#include <functional>
#include <vector>
#include <sstream>

#include <pthread.h>
#include <inttypes.h>

using namespace std;

#include "storage_common.h"
#include "objects_storage_exception.h"
#include "objects_storage_logger.h"


#include "mock_database_wrapper.h"

database_wrapper& 
          database_wrapper::get_instance() throw (objects_storage_exception){
    static database_wrapper dbwrapper_instance;
    return dbwrapper_instance;
};

database_wrapper::
              database_wrapper() throw (objects_storage_exception) :
          counter(0){};

// PRI SUPPORT METODS
vector<byte>
       database_wrapper::pri_occupate_channel_only(byte upo_number, 
                                                byte ie1_number,
                                                byte e1_number,
                                                byte channel_interval,
                                                word channelb_number,
                                                pthread_t pthread_id) {
vector<byte> return_buffer(0);
 if (counter%2==0) {
    vector<byte> type_bytes;

    type_bytes=bytes_of_type<pthread_t>(pthread_id);
    return_buffer.insert(return_buffer.end(),
                                      type_bytes.rbegin(),
                                      type_bytes.rend() );

    return_buffer.push_back(upo_number);
    return_buffer.push_back(ie1_number);
    return_buffer.push_back(e1_number);
    return_buffer.push_back(channel_interval);


    type_bytes=bytes_of_type<word>(channelb_number);
    return_buffer.insert(return_buffer.end(),
                                      type_bytes.rbegin(),
                                      type_bytes.rend() );

}; // if (counter%2==0)
    counter++;
    if (counter>100)  counter=0;

return return_buffer;

};

vector<byte> 
         database_wrapper::pri_occupate_channel_any(byte upo_number, 
                                                byte ie1_number,
                                                byte e1_number,
                                                byte channel_interval,
                                                word channelb_number,
                                                pthread_t pthread_id) {
vector<byte> return_buffer(0);
word new_channelb_number;

 if (counter%2==0) {

    vector<byte> type_bytes;

    type_bytes=bytes_of_type<pthread_t>(pthread_id);
    return_buffer.insert(return_buffer.end(),
                                      type_bytes.rbegin(),
                                      type_bytes.rend() );

    return_buffer.push_back(upo_number);
    return_buffer.push_back(ie1_number);
    return_buffer.push_back(e1_number);
    return_buffer.push_back(channel_interval);

    new_channelb_number=random()%32; //nowhere greater than 32
    if (new_channelb_number==0)  new_channelb_number=1;

    type_bytes=bytes_of_type<word>(channelb_number);
    return_buffer.insert(return_buffer.end(),
                                      type_bytes.rbegin(),
                                      type_bytes.rend() );

}; //  if (counter%2==0)

    counter++;
    if (counter>100)  counter=0;

return return_buffer;
};

bool 
        database_wrapper::pri_free_channel(byte upo_number, 
                                     byte ie1_number,
                                     byte e1_number,
                                     byte channel_interval,
                                     word channelb_number,
                                     pthread_t  pthread_id){

//================delete it==========================
//                    objects_storage_logger& logger_inst=
//                                                   objects_storage_logger::get_instance();
//                    vector<char> tmp_chars(32);
//                    ostringstream message;
//
//                   message<<"Free: ";
//
//                    itoa(pthread_id, &tmp_chars[0], 16);
//                     message<<" tid 0x"<<&tmp_chars[0];
//
//                    itoa(upo_number, &tmp_chars[0], 16);
//                     message<<" upo 0x"<<&tmp_chars[0];
//
//                    itoa(ie1_number, &tmp_chars[0], 16);
//                     message<<" ie1 0x"<<&tmp_chars[0];
//
//                    itoa(e1_number, &tmp_chars[0], 16);
//                     message<<" e1 0x"<<&tmp_chars[0];
//
//                    itoa(channel_interval, &tmp_chars[0], 16);
//                     message<<" interv 0x"<<&tmp_chars[0];
//
//                    itoa(channelb_number, &tmp_chars[0], 16);
//                     message<<" ch_num 0x"<<&tmp_chars[0];
//
//                    logger_inst.log_message
//                                             (objects_storage_logger::INFO_MSG,
//                                                 message.str());
//=================================================

if (counter%2==0) { return true; } else {return false; };

    counter++;
    if (counter>100)  counter=0;

};


