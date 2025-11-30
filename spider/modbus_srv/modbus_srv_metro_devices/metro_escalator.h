#ifndef __METRO_ESCALATOR__
#define __METRO_ESCALATOR__

class metro_escalator : public metro_device {
private:
command_data request_from_socket;

metro_device::command_data_container create_request_to_com_port();
metro_device::command_data
         create_answer_to_socket_func_4
               (metro_device::command_data_container answer_from_com_port) ;


public:

enum {
		ESCALATOR_FIRST_REGISTER = 0,
		ESCALATOR_REGISTERS_COUNT= 38,

       MODBUS_INPUT_REGISTERS_BEGIN_ADDRESS=30001,
       MODBUS_INPUT_REGISTERS_END_ADDRESS=30051,

       MODBUS_FUNCTION_4_ANSWER_MESSAGES_COUNT_INDEX=18,
       MODBUS_FUNCTION_4_ANSWER_MESSAGES_COUNT_INCREM=1,
       MODBUS_FUNCTION_4_ANSWER_UPPER_MESSAGE_ID_INDEX=16,
       MODBUS_FUNCTION_4_ANSWER_UPPER_MESSAGE_ID_INCREM=1,
       MODBUS_FUNCTION_4_ANSWER_MESSAGES_BEGIN_INDEX=63,
       MODBUS_FUNCTION_4_ANSWER_MESSAGES_INCREM=1,
  } ;

enum {
       MESSAGE_RECIEVE_COMMAND_UP=22,
       MESSAGE_RECIEVE_COMMAND_DOWN=23,
       MESSAGE_RECIEVE_COMMAND_STOP=24
 };


/*
constructor and destructor
*/
metro_escalator(int new_number, bool new_log_packets) throw (metro_device::metro_device_exception) :
        metro_device(new_number, new_log_packets) {};
virtual ~metro_escalator() {};
/*
virtual functions
*/
virtual void 
          put_command_answer_from_comport
                  (metro_device::command_data_container answer_from_com_port);

virtual void 
            put_command_request_from_socket
                   (metro_device::command_data request_from_socket);

virtual command_data_container
            get_command_request_to_comport
                      ();

virtual command_data_container
            get_default_command_request_to_comport
                         ();

virtual metro_device::command_data
     get_default_command_request_from_socket();

};

#endif
