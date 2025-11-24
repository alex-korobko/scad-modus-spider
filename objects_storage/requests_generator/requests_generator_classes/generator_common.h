#ifndef _GENERATOR_COMMON_
#define _GENERATOR_COMMON_

typedef uint8_t byte;
typedef uint16_t	word;

typedef vector<byte> bytes;
typedef vector<uint64_t> results;

namespace program_settings {
enum { HEADER_SIZE_IN_BYTES=12,
              COMMAND_CODE_INDEX=1,
              RECEIVE_BUFFER_SIZE=1024,
              MAX_FAILURES_COUNT=2
            };
}; //namespace program_settings

namespace  message_header_type {
enum {SERVICE_QUERY=0,
            ROUTING_QUERY=1,
            DATA_QUERY=2
            };
 }; //namespace  message_header_type

namespace message_header_description {
enum {PID_REGISTRATION=1,
             TID_REGISTRATION=3
            };
}; //namespace message_header_description
namespace  message_header_uid {
enum {UID_UNKNOWN=0,
             UID_CALL_CONTROL=0x12,
             UID_DEPOSITORY=0x13,
             UID_PROC_CONTROL=0x14
            };
}; //namespace  message_header_uid 

namespace message_header_description_flag {
enum { BLOCK_UID_FLAG=0x01,
             DEST_TID_STATIC=0x02,
             WAIT_MSG_FLAG=0x04,
             SRC_TID_STATIC=0x20
             };
}; //namespace message_header_description_flag


/*
	common message structure:
+0:	tMsgHead:
				Type					byte
				Description			byte
				DestinationUID		word		tUIDTypes
				DestinationTID		word		tHandlingSubstances
				SourceUID			word		tUIDTypes
				SourceTID			word		tHandlingSubstances
+10:	MessageClass				byte
		reserve						byte
+12:	content

*/
namespace  message_header_bytes {
enum { 
          INDEX_OF_MESSAGE_TYPE=0,
          INDEX_OF_MESSAGE_DESCRIPTION,
          INDEX_OF_MESSAGE_DESTINATION_UID,
          INDEX_OF_MESSAGE_DESTINATION_TID=4,
          INDEX_OF_MESSAGE_SOURCE_UID=6,
          INDEX_OF_MESSAGE_SOURCE_TID=8,
          INDEX_OF_MESSAGE_CLASS=10,
          INDEX_OF_RESERVED_VALUE=11,
          INDEX_OF_MESSAGE_CONTENT_FIRST_BYTE=12
////    old format of header
//           OFFSET_CHANNEL_D_UPO_NUM=0, // offset in system message header to channel d UPO NUMBER
//           OFFSET_CHANNEL_D_IE1_NUM=1, // offset in system message header to channel d IE1 NUMBER
//           OFFSET_CHANNEL_D_E1_NUM=2, // offset in system message header to channel d E1 NUMBER
//           OFFSET_CHANNEL_D_CHAN_NUM=3, // offset in system message header to channel d CHANNEL NUMBER
//           OFFSET_CALL_POINTER_NUM=7, // offset in system message header to pointer  to call NUMBER
//           OFFSET_CALL_POINTER_FIRST=8, // offset in system message header to pointer  to call FIRST
//           OFFSET_CALL_POINTER_SECOND=9, // offset in system message header to pointer  to call SECOND
//           DESCRIPTOR_SIZE=10  // descriptor header size
           };
 }; //namespace  message_header_bytes

namespace message_content_bytes {
      enum {
           INDEX_OF_MESSAGE_CODE=0
        };
}; //namespace message_content_bytes

namespace database_settings{
enum { UPO_COUNT=4,
                IE1_COUNT=16,
                E1_COUNT=16,
                CHANNELB_COUNT=32 
            };
 }; //namespace database_settings

//COMMON COMMANDS
namespace common_commands {
enum {
          COMMON_UNKNOWN_COMMAND=0
          };
}; //  common_commands

//PROC CONTROL COMMANS
namespace proc_control_commands{
  enum {QUERY_STATE_OF_CHANNEL_B=1, ////ATTENTION - I don`t know code
               ANSWER_STATE_OF_CHANNEL_B=2, //ATTENTION - I don`t know code
               ERROR_IN_STATE_OF_CHANNEL_B=101, ////ATTENTION - I don`t know code
               UNKNOWN_COMMAND=201,
               BAD_COMMAND=202
             };
}; //namespace proc_control_commands

namespace  proc_control_commands_sizes {
 enum {QUERY_STATE_OF_CHANNEL_B=5, //ATTENTION - I don`t know code
               ANSWER_STATE_OF_CHANNEL_B=2, //ATTENTION - I don`t know code
               ERROR_IN_STATE_OF_CHANNEL_B=1, //ATTENTION - I don`t know code
               UNKNOWN_COMMAND=1,
               BAD_COMMAND=1
 };
}; //namespace  proc_control_commands_sizes

//CALL CONTROL COMMANS
namespace call_control_commands{
  enum {REQUEST_TO_OCCUPATE_CHANNEL_B_SIDE_A=1,
               REQUEST_TO_FREE_CHANNEL_B=2,
               REQUEST_TO_OCCUPATE_CHANNEL_B_SIDE_B=6,
               REQUEST_TO_OCCUPATE_CALL_POINTER=7,
               REQUEST_TO_FREE_CALL_POINTER=8,

               ANSWER_TO_OCCUPATE_CHANNEL_B_SIDE_A=11,
               ANSWER_TO_OCCUPATE_CHANNEL_B_SIDE_B=16,
               ANSWER_TO_FREE_CHANNEL_B=12,
               ANSWER_TO_OCCUPATE_CALL_POINTER=17,
               ANSWER_TO_FREE_CALL_POINTER=18,

               ERROR_IN_CHANNEL_B_OCCUPATION_SIDE_A=101,
               ERROR_IN_CHANNEL_B_OCCUPATION_SIDE_B=106,
               ERROR_IN_CHANNEL_B_FREE=102,
               ERROR_IN_CALL_POINTER_OCCUPATE=108,
               ERROR_IN_CALL_POINTER_FREE=109,
               UNKNOWN_COMMAND=201,
               BAD_COMMAND=202
             };
}; //namespace call_control_commands

namespace  call_control_commands_sizes {
 enum { REQUEST_TO_OCCUPATE_CHANNEL_B_SIDE_A=8,
             REQUEST_TO_FREE_CHANNEL_B=5,
             REQUEST_TO_OCCUPATE_CHANNEL_B_SIDE_B=8,
             REQUEST_TO_OCCUPATE_CALL_POINTER=5,
             REQUEST_TO_FREE_CALL_POINTER=5,

             ANSWER_TO_OCCUPATE_CHANNEL_B_SIDE_A=5,
             ANSWER_TO_OCCUPATE_CHANNEL_B_SIDE_B=12,
             ANSWER_TO_FREE_CHANNEL_B=1,
             ANSWER_TO_OCCUPATE_CALL_POINTER=3,
             ANSWER_TO_FREE_CALL_POINTER=1,

             ERROR_IN_CHANNEL_B_OCCUPATION_SIDE_A=1,
             ERROR_IN_CHANNEL_B_OCCUPATION_SIDE_B=1,
             ERROR_IN_CHANNEL_B_FREE=1,
             ERROR_IN_CALL_POINTER_OCCUPATE=1,
             ERROR_IN_CALL_POINTER_FREE=1,
             UNKNOWN_COMMAND=1,
             BAD_COMMAND=1
            };
}; //namespace  call_control_commands_sizes

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

