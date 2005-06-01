#include <functional>
#include <vector>
#include <sstream>

#include <inttypes.h>

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

#include "manager_of_data_storage.h"
#include "call_control_data_manager.h"

call_control_manager& call_control_manager::get_instance(){
   static call_control_manager call_ctrl_mgr_instance;
   return call_ctrl_mgr_instance;
};

vector<byte> 
   call_control_manager::process_command_to_occupate_channel_b_side_a( vector<byte> &command) {
       vector<byte> answer_from_database;
       vector<byte> buffer_to_return(0);
       vector<byte>::iterator beg_iter, end_iter;
       byte upo_number;
       byte ie1_number;
       byte e1_number;
       byte channel_interval;
       word channelb_number;

       try {
              database_wrapper& dbase=database_wrapper::get_instance();

               upo_number=command[1];
               ie1_number=command[2];
               e1_number=command[3];
               channel_interval=command[4];

               beg_iter=command.begin();
               end_iter=beg_iter;

               advance (beg_iter, 5);
               advance (end_iter, 7);
               channelb_number=type_from_bytes<word>(vector<byte>(beg_iter,end_iter));

//==================================
/*
{
              objects_storage_logger& logger_inst=objects_storage_logger::get_instance();
              ostringstream message;
              message<<" in occupate command upo "<<static_cast<int>(upo_number)
                            <<" ie1 "<<static_cast<int>(ie1_number)
                            <<" e1 "<<static_cast<int>(e1_number)
                            <<" ki "<<static_cast<int>(channel_interval)
                            <<" chb_num "<<channelb_number;
              logger_inst.log_message
                                   (objects_storage_logger::DEBUG_MSG,
                                    message.str());
}
*/
//==================================

               if (command[7]!=0) { // only that channel
                                    answer_from_database=
                                     dbase.call_control_occupate_channel_side_a_only(upo_number, 
                                                                     ie1_number,
                                                                     e1_number,
                                                                     channel_interval,
                                                                     channelb_number,
                                                                     manager_of_data_storage::message_source_uid,
                                                                     manager_of_data_storage::message_source_tid);
                                    
                                  } else { //if (command[7]!=0)
                                     answer_from_database=
                                     dbase.call_control_occupate_channel_side_a_any(upo_number, 
                                                ie1_number,
                                                e1_number,
                                                channel_interval,
                                                channelb_number,
                                                manager_of_data_storage::message_source_uid,
                                                manager_of_data_storage::message_source_tid);
                                   };//if (command[7]!=0)

            if (answer_from_database.empty()) {
                 buffer_to_return.push_back(call_control_commands::ERROR_IN_CHANNEL_B_OCCUPATION_SIDE_A);
              } else { //if (answer_from_database.empty())
                  buffer_to_return.push_back(call_control_commands::ANSWER_TO_OCCUPATE_CHANNEL_B_SIDE_A);
                  buffer_to_return.insert(buffer_to_return.end(),
                                                    answer_from_database.begin(),
                                                    answer_from_database.end());
                }; //if (answer_from_database.empty())

      } catch (objects_storage_exception obj_stor_exc){
              objects_storage_logger& logger_inst=objects_storage_logger::get_instance();

              logger_inst.log_message
                                   (objects_storage_logger::ERROR_MSG,
                                     "call_control_manager::process_command_to_occupate_channel_b_side_a : catched exception "+obj_stor_exc.get_description());
              buffer_to_return.push_back(call_control_commands::ERROR_IN_CHANNEL_B_OCCUPATION_SIDE_A); 
       };

//==================================
/*
{
              objects_storage_logger& logger_inst=objects_storage_logger::get_instance();
              ostringstream message;
              message<<" in occupate command ret_buffer ";
               for(bytes::size_type i=0; i<buffer_to_return.size(); i++)
                       message<<" ["<<i<<"]="<<buffer_to_return[i];
              logger_inst.log_message
                                   (objects_storage_logger::DEBUG_MSG,
                                    message.str());
}
*/
//==================================


       return buffer_to_return;
};

vector<byte> 
   call_control_manager::process_command_to_free_channel_b( vector<byte> &command){
       vector<byte>buffer_to_return;

       byte upo_number;
       byte ie1_number;
       byte e1_number;
       byte channel_interval;

      try {
       database_wrapper& dbase=database_wrapper::get_instance();

        upo_number=command[1];
        ie1_number=command[2];
        e1_number=command[3];
        channel_interval=command[4];

          if ( dbase.call_control_free_channel(upo_number, 
                                                ie1_number,
                                                e1_number,
                                                channel_interval) ) {
              buffer_to_return.push_back(call_control_commands::ANSWER_TO_FREE_CHANNEL_B); 
            } else {
              buffer_to_return.push_back(call_control_commands::ERROR_IN_CHANNEL_B_FREE); 
            };
      } catch (objects_storage_exception obj_stor_exc){  //internal failure in initialization
              objects_storage_logger& logger_inst=
                              objects_storage_logger::get_instance();
              logger_inst.log_message
                                   (objects_storage_logger::ERROR_MSG,
                                     "call_control_manager::process_command_to_free_channel_b : catched exception "+obj_stor_exc.get_description());
              buffer_to_return.push_back(call_control_commands::ERROR_IN_CHANNEL_B_FREE); 
       };
       return buffer_to_return;
};

vector<byte>
  call_control_manager::process_command_to_occupate_channel_b_side_b( vector<byte> &command){
       vector<byte> answer_from_database;
       vector<byte> buffer_to_return(0);
       vector<byte>::iterator beg_iter, end_iter;

       int destination;
       byte mg;
       byte pr;
       byte isdn;
       byte count;

       try {
              database_wrapper& dbase=database_wrapper::get_instance();

              beg_iter=command.begin();
              end_iter=beg_iter;
              advance(beg_iter, 1);
              advance(end_iter, 5);
              destination=type_from_bytes<int>(vector<byte>(beg_iter,end_iter));

               mg=command[5];
               pr=command[6];
               isdn=command[7];
               count=command[8];

               answer_from_database=
                dbase.call_control_occupate_channel_side_b(destination, 
                                                                     mg,
                                                                     pr,
                                                                     isdn,
                                                                     count,
                                                                     manager_of_data_storage::message_source_uid,
                                                                     manager_of_data_storage::message_source_tid);

            if (answer_from_database.empty()) {
//==================================
/*
{
              objects_storage_logger& logger_inst=objects_storage_logger::get_instance();
              ostringstream message;
              message<<" empty answer  src uid "<<manager_of_data_storage::message_source_uid
                            <<" src tid "<<manager_of_data_storage::message_source_tid;
              logger_inst.log_message
                                   (objects_storage_logger::DEBUG_MSG,
                                    message.str());
}
*/
//==================================

                 buffer_to_return.push_back(call_control_commands::ERROR_IN_CHANNEL_B_OCCUPATION_SIDE_B);
              } else { //if (answer_from_database.empty())
                  buffer_to_return.push_back(call_control_commands::ANSWER_TO_OCCUPATE_CHANNEL_B_SIDE_B);
                  buffer_to_return.insert(buffer_to_return.end(),
                                                    answer_from_database.begin(),
                                                    answer_from_database.end());
                }; //if (answer_from_database.empty())

      } catch (objects_storage_exception obj_stor_exc){
              objects_storage_logger& logger_inst=objects_storage_logger::get_instance();

              logger_inst.log_message
                                   (objects_storage_logger::ERROR_MSG,
                                     "call_control_manager::process_command_to_occupate_channel_b_side_b : catched exception "+obj_stor_exc.get_description());
              buffer_to_return.push_back(call_control_commands::ERROR_IN_CHANNEL_B_OCCUPATION_SIDE_B); 
       };

       return buffer_to_return;
};

vector<byte>
  call_control_manager::process_command_to_occupate_call_pointer( vector<byte> &command){
       vector<byte> answer_from_database;
       vector<byte> buffer_to_return(0);

       byte upo_number;
       byte ie1_number;
       byte e1_number;
       byte channel_interval;

       try {
              database_wrapper& dbase=database_wrapper::get_instance();

               upo_number=command[1];
               ie1_number=command[2];
               e1_number=command[3];
               channel_interval=command[4];

               answer_from_database=
                dbase.call_control_occupate_call_pointer(upo_number, 
                                                                     ie1_number,
                                                                     e1_number,
                                                                     channel_interval);

            if (answer_from_database.empty()) {
                 buffer_to_return.push_back(call_control_commands::ERROR_IN_CALL_POINTER_OCCUPATE);
              } else { //if (answer_from_database.empty())
                  buffer_to_return.push_back(call_control_commands::ANSWER_TO_OCCUPATE_CALL_POINTER);
                  buffer_to_return.insert(buffer_to_return.end(),
                                                    answer_from_database.begin(),
                                                    answer_from_database.end());
                }; //if (answer_from_database.empty())

      } catch (objects_storage_exception obj_stor_exc){
              objects_storage_logger& logger_inst=objects_storage_logger::get_instance();

              logger_inst.log_message
                                   (objects_storage_logger::ERROR_MSG,
                                     "call_control_manager::process_command_to_occupate_call_pointer : catched exception "+obj_stor_exc.get_description());
              buffer_to_return.push_back(call_control_commands::ERROR_IN_CALL_POINTER_OCCUPATE); 
       };

       return buffer_to_return;

};

vector<byte>
  call_control_manager::process_command_to_free_call_pointer( vector<byte> &command){
       vector<byte> buffer_to_return;
       vector<byte>::iterator beg_iter, end_iter;

       byte upo_number;
       byte ie1_number;
       byte e1_number;
       byte channel_interval;
       word call_pointer_id;
      try {
       database_wrapper& dbase=database_wrapper::get_instance();

        upo_number=command[1];
        ie1_number=command[2];
        e1_number=command[3];
        channel_interval=command[4];
        beg_iter=command.begin();
        advance(beg_iter, 5);
        end_iter=command.end();
        call_pointer_id=type_from_bytes<word>(vector<byte>(beg_iter,end_iter));
          if ( dbase.call_control_free_call_pointer(upo_number, 
                                                ie1_number,
                                                e1_number,
                                                channel_interval,
                                                call_pointer_id) ) {
              buffer_to_return.push_back(call_control_commands::ANSWER_TO_FREE_CALL_POINTER); 
            } else {
              buffer_to_return.push_back(call_control_commands::ERROR_IN_CALL_POINTER_FREE); 
            };
      } catch (objects_storage_exception obj_stor_exc){  //internal failure in initialization
              objects_storage_logger& logger_inst=
                              objects_storage_logger::get_instance();
              logger_inst.log_message
                                   (objects_storage_logger::ERROR_MSG,
                                     "call_control_manager::process_command_to_free_call_pointer : catched exception "+obj_stor_exc.get_description());
              buffer_to_return.push_back(call_control_commands::ERROR_IN_CALL_POINTER_FREE); 
       };
       return buffer_to_return;
};

vector<byte> 
   call_control_manager::process_command(vector<byte> &command){

     if (command.empty()) {
                 vector<byte> msg_to_retun(0);
                 msg_to_retun.push_back(call_control_commands::BAD_COMMAND);
                 return msg_to_retun;
              };

       switch (command[message_content_bytes::INDEX_OF_MESSAGE_CODE]) { 
            case call_control_commands::REQUEST_TO_OCCUPATE_CHANNEL_B_SIDE_A: {
                 return process_command_to_occupate_channel_b_side_a(command);
                 break;
             };  // case pri_commands::REQUEST_TO_OCCUPATE_CHANNEL_B_SIDE_A
            case call_control_commands::REQUEST_TO_OCCUPATE_CHANNEL_B_SIDE_B: {
                 return process_command_to_occupate_channel_b_side_b(command);
                 break;
             };  // case pri_commands::REQUEST_TO_OCCUPATE_CHANNEL_B_SIDE_B
            case call_control_commands::REQUEST_TO_FREE_CHANNEL_B: { 
                 return process_command_to_free_channel_b(command);
                 break;
             };  // case pri_commands::REQUEST_TO_FREE_CHANNEL_B
            case call_control_commands::REQUEST_TO_OCCUPATE_CALL_POINTER: { 
                 return process_command_to_occupate_call_pointer(command);
                 break;
             };  // case pri_commands::REQUEST_TO_OCCUPATE_CALL_POINTER
            case call_control_commands::REQUEST_TO_FREE_CALL_POINTER: { 
                 return process_command_to_free_call_pointer(command);
                 break;
             };  // case pri_commands::REQUEST_TO_FREE_CALL_POINTER

             default : {
                 vector<byte> msg_to_retun=command;
                 msg_to_retun[0]=call_control_commands::UNKNOWN_COMMAND;
                 return msg_to_retun;
              };
       };// switch (command[1])

};

