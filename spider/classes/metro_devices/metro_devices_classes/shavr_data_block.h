#ifndef _SHAVR_DATA_BLOCK_
#define _SHAVR_DATA_BLOCK_

/*
Fields of struct shavr_data_block

modbus register 30008
     high byte
        bit number (1 active state)
        7 Escalator number 4 ready to suppling by input 2
        6 Escalator number 4 is now suppled from input 2
        5 Escalator number 4 ready to suppling by input 1
        4 Escalator number 4 is now suppled from input 1
        3 Escalator number 3 ready to suppling by input 2
        2 Escalator number 3 is now suppled from input 2
        1 Escalator number 3 ready to suppling by input 1
        0 Escalator number 3 is now suppled from input 1

     low byte
        bit number (1 active state)
        7 Escalator number 2 ready to suppling by input 2
        6 Escalator number 2 is now suppled from input 2
        5 Escalator number 2 ready to suppling by input 1
        4 Escalator number 2 is now suppled from input 1
        3 Escalator number 1 ready to suppling by input 2
        2 Escalator number 1 is now suppled from input 2
        1 Escalator number 1 ready to suppling by input 1
        0 Escalator number 1 is now suppled from input 1

modbus register 30009
     high byte
        bit number (1 active state)
        7 Voltage 24 V is present
        6 Reserved
        5 Reserved
        4 Reserved
        3 Reserved
        2 Door of mashzal is openend
        1 Reserved
        0 Reserved
     low byte
        bit number (1 active state)
        7 Reserved
        6 Voltage 220 V is present on input 2
        5 Voltage 220 V is present on input 1
        4 Voltage 220 V is present on UPS output
        3 Voltage 380 V is present on input 2
        2 Voltage 380 V is present on input 1
        1 Fire alarm (pogarnaya signalizaciya) is turned ON if 0
        0 Smoke sensor (datchik dima) is turned ON if 0

modbus register 30010-30011
         frequency value of input 1

modbus register 30012-30013
         average fase a voltage value of input 1
modbus register 30014-30015
         average fase b voltage value of input 1
modbus register 30016-30017
         average fase c voltage value of input 1
modbus register 30018-30019
         average total fase voltage value of input 1

modbus register 30020-30021
         average fase a-b voltage value of input 1
modbus register 30022-30023
         average fase b-c voltage value of input 1
modbus register 30024-30025
         average fase c-a voltage value of input 1
modbus register 30026-30027
         average equivalent line voltage value of input 1

modbus register 30028-30029
         average fase a current value of input 1
modbus register 30030-30031
         average fase b current value of input 1
modbus register 30032-30033
         average fase c current value of input 1
modbus register 30034-30035
         average equivalent current value of input 1

modbus register 30036-30037
         average total active power value of input 1

modbus register 30038-30039
         average total reactive power value of input 1

modbus register 30040-30041
         average total apparent power value of input 1

modbus register 30042-30043
         frequency value of input 2

modbus register 30044-30045
         average fase a voltage value of input 2
modbus register 30046-30047
         average fase b voltage value of input 2
modbus register 30048-30049
         average fase c voltage value of input 2
modbus register 30050-30051
         average total fase voltage value of input 2

modbus register 30052-30053
         average fase a-b voltage value of input 2
modbus register 30054-30055
         average fase b-c voltage value of input 2
modbus register 30056-30057
         average fase c-a voltage value of input 2
modbus register 30058-30059
         average equivalent line voltage value of input 2

modbus register 30060-30061
         average fase a current value of input 2
modbus register 30062-30063
         average fase b current value of input 2
modbus register 30064-30065
         average fase c current value of input 2
modbus register 30066-30067
         average equivalent current value of input 2

modbus register 30068-30069
         average total active power value of input 2

modbus register 30070-30071
         average total reactive power value of input 2

modbus register 30072-30073
         average total apparent power value of input 2

modbus register 30074
	upper_message_id
	id (from 1000 to 1) of upper message in buffer
	if 0 buffer is empty

modbus register 30075
	count_of_messages 
	count of messages in buffer (registers 30031-30051)
	max value COUNT_OF_MESSAGES if 0-buffer is empty

modbus register 30076-30083
	buffer of messages;
*/


struct shavr_data_block : public data_block{

	enum {SIGNAL_VALUE_GREEN=0,
                 SIGNAL_VALUE_BLUE=1,
                 SIGNAL_VALUE_RED=2,
	             SIGNAL_VALUE_GREY=3};

	enum {
      MODBUS_INPUT_REGISTERS_BEGIN_ADDRESS=30001,
      MODBUS_INPUT_REGISTERS_END_ADDRESS=30008+2+128,

      MODBUS_DATA_REGISTERS_BEGIN_ADDRESS=40001,
      MODBUS_DATA_REGISTERS_END_ADDRESS=40004,

      MODBUS_LOGIC_CELLS_BEGIN_ADDRESS=40,
      MODBUS_LOGIC_CELLS_END_ADDRESS=47
    };

	enum {
	INDEX_SIGNAL_ESCALATOR_1_CONNECTED_TO_INPUT_1=0,
    INDEX_SIGNAL_ESCALATOR_1_TELEMETR_INPUT_1,	
	INDEX_SIGNAL_ESCALATOR_1_CONNECTED_TO_INPUT_2,	
    INDEX_SIGNAL_ESCALATOR_1_TELEMETR_INPUT_2,	

	INDEX_SIGNAL_ESCALATOR_2_CONNECTED_TO_INPUT_1,	
    INDEX_SIGNAL_ESCALATOR_2_TELEMETR_INPUT_1,	
	INDEX_SIGNAL_ESCALATOR_2_CONNECTED_TO_INPUT_2,	
    INDEX_SIGNAL_ESCALATOR_2_TELEMETR_INPUT_2,	

	INDEX_SIGNAL_ESCALATOR_3_CONNECTED_TO_INPUT_1,	
    INDEX_SIGNAL_ESCALATOR_3_TELEMETR_INPUT_1,	
	INDEX_SIGNAL_ESCALATOR_3_CONNECTED_TO_INPUT_2,	
    INDEX_SIGNAL_ESCALATOR_3_TELEMETR_INPUT_2,	

	INDEX_SIGNAL_ESCALATOR_4_CONNECTED_TO_INPUT_1,	
    INDEX_SIGNAL_ESCALATOR_4_TELEMETR_INPUT_1,	
	INDEX_SIGNAL_ESCALATOR_4_CONNECTED_TO_INPUT_2,	
    INDEX_SIGNAL_ESCALATOR_4_TELEMETR_INPUT_2,	
	
	INDEX_SIGNAL_SMOKE_SENSOR=0,
	INDEX_SIGNAL_FIRE_ALARM=1,
    INDEX_SIGNAL_380V_ON_INPUT_1=2,
	INDEX_SIGNAL_380V_ON_INPUT_2=3,
	INDEX_SIGNAL_220V_UPS_OUTPUT=4,
	INDEX_SIGNAL_220V_OUTPUT_1=5,
	INDEX_SIGNAL_220V_OUTPUT_2=6,
	
	INDEX_SIGNAL_DOOR_OPENED=10,
	INDEX_SIGNAL_POWER_SUPPLY_24V=15
};



	enum {COUNT_OF_SIGNALS=33,
	COUNT_OF_MESSAGES=8,
	COUNT_OF_PARAMETERS=16+16+2 };

	word upper_message_id;
	word count_of_messages;

	shavr_data_block() :
		data_block(COUNT_OF_SIGNALS,
                          COUNT_OF_MESSAGES,
                          COUNT_OF_PARAMETERS),
                          upper_message_id(0),
                          count_of_messages(0) {};	

   virtual int get_data_block_type_description(){ return data_block::DATA_BLOCK_SHAVR;};
   
   virtual ~shavr_data_block() {};
};

#endif
