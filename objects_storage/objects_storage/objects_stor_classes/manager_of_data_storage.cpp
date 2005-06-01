using namespace std;

#include <functional>
#include <map>
#include <vector>
#include <sstream>

#include <sys/neutrino.h>
#include <inttypes.h>
#include <process.h>
#include <string.h>
#include <errno.h>

#include <iostream.h>

#include "storage_common.h"
#include "dbwrapper_definition.h"
#include "objects_storage_exception.h"
#include "objects_storage_logger.h"

#ifdef _MOCK_DBWRAPPER_
#include "mock_database_wrapper.h"
#else //#ifdef _MOCK_DBWRAPPER_
#ifdef _LINTER_DBWRAPPER_
#include <linapi.h>
#include "linter_database_wrapper.h"
#endif  //#ifdef _LINTER_DBWRAPPER_
#endif  //#ifdef _MOCK_DBWRAPPER_


#include "abstract_job_data_manager.h"
#include "jobs_data_managers_container.h"

#include "pri_data_manager.h"

#include "manager_of_data_storage.h"

manager_of_data_storage* manager_of_data_storage::mngr_instance=NULL;
uint32_t manager_of_data_storage::node_id=0;
pid_t manager_of_data_storage::process_id=0;
int manager_of_data_storage::channel_id=0;

byte manager_of_data_storage::descriptor_call_pointer_num=0;
byte manager_of_data_storage::descriptor_call_pointer_first=0;
byte manager_of_data_storage::descriptior_call_pointer_second=0;

byte manager_of_data_storage::descriptor_channeld_upo_num=0;
byte manager_of_data_storage::descriptor_channeld_ie1_num=0;
byte manager_of_data_storage::descriptor_channeld_e1_num=0;
byte manager_of_data_storage::descriptor_channeld_channel_num=0;

manager_of_data_storage* 
          manager_of_data_storage::get_instance(){
       if (mngr_instance==NULL) 
               mngr_instance=new manager_of_data_storage();

      return mngr_instance;
};

void manager_of_data_storage::initialize() throw (objects_storage_exception){
manager_of_data_storage*  mngr_instance=manager_of_data_storage::get_instance();

mngr_instance->jobs_data_managers.insert(
                                  jobs_data_managers_container::value_type (
                                                        data_managers_types::PRI_DATA_MANAGER,
                                                        pri_data_manager::get_instance()
                                                        )
                                         );

// add new managers here

};

void manager_of_data_storage::run() throw (objects_storage_exception){
manager_of_data_storage*  mngr_instance=manager_of_data_storage::get_instance();
jobs_data_managers_container::iterator current_data_manager_iter;
int connection_id, ret_val, failures_count ;
vector<byte> message_to_send, message_to_receive;

connection_id=ConnectAttach( manager_of_data_storage::node_id,
                                manager_of_data_storage::process_id,
                                manager_of_data_storage::channel_id,
                                0,
                                0);

if (connection_id ==-1) {
       ostringstream exception_description;
       exception_description<<"ConnectAttach(...) to "
                                            <<"node id "<<manager_of_data_storage::node_id<<" "
                                            <<"process id "<<manager_of_data_storage::process_id<<" "
                                            <<"channel id "<<manager_of_data_storage::channel_id<<" "
                                            <<"error : "<<strerror(errno);
      throw objects_storage_exception(exception_description.str());
 };

message_to_send.resize(program_settings::SEND_RECEIVE_BUFFER_SIZE);
message_to_receive.resize(program_settings::SEND_RECEIVE_BUFFER_SIZE);

message_to_receive.assign(message_to_receive.size(),0);

message_to_send[0]=channel_initialization::INIT_REG_NEW_PID;
message_to_send[1]=channel_initialization::INIT_SEND_TO_MASTER;

ret_val= MsgSend( connection_id,
             &message_to_send[0],
//             message_to_send.size(),
             channel_initialization::INIT_MESSAGE_SIZE,
             &message_to_receive[0],
             message_to_receive.size());

if (ret_val ==-1) {
       ostringstream exception_description;
       exception_description<<"manager_of_data_storage::run()  MsgSend(...) error : "<<strerror(errno);
       throw objects_storage_exception(exception_description.str());
   };

failures_count=0;
message_to_send.clear();
 while (true) {
      try {
       message_to_send.insert (message_to_send.begin(),
                                                 descriptor_header::DESCRIPTOR_SIZE,
                                                 0);

       message_to_send[descriptor_header::OFFSET_CHANNEL_D_UPO_NUM]=
                         manager_of_data_storage::descriptor_channeld_upo_num;
       message_to_send[descriptor_header::OFFSET_CHANNEL_D_IE1_NUM]=
                         manager_of_data_storage::descriptor_channeld_ie1_num;
       message_to_send[descriptor_header::OFFSET_CHANNEL_D_E1_NUM]=
                         manager_of_data_storage::descriptor_channeld_e1_num;
       message_to_send[descriptor_header::OFFSET_CHANNEL_D_CHAN_NUM]=
                        manager_of_data_storage::descriptor_channeld_channel_num;

       message_to_send[descriptor_header::OFFSET_CALL_POINTER_NUM]=
                     manager_of_data_storage::descriptor_call_pointer_num;
       message_to_send[descriptor_header::OFFSET_CALL_POINTER_FIRST]=
                     manager_of_data_storage::descriptor_call_pointer_first;
       message_to_send[descriptor_header::OFFSET_CALL_POINTER_SECOND]=
                     manager_of_data_storage::descriptior_call_pointer_second;

      message_to_receive.resize(program_settings::SEND_RECEIVE_BUFFER_SIZE);
//=============delete it================================
/*
   if (true){ostringstream message;
                  vector<char> tmp_chars(32);
                  itoa(message_to_send.size(), &tmp_chars[0], 16);
                  message<<" send size "<<&tmp_chars[0];
                  for (vector<char>::size_type i=0; i<message_to_send.size();i++) {
                        itoa(message_to_send[i], &tmp_chars[0], 16);
                        message<<" 0x"<<&tmp_chars[0];
                 };

                 objects_storage_logger* logger_inst=
                              objects_storage_logger::get_instance();
                 logger_inst->log_message
                                   (objects_storage_logger::INFO_MSG,
                                     message.str());
} 
*/
//===================================================
      ret_val= MsgSend( connection_id,
                        &message_to_send[0],
                        message_to_send.size(),
                        &message_to_receive[0],
                        message_to_receive.size());

       if (ret_val ==-1)  {
             string message("manager_of_data_storage::run() MsgSend(...) error : ");
             message+=strerror(errno);
             failures_count++;
             throw objects_storage_exception(message);
        } ;

//==============delete it=====================
/*
if (true){    ostringstream message;
                   vector<char> tmp_chars(32);
                   itoa(message_to_receive.size(), &tmp_chars[0], 16);
                   message<<" recieve size "<<&tmp_chars[0];
                  for (vector<char>::size_type i=0; i<27; i++) {
                        itoa(message_to_receive[i], &tmp_chars[0], 16);
                        message<<" 0x"<<&tmp_chars[0];
                   };

                 objects_storage_logger* logger_inst=
                              objects_storage_logger::get_instance();

                 logger_inst->log_message
                                   (objects_storage_logger::INFO_MSG,
                                     message.str());
} 
*/
//=========================================

          failures_count=0;
      if (message_to_receive.empty() ||
             message_to_receive.size()<descriptor_header::DESCRIPTOR_SIZE) {
              ostringstream exception_description;
              exception_description<<"manager_of_data_storage::run()  message_to_receive is empty or less than descriptor_header::DESCRIPTOR_SIZE";
              throw objects_storage_exception(exception_description.str());
       };

       manager_of_data_storage::descriptor_channeld_upo_num=
                          message_to_receive[descriptor_header::OFFSET_CHANNEL_D_UPO_NUM];
       manager_of_data_storage::descriptor_channeld_ie1_num=
                          message_to_receive[descriptor_header::OFFSET_CHANNEL_D_IE1_NUM];
       manager_of_data_storage::descriptor_channeld_e1_num=
                          message_to_receive[descriptor_header::OFFSET_CHANNEL_D_E1_NUM];
       manager_of_data_storage::descriptor_channeld_channel_num=
                          message_to_receive[descriptor_header::OFFSET_CHANNEL_D_CHAN_NUM];

       manager_of_data_storage::descriptor_call_pointer_num=
                          message_to_receive[descriptor_header::OFFSET_CALL_POINTER_NUM];
      manager_of_data_storage::descriptor_call_pointer_first=
                message_to_receive[descriptor_header::OFFSET_CALL_POINTER_FIRST];
      manager_of_data_storage::descriptior_call_pointer_second=
                message_to_receive[descriptor_header::OFFSET_CALL_POINTER_SECOND];

       vector<byte>::iterator msg_rec_iter=message_to_receive.begin();
       advance(msg_rec_iter, descriptor_header::DESCRIPTOR_SIZE);
       message_to_receive.erase (message_to_receive.begin(),
                                                   msg_rec_iter);

       current_data_manager_iter=
                       mngr_instance->jobs_data_managers.find(message_to_receive[0]);
       if (current_data_manager_iter==
                                     mngr_instance->jobs_data_managers.end()){
             //what can i do??
              message_to_send=message_to_receive;
              message_to_send[1]=common_commands::COMMON_UNKNOWN_COMMAND;

              ostringstream exception_description;
              exception_description<<"manager_of_data_storage::run()  not found data_manager for descriptor "
                                            <<static_cast<int>(message_to_receive[0]);
              throw objects_storage_exception(exception_description.str());
          }; // if (current_data_manager_iter=jobs_data_managers.end())
             message_to_send=
                  current_data_manager_iter->second->process_command(message_to_receive);
        } catch(objects_storage_exception obj_stor_exc) {
              message_to_send.clear();
              string message("In manager_of_data_storage::run() catched exception :");
              objects_storage_logger* logger_inst=
                              objects_storage_logger::get_instance();
             message+=obj_stor_exc.get_description();
              logger_inst->log_message
                                   (objects_storage_logger::ERROR_MSG,
                                     message);
        }; //catch(objects_storage_exception obj_stor_exc)

      if (failures_count>program_settings::MAX_FAILURES_COUNT)
                                                                     return;

  }; //while (true)

};

