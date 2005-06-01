using namespace std;

#include <functional>
#include <vector>
#include <sstream>

#include <pthread.h>
#include <inttypes.h>

#include "storage_common.h"
#include "objects_storage_exception.h"
#include "objects_storage_logger.h"


#include "mock_database_wrapper.h"

database_wrapper* database_wrapper::dbwrapper_instance=NULL;

database_wrapper* 
          database_wrapper::get_instance(){
     if (dbwrapper_instance==NULL) {
           try {
                dbwrapper_instance=new database_wrapper();
            } catch (objects_storage_exception obj_stor_exc) {
                   string message("In database_wrapper::get_instance() raised exception :");
                   objects_storage_logger* logger_inst=
                                  objects_storage_logger::get_instance();
                  message+=obj_stor_exc.get_description();
                  logger_inst->log_message
                                   (objects_storage_logger::ERROR_MSG,
                                     message);
            };
        };
 
    return dbwrapper_instance;
};

database_wrapper::
              database_wrapper() throw (objects_storage_exception){
vector<char> random_state(32);
initstate( time(NULL), 
               &random_state[0],
              random_state.size());
setstate(&random_state[0]);
};

// PRI SUPPORT METODS
vector<byte>
       database_wrapper::pri_occupate_channel_only(byte upo_number, 
                                                byte ie1_number,
                                                byte e1_number,
                                                byte channel_interval,
                                                word channelb_number,
                                                pthread_t pthread_id) {
vector<byte> return_buffer(0);
 if (random()%2==0) {

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

}; // if (random()%2==0) {
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

if (random()%2==0) {

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

}; // if (random()%2==0) {

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
/*
                    objects_storage_logger* logger_inst=
                                                   objects_storage_logger::get_instance();
                    vector<char> tmp_chars(32);
                    ostringstream message;

                   message<<"Free: ";

                    itoa(pthread_id, &tmp_chars[0], 16);
                     message<<" tid 0x"<<&tmp_chars[0];

                    itoa(upo_number, &tmp_chars[0], 16);
                     message<<" upo 0x"<<&tmp_chars[0];

                    itoa(ie1_number, &tmp_chars[0], 16);
                     message<<" ie1 0x"<<&tmp_chars[0];

                    itoa(e1_number, &tmp_chars[0], 16);
                     message<<" e1 0x"<<&tmp_chars[0];

                    itoa(channel_interval, &tmp_chars[0], 16);
                     message<<" interv 0x"<<&tmp_chars[0];

                    itoa(channelb_number, &tmp_chars[0], 16);
                     message<<" ch_num 0x"<<&tmp_chars[0];

                    logger_inst->log_message
                                               (objects_storage_logger::INFO_MSG,
                                                 message.str());
*/
//=================================================


if (random()%2==0) { return true; } else {return false; };
};


