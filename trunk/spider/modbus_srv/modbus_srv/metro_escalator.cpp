using namespace std;

#include <inttypes.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <sstream>
#include <iostream>

#include "defines.h"

#include "socket_exception.h"
#include "generic_socket.h"
#include "server_socket.h"

#include "comport_exception.h"
#include "comport.h"

#include "program_settings.h"
#include "metro_device.h"
#include "metro_escalator.h"

metro_device::command_data_container
     metro_escalator::get_default_command_request_to_comport(){
    program_settings *sys_sett=program_settings::get_instance();
    command_data_container return_val(0);
 	vector<byte> buffer(0);
	word		crc_value;

	buffer.push_back(get_number());
	buffer.push_back(4);                      // function number
	buffer.push_back(0);
	buffer.push_back(0);
	buffer.push_back(0);
	buffer.push_back(38);

	crc_value = sys_sett->crc(buffer);

	program_settings::bytes tmp_bytes=program_settings::bytes_of_type<word>(crc_value);
	buffer.push_back(tmp_bytes[1]);
	buffer.push_back(tmp_bytes[0]);

    return_val.push_back(buffer);
    
   	return return_val;
};

metro_device::command_data
     metro_escalator::get_default_command_request_from_socket(){
    program_settings *sys_sett=program_settings::get_instance();
 	command_data buffer(0);
	word		crc_value;

	buffer.push_back(get_number());
	buffer.push_back(4);                      // function number
	buffer.push_back(0);
	buffer.push_back(0);
	buffer.push_back(0);
	buffer.push_back(38);

	crc_value = sys_sett->crc(buffer);

	program_settings::bytes tmp_bytes=program_settings::bytes_of_type<word>(crc_value);
	buffer.push_back(tmp_bytes[1]);
	buffer.push_back(tmp_bytes[0]);

   	return buffer;
};


void 
      metro_escalator::put_command_answer_from_comport
                   (metro_device::command_data_container answer_from_com_port){
command_data new_answer_to_socket;
byte function_code;
command_data request;
pthread_mutex_lock(metro_device::get_request_from_socket_mutex());
   request=request_from_socket;
pthread_mutex_unlock(metro_device::get_request_from_socket_mutex());

// answer_from_device
//============================================
/*
{
vector<char> tmp_buffer(32);
if (metro_device::get_number()==2) {

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
}; //if (metro_device::get_number()==2 )
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
   	     } else { // if ((device_answer_header[prog
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

if (get_command_request_to_comport()==
    get_default_command_request_to_comport()) {
          metro_device::set_default_answer_to_socket_buffer(new_answer_to_socket);
      };
//===============================================
/*
    if (metro_device::get_number()==2) {
           vector<char> tmp_buffer(32);
           cout<<"new_answer_to_socket size :"<<new_answer_to_socket.size()<<endl;
            for (metro_device::command_data_container::value_type::size_type i=0;
         i<new_answer_to_socket.size();
//              i<4; //only header
              i++) {
              itoa(new_answer_to_socket[i],
                      &tmp_buffer[0], 16);
              if (i==3 ||
                  (i-3)%8==0) cout<<endl;
              cout<<"\t0x"<<&tmp_buffer[0];
           }; //for (metro_device::buffer_data_type::size_type i=0;
          cout<<endl;
       };
*/
//===============================================

metro_device::set_command_answer_to_socket_buffer(new_answer_to_socket);

};

void 
     metro_escalator::put_command_request_from_socket
                   (metro_device::command_data request_from_socket) {
   pthread_mutex_lock(metro_device::get_request_from_socket_mutex());
     this->request_from_socket=request_from_socket;
     metro_device::set_command_request_to_comport_buffer(create_request_to_com_port()); 
   pthread_mutex_unlock(metro_device::get_request_from_socket_mutex());

//========delete it================
/*
{
vector<char> tmp_buffer(32);

if (metro_device::get_number()==2) {
cout<<"\n request_from_socket size "<<request_from_socket.size()<<" tid "<<pthread_self()<<endl;
for (vector<byte>::size_type i=0; i<request_from_socket.size(); i++) {
          itoa(request_from_socket[i],
                  &tmp_buffer[0], 16);
          if (i==3 ||
              (i-3)%8==0) cout<<endl;
          cout<<"\t0x"<<&tmp_buffer[0];
};
cout<<endl;
}; //if (metro_device::get_number()==1 )
};
*/
//=============================

 };

metro_device::command_data_container
         metro_escalator::create_request_to_com_port(){      
    metro_device::command_data_container data_container(0);
    data_container.push_back(request_from_socket);

    return data_container;
};

metro_device::command_data
         metro_escalator::create_answer_to_socket_func_4
               (metro_device::command_data_container answer_from_com_port){
    program_settings* sett_obj=program_settings::get_instance();
    metro_device::messages_container all_messages;
    word crc_value,
             registers_count,
             messages_in_answer_from_comport_count,
             device_fifo_messages_count,
             device_fifo_upper_message_id,
             message_id;
    program_settings::bytes crc_bytes, bytes_of_type_buffer;
    metro_device::command_data tmp_buffer ,buffer_to_return;
    metro_device::messages_container new_messages, messages_fifo;

//========delete it================
/*
{
vector<char> tmp_buffer(32);
if (metro_device::get_number()==2) {
cout<<"\n answer_from_com_port[0] size "<<answer_from_com_port[0].size()<<endl;
for (vector<byte>::size_type i=0; i<answer_from_com_port[0].size(); i++) {
          itoa(answer_from_com_port[0][i],
                  &tmp_buffer[0], 16);
          if (i==3 ||
              (i-3)%8==0) cout<<endl;
          cout<<"\t0x"<<&tmp_buffer[0];
};
cout<<endl;
}; //if (metro_device::get_number()==2)
};
*/
//=============================

//answer from comport of modbus command 4 must be only by default request. 
//so, bytes count in answer always constant
   command_data_container request_to_comport=
                          get_default_command_request_to_comport();
   command_data::iterator tmp_iter3, tmp_iter4;

  command_data::reverse_iterator tmp_iter1=request_to_comport[0].rend(), tmp_iter2=tmp_iter1;

   advance(tmp_iter1,
                  -(program_settings::MODBUS_FUNCTION_4_REQUEST_REGISTERS_COUNT_INDEX));
   advance(tmp_iter2,
                 -(program_settings::MODBUS_FUNCTION_4_REQUEST_REGISTERS_COUNT_INDEX+
                    program_settings::MODBUS_FUNCTION_4_REQUEST_REGISTERS_COUNT_INCREM));

    registers_count=
                   program_settings::type_from_bytes<word>(command_data(tmp_iter2, tmp_iter1));

	if (registers_count*2 !=answer_from_com_port[0][program_settings::MODBUS_DATA_BYTES_COUNT_INDEX]) {
        program_settings* sys_sett=program_settings::get_instance();
        ostringstream exception_description;
        exception_description<<"In metro_escalator::create_answer_to_socket_func_4 device id "<<this->get_number();
        exception_description<<" recieved "<<answer_from_com_port[0][program_settings::MODBUS_DATA_BYTES_COUNT_INDEX]<<" bytes of data ";
        exception_description<<" but must "<<registers_count*2;
        
        sys_sett->sys_message (program_settings::ERROR_MSG, exception_description.str());
       // what can I do? return error message with code ILLEGAL_DATA_ADDRESS
       return metro_device::generate_error_answer_for_function(
                                               metro_device::get_number(), 
                                               4,
                                               program_settings::MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS);
	};

            tmp_iter1=answer_from_com_port[0].rend();
             tmp_iter2=tmp_iter1;
             advance(tmp_iter1,
                        -(MODBUS_FUNCTION_4_ANSWER_MESSAGES_COUNT_INDEX));
             advance(tmp_iter2,
                         -(MODBUS_FUNCTION_4_ANSWER_MESSAGES_COUNT_INDEX+
                            MODBUS_FUNCTION_4_ANSWER_MESSAGES_COUNT_INCREM));
             messages_in_answer_from_comport_count=
                                         program_settings::type_from_bytes<word>(command_data(tmp_iter2, tmp_iter1));


          tmp_iter3=answer_from_com_port[0].begin();
          tmp_iter4=tmp_iter3;
          advance(tmp_iter4,
                         MODBUS_FUNCTION_4_ANSWER_MESSAGES_BEGIN_INDEX);

          buffer_to_return.insert(buffer_to_return.end(),
                                                tmp_iter3,
                                                tmp_iter4);

//            buffer_to_return[buffer_to_return.size()-1]=0xff;

             tmp_iter1=answer_from_com_port[0].rend();
             tmp_iter2=tmp_iter1;
             advance(tmp_iter1,
                        -(MODBUS_FUNCTION_4_ANSWER_MESSAGES_BEGIN_INDEX));
             advance(tmp_iter2,
                         -(MODBUS_FUNCTION_4_ANSWER_MESSAGES_BEGIN_INDEX+2));

          new_messages.clear();
          // if message id & 0x8000!=0 (it`s a not message id, its block circut id)
          //        then it must be added to high byte of previos message

          int i=0;
          while(i<messages_in_answer_from_comport_count &&
                  tmp_iter1!=answer_from_com_port[0].rbegin() &&
                  tmp_iter2!=answer_from_com_port[0].rbegin()) {
           message_id=
           program_settings::type_from_bytes<word>(command_data(tmp_iter2, tmp_iter1));
          if ((message_id & 0x8000)!=0) {
                  if (new_messages.empty()) {
                      program_settings* sys_sett=program_settings::get_instance();
                      ostringstream exception_description;
                      exception_description<<"In metro_escalator::create_answer_to_socket_func_4 device id "<<this->get_number();
                      exception_description<<" recieved message with id "<<message_id<<" (block crcut?) but new_messages.empty()";

                       sys_sett->sys_message (program_settings::ERROR_MSG, exception_description.str());
                   } else { //if (new_messages.empty())
                       message_id=~message_id;
                       message_id+=1;
                       new_messages[new_messages.size()-1]=message_id;
                   }; //if (new_messages.empty())
               } else { //if (message_id && 0x8000!=0) {
                  bytes_of_type_buffer=program_settings::bytes_of_type<word>(message_id);
                  new_messages.insert(new_messages.end(),
                                                    bytes_of_type_buffer.begin(),
                                                    bytes_of_type_buffer.end());
              }; //if (message_id && 0x8000!=0) {

            tmp_iter1=tmp_iter2;
            advance(tmp_iter2, -2);
            i++;
         };

//      cout<<"new_messages.size() "<<new_messages.size()<<endl;
      metro_device::add_messages_fifo (new_messages);

       messages_fifo=metro_device::get_messages_fifo();
       device_fifo_messages_count=messages_fifo.size();
//       cout<<"device_fifo_messages_count "<<device_fifo_messages_count<<endl;
       device_fifo_upper_message_id=metro_device::get_upper_message_id();
//       cout<<"device_fifo_upper_message_id "<<device_fifo_upper_message_id<<endl;
       tmp_buffer=program_settings::bytes_of_type<word>(device_fifo_messages_count/2);
       buffer_to_return[MODBUS_FUNCTION_4_ANSWER_MESSAGES_COUNT_INDEX]=tmp_buffer[0];
       buffer_to_return[MODBUS_FUNCTION_4_ANSWER_MESSAGES_COUNT_INDEX+
                                                   MODBUS_FUNCTION_4_ANSWER_MESSAGES_COUNT_INCREM]=tmp_buffer[1];

       tmp_buffer=program_settings::bytes_of_type<word>(device_fifo_upper_message_id);
       buffer_to_return[MODBUS_FUNCTION_4_ANSWER_UPPER_MESSAGE_ID_INDEX]=tmp_buffer[0];
       buffer_to_return[MODBUS_FUNCTION_4_ANSWER_UPPER_MESSAGE_ID_INDEX+
                                                    MODBUS_FUNCTION_4_ANSWER_UPPER_MESSAGE_ID_INCREM]=tmp_buffer[1];

     for(command_data::size_type i=0;
            i<program_settings::MAX_MESSAGES_COUNT;
             i++) {
          if (i*2<messages_fifo.size()) {
             buffer_to_return.push_back(messages_fifo[i*2+1]);
             buffer_to_return.push_back(messages_fifo[i*2]);
           } else { //if (i<messages_fifo.size())
             buffer_to_return.push_back(0);
             buffer_to_return.push_back(0);
          }; //if (i<messages_fifo.size())
      }; // for(command_data::size_type i=0;

      crc_value=sett_obj->crc(command_data(buffer_to_return.begin(), buffer_to_return.end()));
      crc_bytes=program_settings::bytes_of_type<word>(crc_value);    

      buffer_to_return.push_back(crc_bytes[1]);
      buffer_to_return.push_back(crc_bytes[0]);

  return buffer_to_return;
};

metro_device::command_data_container 
     metro_escalator::get_command_request_to_comport(){
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
 
