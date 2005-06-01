#ifndef __METRO_UDKU__
#define __METRO_UDKU__

class metro_udku : public metro_device {
private:
command_data request_from_socket;

metro_device::command_data_container create_request_to_com_port();
metro_device::command_data
         create_answer_to_socket_func_4
               (metro_device::command_data_container answer_from_com_port);

metro_device::command_data
     get_default_command_request_from_socket();

int previos_escalator_condition_value;
int previos_rkp_value;
int previos_mu_tu_switcher_value;
int previos_block_circut_value;

public:

enum {
     UDKU_FIRST_REGISTER=0,
     UDKU_MODBUS_REGISTERS_COUNT=9,
     UDKU_MODBUS_REGISTER_INCREM=2,
     UDKU_REGISTERS_COUNT=9,
     UDKU_REGISTER_INCREM=1,

     MESSAGES_UPPER_ID_INDEX=program_settings::MODBUS_HEADER_SIZE+
                 UDKU_REGISTERS_COUNT*UDKU_REGISTER_INCREM+
                 1,
     MESSAGES_UPPER_ID_INCREM=2,

     MESSAGES_COUNT_INDEX=MESSAGES_UPPER_ID_INDEX+MESSAGES_UPPER_ID_INCREM,
     MESSAGES_COUNT_INCREM=2,

     MESSAGES_INDEX=MESSAGES_COUNT_INDEX+MESSAGES_COUNT_INCREM,
     MESSAGES_MODBUS_REGISTERS_COUNT=program_settings::MAX_MESSAGES_COUNT,
     MESSAGES_INCREM=2
 };

enum {
       MESSAGE_STOP_WITH_BREAKING_PATH=6,
       MESSAGE_OWERSPEED_MORE_20_PERC=7,
       MESSAGE_OWERSPEED_MORE_25_PERC=8,
       MESSAGE_NOT_STOP_AFTER_7_SEC=9,
       MESSAGE_CONDITION_BREAK=10,
       MESSAGE_CONDITION_SAMOHOD=11,
       MESSAGE_ERROR_D0=12,
       MESSAGE_ERROR_D90=13,
       MESSAGE_ERROR_ALL_SIGNALS=14,
       MESSAGE_ERROR_CONTACT=15,
       MESSAGE_ESCALATOR_NOT_READY_WITH_BLOCK_CIRCUT_NAME=16,
       MESSAGE_ESCALATOR_READY_WITH_BLOCK_CIRCUT_NAME=17, //now not used
       MESSAGE_ESCALATOR_ON_TU=18,
       MESSAGE_ESCALATOR_ON_MU=19,
       MESSAGE_TU_COMMAND_UP_SUCCESS=20,
       MESSAGE_TU_COMMAND_DOWN_SUCCESS=21
 };

enum {
       MODBUS_REGISTER_3001_LOW_BYTE_GPSTOP_VALUE=0,
       MODBUS_REGISTER_3001_LOW_BYTE_GPUP_VALUE=1,
       MODBUS_REGISTER_3001_LOW_BYTE_GPDOWN_VALUE=2,
       MODBUS_REGISTER_3001_LOW_BYTE_UP20_VALUE=3,
       MODBUS_REGISTER_3001_LOW_BYTE_UP25_VALUE=4,
       MODBUS_REGISTER_3001_LOW_BYTE_NOT7SEC_VALUE=5,
       MODBUS_REGISTER_3001_LOW_BYTE_BREAK_VALUE=6,
       MODBUS_REGISTER_3001_LOW_BYTE_SAMOHOD_VALUE=7,
       MODBUS_REGISTER_3001_LOW_BYTE_BRAK0_VALUE=8,
       MODBUS_REGISTER_3001_LOW_BYTE_BRAK90_VALUE=9,
       MODBUS_REGISTER_3001_LOW_BYTE_BRAKALLES_VALUE=10,
       MODBUS_REGISTER_3001_LOW_BYTE_BRAKKSR_VALUE=11
 };


/*
constructor and destructor
*/
explicit metro_udku(int new_number) 
                throw (metro_device::metro_device_exception):
    metro_device(new_number),
    previos_escalator_condition_value(-1),
    previos_rkp_value(-1),
    previos_mu_tu_switcher_value(-1),
    previos_block_circut_value(-1) {
};

virtual ~metro_udku(){};
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
            get_command_request_to_comport();

virtual command_data_container
            get_default_command_request_to_comport();
};

#endif
