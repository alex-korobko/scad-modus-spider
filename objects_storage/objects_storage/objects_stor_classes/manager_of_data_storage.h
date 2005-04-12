#ifndef _MANAGER_OF_DATA_STORAGE_
#define  _MANAGER_OF_DATA_STORAGE_

class manager_of_data_storage {
  friend class friend_class_of_manager_of_data_storage_only_for_compiler_happening;
  private :
//disable external using for singleton
      manager_of_data_storage () {};
      manager_of_data_storage (const manager_of_data_storage&) {};
      manager_of_data_storage& operator=(const manager_of_data_storage& mgr) { return const_cast<manager_of_data_storage&>(mgr);};
      ~manager_of_data_storage(){};

      static manager_of_data_storage& get_instance();

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

#endif
