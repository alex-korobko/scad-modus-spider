#include <inttypes.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <iostream.h>

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <sstream>

using namespace std;

#include "defines.h"

#include "socket_exception.h"
#include "generic_socket.h"
#include "server_socket.h"

#include "comport_exception.h"
#include "comport.h"

#include "program_settings.h"
#include "metro_device.h"
#include "metro_udku.h"


metro_device::command_data_container
     metro_udku::get_default_command_request_to_comport(){
     command_data_container return_val(0);
     program_settings::bytes tmp_bytes;
     vector<byte> buffer(0);

// com port default request for UDKU
	buffer.push_back(get_number());
	buffer.push_back(4);                      // function number
	buffer.push_back(0);
	buffer.push_back(UDKU_FIRST_REGISTER);
	buffer.push_back(0);
	buffer.push_back(UDKU_MODBUS_REGISTERS_COUNT);

	tmp_bytes=program_settings::bytes_of_type<word>(program_settings::crc(buffer));
	buffer.insert(buffer.end(), tmp_bytes.begin(), tmp_bytes.end());

    return_val.push_back(buffer);
   	return return_val;
};

metro_device::command_data
     metro_udku::get_default_command_request_from_socket(){
 	command_data buffer(0);

	buffer.push_back(get_number());
	buffer.push_back(4);                      // function number
	buffer.push_back(0);
	buffer.push_back(UDKU_FIRST_REGISTER);
	buffer.push_back(0);
	buffer.push_back(UDKU_MODBUS_REGISTERS_COUNT+
                                 2+//MESSAGES_UPPER_ID+MESSAGES_COUNT
                                 program_settings::MAX_MESSAGES_COUNT);

	program_settings::bytes tmp_bytes=program_settings::bytes_of_type<word>(program_settings::crc(buffer));
	buffer.insert(buffer.end(), tmp_bytes.begin(), tmp_bytes.end());

   	return buffer;
};

void 
      metro_udku::put_command_answer_from_comport
                   (metro_device::command_data_container answer_from_com_port){
command_data new_answer_to_socket(0);
byte function_code;
metro_device::command_data request;

pthread_mutex_lock(metro_device::get_request_from_socket_mutex());
   request=request_from_socket;
pthread_mutex_unlock(metro_device::get_request_from_socket_mutex());

// answer_from_device
//============================================
/*
{
vector<char> tmp_buffer(32);
if (metro_device::get_number()==1) {

//cout<<"\n request size "<<request.size()<<endl;
//for (vector<byte>::size_type i=0; i<request.size(); i++) {
//          itoa(request[i],
//                  &tmp_buffer[0], 16);
//          if (i==3 ||
//              (i-3)%8==0) cout<<endl;
//          cout<<"\t0x"<<&tmp_buffer[0];
//};
//cout<<endl;

if (answer_from_com_port.empty()) {
      cout<<"answer_from_com_port is EMPTY"<<endl;
 } else { //if (answer_from_com_port.empty())
for (metro_device::command_data_container::size_type j=0;
       j<answer_from_com_port.size();
       j++) {
           cout<<"answer_from_com_port "<<j<<" tid "<<pthread_self()<<" :"<<endl;
            for (metro_device::command_data_container::value_type::size_type i=0;
              i<answer_from_com_port[j].size() && i<6;
//              i<4; //only header
              i++) {
              itoa(answer_from_com_port[j][i],
                      &tmp_buffer[0], 16);
              if (i==3 ||
                  (i-3)%8==0) cout<<endl;
              cout<<"\t0x"<<&tmp_buffer[0];
           }; //for (metro_device::buffer_data_type::size_type i=0;
      cout<<endl;
    }; //for (metro_device::command_data_container::size_type j=0;
    cout<<endl;
};//if (answer_from_com_port.empty())
}; //if (metro_device::get_number()==1 )
};
*/
//============================================


if (answer_from_com_port.empty() ||
    answer_from_com_port[0].empty()) {
        function_code=request[program_settings::MODBUS_FUNCTION_CODE_INDEX];
		new_answer_to_socket=metro_device::generate_error_answer_for_function(
		                                                              get_number(),
		                                                              function_code,
		                                                              program_settings::MODBUS_EXCEPTION_FAILURE_IN_ASSOCIATED_DEVICE);
} else {
   if ((answer_from_com_port[0][program_settings::MODBUS_FUNCTION_CODE_INDEX]&0x80)
   	     !=0) {
               new_answer_to_socket=answer_from_com_port[0];   	      
   	     } else { // if ((answer_from_com_port[0][program_settings:
              function_code=answer_from_com_port[0][program_settings::MODBUS_FUNCTION_CODE_INDEX];
             switch (function_code) {
               case 4: 
                new_answer_to_socket=create_answer_to_socket_func_4(answer_from_com_port);
                break;
               default:  //if write data function
               new_answer_to_socket=answer_from_com_port[0];
          }; //switch (function_code)
         }; // if ((device_answer_header[prog
};

//========delete it================
//{
//cout<<"\nudku new_answer_to_socket size "<<new_answer_to_socket.size()<<endl;
//vector<char> tmp_chars(10);
//for (int i=0;
//      i<static_cast<int>(new_answer_to_socket.size());
//      i++) {
//itoa(new_answer_to_socket[i],
//       &tmp_chars[0],
//       16);
//   if (i%9==0) cout<<endl;
//   cout<<"  0x"<<&tmp_chars[0];
// };
// cout<<endl;
// };
//=============================

if (get_command_request_to_comport()==
     get_default_command_request_to_comport()) {
          metro_device::set_default_answer_to_socket_buffer(new_answer_to_socket);
    };
/*
pthread_mutex_lock(&request_from_socket_mutex);
request_from_socket.clear();
pthread_mutex_unlock(&request_from_socket_mutex);
*/
metro_device::set_command_answer_to_socket_buffer(new_answer_to_socket);
};

void 
     metro_udku::put_command_request_from_socket
                   (metro_device::command_data request_from_socket) {
   pthread_mutex_lock(metro_device::get_request_from_socket_mutex());
     this->request_from_socket=request_from_socket;
   pthread_mutex_unlock(metro_device::get_request_from_socket_mutex());

   metro_device::set_command_request_to_comport_buffer(create_request_to_com_port()); 
 };

metro_device::command_data_container
         metro_udku::create_request_to_com_port(){
    metro_device::command_data_container data_container(0);
    metro_device::command_data local_request;
    pthread_mutex_lock(metro_device::get_request_from_socket_mutex());
      local_request=request_from_socket;
    pthread_mutex_unlock(metro_device::get_request_from_socket_mutex());

    if (local_request==
        get_default_command_request_from_socket()) {
            data_container=get_default_command_request_to_comport();
         } else {
            data_container.push_back(local_request);

             //detect is command for udku
             if (local_request.size()==8 &&
                  local_request[1]==5) { //command 5
                  metro_device::command_data::iterator tmp_iter=local_request.begin();
                  advance(tmp_iter, local_request.size()-2);
                  //crc check
                  word calc_crc=program_settings::crc(program_settings::bytes(local_request.begin(), tmp_iter));
                  word org_crc=program_settings::type_from_bytes<word>(program_settings::bytes(tmp_iter, local_request.end()));
                   if (calc_crc==org_crc) {
                          metro_device::messages_container new_messages(0);
                           switch (local_request[3]) {
                              case 0:
                                 new_messages.push_back(0);
                                 new_messages.push_back(MESSAGE_RECIEVE_COMMAND_UP);
                                 break;
                               case 1:
                                 new_messages.push_back(0);
                                 new_messages.push_back(MESSAGE_RECIEVE_COMMAND_DOWN);
                                  break;
                               case 2:
                                 new_messages.push_back(0);
                                 new_messages.push_back(MESSAGE_RECIEVE_COMMAND_STOP);
                                  break;
                            };
                            if (!new_messages.empty()) metro_device::add_messages_fifo (new_messages);
                       }; //if (calc_crc==org_crc)
              }; //if (request_from_socket.size()==8 &&
         };
    return data_container;
};

metro_device::command_data
    metro_udku::create_answer_to_socket_func_4
               (metro_device::command_data_container answer_from_com_port){
    program_settings* sett_obj=program_settings::get_instance();
    metro_device::messages_container all_messages;
    program_settings::bytes tmp_value_bytes;
    metro_device::command_data  answer_to_socket(0);
    metro_device::messages_container new_messages(0);
    metro_device::command_data::iterator iter_beg, iter_end;

//=======delete it====================
//cout <<"answer_from_com_port[1]"<<endl;
//  vector<char> tmp_chars(40);
//	for(metro_device::command_data::size_type i=0;
//	      i<answer_from_com_port[2].size();
//	      i++) {
//	  if (i%8 ==0) cout<<endl;
//	  itoa(answer_from_com_port[2][i],
//	        &tmp_chars[0],
//	        16);
//	  cout<<"  0x"<<&tmp_chars[0];
//   };
//   cout<<endl;
//==================================

//     UDKU
	if(answer_from_com_port[0].size()!=
	    program_settings::MODBUS_HEADER_SIZE+
        UDKU_MODBUS_REGISTERS_COUNT*UDKU_MODBUS_REGISTER_INCREM+
        program_settings::MODBUS_CRC_SIZE) {
                  return metro_device::generate_error_answer_for_function
                     ( get_number(),
                        4,
                       program_settings::MODBUS_EXCEPTION_FAILURE_IN_ASSOCIATED_DEVICE);
             }; //if(answer_from_com_port[0].size()!=

        answer_to_socket.push_back(get_number());
        answer_to_socket.push_back(4);
        answer_to_socket.push_back(
           UDKU_MODBUS_REGISTERS_COUNT*UDKU_MODBUS_REGISTER_INCREM+
           MESSAGES_UPPER_ID_INCREM+MESSAGES_COUNT_INCREM+
           MESSAGES_MODBUS_REGISTERS_COUNT*MESSAGES_INCREM);
        iter_beg=answer_from_com_port[0].begin();
        iter_end=iter_beg;
        advance(iter_beg, program_settings::MODBUS_HEADER_SIZE);
        advance(iter_end, answer_from_com_port[0].size()-program_settings::MODBUS_CRC_SIZE);
        answer_to_socket.insert(answer_to_socket.end(),
                        iter_beg, iter_end);
//messages generating
        new_messages.clear();
       if ( previos_escalator_condition_value!=-1)
        switch (answer_from_com_port[0][program_settings::MODBUS_DATA_BYTES_COUNT_INDEX+2]) {
          case MODBUS_REGISTER_3001_LOW_BYTE_GPSTOP_VALUE :
               if (previos_escalator_condition_value!=MODBUS_REGISTER_3001_LOW_BYTE_GPSTOP_VALUE &&
                    (previos_escalator_condition_value==MODBUS_REGISTER_3001_LOW_BYTE_GPUP_VALUE  ||
                      previos_escalator_condition_value==MODBUS_REGISTER_3001_LOW_BYTE_GPDOWN_VALUE ||
                      previos_escalator_condition_value==MODBUS_REGISTER_3001_LOW_BYTE_UP20_VALUE ||
                      previos_escalator_condition_value==MODBUS_REGISTER_3001_LOW_BYTE_UP25_VALUE||
                      previos_escalator_condition_value==MODBUS_REGISTER_3001_LOW_BYTE_NOT7SEC_VALUE||
                      previos_escalator_condition_value==MODBUS_REGISTER_3001_LOW_BYTE_BREAK_VALUE||
                      previos_escalator_condition_value==MODBUS_REGISTER_3001_LOW_BYTE_SAMOHOD_VALUE)) {
                          new_messages.push_back(0);
                          new_messages.push_back(MESSAGE_STOP_WITH_BREAKING_PATH);
                    };
                  break;
          case MODBUS_REGISTER_3001_LOW_BYTE_UP20_VALUE:
                   if (previos_escalator_condition_value!=
                        MODBUS_REGISTER_3001_LOW_BYTE_UP20_VALUE) {
			                    new_messages.push_back(0);
   					            new_messages.push_back(MESSAGE_OWERSPEED_MORE_20_PERC);
                            };
                  break;
          case MODBUS_REGISTER_3001_LOW_BYTE_UP25_VALUE:
                   if (previos_escalator_condition_value!=
                        MODBUS_REGISTER_3001_LOW_BYTE_UP25_VALUE) {
                              new_messages.push_back(0);
                               new_messages.push_back(MESSAGE_OWERSPEED_MORE_25_PERC);
                            };
                  break;
          case MODBUS_REGISTER_3001_LOW_BYTE_NOT7SEC_VALUE:
                   if (previos_escalator_condition_value!=
                        MODBUS_REGISTER_3001_LOW_BYTE_NOT7SEC_VALUE) {
			                    new_messages.push_back(0);
   				                 new_messages.push_back(MESSAGE_NOT_STOP_AFTER_7_SEC);
                          };
                  break;
          case MODBUS_REGISTER_3001_LOW_BYTE_BREAK_VALUE:
                   if (previos_escalator_condition_value!=
                        MODBUS_REGISTER_3001_LOW_BYTE_BREAK_VALUE) {
			                    new_messages.push_back(0);
   				                 new_messages.push_back(MESSAGE_CONDITION_BREAK);
                          };
                  break;
          case MODBUS_REGISTER_3001_LOW_BYTE_SAMOHOD_VALUE:
                   if (previos_escalator_condition_value!=
                        MODBUS_REGISTER_3001_LOW_BYTE_SAMOHOD_VALUE) {
			                    new_messages.push_back(0);
   				                 new_messages.push_back(MESSAGE_CONDITION_SAMOHOD);
                          };
                  break;
          case MODBUS_REGISTER_3001_LOW_BYTE_BRAK0_VALUE:
                   if (previos_escalator_condition_value!=
                        MODBUS_REGISTER_3001_LOW_BYTE_BRAK0_VALUE) {
			                    new_messages.push_back(0);
   				                 new_messages.push_back(MESSAGE_ERROR_D0);
                          };
                  break;
         case MODBUS_REGISTER_3001_LOW_BYTE_BRAK90_VALUE:
                   if (previos_escalator_condition_value!=
                        MODBUS_REGISTER_3001_LOW_BYTE_BRAK90_VALUE) {
			                    new_messages.push_back(0);
   				                 new_messages.push_back(MESSAGE_ERROR_D90);
                         };
             break;
         case MODBUS_REGISTER_3001_LOW_BYTE_BRAKALLES_VALUE:
                   if (previos_escalator_condition_value!=
                        MODBUS_REGISTER_3001_LOW_BYTE_BRAKALLES_VALUE) {
			                    new_messages.push_back(0);
   				                 new_messages.push_back(MESSAGE_ERROR_ALL_SIGNALS);
                          };
             break;
         case MODBUS_REGISTER_3001_LOW_BYTE_BRAKKSR_VALUE:
                   if (previos_escalator_condition_value!=
                        MODBUS_REGISTER_3001_LOW_BYTE_BRAKKSR_VALUE) {
		                    new_messages.push_back(0);
  			                  new_messages.push_back(MESSAGE_ERROR_CONTACT);
                           };
            break;
        default :
          ostringstream error_message;
          error_message<<"In metro_udku::create_answer_to_socket_func_4 unknown value in 30001 register`s low byte : "<<static_cast<int>(answer_from_com_port[0][program_settings::MODBUS_DATA_BYTES_COUNT_INDEX+2]);
           sett_obj->sys_message(program_settings::ERROR_MSG, error_message.str());
        }; //switch (answer_from_com_port[0][program_settings:

         previos_escalator_condition_value=answer_from_com_port[0][program_settings::MODBUS_DATA_BYTES_COUNT_INDEX+2];

   if (previos_rkp_value!=-1 &&
       (answer_from_com_port[0][program_settings::MODBUS_DATA_BYTES_COUNT_INDEX+16] & 0x01)!=0)  //on TU
         if (previos_rkp_value!=(answer_from_com_port[0][program_settings::MODBUS_DATA_BYTES_COUNT_INDEX+16] & 0x80) &&
              (answer_from_com_port[0][program_settings::MODBUS_DATA_BYTES_COUNT_INDEX+16] & 0x80)==0)
                   if ((answer_from_com_port[0][program_settings::MODBUS_DATA_BYTES_COUNT_INDEX+16] & 0x08)!=0) {
                              new_messages.push_back(0);
                              new_messages.push_back(MESSAGE_TU_COMMAND_UP_SUCCESS);
                         } else if ((answer_from_com_port[0][program_settings::MODBUS_DATA_BYTES_COUNT_INDEX+16] & 0x04)!=0) {
                               new_messages.push_back(0);
                               new_messages.push_back(MESSAGE_TU_COMMAND_DOWN_SUCCESS);
                         } else {
                               sett_obj->sys_message(program_settings::ERROR_MSG, "In metro_udku::create_answer_to_socket_func_4 RKP is 1 but RPN & RPV is 0");
                         };
    previos_rkp_value=answer_from_com_port[0][program_settings::MODBUS_DATA_BYTES_COUNT_INDEX+16] & 0x80;

   if (previos_mu_tu_switcher_value!=-1) 
         if (previos_mu_tu_switcher_value!=(answer_from_com_port[0][program_settings::MODBUS_DATA_BYTES_COUNT_INDEX+16] & 0x01))
                   if ((answer_from_com_port[0][program_settings::MODBUS_DATA_BYTES_COUNT_INDEX+16] & 0x01)!=0) {
                              new_messages.push_back(0);
                              new_messages.push_back(MESSAGE_ESCALATOR_ON_TU);
                         } else {
                               new_messages.push_back(0);
                               new_messages.push_back(MESSAGE_ESCALATOR_ON_MU);
                         };
    previos_mu_tu_switcher_value=answer_from_com_port[0][program_settings::MODBUS_DATA_BYTES_COUNT_INDEX+16] & 0x01;

   if (previos_block_circut_value!=-1) 
         if (previos_block_circut_value!=(answer_from_com_port[0][program_settings::MODBUS_DATA_BYTES_COUNT_INDEX+16] & 0x02) &&
              (answer_from_com_port[0][program_settings::MODBUS_DATA_BYTES_COUNT_INDEX+16] & 0x02)!=0) {
                     // deside, which block circut is breaked
                    byte index_of_block_circut=0;
                     if ((answer_from_com_port[0][program_settings::MODBUS_DATA_BYTES_COUNT_INDEX+17] & 0xff)!=0) {
                            for (index_of_block_circut=0; index_of_block_circut<8;index_of_block_circut++)
                               if ((answer_from_com_port[0][program_settings::MODBUS_DATA_BYTES_COUNT_INDEX+17] & (0x8>>index_of_block_circut))!=0) break;
                        } else if ((answer_from_com_port[0][program_settings::MODBUS_DATA_BYTES_COUNT_INDEX+18] & 0xff)!=0) {
                            for (index_of_block_circut=0; index_of_block_circut<8;index_of_block_circut++)
                               if ((answer_from_com_port[0][program_settings::MODBUS_DATA_BYTES_COUNT_INDEX+18] & (0x8>>index_of_block_circut))!=0) break;
                               index_of_block_circut+=8;
                        } else if ((answer_from_com_port[0][program_settings::MODBUS_DATA_BYTES_COUNT_INDEX+15] & 0xc0)!=0) {
                            for (index_of_block_circut=0; index_of_block_circut<3;index_of_block_circut++)
                               if ((answer_from_com_port[0][program_settings::MODBUS_DATA_BYTES_COUNT_INDEX+15] & (0x8>>index_of_block_circut))!=0) break;
                                index_of_block_circut+=16;
                        } else {
                            sett_obj->sys_message(program_settings::ERROR_MSG, "In metro_udku::create_answer_to_socket_func_4 previos_block_circut_value!=new value, but all crircuts is 0");
                        };
                     new_messages.push_back(index_of_block_circut);
                     new_messages.push_back(MESSAGE_ESCALATOR_NOT_READY_WITH_BLOCK_CIRCUT_NAME);
            };//if (previos_block_circut_value!=(answer_from_com_port[0][program_settings::MODBUS
      previos_block_circut_value=answer_from_com_port[0][program_settings::MODBUS_DATA_BYTES_COUNT_INDEX+16] & 0x02;

     if (!new_messages.empty())
         metro_device::add_messages_fifo (new_messages);

//messages
   tmp_value_bytes=program_settings::bytes_of_type<word>(metro_device::get_upper_message_id());
    answer_to_socket.insert(answer_to_socket.end(),
                                             tmp_value_bytes.begin(),
                                             tmp_value_bytes.end());

   tmp_value_bytes=program_settings::bytes_of_type<word>(metro_device::get_messages_count()/2);
    answer_to_socket.insert(answer_to_socket.end(),
                                             tmp_value_bytes.begin(),
                                             tmp_value_bytes.end());

    all_messages=metro_device::get_messages_fifo();
    answer_to_socket.insert(answer_to_socket.end(),
                                                all_messages.begin(),
                                                all_messages.end()); 

   int  sizes_diff=program_settings::MODBUS_HEADER_SIZE+
           UDKU_MODBUS_REGISTERS_COUNT*UDKU_MODBUS_REGISTER_INCREM+
           MESSAGES_UPPER_ID_INCREM+MESSAGES_COUNT_INCREM+
           MESSAGES_MODBUS_REGISTERS_COUNT*MESSAGES_INCREM-
           answer_to_socket.size();

    if (sizes_diff>0) 
                 answer_to_socket.insert (answer_to_socket.end(),
                                                    sizes_diff,
                                                    0);

    tmp_value_bytes=program_settings::bytes_of_type<word>(program_settings::crc(answer_to_socket));
    answer_to_socket.insert(answer_to_socket.end(),
                                             tmp_value_bytes.begin(),
                                             tmp_value_bytes.end());

//=======delete it====================
// answer_from_device
//============================================
{
if (metro_device::get_number()==1) {

cout <<"answer_to_socket"<<endl;
  vector<char> tmp_chars(40);
	for(metro_device::command_data::size_type i=0;
	      i<answer_to_socket.size();
	      i++) {
	  if (i%8 ==0) cout<<endl;
	  itoa(answer_to_socket[i],
	        &tmp_chars[0],
	        16);
	  cout<<"  0x"<<&tmp_chars[0];
   };
   cout<<endl;
};//if (metro_device::get_number()==1)
};
//==================================

  return answer_to_socket;
};

metro_device::command_data_container 
     metro_udku::get_command_request_to_comport(){
     command_data_container  return_data;
     return_data=metro_device::get_command_request_to_comport_buffer();
     //if not present data from socket, generate default request to state control
     if (return_data.empty()) {
        return_data=get_default_command_request_to_comport();
        metro_device::set_command_request_to_comport_buffer(return_data);

        pthread_mutex_lock(metro_device::get_request_from_socket_mutex());
             request_from_socket=get_default_command_request_from_socket();
        pthread_mutex_unlock(metro_device::get_request_from_socket_mutex());
        };

     return return_data;
 };
 
