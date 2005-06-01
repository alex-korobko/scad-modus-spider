class objects_storage_logger {
    private :
      objects_storage_logger(){};

      static objects_storage_logger* logger_instance;
    public:
      enum message_type {DEBUG_MSG, INFO_MSG, ERROR_MSG};
      void log_message(const message_type msg_type,
                                     const string message);
       static objects_storage_logger* get_instance();
};
