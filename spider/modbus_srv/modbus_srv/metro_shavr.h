#ifndef __METRO_SHAVR__
#define __METRO_SHAVR__

class metro_shavr : public metro_device {
private:
command_data request_from_socket;

metro_device::command_data_container create_request_to_com_port();
metro_device::command_data
         create_answer_to_socket_func_4
               (metro_device::command_data_container answer_from_com_port);

metro_device::command_data
     get_default_command_request_from_socket();

//automats
int A0_state;

bool A0_x1, A0_x2,
        A0_x3, A0_x4,
        A0_x5,

        A0_x10, A0_x11,
        A0_x12, A0_x13,
        A0_x14, A0_x15,
        A0_x16, A0_x17,
        A0_x18, A0_x19,
        A0_x20, A0_x21,
        A0_x22, A0_x23,
        A0_x24, A0_x25;

bool A1_x3;
bool A2_x3;
bool A3_x3;

void A0(int event) 
          throw (metro_device::metro_device_exception);

void A0_z1() {A0_x5=true;};
void A0_z2() {A0_x5=false;};

public:
   enum {
         A0_OFFLINE=0,
         A0_EXCEPTION,
         A0_DISABLED_1_DISABLED_2,
         A0_ENABLED_1_ENABLED_2,
         A0_DISABLED_1_ENABLED_2,
         A0_ENABLED_1_DISABLED_2
   };

enum {
     UDKU_FIRST_REGISTER=7,
     UDKU_MODBUS_REGISTERS_COUNT=2,
     UDKU_MODBUS_REGISTER_INCREM=2,
     UDKU_REGISTERS_COUNT=2,
     UDKU_REGISTER_INCREM=2,

     POWER_COUNTER_1_FIRST_REGISTER=0x47,
     POWER_COUNTER_1_MODBUS_REGISTERS_COUNT=0x20,
     POWER_COUNTER_1_MODBUS_REGISTER_INCREM=2,
     POWER_COUNTER_1_REGISTERS_COUNT=16,
     POWER_COUNTER_1_REGISTER_INCREM=4,

     POWER_COUNTER_2_FIRST_REGISTER=POWER_COUNTER_1_FIRST_REGISTER,
     POWER_COUNTER_2_MODBUS_REGISTERS_COUNT=
                                  POWER_COUNTER_1_MODBUS_REGISTERS_COUNT,
     POWER_COUNTER_2_MODBUS_REGISTER_INCREM=
                                  POWER_COUNTER_1_MODBUS_REGISTER_INCREM,
     POWER_COUNTER_2_REGISTERS_COUNT=
                                  POWER_COUNTER_1_REGISTERS_COUNT,
     POWER_COUNTER_2_REGISTER_INCREM=
                                  POWER_COUNTER_1_REGISTER_INCREM,

     MESSAGES_UPPER_ID_INDEX=program_settings::MODBUS_HEADER_SIZE+
                 UDKU_REGISTERS_COUNT*UDKU_REGISTER_INCREM+
                 POWER_COUNTER_2_REGISTERS_COUNT*POWER_COUNTER_1_REGISTER_INCREM+
                 POWER_COUNTER_2_REGISTERS_COUNT*POWER_COUNTER_2_REGISTER_INCREM+
                 1,
     MESSAGES_UPPER_ID_INCREM=2,

     MESSAGES_COUNT_INDEX=MESSAGES_UPPER_ID_INDEX+MESSAGES_UPPER_ID_INCREM,
     MESSAGES_COUNT_INCREM=2,

     MESSAGES_INDEX=MESSAGES_COUNT_INDEX+MESSAGES_COUNT_INCREM,
     MESSAGES_MODBUS_REGISTERS_COUNT=program_settings::MAX_MESSAGES_COUNT,
     MESSAGES_INCREM=2
 };

enum {
       MESSAGE_INPUT_1_DISCONNECTED=6,
       MESSAGE_INPUT_1_CONNECTED=7,
       MESSAGE_INPUT_2_DISCONNECTED=8,
       MESSAGE_INPUT_2_CONNECTED=9,

       MESSAGE_ESCALATOR_1_CONNECTED_TO_INPUT_1=10,
       MESSAGE_ESCALATOR_1_DISCONNECTED_FROM_INPUT_1=11,
       MESSAGE_ESCALATOR_1_CONNECTED_TO_INPUT_2=12,
       MESSAGE_ESCALATOR_1_DISCONNECTED_FROM_INPUT_2=13,

       MESSAGE_ESCALATOR_2_CONNECTED_TO_INPUT_1=14,
       MESSAGE_ESCALATOR_2_DISCONNECTED_FROM_INPUT_1=15,
       MESSAGE_ESCALATOR_2_CONNECTED_TO_INPUT_2=16,
       MESSAGE_ESCALATOR_2_DISCONNECTED_FROM_INPUT_2=17,

       MESSAGE_ESCALATOR_3_CONNECTED_TO_INPUT_1=18,
       MESSAGE_ESCALATOR_3_DISCONNECTED_FROM_INPUT_1=19,
       MESSAGE_ESCALATOR_3_CONNECTED_TO_INPUT_2=20,
       MESSAGE_ESCALATOR_3_DISCONNECTED_FROM_INPUT_2=21,

       MESSAGE_ESCALATOR_4_CONNECTED_TO_INPUT_1=22,
       MESSAGE_ESCALATOR_4_DISCONNECTED_FROM_INPUT_1=23,
       MESSAGE_ESCALATOR_4_CONNECTED_TO_INPUT_2=24,
       MESSAGE_ESCALATOR_4_DISCONNECTED_FROM_INPUT_2=25,

       MESSAGE_DOOR_OPENED=26,
       MESSAGE_FIRE_ALARM_ACTIVATED=28,

       MESSAGE_RECIEVED_ESCALATOR_1_CONNECT_TO_INPUT_1=30,
       MESSAGE_RECIEVED_ESCALATOR_1_DISCONNECT_FROM_INPUT_1=31,
       MESSAGE_RECIEVED_ESCALATOR_1_CONNECT_TO_INPUT_2=32,
       MESSAGE_RECIEVED_ESCALATOR_1_DISCONNECT_FROM_INPUT_2=33,

       MESSAGE_RECIEVED_ESCALATOR_2_CONNECT_TO_INPUT_1=34,
       MESSAGE_RECIEVED_ESCALATOR_2_DISCONNECT_FROM_INPUT_1=35,
       MESSAGE_RECIEVED_ESCALATOR_2_CONNECT_TO_INPUT_2=36,
       MESSAGE_RECIEVED_ESCALATOR_2_DISCONNECT_FROM_INPUT_2=37,

       MESSAGE_RECIEVED_ESCALATOR_3_CONNECT_TO_INPUT_1=38,
       MESSAGE_RECIEVED_ESCALATOR_3_DISCONNECT_FROM_INPUT_1=39,
       MESSAGE_RECIEVED_ESCALATOR_3_CONNECT_TO_INPUT_2=40,
       MESSAGE_RECIEVED_ESCALATOR_3_DISCONNECT_FROM_INPUT_2=41,

       MESSAGE_RECIEVED_ESCALATOR_4_CONNECT_TO_INPUT_1=42,
       MESSAGE_RECIEVED_ESCALATOR_4_DISCONNECT_FROM_INPUT_1=43,
       MESSAGE_RECIEVED_ESCALATOR_4_CONNECT_TO_INPUT_2=44,
       MESSAGE_RECIEVED_ESCALATOR_4_DISCONNECT_FROM_INPUT_2=45

 };

/*
constructor and destructor
*/
explicit metro_shavr(int new_number) 
                throw (metro_device::metro_device_exception) :
    metro_device(new_number),
    A0_state(A0_OFFLINE),
    A0_x5(false),  //offline 
    A1_x3(false),
    A2_x3(false),
    A3_x3(false) {
};

virtual ~metro_shavr(){};
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
