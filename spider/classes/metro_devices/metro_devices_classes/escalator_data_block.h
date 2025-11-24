#ifndef _ESCALATOR_DATA_BLOCK_
#define _ESCALATOR_DATA_BLOCK_

/*
Fields of struct escalator_data_block;

	enum {COUNT_OF_CONTACTORS_REGISTERS=1,
           COUNT_OF_AUTOMATS_REGISTERS=1,
           COUNT_OF_OTHER_REGISTERS=1,
           COUNT_OF_MESSAGES=8,
           COUNT_OF_BLOCK_CIRCUT_REGISTERS=9
	};

status : 
	modbus register 30001 Condition 
	escakator`s status 
	see enum  STATE_UP_...  in system_settings class for values
mode :
	modbus register 30002 Mode
	escakator`s mode
	see enum  MODE_... in system_settings class for values
ready :
	modbus register 30003 Gotov
	escakator`s ready state
	see enum  STATE_MAIN_... in system_settings class for values
block_circuts_status :
	modbus register 30004 BlockCircute
	escalator`s block circuts state
	1-normal 0-not normal Other-not defined
breaking_path_status :
	modbus register 30005 BremseCondition
	escalator`s  breaking path state
	1-normal 0-not normal Other-not defined
channels :
	modbus register 30006 BU1, BU2, BU3
	state of three undepended control channels
	each of channels occupate two binary positions in register
		(in binary) 00-blue 01-green 11-gray //NOTICE: check gray value 
		data of BU1 occupate 0 and 1 positions of register( 2^0 and 2^1)
		data of BU2 occupate 2 and 3 positions of register( 2^2 and 2^3)
		data of BU3 occupate 4 and 5 positions of register( 2^4 and 2^5)
message_number :
	modbus register 30007 Messer
	id (from 1000 to 1) of upper message in buffer
count_of_messages :
	modbus register 30008 Upper
	count of messages in buffer (registers 30031-30051)
	max value COUNT_OF_MESSAGES if 0-buffer is empty
reserved1 :
	modbus register 30009 no name
	reserved by future
breaking_path_value:
	modbus registers 30010-30011 Nbremse
	value of breaking path
running_path_value:
	modbus registers 30012-30013 Nprobeg
	value of running path
reserved2 :
	modbus register 30014 no name
	reserved by future
reserved3 :
	modbus register 30015 no name
	reserved by future
signals :
	modbus registers at 30016 
	to 30016
		+COUNT_OF_BLOCK_CIRCUT_REGISTERS
		+COUNT_OF_CONTACTORS_REGISTERS
		+COUNT_OF_AUTOMATS_REGISTERS
		+COUNT_OF_OTHER_REGISTERS
	includes 
		modbus registers 
			30016 -
			30016
			+COUNT_OF_BLOCK_CIRCUT_REGISTERS
			NBlock
			each block circut occupate two binary positions in register
			in each register can contain 8 block circuts
			values (binary)
				00-green 01-blue  10-red 11-grey

		modbus registers
			30016
			+COUNT_OF_BLOCK_CIRCUT_REGISTERS
			+1  -
			30016
			+COUNT_OF_BLOCK_CIRCUT_REGISTERS
			+COUNT_OF_CONTACTORS_REGISTERS 
			NContactor
			each contactor occupate two binary positions in register
			in each register can contain 8 contactors
			values (binary)
				00-green 01-blue  10-red 11-grey

		modbus registers 
			30016
			+COUNT_OF_BLOCK_CIRCUT_REGISTERS
			+COUNT_OF_CONTACTORS_REGISTERS 
			+1 -
			30016
			+COUNT_OF_BLOCK_CIRCUT_REGISTERS
			+COUNT_OF_CONTACTORS_REGISTERS 
			+COUNT_OF_AUTOMATS_REGISTERS
			NAutomat
			each automat occupate two binary positions in register
			in each register can contain 8 automats
			values (binary)
				00-green 01-blue  10-red 11-grey


		modbus registers 
			30016
			+COUNT_OF_BLOCK_CIRCUT_REGISTERS
			+COUNT_OF_CONTACTORS_REGISTERS 
			+COUNT_OF_AUTOMATS_REGISTERS
			+1 -
			30016
			+COUNT_OF_BLOCK_CIRCUT_REGISTERS
			+COUNT_OF_CONTACTORS_REGISTERS 
			+COUNT_OF_AUTOMATS_REGISTERS
			+COUNT_OF_OTHER_REGISTERS
			NOS
			each other parameter occupate two binary positions in register
			in each register can contain 8 other parameters
			values (binary)
				00-green 01-blue  10-red 11-grey

signals :
	modbus registers at
	30031	
	to 
		30031
		+COUNT_OF_MESSAGES
	includes 
		messages 

*/


struct escalator_data_block : public data_block{
	enum {
        MODBUS_INPUT_REGISTERS_BEGIN_ADDRESS=30001,
        MODBUS_DIRECTION_REGISTERS_BEGIN_ADDRESS=40001,

        MODBUS_LOW_INPUT_ADDRESS=30001,
        MODBUS_HIGH_INPUT_ADDRESS=30051,
        MODBUS_LOW_DIRECTION_ADDRESS=40001,
        MODBUS_HIGH_DIRECTION_ADDRESS=40003};

	enum {SIGNAL_VALUE_GREEN=0,
                SIGNAL_VALUE_BLUE=1,
                SIGNAL_VALUE_RED=2,
	            SIGNAL_VALUE_GREY=3};

	enum {SIGNALS_COUNT=95,
                PARAMETERS_COUNT=7,
                MESSAGES_COUNT=2};

	enum {ALERT_ERROR_MESSAGES_TYPE=2};


	enum {INDEX_SIGNAL_BLOCK_CIRCUT_STATUS=1,
	INDEX_SIGNAL_BREAKING_PATH_STATUS,
    INDEX_CHANNEL_A_STATUS,
    INDEX_CHANNEL_B_STATUS,
    INDEX_CHANNEL_C_STATUS,
    INDEX_OF_BLOCK_CIRCUTS,
    INDEX_OF_MASHZAL_DOOR=52,
    INDEX_OF_CONTACTORS=71,
    INDEX_OF_AUTOMATS=79,
    INDEX_OF_OTHER=87};

	enum {
	INDEX_PARAM_STATE_VALUE=1,
	INDEX_PARAM_MODE_VALUE=2,
	INDEX_PARAM_READY_VALUE=3,
	INDEX_PARAM_BREAKING_PATH_VALUE=4,
	INDEX_PARAM_RUNNING_PATH_VALUE=5,
	INDEX_PARAM_ISOLATION_RESISTANCE_VALUE=6};

	escalator_data_block() :
	data_block( SIGNALS_COUNT,
					   MESSAGES_COUNT,
					   PARAMETERS_COUNT) {};

   virtual int get_data_block_type_description(){ return data_block::DATA_BLOCK_ESCALATOR;};
   
   virtual ~escalator_data_block(){};
};

#endif
