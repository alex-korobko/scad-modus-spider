using namespace std;

#include <string>

#include <sys/slog.h>
#include <sys/slogcodes.h>

#include "generator_logger.h"

generator_logger* generator_logger::logger_instance=NULL;

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

generator_logger* 
           generator_logger::get_instance(){
        if (logger_instance==NULL) {
                  logger_instance=new generator_logger();
          };

      return logger_instance;
 };

