class manager_of_data_storage {
  private :
       jobs_data_managers_container jobs_data_managers;

      manager_of_data_storage () {};

        static manager_of_data_storage* mngr_instance;
        static manager_of_data_storage* get_instance();

  public  :
       static uint32_t node_id;
       static pid_t process_id;
       static int channel_id;

       static byte descriptor_call_pointer_num;
       static byte descriptor_call_pointer_first;
       static byte descriptior_call_pointer_second;

       static byte descriptor_channeld_upo_num;
       static byte descriptor_channeld_ie1_num;
       static byte descriptor_channeld_e1_num;
       static byte descriptor_channeld_channel_num;

       static void initialize() throw (objects_storage_exception);
       static void run() throw (objects_storage_exception);
};
