#ifndef _ESCALATOR_DATA_BLOCK_
#define _ESCALATOR_DATA_BLOCK_

/*
Fields of struct escalator_data_block;

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
	number of that message in buffer
	max value COUNT_OF_MESSAGES if 0-buffer is empty
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


typedef struct data_block
{
	typedef  vector<word> data_container;
	enum {
	COUNT_OF_CONTACTORS_REGISTERS=1,
	COUNT_OF_AUTOMATS_REGISTERS=1,
	COUNT_OF_OTHER_REGISTERS=1,
	COUNT_OF_MESSAGES=8,
	COUNT_OF_BLOCK_CIRCUT_REGISTERS=9
	};

	data_container signals;
	data_container  messages;
	
	word 	status;
	word	 mode;
	word  ready;
	word block_circuts_status;
	word breaking_path_status;
	word channels;
	word	 message_number;
	word	 count_of_messages;
	word	 reserved1;
	dword breaking_path_value;
	dword running_path_value;
	word	 reserved2;
	word reserved3;
	
	data_block() :
	signals(COUNT_OF_BLOCK_CIRCUT_REGISTERS+
                          COUNT_OF_CONTACTORS_REGISTERS+
                          COUNT_OF_AUTOMATS_REGISTERS+
                          COUNT_OF_OTHER_REGISTERS
						),
	messages(COUNT_OF_MESSAGES)
	{};	

};

#endif