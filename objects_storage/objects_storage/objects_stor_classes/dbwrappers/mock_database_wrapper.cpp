#include <functional>
#include <vector>
#include <sstream>

#include <time.h>
#include <inttypes.h>

using namespace std;

#include "storage_common.h"
#include "objects_storage_exception.h"
#include "objects_storage_logger.h"

#include "mock_database_wrapper.h"

static char state_random[32];
int database_wrapper::upo_count=4;
int database_wrapper::ie1_count=16;
int database_wrapper::e1_count=16;
int database_wrapper::channelb_count=32;
int database_wrapper::max_channel_number=4096;


database_wrapper& 
          database_wrapper::get_instance() throw (objects_storage_exception){
    static database_wrapper dbwrapper_instance;
    return dbwrapper_instance;
};

database_wrapper::
              database_wrapper() throw (objects_storage_exception) :
          counter(0){
   initstate( time(NULL), state_random, sizeof(state_random));
   setstate(state_random);
};

// call control metods
bytes database_wrapper::call_control_occupate_channel_side_a_only(byte upo_number, 
                                                byte ie1_number,
                                                byte e1_number,
                                                byte channel_interval,
                                                word channelb_number,
                                                word source_uid,
                                                word source_tid){
/*
Канал B занят успешно:
номер байта       |    содержимое
----------------------|---------------------
	0				   |		11      код ответа
	1                     |      UPO   координата канала В
	2                     |       IE1    координата канала В
	3                     |       IE      координата канала В
	4				   |       KI      координата канала В

if error return empty buffer
*/
bytes ret_buffer;
byte upo, ie1, e1, ki;

//========= delete it =====================
{
        objects_storage_logger& logger_inst=objects_storage_logger::get_instance();
        ostringstream message;
        message<<"call_control_occupate_channel_side_a_only "
                       <<" upo_number "<<static_cast<int>(upo_number)
                       <<" ie1_number "<<static_cast<int>(ie1)
                       <<" e1_number "<<static_cast<int>(e1)
                       <<" channel_interval "<<static_cast<int>(channel_interval)
                       <<" channelb_number "<<channelb_number
                       <<" source_uid "<<source_uid
                       <<" source_tid "<<source_tid;
        logger_inst.log_message
                   (objects_storage_logger::ERROR_MSG,
                    message.str());
}
//====================================

if (counter>2048) counter=0;
if ((counter++%7)==0) return ret_buffer;

upo=random();
if (upo>=upo_count) upo=upo%upo_count;
ie1=random();
if (ie1>=ie1_count) ie1=ie1%ie1_count;
e1=random();
if (e1>=e1_count) e1=e1%e1_count;
ki=random();
if (ki>=channelb_count) ki=ki%channelb_count;

ret_buffer.push_back(upo);
ret_buffer.push_back(ie1);
ret_buffer.push_back(e1);
ret_buffer.push_back(ki);
return ret_buffer;
};

bytes database_wrapper::call_control_occupate_channel_side_a_any(byte upo_number, 
                                                byte ie1_number,
                                                byte e1_number,
                                                byte channel_interval,
                                                word channelb_number,
                                                word source_uid,
                                                word source_tid){
/*
Канал B занят успешно:
номер байта       |    содержимое
----------------------|---------------------
	0				   |		11      код ответа
	1                     |      UPO   координата канала В
	2                     |       IE1    координата канала В
	3                     |       IE      координата канала В
	4				   |       KI      координата канала В

if error return empty buffer
*/
bytes ret_buffer;
byte upo, ie1, e1, ki;

if (counter>2048) counter=0;
if ((counter++%7)==0) return ret_buffer;

upo=random();
if (upo>=upo_count) upo=upo%upo_count;
ie1=random();
if (ie1>=ie1_count) ie1=ie1%ie1_count;
e1=random();
if (e1>=e1_count) e1=e1%e1_count;
ki=random();
if (ki>=channelb_count) ki=ki%channelb_count;

ret_buffer.push_back(upo);
ret_buffer.push_back(ie1);
ret_buffer.push_back(e1);
ret_buffer.push_back(ki);
return ret_buffer;
};

bytes database_wrapper::call_control_occupate_channel_side_b(int destination,
                                                byte mg,
                                                byte priority,
                                                byte isdn,
                                                byte channels_count,
                                                word source_uid,
                                                word source_tid) {
/*
Канал B занят успешно:
номер байта       |    содержимое
----------------------|---------------------
	0				   |		11      код ответа
	1                     |      UPO   координата канала В
	2                     |       IE1    координата канала В
	3                     |       IE      координата канала В
	4				   |       KI      координата канала В

if error return empty buffer
*/

bytes ret_buffer;
byte upo, ie1, e1, ki, channel_number;

if (counter>2048) counter=0;
if ((counter++%7)==0) return ret_buffer;

upo=random();
if (upo>=upo_count) upo=upo%upo_count;
ie1=random();
if (ie1>=ie1_count) ie1=ie1%ie1_count;
e1=random();
if (e1>=e1_count) e1=e1%e1_count;
ki=random();
if (ki>=channelb_count) ki=ki%channelb_count;

channel_number=random();

ret_buffer.push_back(upo);
ret_buffer.push_back(ie1);
ret_buffer.push_back(e1);
ret_buffer.push_back(ki);
ret_buffer.push_back(channel_number);
ret_buffer.push_back(0);

//OKS
ret_buffer.push_back(0);

ret_buffer.push_back(0);
ret_buffer.push_back(0);
ret_buffer.push_back(1);
ret_buffer.push_back(17);

return ret_buffer;
};

bytes database_wrapper::call_control_occupate_call_pointer(byte upo_number, 
                                                byte ie1_number,
                                                byte e1_number,
                                                byte interval){
/*
Указатель вызова занят:
номер байта       |    содержимое
----------------------|---------------------
	0				   |		17      код ответа
	1                     |      CP    указатель вызова мл.
	2                     ¦      CP    указатель вызова ст.
if error return empty buffer
*/
bytes ret_buffer;
byte call_pointer_id;
call_pointer_id=random();
ret_buffer.push_back(call_pointer_id);
ret_buffer.push_back(0);
return ret_buffer;
};

bool database_wrapper::call_control_free_channel(byte upo_number, 
                                     byte ie1_number,
                                     byte e1_number,
                                     byte interval){
if (counter>2048) counter=0;
if ((counter++%7)==0) return false;
return true;
};

bool database_wrapper::call_control_free_call_pointer(byte upo_number, 
                                     byte ie1_number,
                                     byte e1_number,
                                     byte interval,
                                     word call_pointer){
if (counter>2048) counter=0;
if ((counter++%7)==0) return false;
return true;
};

//proc control metods
bytes  database_wrapper::proc_control_check_state_b(byte upo_number,
                                     byte ie1_number,
                                     byte e1_number,
                                     byte interval ){
/*
unknown command don`t touch taht 
*/
bytes ret_buffer;
if (counter>2048) counter=0;
if ((counter++%7)==0) return ret_buffer;

ret_buffer.push_back(0);
return ret_buffer;
};


