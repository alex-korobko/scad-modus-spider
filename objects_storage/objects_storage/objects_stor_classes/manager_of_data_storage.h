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

/*
          INDEX_OF_MESSAGE_TYPE=0,
          INDEX_OF_MESSAGE_DESCRIPTION,
          INDEX_OF_MESSAGE_DESTINATION_UID,
          INDEX_OF_MESSAGE_DESTINATIN_TID=4,
          INDEX_OF_MESSAGE_SOURCE_UID=6,
          INDEX_OF_MESSAGE_SOURCE_TID=8,
          INDEX_OF_MESSAGE_CLASS=10,
          INDEX_OF_MESSAGE_CONTENT_FIRST_BYTE=12
*/

       static byte message_type;
       static byte message_description;
       static word message_destination_uid;
       static word message_destination_tid;
       static word message_source_uid;
       static word message_source_tid;
       static byte message_class;
       static byte message_reserved_value;

       static void initialize() throw (objects_storage_exception);
       static void run() throw (objects_storage_exception);
};

#endif
