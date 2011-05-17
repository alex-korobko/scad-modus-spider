using namespace std;

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

#include "defines.h"

#include "socket_exception.h"
#include "generic_socket.h"
#include "server_socket.h"

#include "comport_exception.h"
#include "comport.h"

#include "program_settings.h"
#include "metro_device.h"
#include "metro_shavr.h"

void metro_shavr::A0(int event) 
          throw (metro_device::metro_device_exception){
 int old_automat_state=A0_state;
 
 switch (A0_state) {
    case A0_OFFLINE:
         if (A0_x1 &&
             !A0_x2) {
           A0_state=A0_EXCEPTION;
           break;
         };
         if (A0_x1 &&
             !A0_x3 && !A0_x4) {
           A0_state=A0_DISABLED_1_DISABLED_2;
           break;
         };
         if (A0_x1 &&
             A0_x3 && A0_x4) {
           A0_state=A0_ENABLED_1_ENABLED_2;
           break;
         };
         if (A0_x1 &&
             !A0_x3 && A0_x4) {
           A0_state=A0_DISABLED_1_ENABLED_2;
           break;
         };
         if (A0_x1 &&
             A0_x3 && !A0_x4) {
           A0_state=A0_ENABLED_1_DISABLED_2;
           break;
         };
     break;
    case A0_EXCEPTION:
          if (!A0_x1) {
           A0_state=A0_OFFLINE;
           break;
         };
         if (A0_x2 &&
             !A0_x3 && !A0_x4) {
           A0_state=A0_DISABLED_1_DISABLED_2;
           break;
         };
         if (A0_x2 &&
             A0_x3 && A0_x4) {
           A0_state=A0_ENABLED_1_ENABLED_2;
           break;
         };
         if (A0_x2 &&
             !A0_x3 && A0_x4) {
           A0_state=A0_DISABLED_1_ENABLED_2;
           break;
         };
         if (A0_x2 &&
             A0_x3 && !A0_x4) {
           A0_state=A0_ENABLED_1_DISABLED_2;
           break;
         };     
    break;
    case A0_DISABLED_1_DISABLED_2:
          if (!A0_x1) {
           A0_state=A0_OFFLINE;
           break;
          };
          if (!A0_x2) {
           A0_state=A0_EXCEPTION;
           break;
          };
         if (event==1) {
                A0_z1();
               break;
             };
         if (A0_x3 && A0_x4) {
           A0_state=A0_ENABLED_1_ENABLED_2;
           break;
         };
         if (!A0_x3 && A0_x4) {
           A0_state=A0_DISABLED_1_ENABLED_2;
           break;
         };
         if (A0_x3 && !A0_x4) {
           A0_state=A0_ENABLED_1_DISABLED_2;
           break;
         };     
     break;
    case A0_ENABLED_1_ENABLED_2:
          if (!A0_x1) {
           A0_state=A0_OFFLINE;
           break;
          };
          if (!A0_x2) {
           A0_state=A0_EXCEPTION;
           break;
          };
         if (event==1) {
                A0_z1();
               break;
             };
         if (!A0_x3 && !A0_x4) {
           A0_state=A0_DISABLED_1_DISABLED_2;
           break;
         };
         if (!A0_x3 && A0_x4) {
           A0_state=A0_DISABLED_1_ENABLED_2;
           break;
         };
         if (A0_x3 && !A0_x4) {
           A0_state=A0_ENABLED_1_DISABLED_2;
           break;
         };     
     break;
    case A0_DISABLED_1_ENABLED_2:
          if (!A0_x1) {
           A0_state=A0_OFFLINE;
           break;
          };
          if (!A0_x2) {
           A0_state=A0_EXCEPTION;
           break;
          };
         if (event==1) {
                A0_z1();
               break;
             };
         if (!A0_x3 && !A0_x4) {
           A0_state=A0_DISABLED_1_DISABLED_2;
           break;
         };
         if (A0_x3 && A0_x4) {
           A0_state=A0_ENABLED_1_ENABLED_2;
           break;
         };
         if (A0_x3 && !A0_x4) {
           A0_state=A0_ENABLED_1_DISABLED_2;
           break;
         };     
     break;
    case A0_ENABLED_1_DISABLED_2:
          if (!A0_x1) {
           A0_state=A0_OFFLINE;
           break;
          };
          if (!A0_x2) {
           A0_state=A0_EXCEPTION;
           break;
          };
         if (event==1) {
                A0_z1();
               break;
             };
         if (!A0_x3 && !A0_x4) {
           A0_state=A0_DISABLED_1_DISABLED_2;
           break;
         };
         if (A0_x3 && A0_x4) {
           A0_state=A0_ENABLED_1_ENABLED_2;
           break;
         };
         if (!A0_x3 && A0_x4) {
           A0_state=A0_DISABLED_1_ENABLED_2;
           break;
         };     
     break;
    default :
      ostringstream exception_description;
      exception_description<<"metro_shavr::A0(...) device number "<<metro_device::get_number();
      exception_description<<" wrong A0 state "<<A0_state;
      throw metro_device::metro_device_exception(exception_description.str());        
 }; //switch (A0_state)

 if (old_automat_state==A0_state) return;
 
//initializations on enter to new state
 switch (A0_state) {
     case A0_OFFLINE:
     A0_z2();
     break;
     case A0_EXCEPTION:
     A0_z2();
     break;
     case A0_DISABLED_1_DISABLED_2:
     break;
     case A0_ENABLED_1_ENABLED_2:
     break;
     case A0_DISABLED_1_ENABLED_2:
     break;
     case A0_ENABLED_1_DISABLED_2:
     break;
     default :
      ostringstream exception_description;
      exception_description<<"metro_shavr::A0(...) device number "<<metro_device::get_number();
      exception_description<<" after change A0 has undefined state "<<A0_state;
      throw metro_device::metro_device_exception(exception_description.str());        
 }; //switch (A0_state)
};

metro_device::command_data_container
     metro_shavr::get_default_command_request_to_comport(){
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

// com port default request for  first power counter
    buffer.clear();
	buffer.push_back(get_number()+1);
	buffer.push_back(4);                      // function number
	buffer.push_back(0);
	buffer.push_back(POWER_COUNTER_1_FIRST_REGISTER);
	buffer.push_back(0);
	buffer.push_back(POWER_COUNTER_1_MODBUS_REGISTERS_COUNT);

	tmp_bytes=program_settings::bytes_of_type<word>(program_settings::crc(buffer));
	buffer.insert(buffer.end(), tmp_bytes.begin(), tmp_bytes.end());
    return_val.push_back(buffer);

// com port default request for second power counter
    buffer.clear();
	buffer.push_back(get_number()+2);
	buffer.push_back(4);                      // function number
	buffer.push_back(0);
	buffer.push_back(POWER_COUNTER_2_FIRST_REGISTER);
	buffer.push_back(0);
	buffer.push_back(POWER_COUNTER_2_MODBUS_REGISTERS_COUNT);

	tmp_bytes=program_settings::bytes_of_type<word>(program_settings::crc(buffer));
	buffer.insert(buffer.end(),tmp_bytes.begin(),tmp_bytes.end());
    return_val.push_back(buffer);

   	return return_val;
};

metro_device::command_data
     metro_shavr::get_default_command_request_from_socket(){
 	command_data buffer(0);


	buffer.push_back(get_number());
	buffer.push_back(4);                      // function number
	buffer.push_back(0);
	buffer.push_back(UDKU_FIRST_REGISTER);
	buffer.push_back(0);
	buffer.push_back(UDKU_MODBUS_REGISTERS_COUNT+
                                 POWER_COUNTER_1_MODBUS_REGISTERS_COUNT+
                                 POWER_COUNTER_2_MODBUS_REGISTERS_COUNT+
                                 2+//MESSAGES_UPPER_ID+MESSAGES_COUNT
                                 MESSAGES_MODBUS_REGISTERS_COUNT);

	program_settings::bytes tmp_bytes=program_settings::bytes_of_type<word>(program_settings::crc(buffer));
	buffer.insert(buffer.end(), tmp_bytes.begin(),  tmp_bytes.end());
   	return buffer;
};

void 
      metro_shavr::put_command_answer_from_comport
                   (metro_device::command_data_container answer_from_com_port){
command_data new_answer_to_socket(0);
byte function_code;
metro_device::command_data request;
pthread_mutex_lock(metro_device::get_request_from_socket_mutex());
   request=request_from_socket;
pthread_mutex_unlock(metro_device::get_request_from_socket_mutex());


// cout<<"answer_from_com_port.size() "
//       <<answer_from_com_port.size()
//       <<" answer_from_com_port[0].size() "<<(answer_from_com_port.empty()?0:static_cast<int>(answer_from_com_port[0].size()))
//       <<" function_code "<<static_cast<int>(function_code)<<endl;

//========delete it================
/*
 {
 cout<<"\nshavr request size "<<request.size()<<endl;
 vector<char> tmp_chars(10);
 for (int i=0;
      i<static_cast<int>(request.size());
      i++) {
 itoa(request[i],
       &tmp_chars[0],
       16);
   if (i%9==0) cout<<endl;
   cout<<"  0x"<<&tmp_chars[0];
 };
 cout<<endl;
 };
*/
//=============================


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
               case 4: {
                new_answer_to_socket=create_answer_to_socket_func_4(answer_from_com_port);
                break;
               };
               default: { //if error or write data function
               new_answer_to_socket=answer_from_com_port[0];
             };
          }; //switch (function_code)
         }; // if ((device_answer_header[prog
};

//========delete it================
/*
{
cout<<"\nshavr new_answer_to_socket size "<<new_answer_to_socket.size()<<endl;
vector<char> tmp_chars(10);
for (int i=0;
      i<static_cast<int>(new_answer_to_socket.size());
      i++) {
itoa(new_answer_to_socket[i],
       &tmp_chars[0],
       16);
   if (i%9==0) cout<<endl;
   cout<<"  0x"<<&tmp_chars[0];
 };
 cout<<endl;
 };
*/
//=============================

if (get_command_request_to_comport()==
     get_default_command_request_to_comport()) {
          metro_device::set_default_answer_to_socket_buffer(new_answer_to_socket);
    };

metro_device::set_command_answer_to_socket_buffer(new_answer_to_socket);
};

void 
     metro_shavr::put_command_request_from_socket
                   (metro_device::command_data request_from_socket) {

   pthread_mutex_lock(metro_device::get_request_from_socket_mutex());
     this->request_from_socket=request_from_socket;
   pthread_mutex_unlock(metro_device::get_request_from_socket_mutex());

   metro_device::set_command_request_to_comport_buffer(create_request_to_com_port()); 
 };

metro_device::command_data_container
         metro_shavr::create_request_to_com_port(){      
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

             //detect is command for shavr
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
                                 if (local_request[4]==0) {
                                 new_messages.push_back(0);
                                 new_messages.push_back(MESSAGE_RECIEVED_ESCALATOR_1_CONNECT_TO_INPUT_1);
                                 } else {
                                 new_messages.push_back(0);
                                 new_messages.push_back(MESSAGE_RECIEVED_ESCALATOR_1_DISCONNECT_FROM_INPUT_1);
                                 };
                                 break;
                               case 1:
                                 if (local_request[4]==0) {
                                 new_messages.push_back(0);
                                 new_messages.push_back(MESSAGE_RECIEVED_ESCALATOR_1_CONNECT_TO_INPUT_2);
                                 } else {
                                 new_messages.push_back(0);
                                 new_messages.push_back(MESSAGE_RECIEVED_ESCALATOR_1_DISCONNECT_FROM_INPUT_2);
                                 };
                                 break;
                               case 2:
                                 if (local_request[4]==0) {
                                 new_messages.push_back(0);
                                 new_messages.push_back(MESSAGE_RECIEVED_ESCALATOR_2_CONNECT_TO_INPUT_1);
                                 } else {
                                 new_messages.push_back(0);
                                 new_messages.push_back(MESSAGE_RECIEVED_ESCALATOR_2_DISCONNECT_FROM_INPUT_1);
                                 };
                                 break;
                               case 3:
                                 if (local_request[4]==0) {
                                 new_messages.push_back(0);
                                 new_messages.push_back(MESSAGE_RECIEVED_ESCALATOR_2_CONNECT_TO_INPUT_2);
                                 } else {
                                 new_messages.push_back(0);
                                 new_messages.push_back(MESSAGE_RECIEVED_ESCALATOR_2_DISCONNECT_FROM_INPUT_2);
                                 };
                                 break;
                               case 4:
                                 if (local_request[4]==0) {
                                 new_messages.push_back(0);
                                 new_messages.push_back(MESSAGE_RECIEVED_ESCALATOR_3_CONNECT_TO_INPUT_1);
                                 } else {
                                 new_messages.push_back(0);
                                 new_messages.push_back(MESSAGE_RECIEVED_ESCALATOR_3_DISCONNECT_FROM_INPUT_1);
                                 };
                                 break;
                               case 5:
                                 if (local_request[4]==0) {
                                 new_messages.push_back(0);
                                 new_messages.push_back(MESSAGE_RECIEVED_ESCALATOR_3_CONNECT_TO_INPUT_2);
                                 } else {
                                 new_messages.push_back(0);
                                 new_messages.push_back(MESSAGE_RECIEVED_ESCALATOR_3_DISCONNECT_FROM_INPUT_2);
                                 };
                                 break;
                               case 6:
                                 if (local_request[4]==0) {
                                 new_messages.push_back(0);
                                 new_messages.push_back(MESSAGE_RECIEVED_ESCALATOR_4_CONNECT_TO_INPUT_1);
                                 } else {
                                 new_messages.push_back(0);
                                 new_messages.push_back(MESSAGE_RECIEVED_ESCALATOR_4_DISCONNECT_FROM_INPUT_1);
                                 };
                                 break;
                               case 7:
                                 if (local_request[4]==0) {
                                 new_messages.push_back(0);
                                 new_messages.push_back(MESSAGE_RECIEVED_ESCALATOR_4_CONNECT_TO_INPUT_2);
                                 } else {
                                 new_messages.push_back(0);
                                 new_messages.push_back(MESSAGE_RECIEVED_ESCALATOR_4_DISCONNECT_FROM_INPUT_2);
                                 };
                                 break;
                            };
                            if (!new_messages.empty()) metro_device::add_messages_fifo (new_messages);
                       }; //if (calc_crc==org_crc)
              }; //if (local_request.size()==8 &&


         };
    return data_container;
};

metro_device::command_data
    metro_shavr::create_answer_to_socket_func_4
               (metro_device::command_data_container answer_from_com_port){
    metro_device::messages_container all_messages;

    program_settings::bytes tmp_value_bytes;
    metro_device::command_data  answer_to_socket(0);
    metro_device::messages_container new_messages(0);
    metro_device::command_data::iterator iter_beg, iter_end;
    int old_A0_state=A0_state;
    bool old_A0_x3=A0_x3,
            old_A0_x4=A0_x4,
	        old_A0_x10=A0_x10,
	        old_A0_x11=A0_x11,
	        old_A0_x12=A0_x12,
	        old_A0_x13=A0_x13,
	        old_A0_x14=A0_x14,
	        old_A0_x15=A0_x15,
	        old_A0_x16=A0_x16,
	        old_A0_x17=A0_x17,
            old_A1_x3=A1_x3,
            old_A2_x3=A2_x3,
            old_A3_x3=A3_x3;

//=======delete it====================
//{
//cout <<"answer_from_com_port[0] from shavr"<<endl;
//  vector<char> tmp_chars(40);
//	for(metro_device::command_data::size_type i=0;
//	      i<answer_from_com_port[0].size();
//	      i++) {
//	  if (i%8 ==0) cout<<endl;
//	  itoa(answer_from_com_port[0][i],
//	        &tmp_chars[0],
//	        16);
//	  cout<<"  0x"<<&tmp_chars[0];
//   };
//   cout<<endl;
//}
//==================================


//     UDKU
	if(answer_from_com_port[0].size()!=
	    program_settings::MODBUS_HEADER_SIZE+
        UDKU_REGISTERS_COUNT*UDKU_REGISTER_INCREM+
        program_settings::MODBUS_CRC_SIZE) {
                  A0_x2=false;
                  A0(7); // event 7 - data refreshed
                  return metro_device::generate_error_answer_for_function
                     ( get_number(),
                       4,
                       program_settings::MODBUS_EXCEPTION_FAILURE_IN_ASSOCIATED_DEVICE);
             }; //if(answer_from_com_port[0].size()!=

        answer_to_socket.push_back(get_number());
        answer_to_socket.push_back(4);
        answer_to_socket.push_back(
           UDKU_MODBUS_REGISTERS_COUNT*UDKU_MODBUS_REGISTER_INCREM+
           POWER_COUNTER_1_MODBUS_REGISTERS_COUNT*POWER_COUNTER_1_MODBUS_REGISTER_INCREM+
           POWER_COUNTER_2_MODBUS_REGISTERS_COUNT*POWER_COUNTER_2_MODBUS_REGISTER_INCREM+
           MESSAGES_UPPER_ID_INCREM+MESSAGES_COUNT_INCREM+
           MESSAGES_MODBUS_REGISTERS_COUNT*MESSAGES_INCREM);
        iter_beg=answer_from_com_port[0].begin();
        iter_end=iter_beg;
        advance(iter_beg, program_settings::MODBUS_HEADER_SIZE);
        advance(iter_end, answer_from_com_port[0].size()-program_settings::MODBUS_CRC_SIZE);
        answer_to_socket.insert(answer_to_socket.end(),
                        iter_beg, iter_end);

//autmatas variables
          //global
           A0_x1=true;
           A0_x2=true;
           A0_x4=false;
          //escalator 3
           A0_x14=
              ((answer_from_com_port[0][program_settings::MODBUS_DATA_BYTES_COUNT_INDEX+1]&0x1)!=0);
           A0_x22=
              ((answer_from_com_port[0][program_settings::MODBUS_DATA_BYTES_COUNT_INDEX+1]&0x2)!=0);
           A0_x15=
              ((answer_from_com_port[0][program_settings::MODBUS_DATA_BYTES_COUNT_INDEX+1]&0x4)!=0);
           A0_x23=
              ((answer_from_com_port[0][program_settings::MODBUS_DATA_BYTES_COUNT_INDEX+1]&0x8)!=0);
          //escalator 4
           A0_x16=
              ((answer_from_com_port[0][program_settings::MODBUS_DATA_BYTES_COUNT_INDEX+1]&0x10)!=0);
           A0_x24=
              ((answer_from_com_port[0][program_settings::MODBUS_DATA_BYTES_COUNT_INDEX+1]&0x20)!=0);
           A0_x17=
              ((answer_from_com_port[0][program_settings::MODBUS_DATA_BYTES_COUNT_INDEX+1]&0x40)!=0);
           A0_x25=
              ((answer_from_com_port[0][program_settings::MODBUS_DATA_BYTES_COUNT_INDEX+1]&0x80)!=0);
          //escalator 1
           A0_x10=
              ((answer_from_com_port[0][program_settings::MODBUS_DATA_BYTES_COUNT_INDEX+2]&0x1)!=0);
           A0_x18=
              ((answer_from_com_port[0][program_settings::MODBUS_DATA_BYTES_COUNT_INDEX+2]&0x2)!=0);
           A0_x11=
              ((answer_from_com_port[0][program_settings::MODBUS_DATA_BYTES_COUNT_INDEX+2]&0x4)!=0);
           A0_x19=
              ((answer_from_com_port[0][program_settings::MODBUS_DATA_BYTES_COUNT_INDEX+2]&0x8)!=0);
          //escalator 2
           A0_x12=
              ((answer_from_com_port[0][program_settings::MODBUS_DATA_BYTES_COUNT_INDEX+2]&0x10)!=0);
           A0_x20=
              ((answer_from_com_port[0][program_settings::MODBUS_DATA_BYTES_COUNT_INDEX+2]&0x20)!=0);
           A0_x13=
              ((answer_from_com_port[0][program_settings::MODBUS_DATA_BYTES_COUNT_INDEX+2]&0x40)!=0);
           A0_x21=
              ((answer_from_com_port[0][program_settings::MODBUS_DATA_BYTES_COUNT_INDEX+2]&0x80)!=0);
          //other
           A0_x3=
              ((answer_from_com_port[0][program_settings::MODBUS_DATA_BYTES_COUNT_INDEX+4]&0x4)!=0);
           A0_x4=
              ((answer_from_com_port[0][program_settings::MODBUS_DATA_BYTES_COUNT_INDEX+4]&0x8)!=0);
           A1_x3=
              ((answer_from_com_port[0][program_settings::MODBUS_DATA_BYTES_COUNT_INDEX+3]&0x4)!=0);
           A2_x3=
              ((answer_from_com_port[0][program_settings::MODBUS_DATA_BYTES_COUNT_INDEX+4]&0x2)==0);
           A3_x3=
              ((answer_from_com_port[0][program_settings::MODBUS_DATA_BYTES_COUNT_INDEX+4]&0x1)==0);


             A0(7); // event 7 - data refreshed
             new_messages.clear();
             if (old_A0_state!=A0_state) {
             };//if (old_A0_state!=A0_state)

            if (old_A0_state!=A0_OFFLINE &&
                 old_A0_state!=A0_EXCEPTION) {
             if (old_A0_x3!=A0_x3) {
                  if (A0_x3) {
                          new_messages.push_back(0);
                          new_messages.push_back(MESSAGE_INPUT_1_CONNECTED);
                    } else {
                          new_messages.push_back(0);
                          new_messages.push_back(MESSAGE_INPUT_1_DISCONNECTED);
                    };
              }; //if (old_A0_x10!=A0_x10)

             if (old_A0_x4!=A0_x4) {
                  if (A0_x4) {
                          new_messages.push_back(0);
                          new_messages.push_back(MESSAGE_INPUT_2_CONNECTED);
                    } else {
                          new_messages.push_back(0);
                          new_messages.push_back(MESSAGE_INPUT_2_DISCONNECTED);
                    };
              }; //if (old_A0_x10!=A0_x10)

             if (old_A0_x10!=A0_x10) {
                      if (A0_x10) {
                          new_messages.push_back(0);
                          new_messages.push_back(MESSAGE_ESCALATOR_1_CONNECTED_TO_INPUT_1);
                        } else {
                          new_messages.push_back(0);
                          new_messages.push_back(MESSAGE_ESCALATOR_1_DISCONNECTED_FROM_INPUT_1);
                        };
              }; //if (old_A0_x10!=A0_x10)

              if (old_A0_x11!=A0_x11) {
                      if (A0_x11) {
                          new_messages.push_back(0);
                          new_messages.push_back(MESSAGE_ESCALATOR_1_CONNECTED_TO_INPUT_2);
                        } else {
                          new_messages.push_back(0);
                          new_messages.push_back(MESSAGE_ESCALATOR_1_DISCONNECTED_FROM_INPUT_2);
                        };
              }; //if (old_A0_x11!=A0_x11)

		       if (old_A0_x12!=A0_x12) {
                      if (A0_x12) {
                          new_messages.push_back(0);
                          new_messages.push_back(MESSAGE_ESCALATOR_2_CONNECTED_TO_INPUT_1);
                        } else {
                          new_messages.push_back(0);
                          new_messages.push_back(MESSAGE_ESCALATOR_2_DISCONNECTED_FROM_INPUT_1);
                        };
               }; //if (old_A0_x12!=A0_x12)

               if (old_A0_x13!=A0_x13) {
                      if (A0_x13) {
                          new_messages.push_back(0);
                          new_messages.push_back(MESSAGE_ESCALATOR_2_CONNECTED_TO_INPUT_2);
                        } else {
                          new_messages.push_back(0);
                          new_messages.push_back(MESSAGE_ESCALATOR_2_DISCONNECTED_FROM_INPUT_2);
                        };
               }; //if (old_A0_x13!=A0_x13)

	           if (old_A0_x14!=A0_x14){
                      if (A0_x14) {
                          new_messages.push_back(0);
                          new_messages.push_back(MESSAGE_ESCALATOR_3_CONNECTED_TO_INPUT_1);
                        } else {
                          new_messages.push_back(0);
                          new_messages.push_back(MESSAGE_ESCALATOR_3_DISCONNECTED_FROM_INPUT_1);
                        };
               }; //if (old_A0_x14!=A0_x14)

	           if (old_A0_x15!=A0_x15) {
                      if (A0_x15) {
                          new_messages.push_back(0);
                          new_messages.push_back(MESSAGE_ESCALATOR_3_CONNECTED_TO_INPUT_2);
                        } else {
                          new_messages.push_back(0);
                          new_messages.push_back(MESSAGE_ESCALATOR_3_DISCONNECTED_FROM_INPUT_2);
                        };
               }; //if (old_A0_x15!=A0_x15)

	           if (old_A0_x16!=A0_x16) {
                      if (A0_x16) {
                          new_messages.push_back(0);
                          new_messages.push_back(MESSAGE_ESCALATOR_4_CONNECTED_TO_INPUT_1);
                        } else {
                          new_messages.push_back(0);
                          new_messages.push_back(MESSAGE_ESCALATOR_4_DISCONNECTED_FROM_INPUT_1);
                        };
                }; //if (old_A0_x16!=A0_x16)

	           if (old_A0_x17!=A0_x17) {
                      if (A0_x17) {
                          new_messages.push_back(0);
                          new_messages.push_back(MESSAGE_ESCALATOR_4_CONNECTED_TO_INPUT_2);
                        } else {
                          new_messages.push_back(0);
                          new_messages.push_back(MESSAGE_ESCALATOR_4_DISCONNECTED_FROM_INPUT_2);
                        };
               }; //if (old_A0_x17!=A0_x17)

               if(old_A1_x3!=A1_x3) {
                     if (A1_x3) {
                          new_messages.push_back(0);
                          new_messages.push_back(MESSAGE_DOOR_OPENED);
                          };
               }; //if(old_A1_x3!=A1_x3)

               if (old_A2_x3!=A2_x3 ) {
                     if (A2_x3) {
                          new_messages.push_back(0);
                          new_messages.push_back(MESSAGE_FIRE_ALARM_ACTIVATED);
                          };
               }; //if (old_A2_x3!=A2_x3 )

               if (old_A3_x3!=A3_x3 ) {
                     if (A3_x3) {
                          new_messages.push_back(0);
                          new_messages.push_back(MESSAGE_FIRE_ALARM_ACTIVATED);
                          };
               }; //if (old_A3_x3!=A3_x3 )


            if (!new_messages.empty())
                metro_device::add_messages_fifo (new_messages);
           }; //

//    first power counter
	if(answer_from_com_port[1].size()!=
	    program_settings::MODBUS_HEADER_SIZE+
       POWER_COUNTER_1_MODBUS_REGISTERS_COUNT*POWER_COUNTER_1_MODBUS_REGISTER_INCREM+
        program_settings::MODBUS_CRC_SIZE) {
            answer_to_socket.insert(answer_to_socket.end(),
                POWER_COUNTER_1_MODBUS_REGISTERS_COUNT*POWER_COUNTER_1_MODBUS_REGISTER_INCREM,
                0);
         } else { //if(answer_from_com_port[1].size()!=
               iter_beg=answer_from_com_port[1].begin();
               iter_end=iter_beg;
               advance(iter_beg, program_settings::MODBUS_HEADER_SIZE);
               advance(iter_end, answer_from_com_port[1].size()-program_settings::MODBUS_CRC_SIZE);
               answer_to_socket.insert(answer_to_socket.end(),
                                 iter_beg, iter_end);
         };//if(answer_from_com_port[1].size()!=

//   second power counter
	if(answer_from_com_port[2].size()!=
	    program_settings::MODBUS_HEADER_SIZE+
        POWER_COUNTER_2_MODBUS_REGISTERS_COUNT*POWER_COUNTER_2_MODBUS_REGISTER_INCREM+
        program_settings::MODBUS_CRC_SIZE) {
            answer_to_socket.insert(answer_to_socket.end(),
                POWER_COUNTER_2_MODBUS_REGISTERS_COUNT*POWER_COUNTER_2_MODBUS_REGISTER_INCREM,
                0);
         } else { //if(answer_from_com_port[2].size()!=
               iter_beg=answer_from_com_port[2].begin();
               iter_end=iter_beg;
               advance(iter_beg, program_settings::MODBUS_HEADER_SIZE);
               advance(iter_end, answer_from_com_port[2].size()-program_settings::MODBUS_CRC_SIZE);
               answer_to_socket.insert(answer_to_socket.end(),
                                 iter_beg, iter_end);
         };//if(answer_from_com_port[2].size()!=

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
           POWER_COUNTER_1_MODBUS_REGISTERS_COUNT*POWER_COUNTER_1_MODBUS_REGISTER_INCREM+
           POWER_COUNTER_2_MODBUS_REGISTERS_COUNT*POWER_COUNTER_2_MODBUS_REGISTER_INCREM+
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
//{
//cout <<"answer_to_socket for shavr"<<endl;
//  vector<char> tmp_chars(40);
//	for(metro_device::command_data::size_type i=0;
//	      i<answer_to_socket.size();
//	      i++) {
//	  if (i%8 ==0) cout<<endl;
//	  itoa(answer_to_socket[i],
//	        &tmp_chars[0],
//	        16);
//	  cout<<"  0x"<<&tmp_chars[0];
//   };
//   cout<<endl;
//}
//==================================

  return answer_to_socket;
};

metro_device::command_data_container 
     metro_shavr::get_command_request_to_comport(){
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
 
