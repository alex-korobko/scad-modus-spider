#ifndef __PROGRAM_SETTINGS__
#define __PROGRAM_SETTINGS__

class program_settings {
private :
    program_settings();
    static program_settings* program_settings_instance;
public :
typedef vector<byte> bytes;
enum msg_type {ERROR_MSG=0, 
            INFO_MSG=1,
            DEBUG_MSG=2};

enum {NO_RESPONSE_TRYS=4,

           WATCH_DOG_PORT=0x0443,
           WATCH_DOG_INTERVAL=4,

           MAX_MESSAGES_COUNT=8,
	       MAX_UPPER_MESSAGE_ID_VALUE=200,

	 	   ECHO_TCP_PORT=7,
	 	   MODBUS_TCP_PORT=5502,
		   MODBUS_COM_PORT=2,
	 	   MODBUS_BYTES_PER_MESSAGE=2,

		   MODBUS_HEADER_SIZE=3,
		   MODBUS_WRITE_COMMAND_ANSWER_FRAGMENT=3,
	 	   MODBUS_DEVICE_NUMBER_INDEX=0,
	 	   MODBUS_DATA_BYTES_COUNT_INDEX=2,
	 	   MODBUS_FUNCTION_CODE_INDEX=1,
	 	   MODBUS_CRC_SIZE=2,
	 	   MODBUS_REQUEST_HEADER_SIZE=2,

           MODBUS_FUNCTION_4_REQUEST_PARTIAL_SIZE=4,
           MODBUS_FUNCTION_5_REQUEST_PARTIAL_SIZE=4,
           MODBUS_FUNCTION_6_REQUEST_PARTIAL_SIZE=4,
           MODBUS_FUNCTION_16_REQUEST_PARTIAL_SIZE=5,

           MODBUS_FUNCTION_4_ANSWER_PARTIAL_SIZE=1,
           MODBUS_FUNCTION_5_ANSWER_PARTIAL_SIZE=4,
           MODBUS_FUNCTION_6_ANSWER_PARTIAL_SIZE=4,
           MODBUS_FUNCTION_16_ANSWER_PARTIAL_SIZE=4,

           MODBUS_FUNCTION_4_REQUEST_BEGIN_ADDRESS_INDEX=3,
           MODBUS_FUNCTION_4_REQUEST_BEGIN_ADDRESS_INCREM=1,
           MODBUS_FUNCTION_4_REQUEST_REGISTERS_COUNT_INDEX=5,
           MODBUS_FUNCTION_4_REQUEST_REGISTERS_COUNT_INCREM=1,

           MODBUS_EXCEPTION_ILLEGAL_FUNCTION=1,
           MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS=2,
           MODBUS_EXCEPTION_FAILURE_IN_ASSOCIATED_DEVICE=4,

           MODBUS_ESCALATOR_DEFAULT_REQUEST_FUNCTION=4,

           SOCKET_SLEEP_BETWEEN_ECHO_FAILURES=5,
           CONNECT_TO_DEVICE_TIMEOUT=15, //S
           RECIEVE_SEND_TO_DEVICE_TIMEOUT=15, //S
	 	   RECV_SEND_TIMEOUT= 15, //S
  	 	   SOCKET_FAILURES_COUNT=3,
	 	   BACKLOG=5,

           SOCKET_USLEEP_BETWEEN_FAILURES=50000,
           SOCKET_REQUEST_SIZE=512,
//	 	   COMPORT_USLEEP_BETWEEN_COMMANDS=300000
	 	   COMPORT_USLEEP_BETWEEN_COMMANDS=200000, 
			RTS_DELAY = 5000,
			COMPORT_RECIEVE_TIMEOUT=10
		};

enum route_commands {
        ADD_ROUTE=0,
        DELETE_ROUTE
     };

//system messages
	void sys_message (const msg_type type, const string mess_text);

//CRC16 coding function
	static word crc(vector<byte> buffer);

// splitting and merging per bytes -  very dangerous templates using
   template<typename T>
	static bytes bytes_of_type (T value_to_splitting, bool big_indian=true) {
		    bytes ret_val(sizeof(T));
         	for (bytes::size_type i=0; i<ret_val.size(); i++)
                 if (big_indian) {
                     ret_val[ret_val.size()-i-1]=((value_to_splitting & (0x00FF<<i*8))>>i*8);
                   } else { // if (big_indian)
                     ret_val[i]=((value_to_splitting & (0x00FF<<i*8))>>i*8);
                   };

/*
		while (ret_val.size()>1 &&
		          ret_val[ret_val.size()-1]==0) {
            ret_val.pop_back();
		 };
it`s right, but for crc calculation to many bytes returned, if crc value < 255 ( 0x0f returns vector (f) with size 1)
*/
		return  ret_val;
       };

	template<typename T>
	static T type_from_bytes (bytes bytes_to_merging, bool big_indian=true){
               T ret_val=0;
	       bytes::size_type count=std::min(bytes_to_merging.size(), sizeof(T));
               if (big_indian) {
                 for (bytes::size_type i=0; i<count; i++)
                                      ret_val |=(bytes_to_merging[count-i-1] << (i*8) );
                } else { // if (big_indian)
                 for (bytes::size_type i=0; i<count; i++)
                                                    ret_val |=(bytes_to_merging[i] << (i*8) );
                }; //if (big_indian)

            return  ret_val;
    };


  static program_settings* get_instance();
};

#endif
