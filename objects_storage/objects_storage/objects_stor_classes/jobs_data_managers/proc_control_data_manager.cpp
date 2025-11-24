
#include <functional>
#include <vector>
#include <sstream>

#include <pthread.h>
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

#include "proc_control_data_manager.h"

proc_control_manager& proc_control_manager::get_instance(){
    static proc_control_manager proc_ctrl_mgr_instance;
    return proc_ctrl_mgr_instance;
};

vector<byte> 
       proc_control_manager::process_command_to_query_state_of_channel_b( vector<byte> &command){
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
                dbase.proc_control_check_state_b(upo_number, 
                                                                     ie1_number,
                                                                     e1_number,
                                                                     channel_interval);

            if (answer_from_database.empty()) {
                 buffer_to_return.push_back(proc_control_commands::ERROR_IN_STATE_OF_CHANNEL_B);
              } else { //if (answer_from_database.empty())
                  buffer_to_return.push_back(proc_control_commands::ANSWER_STATE_OF_CHANNEL_B);
                  buffer_to_return.insert(buffer_to_return.end(),
                                                    answer_from_database.begin(),
                                                    answer_from_database.end());
                }; //if (answer_from_database.empty())

      } catch (objects_storage_exception obj_stor_exc){
              objects_storage_logger& logger_inst=objects_storage_logger::get_instance();

              logger_inst.log_message
                                   (objects_storage_logger::ERROR_MSG,
                                     "proc_control_manager::process_command_to_query_state_of_channel_b : catched exception "+obj_stor_exc.get_description());
              buffer_to_return.push_back(proc_control_commands::ERROR_IN_STATE_OF_CHANNEL_B); 
       };

       return buffer_to_return;
};


vector<byte> 
        proc_control_manager::process_command(vector<byte> &command){

     if (command.empty()) {
                 vector<byte> msg_to_retun(0);
                 msg_to_retun.push_back(proc_control_commands::BAD_COMMAND);
                 return msg_to_retun;
              };

       switch (command[message_content_bytes::INDEX_OF_MESSAGE_CODE]) {
            case proc_control_commands::QUERY_STATE_OF_CHANNEL_B : {
                 return process_command_to_query_state_of_channel_b(command);
                 break;
             };  // case pri_commands::PRI_COMMAND_REQUEST_TO_OCCUPATE_CODE
             default : {
                 vector<byte> msg_to_retun=command;
                 msg_to_retun[0]=proc_control_commands::UNKNOWN_COMMAND;
                 return msg_to_retun;
              };
       };// switch (command[1])

};



