#ifndef __GENERATOR_LOGGER__
#define __GENERATOR_LOGGER__
class generator_logger {
  friend class friend_class_of_generator_logger_only_for_compiler_happening;
    private :
      generator_logger(){};
      generator_logger(const generator_logger&){};
      generator_logger& operator=(const generator_logger& obj_stor_logger_inst){return const_cast<generator_logger&>(obj_stor_logger_inst);};
      ~generator_logger(){};

    public:
      enum message_type {DEBUG_MSG, INFO_MSG, ERROR_MSG};
      void log_message(const message_type msg_type,
                                     const string message);
       static generator_logger& get_instance();
};
#endif
