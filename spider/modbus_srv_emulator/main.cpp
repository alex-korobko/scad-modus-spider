using namespace std;

#include <Ap.h>

#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/neutrino.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/syspage.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <net/route.h>
#include <sys/ioctl.h> 

#include <signal.h>
#include <inttypes.h>

#include <photon/PxProto.h>
#include <iostream.h>

#include <string>
#include <vector>
#include <map>
#include <hash_map>
#include <functional>
#include <sstream>
#include <fstream>

#include "defines.h"
#include "spider_exception.h"
#include "system_settings.h"
#include "system_settings_spider.h"
#include "program_settings.h"
#include "msg_dict_container.h"
#include "metro_device.h"
#include "socket_exception.h"
#include "generic_socket.h"
#include "server_socket.h"
#include "client_socket.h"

string welcome_message("");
double comport_delay_koeff=1;

// Private function to read response data from file: device_number/command_number.txt
static metro_device::command_data read_response_from_file(byte device_number, byte command_code) {
    metro_device::command_data response_data;    
    
    try {
        // Create file path: device_number/command_number.txt
        ostringstream file_path;
        vector<char> tmp_chars(20);
        itoa(device_number, &tmp_chars[0], 10);
        file_path << &tmp_chars[0] << "/";
        itoa(command_code, &tmp_chars[0], 10);
        file_path << &tmp_chars[0] << ".txt";
        
        // Open file for reading
        ifstream response_file(file_path.str().c_str());
        if (!response_file.is_open()) {
            return response_data; // Return empty vector if file doesn't exist
        }
        
        // Read file content - each line contains one hexadecimal value (e.g., 0x2 or 0x26)
        string line;
        while (getline(response_file, line)) {
            // Trim whitespace from line
            size_t start = line.find_first_not_of(" \t\r\n");
            if (start == string::npos) {
                continue; // Skip empty lines
            }
            size_t end = line.find_last_not_of(" \t\r\n");
            string trimmed_line = line.substr(start, end - start + 1);
            
            // Skip empty lines after trimming
            if (trimmed_line.empty()) {
                continue;
            }
            
            // Skip lines that start with # (comments)
            if (trimmed_line[0] == '#') {
                continue;
            }
            
            // Parse hex value (format: 0xNN or 0XNN)
            unsigned int byte_value = 0;
            bool parsed = false;
            
            if (trimmed_line.length() >= 2 && (trimmed_line.substr(0, 2) == "0x" || trimmed_line.substr(0, 2) == "0X")) {
                // Format: 0xNN
                istringstream hex_stream(trimmed_line);
                if (hex_stream >> hex >> byte_value) {
                    parsed = true;
                }
            } else {
                // Try to parse as hex without 0x prefix
                istringstream hex_stream(trimmed_line);
                if (hex_stream >> hex >> byte_value) {
                    parsed = true;
                }
            }
            
            if (parsed && byte_value <= 0xFF) {
                response_data.push_back(static_cast<byte>(byte_value));
            }
        }
        
        response_file.close();
    } catch (...) {
        // Return empty vector on any error
        response_data.clear();
    }
    
    return response_data;
}

void log_buffer( string prefix,
	metro_device::command_data device_answer )
{
    program_settings *sett_obj=program_settings::get_instance();
    ostringstream descrpt_message;
	descrpt_message<< prefix;
    vector<char> tmp_chars(32);
    for (metro_device::command_data::size_type
            i =0; i<device_answer.size();i++) {
                 itoa(device_answer[i], &tmp_chars[0], 16);
                 descrpt_message<<" 0x"<<&tmp_chars[0];
			if (i>0 && (i%5)==0) {descrpt_message<<endl;}
	};

  sett_obj->sys_message(program_settings::INFO_MSG, descrpt_message.str());
}

void* socket_thread_function(void* socket_ptr){
	if (socket_ptr==NULL) return 0;
    program_settings* sett_obj=program_settings::get_instance();
	generic_socket *socket_device=static_cast<server_socket*>(socket_ptr);

   metro_device::command_data telemetr_request;
   metro_device::command_data::iterator tmp_iter;
   metro_device::command_data 
            telemetr_answer;

   program_settings::bytes telemetr_crc;
   word crc_value;
   int  failures_count=0;

  while(failures_count<program_settings::SOCKET_FAILURES_COUNT) {
      try {
      telemetr_request.clear();
	    socket_device->recv(telemetr_request,
                                            program_settings::MODBUS_REQUEST_HEADER_SIZE);

        switch (telemetr_request[program_settings::MODBUS_FUNCTION_CODE_INDEX]) {
             case 4: {
  	          socket_device->recv(telemetr_request,
                                            program_settings::MODBUS_FUNCTION_4_REQUEST_PARTIAL_SIZE);
             break; };
             case 5: {
  	          socket_device->recv(telemetr_request,
                                            program_settings::MODBUS_FUNCTION_5_REQUEST_PARTIAL_SIZE);
             break; };
             case 6: {
  	          socket_device->recv(telemetr_request,
                                            program_settings::MODBUS_FUNCTION_6_REQUEST_PARTIAL_SIZE);
             break; };
             case 16: {
  	          socket_device->recv(telemetr_request,
                                            program_settings::MODBUS_FUNCTION_16_REQUEST_PARTIAL_SIZE);
              byte data_bytes_count=telemetr_request[telemetr_request.size()-1];
  	          socket_device->recv(telemetr_request,
                                            data_bytes_count);
             break; } ;
             default: 
              string message="In request to device ";
              vector<char> tmp_chars(20);
              itoa(telemetr_request[program_settings::MODBUS_DEVICE_NUMBER_INDEX], &tmp_chars[0], 10);
              message+=&tmp_chars[0];
              message+=" not supported function ";
              itoa(telemetr_request[program_settings::MODBUS_FUNCTION_CODE_INDEX], &tmp_chars[0], 10);
              message+=&tmp_chars[0];
              sett_obj->sys_message(program_settings::ERROR_MSG, message);
              failures_count++;
              telemetr_answer=metro_device::generate_error_answer_for_function ( 
                                             telemetr_request[program_settings::MODBUS_DEVICE_NUMBER_INDEX],
                                             telemetr_request[program_settings::MODBUS_FUNCTION_CODE_INDEX],
                                             program_settings::MODBUS_EXCEPTION_ILLEGAL_FUNCTION);
  		      socket_device->send(telemetr_answer);
              continue;
         };
	     socket_device->recv(telemetr_request, program_settings::MODBUS_CRC_SIZE);

         //CRC check
         if (telemetr_request.size()<=program_settings::MODBUS_CRC_SIZE)   throw socket_exception ("telemetr_request.size()<=program_settings::MODBUS_CRC_SIZE]");
         tmp_iter=telemetr_request.begin();
         advance(tmp_iter, telemetr_request.size()-program_settings::MODBUS_CRC_SIZE);
		 crc_value=program_settings::crc(metro_device::command_data(
		                                                                   telemetr_request.begin(),
		                                                                   tmp_iter));

        telemetr_crc=program_settings::bytes_of_type<word>(crc_value);
		   if (telemetr_crc[0]!=telemetr_request[telemetr_request.size()-2] ||
		       telemetr_crc[1]!=telemetr_request[telemetr_request.size()-1] ) { 
		         throw socket_exception("Bad packet CRC");
                 };

          // Print request to stdout
          cout << "Request to device: ";
          vector<char> tmp_chars(32);
          for (metro_device::command_data::size_type i = 0; i < telemetr_request.size(); i++) {
              itoa(telemetr_request[i], &tmp_chars[0], 16);
              cout << "0x" << &tmp_chars[0];
              if (i < telemetr_request.size() - 1) cout << " ";
          }
          cout << endl;

          // Read response data from file
          byte device_number = telemetr_request[program_settings::MODBUS_DEVICE_NUMBER_INDEX];
          byte command_code = telemetr_request[program_settings::MODBUS_FUNCTION_CODE_INDEX];
          telemetr_answer = read_response_from_file(device_number, command_code);
          
          // If file response is empty, generate error response
          if (telemetr_answer.empty()) {
              ostringstream exception_descripton;
              exception_descripton<<"modbus_srv socket_thread_function (... ) No response file found for device "<<
                                     static_cast<int>(device_number)<<" command "<<static_cast<int>(command_code);
              sett_obj->sys_message(program_settings::ERROR_MSG, exception_descripton.str());
              
              telemetr_answer=metro_device::generate_error_answer_for_function ( 
                                             device_number,
                                             command_code,
                                             program_settings::MODBUS_EXCEPTION_FAILURE_IN_ASSOCIATED_DEVICE);
          } else {
              // Calculate and append CRC bytes to file response
              word crc_value = program_settings::crc(telemetr_answer);
              program_settings::bytes crc_bytes = program_settings::bytes_of_type<word>(crc_value);
              telemetr_answer.insert(telemetr_answer.end(), crc_bytes.begin(), crc_bytes.end());
          }

          failures_count=0;
    } //try
	catch (socket_exception sock_exc){
	   string message("modbus_srv  socket_thread_function (... ) Fail reciving data from telemetry : ");	
	   message+=sock_exc.get_description();
       sett_obj->sys_message(program_settings::ERROR_MSG, message);
       failures_count++;
       continue;
	} //catch (socket_exception

	try {
		// Send response from file
		socket_device->send(telemetr_answer);
		failures_count=0;
	}	catch (socket_exception sock_exc){
	   string message("modbus_srv  socket_thread_function (... ) Fail reciving data from telemetry : ");	
	   message+=sock_exc.get_description();
       sett_obj->sys_message(program_settings::ERROR_MSG, message);
       failures_count++;
       continue;
	}; //catch (socket_exception
	
	}; //while(failures_count<program_settings::SOCKET_FAILURES_COUNT)

    delete(socket_device);

    return 0;
};


string usage(string program_name) {
ostringstream usage_descr;
 usage_descr<<"Usage :\n"
         <<program_name<<" configuration_file\n"
         <<"\twhere :\n"
         <<"\t\tconfiguration_file file with configuration settings \n"
         <<"\t\t\tby example /etc/modbus_srv.conf\n"
         <<endl;

return usage_descr.str();
};


int main(int argc, const char *argv[]){
  server_socket *socket_device;
  generic_socket *accepted_socket=NULL; //make compiler happy :)
	pthread_attr_t      attr;
  program_settings* sett_obj=program_settings::get_instance();
  uint64_t recv_send_timeout_nanosec(program_settings::RECV_SEND_TIMEOUT);
	int pthread_creating_result;
    recv_send_timeout_nanosec*=1000000000;

   if (!welcome_message.empty())
	cout<<"\n================================================================="
           <<welcome_message
  <<"\n================================================================="<<endl;

	if (pthread_attr_init(&attr)!=EOK){
          sett_obj->sys_message(program_settings::ERROR_MSG, "Can`t pthread_attr_init(&attr)");
	      return 0;
          };
	if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)!=EOK){
          sett_obj->sys_message(program_settings::ERROR_MSG, "Can`t pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)");
	      return 0;
         };

   signal( SIGPIPE, SIG_IGN );

	try {
		socket_device = new  server_socket(recv_send_timeout_nanosec,
                                      program_settings::BACKLOG,
                                      program_settings::MODBUS_TCP_PORT);
        socket_device->initialize();
	} catch (socket_exception sock_exc){
	     sett_obj->sys_message(program_settings::ERROR_MSG, sock_exc.get_description());
	     return 0;
	};

	while(true){
		try {
		    accepted_socket=socket_device->accept();
		} catch (socket_exception sock_exc){
              sett_obj->sys_message(program_settings::ERROR_MSG, sock_exc.get_description());
	    };

  pthread_creating_result=pthread_create(NULL, &attr, &socket_thread_function, accepted_socket);
		if ( pthread_creating_result != EOK){
			   string message("fail to create device thread : ");	
			   message+=strerror(pthread_creating_result);
               sett_obj->sys_message(program_settings::ERROR_MSG, message);
		};

	}; // while(true)

	return 0;
}

