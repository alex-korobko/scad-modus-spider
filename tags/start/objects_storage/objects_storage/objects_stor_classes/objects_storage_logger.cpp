using namespace std;

#include <string>

#include <sys/slog.h>
#include <sys/slogcodes.h>

#include "objects_storage_logger.h"

objects_storage_logger* objects_storage_logger::logger_instance=NULL;

void 
    objects_storage_logger::log_message
                                   (const message_type msg_type,
                                     const string message) {
string buffer;
buffer.reserve(message.size()+12);

	switch(msg_type)
	{
		case ERROR_MSG:
			buffer="ERROR: ";
			buffer+=message;
			vslogf(_SLOG_SETCODE(_SLOGC_OBJ_STOR, 0), _SLOG_ERROR, buffer.c_str(), NULL);
			break;
		case INFO_MSG:
			buffer="INFO: ";
			buffer+=message;
			vslogf(_SLOG_SETCODE(_SLOGC_OBJ_STOR, 0), _SLOG_INFO, buffer.c_str(), NULL);
      		break;
		case DEBUG_MSG:
			buffer="DEBUG: ";
			buffer+=message;
			vslogf(_SLOG_SETCODE(_SLOGC_OBJ_STOR, 0), _SLOG_DEBUG1, buffer.c_str(), NULL);
      		break;
      	default:
			buffer="UNDEFINED: ";
			buffer+=message;
			vslogf(_SLOG_SETCODE(_SLOGC_OBJ_STOR, 0), _SLOG_ERROR, buffer.c_str(), NULL);
			break;
  };
}

objects_storage_logger* 
           objects_storage_logger::get_instance(){
        if (logger_instance==NULL) {
                  logger_instance=new objects_storage_logger();
          };

      return logger_instance;
 };

