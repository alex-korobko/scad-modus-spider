using namespace std;

#include <functional>
#include <vector>
#include <sstream>

#include <pthread.h>
#include <inttypes.h>

#include "storage_common.h"
#include "dbwrapper_definition.h"
#include "objects_storage_exception.h"
#include "objects_storage_logger.h"

#ifdef _MOCK_DBWRAPPER_
#include "mock_database_wrapper.h"
#else  //#ifdef _MOCK_DBWRAPPER_
#ifdef _LINTER_DBWRAPPER_
#include <linapi.h>
#include "linter_database_wrapper.h"
#endif  //#ifdef _LINTER_DBWRAPPER_
#endif  //#ifdef _MOCK_DBWRAPPER_

#include "abstract_job_data_manager.h"
#include "pri_data_manager.h"

int pri_data_manager::data_manager_type=data_managers_types::PRI_DATA_MANAGER;
pri_data_manager* pri_data_manager::pri_mgr_instance=NULL;

vector<byte> 
   pri_data_manager::process_command_to_occupate( vector<byte> command) {
       vector<byte>answer_from_database;
       vector<byte>buffer_to_return(0);

       byte upo_number;
       byte ie1_number;
       byte e1_number;
       byte channel_interval;
       word channelb_number;
       pthread_t pthread_id;

       database_wrapper* dbase=database_wrapper::get_instance();

       if (dbase==NULL) {  //internal failure in initialization
              objects_storage_logger* logger_inst=
                              objects_storage_logger::get_instance();

              logger_inst->log_message
                                   (objects_storage_logger::ERROR_MSG,
                                     "pri_data_manager::process_command_to_free : database_wrapper::get_instance() return NULL");

            vector<byte>::iterator cmd_iter=command.begin();
             advance (cmd_iter,
                            pri_commands_sizes::PRI_COMMAND_REQUEST_TO_OCCUPATE_SIZE);
            buffer_to_return.insert (buffer_to_return.end(),
                                                    command.begin(),
                                                    cmd_iter);
             buffer_to_return[program_settings::DESCRIPTOR_INDEX]=
                                                data_managers_types::PRI_DATA_MANAGER;
             buffer_to_return[program_settings::COMMAND_CODE_INDEX]=
                                pri_commands::PRI_COMMAND_ERROR_IN_OCCUPATION;
 
           return buffer_to_return;
       };

               vector<byte>::reverse_iterator beg_iter=command.rend(), end_iter=beg_iter;
               advance (beg_iter, -2); 
               advance (end_iter, -(2+sizeof(pthread_t)));
               pthread_id=type_from_bytes<pthread_t>(vector<byte>(end_iter, beg_iter));

               upo_number=command[1+sizeof(pthread_id)+1];
               ie1_number=command[1+sizeof(pthread_id)+2];
               e1_number=command[1+sizeof(pthread_id)+3];
               channel_interval=command[1+sizeof(pthread_id)+4];

               beg_iter=command.rend();
               end_iter=beg_iter;

               advance (beg_iter, -(2+sizeof(pthread_t)+4)); 
               advance (end_iter, -(2+sizeof(pthread_t)+4+sizeof(word)));
               channelb_number=type_from_bytes<word>(vector<byte>(end_iter, beg_iter));

               if (command[1+sizeof(pthread_id)+6]!=0) { // only that channel
                                    answer_from_database=
                                     dbase->pri_occupate_channel_only(upo_number, 
                                                                     ie1_number,
                                                                     e1_number,
                                                                     channel_interval,
                                                                     channelb_number,
                                                                     pthread_id);
                                  } else { //if (command[2+sizeof(pthread_id)+6]!=0) 
                                     answer_from_database=
                                     dbase->pri_occupate_channel_any(upo_number, 
                                                ie1_number,
                                                e1_number,
                                                channel_interval,
                                                channelb_number,
                                                pthread_id);
                                   };//if (command[2+sizeof(pthread_id)+6]!=0)

     if (answer_from_database.empty()) {

            vector<byte>::iterator cmd_iter=command.begin();
             advance (cmd_iter,
                            pri_commands_sizes::PRI_COMMAND_REQUEST_TO_OCCUPATE_SIZE);
            buffer_to_return.insert (buffer_to_return.end(),
                                                    command.begin(),
                                                    cmd_iter);
 
             buffer_to_return[program_settings::DESCRIPTOR_INDEX]=
                                          data_managers_types::PRI_DATA_MANAGER;
             buffer_to_return[program_settings::COMMAND_CODE_INDEX]=
                      pri_commands::PRI_COMMAND_ERROR_IN_OCCUPATION;

       } else { //if (answer_from_database.empty())
             buffer_to_return.push_back(data_managers_types::PRI_DATA_MANAGER);
             buffer_to_return.push_back(pri_commands::PRI_COMMAND_ANSWER_TO_OCCUPATE_CODE);
             buffer_to_return.insert(buffer_to_return.end(),
                                                    answer_from_database.begin(),
                                                    answer_from_database.end());
       }; //if (answer_from_database.empty())

       return buffer_to_return;
};

vector<byte> 
       pri_data_manager::process_command_to_free( vector<byte> command){
       vector<byte>buffer_to_return;

       byte upo_number;
       byte ie1_number;
       byte e1_number;
       byte channel_interval;
       word channelb_number;
       pthread_t pthread_id;
       database_wrapper* dbase=database_wrapper::get_instance();


       if (dbase==NULL) {  //internal failure in initialization
              objects_storage_logger* logger_inst=
                              objects_storage_logger::get_instance();

              logger_inst->log_message
                                   (objects_storage_logger::ERROR_MSG,
                                     "pri_data_manager::process_command_to_free : database_wrapper::get_instance() return NULL");


            vector<byte>::iterator cmd_iter=command.begin();
             advance (cmd_iter,
                            pri_commands_sizes::PRI_COMMAND_REQUEST_TO_FREE_SIZE);
            buffer_to_return.insert (buffer_to_return.end(),
                                                    command.begin(),
                                                    cmd_iter);
 
             buffer_to_return[program_settings::DESCRIPTOR_INDEX]=
                             data_managers_types::PRI_DATA_MANAGER;
             buffer_to_return[program_settings::COMMAND_CODE_INDEX]=
                          pri_commands::PRI_COMMAND_ERROR_IN_FREE;
 
            return buffer_to_return;

       };

       vector<byte>::reverse_iterator beg_iter=command.rend(), end_iter=beg_iter;
       advance (beg_iter, -2); 
       advance (end_iter, -(2+sizeof(pthread_t)));
       pthread_id=type_from_bytes<pthread_t>(vector<byte>(end_iter, beg_iter));

     upo_number=command[1+sizeof(pthread_id)+1];
     ie1_number=command[1+sizeof(pthread_id)+2];
     e1_number=command[1+sizeof(pthread_id)+3];
     channel_interval=command[1+sizeof(pthread_id)+4];

     beg_iter=command.rend();
     end_iter=beg_iter;
     advance (beg_iter, -(2+sizeof(pthread_t)+4)); 
     advance (end_iter, -(2+sizeof(pthread_t)+4+sizeof(word)));
     channelb_number=type_from_bytes<word>(vector<byte>(end_iter, beg_iter));

        if ( dbase->pri_free_channel(upo_number, 
                                                ie1_number,
                                                e1_number,
                                                channel_interval,
                                                channelb_number,
                                                pthread_id) ) {
            vector<byte>::iterator cmd_iter=command.begin();
             advance (cmd_iter,
                            pri_commands_sizes::PRI_COMMAND_REQUEST_TO_FREE_SIZE);
            buffer_to_return.insert (buffer_to_return.end(),
                                                    command.begin(),
                                                    cmd_iter);
             buffer_to_return[program_settings::DESCRIPTOR_INDEX]=
                                                          data_managers_types::PRI_DATA_MANAGER;
             buffer_to_return[program_settings::COMMAND_CODE_INDEX]=
                                   pri_commands::PRI_COMMAND_ANSWER_TO_FREE_CODE;
       } else {
            vector<byte>::iterator cmd_iter=command.begin();
             advance (cmd_iter,
                            pri_commands_sizes::PRI_COMMAND_REQUEST_TO_FREE_SIZE);
            buffer_to_return.insert (buffer_to_return.end(),
                                                    command.begin(),
                                                    cmd_iter);

             buffer_to_return[program_settings::DESCRIPTOR_INDEX]=
                                                data_managers_types::PRI_DATA_MANAGER;
             buffer_to_return[program_settings::COMMAND_CODE_INDEX]=
                                        pri_commands::PRI_COMMAND_ERROR_IN_FREE;
       };



       return buffer_to_return;
};


vector<byte> 
           pri_data_manager::process_command(vector<byte> command){

     if (command.empty() ||
         command[program_settings::DESCRIPTOR_INDEX]!=
                             data_managers_types::PRI_DATA_MANAGER) {
                 vector<byte> msg_to_retun(0);
                 msg_to_retun.push_back(data_managers_types::PRI_DATA_MANAGER);
                 msg_to_retun.push_back(pri_commands::PRI_BAD_COMMAND);
                 return msg_to_retun;
              };

       switch (command[program_settings::COMMAND_CODE_INDEX]) { 
            case pri_commands::PRI_COMMAND_REQUEST_TO_OCCUPATE_CODE : {
                 return process_command_to_occupate(command);
                 break;
             };  // case pri_commands::PRI_COMMAND_REQUEST_TO_OCCUPATE_CODE
            case pri_commands::PRI_COMMAND_REQUEST_TO_FREE_CODE : { 
                 return process_command_to_free(command);
                 break;
             };  // case pri_commands::PRI_COMMAND_REQUEST_TO_OCCUPATE_CODE
             default : {
                 vector<byte> msg_to_retun=command;
                 msg_to_retun[1]=pri_commands::PRI_UNKNOWN_COMMAND;
                 return msg_to_retun;
              };
       };// switch (command[1])

};

pri_data_manager* pri_data_manager::get_instance(){
    if (pri_mgr_instance==NULL) {
                  pri_mgr_instance=new pri_data_manager();
           }
    return pri_mgr_instance;
};
