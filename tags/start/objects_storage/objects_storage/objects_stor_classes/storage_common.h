#ifndef _STORAGE_COMMON_
#define _STORAGE_COMMON_

typedef uint8_t byte;
typedef uint16_t	word;

namespace program_settings {
    enum { DESCRIPTOR_INDEX=0,
                  COMMAND_CODE_INDEX=1,
                  SEND_RECEIVE_BUFFER_SIZE=1024,
                  MAX_FAILURES_COUNT=2};
};
namespace channel_initialization {
enum {INIT_REG_NEW_PID=1,
             INIT_SEND_TO_MASTER=1,
             INIT_MESSAGE_SIZE=2};
};

namespace descriptor_header {
       enum { OFFSET_CHANNEL_D_UPO_NUM=0, // offset in system message header to channel d UPO NUMBER
           OFFSET_CHANNEL_D_IE1_NUM=1, // offset in system message header to channel d IE1 NUMBER
           OFFSET_CHANNEL_D_E1_NUM=2, // offset in system message header to channel d E1 NUMBER
           OFFSET_CHANNEL_D_CHAN_NUM=3, // offset in system message header to channel d CHANNEL NUMBER
           OFFSET_CALL_POINTER_NUM=7, // offset in system message header to pointer  to call NUMBER
           OFFSET_CALL_POINTER_FIRST=8, // offset in system message header to pointer  to call FIRST
           OFFSET_CALL_POINTER_SECOND=9, // offset in system message header to pointer  to call SECOND
           DESCRIPTOR_SIZE=10  };// descriptor header size
};

namespace database_settings {
  enum { UPO_COUNT=4,
                IE1_COUNT=16,
                E1_COUNT=16,
                CHANNELB_COUNT=32 };
};

namespace data_managers_types{
    enum { PRI_DATA_MANAGER = 2 };
;}

//COMMON COMMANDS
namespace common_commands {
      enum {
      COMMON_UNKNOWN_COMMAND=0
       };
};
//PRI COMMANDS
namespace pri_commands {
  enum {PRI_COMMAND_REQUEST_TO_OCCUPATE_CODE=1,
                PRI_COMMAND_REQUEST_TO_FREE_CODE=2,
                PRI_COMMAND_ANSWER_TO_OCCUPATE_CODE=11,
                PRI_COMMAND_ANSWER_TO_FREE_CODE=12,
                PRI_COMMAND_ERROR_IN_OCCUPATION=101,
                PRI_COMMAND_ERROR_IN_FREE=102,
                PRI_UNKNOWN_COMMAND=201,
                PRI_BAD_COMMAND=202};
};

namespace pri_commands_sizes {
 enum {PRI_COMMAND_REQUEST_TO_OCCUPATE_SIZE=13,
PRI_COMMAND_REQUEST_TO_FREE_SIZE=12};
};
// splitting and merging per bytes -  very dangerous templates using
	template<typename T>
	vector<byte> bytes_of_type (T value_to_splitting) {
		vector<byte> ret_val(sizeof(T));
		for (vector<byte>::size_type i=0; i<ret_val.size();i++){
			ret_val[i]=((value_to_splitting & (0x00FF<<i*8))>>i*8);
		};
		return  ret_val;
   };

	template<typename T>
	T type_from_bytes (vector<byte> bytes_to_merging){
			T ret_val=0;
			vector<byte>::size_type count=__min(bytes_to_merging.size(), sizeof(T));
			for (vector<byte>::size_type i=0; i<count; i++){
				ret_val |=(bytes_to_merging[i] << (i*8) );
            };

            return  ret_val;
    };


#endif //_STORAGE_COMMON_H_

