#ifndef _OBJECTS_STORAGE_LOGGER_
#define  _OBJECTS_STORAGE_LOGGER_
class objects_storage_logger  {
  friend class friend_class_of_objects_storage_logger_only_for_compiler_happening;
    private :
      objects_storage_logger(){};
      objects_storage_logger(const objects_storage_logger&){};
      objects_storage_logger& operator=(const objects_storage_logger& obj_stor_logger_inst){return const_cast<objects_storage_logger&>(obj_stor_logger_inst);};
      ~objects_storage_logger(){};

    public:
      enum message_type {DEBUG_MSG, INFO_MSG, ERROR_MSG};
      void log_message(const message_type msg_type,
                                     const string message);
      static objects_storage_logger& get_instance();
};
#endif
