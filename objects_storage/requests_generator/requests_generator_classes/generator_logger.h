class generator_logger {
    private :
      generator_logger(){};

      static generator_logger* logger_instance;
    public:
      enum message_type {DEBUG_MSG, INFO_MSG, ERROR_MSG};
      void log_message(const message_type msg_type,
                                     const string message);
       static generator_logger* get_instance();
};
