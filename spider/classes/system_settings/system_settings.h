#ifndef __SYSTEM_H__
#define __SYSTEM_H__

class system_settings {
private:
typedef vector<string> strings_container;

strings_container escalator_mode_text;
strings_container escalator_ready_text;

strings_container udku_mode_text;
strings_container udku_type_text;
strings_container udku_position_text;

strings_container directions_text_engl;
strings_container directions_text_russ;
strings_container outer_states_text;
strings_container start_days_modes_text_engl;
strings_container start_days_modes_text_russ;
strings_container parameters_in_messages_strings;

string devices_config_name;
string global_messages_name;
string messages_types_name;
string devices_types_name;
string main_log_name;
string dispatchers_name;

string routing_name;

public:
typedef strings_container::size_type strings_size_type;
typedef  vector<byte>  bytes;


system_settings();
virtual ~system_settings() {};
//Enumerated constants

	enum {	BLOCK_NORMA=1,
				BREAKING_PATH_NORMA=1
				};

	//metro device outer states
	enum {	DISABLED=0,
				ENABLED, 
				OUTER_STATES_COUNT
				};

   
	//escalators directions
	enum {DIRECTION_STOP=0,
				DIRECTION_UP,
				DIRECTION_DOWN,
				DIRECTION_REVERSE,
				DIRECTIONS_COUNT
				};

	//shavr commands
	enum {TURN_ON_ESC_1_TO_INPUT_1=0,
				TURN_OFF_ESC_1_TO_INPUT_1,
				TURN_ON_ESC_1_TO_INPUT_2,
				TURN_OFF_ESC_1_TO_INPUT_2,

                TURN_ON_ESC_2_TO_INPUT_1,
				TURN_OFF_ESC_2_TO_INPUT_1,
				TURN_ON_ESC_2_TO_INPUT_2,
				TURN_OFF_ESC_2_TO_INPUT_2,

                TURN_ON_ESC_3_TO_INPUT_1,
				TURN_OFF_ESC_3_TO_INPUT_1,
				TURN_ON_ESC_3_TO_INPUT_2,
				TURN_OFF_ESC_3_TO_INPUT_2,

                TURN_ON_ESC_4_TO_INPUT_1,
				TURN_OFF_ESC_4_TO_INPUT_1,
				TURN_ON_ESC_4_TO_INPUT_2,
				TURN_OFF_ESC_4_TO_INPUT_2
				};


	//escalator start days modes
	enum {START_DAY_MODE_EVERYDAY=0,
				START_DAY_MODE_NEVER,
				START_DAY_MODE_EVEN,
				START_DAY_MODE_ODD,
				START_DAYS_MODES_COUNT,
				START_HOUR_MIN=0,
				START_HOUR_MAX=23
				};

	//escalator command codes (for modbus register 40001)
	enum {COMMAND_UP=0xE0,
				COMMAND_DOWN=0xE1,
				COMMAND_STOP=0xE2,
                COMMAND_CHANCEL=0xFE,
                COMMAND_ACCEPT=0xFF
				};

	//escalator states from variable Condition (modbus register 30001) 
	enum {ESCALATOR_STATE_UP_MAIN_DRIVE=2,
				ESCALATOR_STATE_DOWN_MAIN_DRIVE,
				ESCALATOR_STATE_UP_SMALL_DRIVE,
				ESCALATOR_STATE_DOWN_SMALL_DRIVE,
				ESCALATOR_STATE_STOP
				};

	//escalator`s modes codes  from variable Mode (modbus register 30002) 
	enum {ESCALATOR_MODE_UNUSED=0,
				ESCALATOR_MODE_MAIN_DRIVE_AT_PANEL, 
				ESCALATOR_MODE_MAIN_DRIVE_AT_BEANCH_BOARD, 
				ESCALATOR_MODE_MAIN_DRIVE_AT_REMOTE_CONTROL,
				ESCALATOR_MODE_SMALL_DRIVE_AT_PANEL,
				ESCALATOR_MODE_SMALL_DRIVE_AT_BEANCH_BOARD,
				ESCALATOR_MODE_TEST,
				ESCALATOR_MODE_BRAKING_OFF, 
				ESCALATOR_MODE_RUNNING_OUT, 
				ESCALATOR_MODES_COUNT};
	
	//escalator`s  ready states codes from variable Gotov (modbus register 30003) 
	enum {ESCALATOR_STATE_MAIN_DRIVE_READY=0,
				ESCALATOR_STATE_SMALL_DRIVE_READY,
				ESCALATOR_STATE_TEST_READY,
				ESCALATOR_STATE_BRAKING_OFF_READY,
				ESCALATOR_STATE_RUNNING_OUT_READY,
				ESCALATOR_STATE_NOT_READY,
                ESCALATOR_STATE_WORKING,
				ESCALATOR_STATES_COUNT
				};

	//udku`s  mode states codes from variables Condition and FlagBrak (modbus register 30001 low byte) 
	enum {UDKU_MODE_GPSTOP,
                UDKU_MODE_GPUP,
                UDKU_MODE_GPDOWN,
                UDKU_MODE_UP20,
                UDKU_MODE_UP25,
                UDKU_MODE_NOT7SEC,
                UDKU_MODE_BREAK,
                UDKU_MODE_SAMOHOD,
                UDKU_MODE_BRAK0,
                UDKU_MODE_BRAK90,
                UDKU_MODE_BRAKALLES,
                UDKU_MODE_BRAKKSR,
                UDKU_MODE_UNUSED,
				UDKU_MODES_COUNT};

	//udku`s type codes from variable TypElevator (modbus register 30002 low byte)
	enum {UDKU_TYPE_LT_1,
                UDKU_TYPE_LT_2_1,
                UDKU_TYPE_LT_2_2,
                UDKU_TYPE_LT_3_1,
                UDKU_TYPE_LT_3_2,
                UDKU_TYPE_LT_3M_1,
                UDKU_TYPE_LT_3M_2,
                UDKU_TYPE_LT_4_1,
                UDKU_TYPE_LT_4_2,
                UDKU_TYPE_LT_5_1,
                UDKU_TYPE_LT_5_2,
                UDKU_TYPE_ET_2,
                UDKU_TYPE_ET_3_1,
                UDKU_TYPE_ET_3_2,
                UDKU_TYPE_ET_3M_1,
                UDKU_TYPE_ET_5,
                UDKU_TYPE_ET_5M_1,
                UDKU_TYPE_EM_4,
                UDKU_TYPE_EM_5_5,
                UDKU_TYPE_UNUSED,
				UDKU_TYPES_COUNT};

	//udku`s position codes from variable DKSE (modbus register 30002 high byte)
	enum {UDKU_POSITION_LEFT,
                UDKU_POSITION_RIGHT,
                UDKU_POSITION_UNUSED,
				UDKU_POSITIONS_COUNT};

// message windows and system messages types
	enum{
				DEBUG_MSG=0,
				ERROR_MSG,
				INFO_MSG,
				WARN_MSG,
				MESSAGES_COUNT
			};

//parameters in messages indexes in strings_container parameters_in_messages_strings
        enum {
               PARAMETER_NAME_BREAKING_PATH_VALUE=0,
               PARAMETER_NAME_BLOCK_CIRCUT_NAME,
               PARAMETER_VALUE,
               PARAMETER_NAME_DISPATCHER,
               PARAMETER_NAME_STOP_CAUSE,
               PARAMETERS_NAMES_COUNT
       };



//predefined log messages ids
   enum {MESSAGE_ID_SYSTEM_STARTED=1,

                DISPATCHER_BEGIN_SMENU=218,
                DISPATCHER_MAKE_CMD_STOP=219,
                DISPATCHER_MAKE_CMD_DOWN=220,
                DISPATCHER_MAKE_CMD_UP=221,
                DISPATCHER_SEND_CMD_UP=223,
                DISPATCHER_SEND_CMD_DOWN=222,
                DISPATCHER_SEND_CMD_STOP=224,
                DISPATCHER_KVIT_MASHZAL_DOOR=225,
                DISPATCHER_CHANCEL_CMD_UP=226,
                DISPATCHER_CHANCEL_CMD_DOWN=227,
                DISPATCHER_MAKE_CMD=228,
                DISPATCHER_SEND_CMD=229,
                DISPATCHER_CHANCEL_CMD=230,

				MESSAGE_ID_STATION_OFFLINE = 240,
				MESSAGE_ID_DEVICE_OFFLINE = 241
                };

//predefined log messages types ids
    enum {  
				  MESSAGE_TYPE_ERROR_MESSAGE=2,
				  MESSAGE_TYPE_INFO_MESSAGE=4,
                  MESSAGE_TYPE_DISPATCHER_DOING=5
               };

// other system settings
	enum {
//				SYSTEM_TIMER=_PULSE_CODE_MINAVAIL+1,
				PING_TIMEOUT=2000000, // мкс
				OFFLINE_DELAY=20, //c
				CONDUCTION_DELAY=300, //c = 5 min 
				SYSTEM_TICK	=1, // c

				ECHO_TCP_PORT=7,
				MODBUS_TCP_PORT=5502,
                MAX_MESSAGES_COUNT=8,
                MAX_UPPER_MESSAGE_ID_VALUE=200,

                MODBUS_BYTES_PER_MESSAGE=2,

                MODBUS_WRITE_COMMAND_ANSWER_FRAGMENT=3,
                MODBUS_HEADER_SIZE=3,
                MODBUS_CRC_SIZE=2,

                MODBUS_DEVICE_NUMBER_INDEX=0,
                MODBUS_DATA_BYTES_COUNT_INDEX=2,
                MODBUS_FUNCTION_CODE_INDEX=1,

                MODBUS_FUNCTION_4_REQUEST_BEGIN_ADDRESS_INDEX=3,
                MODBUS_FUNCTION_4_REQUEST_BEGIN_ADDRESS_INCREM=1,
                MODBUS_FUNCTION_4_REQUEST_REGISTERS_COUNT_INDEX=5,
                MODBUS_FUNCTION_4_REQUEST_REGISTERS_COUNT_INCREM=1,
                MODBUS_FUNCTION_4_ANSWER_MESSAGES_COUNT_INDEX=8,
                MODBUS_FUNCTION_4_ANSWER_UPPER_MESSAGE_ID_INDEX=9,
                MODBUS_FUNCTION_4_ANSWER_MESSAGES_BEGIN_INDEX=32,

                MODBUS_FUNCTION_5_ADDRESS_INDEX=2,
                MODBUS_FUNCTION_5_ADDRESS_INCREM=2,
                MODBUS_FUNCTION_5_VALUE_INDEX=4,
                MODBUS_FUNCTION_5_VALUE_INCREM=2,

                MODBUS_FUNCTION_6_REQUEST_ADDRESS_INDEX=2,
                MODBUS_FUNCTION_6_REQUEST_ADDRESS_INCREM=2,
                MODBUS_FUNCTION_6_VALUE_INDEX=4,
                MODBUS_FUNCTION_6_VALUE_INCREM=2,

                MODBUS_FUNCTION_16_HEADER_SIZE=7,
                MODBUS_FUNCTION_16_ADDRESS_INDEX=2,
                MODBUS_FUNCTION_16_ADDRESS_INCREM=2,
                MODBUS_FUNCTION_16_REGISTERS_COUNT_INDEX=4,
                MODBUS_FUNCTION_16_REGISTERS_COUNT_INCREM=2,
                MODBUS_FUNCTION_16_REQUEST_BYTES_COUNT_INDEX=6,
                MODBUS_FUNCTION_16_REQUEST_BYTES_COUNT_INCREM=1,
                MODBUS_FUNCTION_16_REQUEST_DATA_INDEX=7,
                MODBUS_FUNCTION_16_REQUEST_DATA_INCREM=2,

                MODBUS_EXCEPTION_ILLEGAL_FUNCTION=1,
                MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS=2,
                MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE=3,
                MODBUS_EXCEPTION_FAILURE_IN_ASSOCIATED_DEVICE=4,
                
//                SOCKET_FAILURES_COUNT, //not used
				TIME_CORR_INTER=10000, //interval (in cicles of requests to hardware) between sending timeset request
                RECONNECTS_TO_DEVICE_COUNT=3,
                CONNECT_TO_DEVICE_TIMEOUT=9, //seconds
                RECIEVE_SEND_TO_DEVICE_TIMEOUT=5, //seconds
                DELAY_BETWEEN_REQUESTS_TO_DEVICE=900000 //microseconds
				};

 enum types_of_system_settings{
        SPIDER_SYSTEM_SETTINGS=0
 };

//get_ and set_ metods for private data members

	strings_container get_directions_engl_strings() {return directions_text_engl;};
	strings_container get_directions_russ_strings() {return directions_text_russ;};
	strings_container get_start_days_modes_engl_strings() {return start_days_modes_text_engl;};
	strings_container get_start_days_modes_russ_strings() {return start_days_modes_text_russ;};
	strings_container get_outer_states_strings() {return outer_states_text;};
	strings_container get_parameters_in_messages_strings() {return parameters_in_messages_strings;};

	string get_escalator_mode_text(system_settings::strings_size_type escalator_mode_index){
		if (escalator_mode_index>=0 &&
			escalator_mode_index<escalator_mode_text.size()) 
			{
				return(escalator_mode_text[escalator_mode_index]);
			} else {
				return(escalator_mode_text[ESCALATOR_MODE_UNUSED]); //what can I do???
			};
	};

	string get_escalator_ready_text(system_settings::strings_size_type escalator_ready_index) {
		if (escalator_ready_index>=0 &&
			escalator_ready_index<ESCALATOR_STATES_COUNT) {
				return(escalator_ready_text[escalator_ready_index]);
			} else {
				return(escalator_ready_text[ESCALATOR_STATE_NOT_READY]); //what can I do???
			};
	}

    string get_udku_mode_text(system_settings::strings_size_type udku_mode_index) {
		if (udku_mode_index>=0 &&
			udku_mode_index<udku_mode_text.size()) {
				return(udku_mode_text[udku_mode_index]);
			} else {
				return(udku_mode_text[UDKU_MODE_UNUSED]); //what can I do???
			};

	};

    string get_udku_type_text(system_settings::strings_size_type udku_type_index){
		if (udku_type_index>=0 &&
			udku_type_index<udku_type_text.size()) {
				return(udku_type_text[udku_type_index]);
			} else {
				return(udku_type_text[UDKU_TYPE_UNUSED]); //what can I do???
			};

	};

    string get_udku_position_text(system_settings::strings_size_type udku_position_index){
		if (udku_position_index>=0 &&
			udku_position_index<udku_position_text.size()) {
				return(udku_position_text[udku_position_index]);
			} else {
				return(udku_position_text[UDKU_POSITION_UNUSED]); //what can I do???
			};

	};



//system messages
	void sys_message (const int type, const string mess_text);

//config file names in project
	string get_devices_config_name() {return(devices_config_name);};
	string get_global_messages_name() {return(global_messages_name);};
	string get_messages_types_name() {return(messages_types_name);};
	string get_devices_types_name() {return(devices_types_name);};
    string get_dispatchers_name() {return(dispatchers_name);};
	string get_main_log_name() {return(main_log_name);};
	string get_routing_name() {return(routing_name);};

//virtual metods
virtual types_of_system_settings get_system_settings_type()=0;

//static methods

//CRC16 coding function
	static word crc(bytes buffer);

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

   static byte decode_to_binary_decimal_notation (byte binary_value_less_100) throw (spider_exception);
};

#endif
