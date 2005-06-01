#ifndef _DOOR_DATA_BLOCK_
#define _DOOR_DATA_BLOCK_

/*
Fields of struct door_data_block

modbus register 30001
     high byte
        bit number (1 active state)
        7 Unused
        6 Unused
        5 Unused
        4 Unused
        3 Unused
        2 Unused
        1 Unused
        0 Unused

     low byte
        bit number (1 active state)
        7 Unused
        6 Unused
        5 Unused
        4 Unused
        3 Unused
        2 Unused
        1 Unused
        0 Door opened  SIGNAL_VALUE_RED (if 0 door closed SIGNAL_VALUE_GREEN)
*/


typedef struct door_data_block : public data_block{

	enum {SIGNAL_VALUE_GREEN=0,
                 SIGNAL_VALUE_BLUE=1,
                 SIGNAL_VALUE_RED=2,
	             SIGNAL_VALUE_GREY=3};

	enum {
      MODBUS_INPUT_REGISTERS_BEGIN_ADDRESS=30001,
      MODBUS_INPUT_REGISTERS_END_ADDRESS=30001,
    };

	enum {
	INDEX_SIGNAL_DOOR_OPENED=0,
    };


	enum {COUNT_OF_SIGNALS=2, //signal with index 0 always unused
	COUNT_OF_MESSAGES=1,
	COUNT_OF_PARAMETERS=0 };

	word upper_message_id;
	word count_of_messages;

	door_data_block() :
		data_block(COUNT_OF_SIGNALS,
                          COUNT_OF_MESSAGES,
                          COUNT_OF_PARAMETERS),
                          upper_message_id(0),
                          count_of_messages(0) {};	

   virtual int get_data_block_type_description(){ return data_block::DATA_BLOCK_DOOR;};
   virtual ~door_data_block() {};
};

#endif

