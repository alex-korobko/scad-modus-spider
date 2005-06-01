#include <functional>
#include <map>
#include <vector>
#include <sstream>

#include <sys/neutrino.h>
#include <inttypes.h>
#include <process.h>
#include <string.h>
#include <errno.h>

#include <ostream>

using namespace std;

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
#else
#ifdef _EXTREAME_DB_DBWRAPPER_
#include "extreamedb_database_wrapper.h"
#endif  //#ifdef _EXTREAME_DB_DBWRAPPER_
#endif  //#ifdef _LINTER_DBWRAPPER_
#endif  //#ifdef _MOCK_DBWRAPPER_

#include "call_control_data_manager.h"
#include "proc_control_data_manager.h"
#include "manager_of_data_storage.h"

uint32_t manager_of_data_storage::node_id=0;
pid_t manager_of_data_storage::process_id=0;
int manager_of_data_storage::channel_id=0;

byte manager_of_data_storage::message_type=0;
byte manager_of_data_storage::message_description=0;
word manager_of_data_storage::message_destination_uid=0;
word manager_of_data_storage::message_destination_tid=0;
word manager_of_data_storage::message_source_uid=0;
word manager_of_data_storage::message_source_tid=0;
byte manager_of_data_storage::message_class=0;
byte manager_of_data_storage::message_reserved_value=0;



manager_of_data_storage&
          manager_of_data_storage::get_instance(){
      static manager_of_data_storage mngr_instance;
      return mngr_instance;
};

void manager_of_data_storage::run() throw (objects_storage_exception){
int connection_id, ret_val, failures_count ;
bytes message_to_send, message_to_receive;
bytes tmp_buffer;
word tmp_word_value;
bytes::iterator msg_rec_iter;
bytes::iterator msg_rec_iter1, msg_rec_iter2;

message_to_send.reserve(program_settings::SEND_RECIEVE_BUFFER_SIZE);
message_to_receive.reserve(program_settings::SEND_RECIEVE_BUFFER_SIZE);

database_wrapper&  dbwrapper=database_wrapper::get_instance();

//only for compiler happening
connection_id=dbwrapper.get_counter_value();

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

message_to_receive.resize(program_settings::SEND_RECIEVE_BUFFER_SIZE);

//initial message descriptor 1 - new pid registration preparing
message_to_send.clear();
message_to_send.push_back(message_header_type::SERVICE_QUERY);
message_to_send.push_back(message_header_description::PID_REGISTRATION);

message_to_send.push_back(message_header_uid::UID_UNKNOWN); //low byte destination uid
message_to_send.push_back(0); //high byte destination uid

message_to_send.push_back(0); //low byte destination tid
message_to_send.push_back(0); //high byte destination tid

message_to_send.push_back(message_header_uid::UID_DEPOSITORY); //low byte source uid
message_to_send.push_back(0); //high byte source uid

message_to_send.push_back(1); //low byte source tid
message_to_send.push_back(0); //high byte source tid

message_to_send.push_back(0); //message class
message_to_send.push_back(0); //reserved

////=============delete it================================
/*{
                  ostringstream message;
                  vector<char> tmp_chars(32);
                  itoa(message_to_send.size(), &tmp_chars[0], 16);
                  message<<" send REQUEST 1 size 0x"<<&tmp_chars[0]<<" message ";
                 for (vector<char>::size_type i=0; i<message_to_send.size();i++) {
                        itoa(message_to_send[i], &tmp_chars[0], 16);
                        message<<" 0x"<<&tmp_chars[0];
                 };

                 objects_storage_logger& logger_inst=
                              objects_storage_logger::get_instance();
                 logger_inst.log_message
                                  (objects_storage_logger::INFO_MSG,
                                     message.str());
}
*/
////===================================================

ret_val= MsgSend( connection_id,
             &message_to_send[0],
             message_to_send.size(),
             &message_to_receive[0],
             message_to_receive.size());

if (ret_val ==-1) {
       ostringstream exception_description;
       exception_description<<"manager_of_data_storage::run()  MsgSend(...) error : "<<strerror(errno);
       throw objects_storage_exception(exception_description.str());
   };

//initial message descriptior 2 - new tid registration preparing
message_to_send.clear();
message_to_send.push_back(message_header_type::SERVICE_QUERY);
message_to_send.push_back(message_header_description::TID_REGISTRATION);

message_to_send.push_back(message_header_uid::UID_UNKNOWN); //low byte destination uid
message_to_send.push_back(0); //high byte destination uid

message_to_send.push_back(0); //low byte destination tid
message_to_send.push_back(0); //high byte destination tid

message_to_send.push_back(message_header_uid::UID_DEPOSITORY); //low byte source uid
message_to_send.push_back(0); //high byte source uid

message_to_send.push_back(1); //low byte source tid
message_to_send.push_back(0); //high byte source tid

message_to_send.push_back(0); //message class
message_to_send.push_back(0); //reserved
  
////=============delete it================================
/*
{
                  ostringstream message;
                  vector<char> tmp_chars(32);
                  itoa(message_to_send.size(), &tmp_chars[0], 16);
                  message<<" send REQUEST 2 size 0x"<<&tmp_chars[0]<<" message ";
                 for (vector<char>::size_type i=0; i<message_to_send.size();i++) {
                        itoa(message_to_send[i], &tmp_chars[0], 16);
                        message<<" 0x"<<&tmp_chars[0];
                 };

                 objects_storage_logger& logger_inst=
                              objects_storage_logger::get_instance();
                 logger_inst.log_message
                                  (objects_storage_logger::INFO_MSG,
                                     message.str());
}
*/
////===================================================


ret_val= MsgSend( connection_id,
             &message_to_send[0],
             message_to_send.size(),
             &message_to_receive[0],
             message_to_receive.size());

if (ret_val ==-1) {
       ostringstream exception_description;
       exception_description<<"manager_of_data_storage::run()  MsgSend(...) error : "<<strerror(errno);
       throw objects_storage_exception(exception_description.str());
   };

//prepare descriptior for request to new command
       manager_of_data_storage::message_type=message_header_type::DATA_QUERY;
       manager_of_data_storage::message_description=message_header_description::TID_REGISTRATION;//I don`t know why that field left unchanged
       manager_of_data_storage::message_destination_uid=message_header_uid::UID_UNKNOWN;
       manager_of_data_storage::message_destination_tid=0;
       manager_of_data_storage::message_source_uid=message_header_uid::UID_DEPOSITORY;
       manager_of_data_storage::message_source_tid=1;
       manager_of_data_storage::message_class=0;
       manager_of_data_storage::message_reserved_value=0;

        message_to_send.clear();

//ATTENTION!! Add new jobs data managers here
call_control_manager& call_control_manager_instance=call_control_manager::get_instance();
proc_control_manager& proc_control_manager_instance=proc_control_manager::get_instance();

 failures_count=0;
 while (true) {
      message_to_receive.resize(program_settings::SEND_RECIEVE_BUFFER_SIZE);
      try {
       message_to_send.insert(message_to_send.begin(),1, manager_of_data_storage::message_reserved_value);
       message_to_send.insert(message_to_send.begin(),1, manager_of_data_storage::message_class);
       tmp_buffer=bytes_of_type<word>(manager_of_data_storage::message_source_tid);
       message_to_send.insert(message_to_send.begin(),tmp_buffer.begin(),tmp_buffer.end());
       tmp_buffer=bytes_of_type<word>(manager_of_data_storage::message_source_uid);
       message_to_send.insert(message_to_send.begin(),tmp_buffer.begin(),tmp_buffer.end());
       tmp_buffer=bytes_of_type<word>(manager_of_data_storage::message_destination_tid);
       message_to_send.insert(message_to_send.begin(),tmp_buffer.begin(),tmp_buffer.end());
       tmp_buffer=bytes_of_type<word>(manager_of_data_storage::message_destination_uid);
       message_to_send.insert(message_to_send.begin(),tmp_buffer.begin(),tmp_buffer.end());
       message_to_send.insert(message_to_send.begin(), manager_of_data_storage::message_description);
       message_to_send.insert(message_to_send.begin(), manager_of_data_storage::message_type);

////=============delete it================================
/*
{
                  ostringstream message;
                  vector<char> tmp_chars(32);
                  itoa(message_to_send.size(), &tmp_chars[0], 16);
                  message<<" send size 0x"<<&tmp_chars[0]<<" message ";
                 for (vector<char>::size_type i=0; i<message_to_send.size();i++) {
                        itoa(message_to_send[i], &tmp_chars[0], 16);
                        message<<" 0x"<<&tmp_chars[0];
                 };

                 objects_storage_logger& logger_inst=
                              objects_storage_logger::get_instance();
                 logger_inst.log_message
                                  (objects_storage_logger::INFO_MSG,
                                     message.str());
}
*/
////===================================================

       manager_of_data_storage::message_type=0;
       manager_of_data_storage::message_description=0;
       manager_of_data_storage::message_destination_uid=0;
       manager_of_data_storage::message_destination_tid=0;
       manager_of_data_storage::message_source_uid=0;
       manager_of_data_storage::message_source_tid=0;
       manager_of_data_storage::message_class=0;
       manager_of_data_storage::message_reserved_value=0;

       ret_val= MsgSend( connection_id,
                        &message_to_send[0],
                        message_to_send.size(),
                        &message_to_receive[0],
                        message_to_receive.size());

       message_to_send.clear();

       if (ret_val ==-1)  {
             string message("manager_of_data_storage::run() MsgSend(...) error : ");
             message+=strerror(errno);
             vector<char> tmp_chars(20);
             message+=" errno code ";
             itoa(errno, &tmp_chars[0], 10);
             message+=&tmp_chars[0];
             failures_count++;
             throw objects_storage_exception(message);
        } ;

////==============delete it=====================
/*
{    
                   ostringstream message;
                   vector<char> tmp_chars(32);
                   itoa( ret_val, &tmp_chars[0], 16);
                   message<<" recieve size "<<&tmp_chars[0]<<" message ";
                  for (vector<char>::size_type i=0; static_cast<int>(i)<ret_val; i++) {
                        itoa(message_to_receive[i], &tmp_chars[0], 16);
                        message<<" 0x"<<&tmp_chars[0];
                   };
                 objects_storage_logger& logger_inst=
                              objects_storage_logger::get_instance();

                 logger_inst.log_message
                                   (objects_storage_logger::INFO_MSG,
                                     message.str());
}
*/
////=========================================
       message_to_receive.resize(ret_val);

       manager_of_data_storage::message_type=message_to_receive[message_header_bytes::INDEX_OF_MESSAGE_TYPE];
       manager_of_data_storage::message_description=message_to_receive[message_header_bytes::INDEX_OF_MESSAGE_DESCRIPTION];

       msg_rec_iter1=message_to_receive.begin();
       msg_rec_iter2=msg_rec_iter1;
       advance(msg_rec_iter1, message_header_bytes::INDEX_OF_MESSAGE_DESTINATION_UID);
       advance(msg_rec_iter2, message_header_bytes::INDEX_OF_MESSAGE_DESTINATION_UID+2);
       manager_of_data_storage::message_destination_uid=type_from_bytes<word>(bytes(msg_rec_iter1, msg_rec_iter2));

       msg_rec_iter1=message_to_receive.begin();
       msg_rec_iter2=msg_rec_iter1;
       advance(msg_rec_iter1,  message_header_bytes::INDEX_OF_MESSAGE_DESTINATION_TID);
       advance(msg_rec_iter2, message_header_bytes::INDEX_OF_MESSAGE_DESTINATION_TID+2);
       manager_of_data_storage::message_destination_tid=type_from_bytes<word>(bytes(msg_rec_iter1, msg_rec_iter2));

       msg_rec_iter1=message_to_receive.begin();
       msg_rec_iter2=msg_rec_iter1;
       advance(msg_rec_iter1, message_header_bytes::INDEX_OF_MESSAGE_SOURCE_UID);
       advance(msg_rec_iter2, message_header_bytes::INDEX_OF_MESSAGE_SOURCE_UID+2);
       manager_of_data_storage::message_source_uid=type_from_bytes<word>(bytes(msg_rec_iter1, msg_rec_iter2));

       msg_rec_iter1=message_to_receive.begin();
       msg_rec_iter2=msg_rec_iter1;
       advance(msg_rec_iter1, message_header_bytes::INDEX_OF_MESSAGE_SOURCE_TID);
       advance(msg_rec_iter2, message_header_bytes::INDEX_OF_MESSAGE_SOURCE_TID+2);
       manager_of_data_storage::message_source_tid=type_from_bytes<word>(bytes(msg_rec_iter1, msg_rec_iter2));

       manager_of_data_storage::message_class=message_to_receive[message_header_bytes::INDEX_OF_MESSAGE_CLASS];
       manager_of_data_storage::message_reserved_value=message_to_receive[message_header_bytes::INDEX_OF_RESERVED_VALUE];

       msg_rec_iter=message_to_receive.begin();
       advance(msg_rec_iter, program_settings::HEADER_SIZE_IN_BYTES);
       message_to_receive.erase (message_to_receive.begin(),
                                                     msg_rec_iter);

     if (message_to_receive.empty()) { // - received only header
                //what can i do??
                message_to_send.clear();
                message_to_send.push_back(common_commands::COMMON_UNKNOWN_COMMAND);

                ostringstream exception_description;
                exception_description<<"manager_of_data_storage::run() empty message from source uid 0x"<<hex
                                                  <<manager_of_data_storage::message_source_uid
                                                  <<" source tid 0x"
                                                  <<manager_of_data_storage::message_source_tid;
                throw objects_storage_exception(exception_description.str());
           };

       switch (manager_of_data_storage::message_source_uid)  { //which of data managers can process command
              //ATTENTION!! Add new jobs data managers here
              case (data_managers_types::CALL_CONTROL_MANAGER) : 
                  message_to_send=
                     call_control_manager_instance.process_command(message_to_receive);
                  break;
              case (data_managers_types::PROC_CONTROL_MANAGER) :
                  message_to_send=
                     proc_control_manager_instance.process_command(message_to_receive);
                  break;
              default:
                //what can i do??
                message_to_send.clear();
                message_to_send.push_back(common_commands::COMMON_UNKNOWN_COMMAND);

                ostringstream exception_description;
                exception_description<<"manager_of_data_storage::run()  not found data_manager for descriptor "
                                                    <<static_cast<int>(message_to_receive[0]);
                throw objects_storage_exception(exception_description.str());
            }; //switch (message_to_receive[0]) 

             manager_of_data_storage::message_type=message_header_type::ROUTING_QUERY;

            manager_of_data_storage::message_description&=~message_header_description_flag::DEST_TID_STATIC;
             if ((manager_of_data_storage::message_description&message_header_description_flag::SRC_TID_STATIC)!=0)
                    manager_of_data_storage::message_description|=message_header_description_flag::DEST_TID_STATIC;

             manager_of_data_storage::message_description&=~message_header_description_flag::SRC_TID_STATIC;
             manager_of_data_storage::message_description&=~message_header_description_flag::BLOCK_UID_FLAG;
             manager_of_data_storage::message_description|=message_header_description_flag::WAIT_MSG_FLAG;

             tmp_word_value=manager_of_data_storage::message_destination_uid;
             manager_of_data_storage::message_destination_uid=manager_of_data_storage::message_source_uid;
             manager_of_data_storage::message_source_uid=tmp_word_value;

             tmp_word_value=manager_of_data_storage::message_destination_tid;
             manager_of_data_storage::message_destination_tid=manager_of_data_storage::message_source_tid;
             manager_of_data_storage::message_source_tid=tmp_word_value;

        } catch(objects_storage_exception obj_stor_exc) {
			  if  ( manager_of_data_storage::message_type==0 &&
			        manager_of_data_storage::message_description==0 &&
			        manager_of_data_storage::message_destination_uid==0&&
     				   manager_of_data_storage::message_destination_tid==0 &&
 			        manager_of_data_storage::message_source_uid==0 &&
     				   manager_of_data_storage::message_source_tid==0 &&
                       manager_of_data_storage::message_class==0 &&
                       manager_of_data_storage::message_reserved_value==0) {
                         //prepare descriptior for request to new command
                         manager_of_data_storage::message_type=message_header_type::DATA_QUERY;
                         manager_of_data_storage::message_description=message_header_description::TID_REGISTRATION;//I don`t know why that field left unchanged
                         manager_of_data_storage::message_destination_uid=message_header_uid::UID_UNKNOWN;
                         manager_of_data_storage::message_destination_tid=0;
                         manager_of_data_storage::message_source_uid=message_header_uid::UID_DEPOSITORY;
                         manager_of_data_storage::message_source_tid=1;
                      } else {
                          manager_of_data_storage::message_type=message_header_type::ROUTING_QUERY;

                          manager_of_data_storage::message_description&=~message_header_description_flag::BLOCK_UID_FLAG;
                          manager_of_data_storage::message_description|=message_header_description_flag::WAIT_MSG_FLAG;

                          tmp_word_value=manager_of_data_storage::message_destination_uid;
                          manager_of_data_storage::message_destination_uid=manager_of_data_storage::message_source_uid;
                          manager_of_data_storage::message_source_uid=tmp_word_value;

                          tmp_word_value=manager_of_data_storage::message_destination_tid;
                          manager_of_data_storage::message_destination_tid=manager_of_data_storage::message_source_tid;
                          manager_of_data_storage::message_source_tid=tmp_word_value;
                      };

              objects_storage_logger& logger_inst=
                              objects_storage_logger::get_instance();
              logger_inst.log_message
                                   (objects_storage_logger::ERROR_MSG,
                                    "In manager_of_data_storage::run() catched exception :"+obj_stor_exc.get_description());
        }; //catch(objects_storage_exception obj_stor_exc)

      if (failures_count>program_settings::MAX_FAILURES_COUNT)
                                                                     return;

  }; //while (true)

};

