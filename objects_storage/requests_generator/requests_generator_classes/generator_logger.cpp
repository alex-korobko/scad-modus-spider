#include <string>

#include <sys/slog.h>
#include <sys/slogcodes.h>

using namespace std;

#include "generator_logger.h"

void 
    generator_logger::log_message
                                   (const message_type msg_type,
                                     const string message) {
string buffer;
buffer.reserve(message.size()+12);

	switch(msg_type)
	{
		case ERROR_MSG:
			buffer="ERROR: ";
			buffer+=message;
			vslogf(_SLOG_SETCODE(_SLOGC_REQ_GEN, 0), _SLOG_ERROR, buffer.c_str(), NULL);
			break;
		case INFO_MSG:
			buffer="INFO: ";
			buffer+=message;
			vslogf(_SLOG_SETCODE(_SLOGC_REQ_GEN, 0), _SLOG_INFO, buffer.c_str(), NULL);
      		break;
		case DEBUG_MSG:
			buffer="DEBUG: ";
			buffer+=message;
			vslogf(_SLOG_SETCODE(_SLOGC_REQ_GEN, 0), _SLOG_DEBUG1, buffer.c_str(), NULL);
      		break;
      	default:
			buffer="UNDEFINED: ";
			buffer+=message;
			vslogf(_SLOG_SETCODE(_SLOGC_REQ_GEN, 0), _SLOG_ERROR, buffer.c_str(), NULL);
			break;
  };
}

generator_logger&
           generator_logger::get_instance(){
      static generator_logger gen_logger_inst;
      return gen_logger_inst;
 };

