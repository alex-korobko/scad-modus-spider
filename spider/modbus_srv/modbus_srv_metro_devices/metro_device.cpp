using namespace std;
#include <pthread.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>

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

metro_device::metro_device(int new_device_number, bool new_log_packets) 
 throw (metro_device::metro_device_exception) :
     device_number(new_device_number),
     upper_message_id(0),
     messages(0),
	 log_packets(new_log_packets) {

ostringstream exception_description;
int  ret_val;

ret_val=pthread_cond_init (&data_transfer_process_cond_var, NULL);
if (ret_val!=EOK) {
	exception_description<<"Can`t initialize data_transfer_process_cond_var : "<<strerror(ret_val);
     throw metro_device::metro_device_exception(exception_description.str());
	};

ret_val=pthread_mutex_init (&data_transfer_process_mutex, NULL);
if (ret_val!=EOK) {
	exception_description<<"Can`t initialize data_transfer_process_mutex : "<<strerror(ret_val);
     throw metro_device::metro_device_exception(exception_description.str());
	};

ret_val=pthread_mutex_init (&sockets_to_device_queue_mutex, NULL);
if (ret_val!=EOK) {
	exception_description<<"Can`t initialize sockets_to_device_queue_mutex : "<<strerror(ret_val);
     throw metro_device::metro_device_exception(exception_description.str());
	};

ret_val=pthread_mutex_init (&default_answer_to_socket_mutex, NULL);
if (ret_val!=EOK) {
	exception_description<<"Can`t initialize default_answer_to_socket_mutex : "<<strerror(ret_val);
     throw metro_device::metro_device_exception(exception_description.str());
	};

};

metro_device::~metro_device() {
   pthread_cond_destroy(&data_transfer_process_cond_var);
   pthread_mutex_destroy(&data_transfer_process_mutex);

   pthread_mutex_destroy(&default_answer_to_socket_mutex);
   pthread_mutex_destroy(&sockets_to_device_queue_mutex);
};

void metro_device::add_messages_fifo
         (metro_device::messages_container messages_from_device) { //from comport
	messages.insert(messages.end(),
	                          messages_from_device.begin(),
	                          messages_from_device.end());
	upper_message_id+=messages_from_device.size()/program_settings::MODBUS_BYTES_PER_MESSAGE;
	if (upper_message_id>program_settings::MAX_UPPER_MESSAGE_ID_VALUE)
	        upper_message_id-=program_settings::MAX_UPPER_MESSAGE_ID_VALUE;

	if (messages.size()>
     program_settings::MAX_MESSAGES_COUNT*
     program_settings::MODBUS_BYTES_PER_MESSAGE){
            messages_container::iterator erase_iter=messages.begin();
            advance(erase_iter, messages.size()-program_settings::MAX_MESSAGES_COUNT*program_settings::MODBUS_BYTES_PER_MESSAGE);
     		messages.erase(messages.begin(), erase_iter);
		};
};

metro_device::command_data
     metro_device::generate_error_answer_for_function
                             ( int device_number,
                               int function_code,
                               int exception_code){
 	command_data buffer(0);

	buffer.push_back(device_number);
	buffer.push_back(function_code | 0x80);                      // function number with error detected flag
	buffer.push_back(exception_code);

	program_settings::bytes tmp_bytes=program_settings::bytes_of_type<word>(program_settings::crc(buffer));
	buffer.insert(buffer.end(), tmp_bytes.begin(), tmp_bytes.end());

   	return buffer;

};

