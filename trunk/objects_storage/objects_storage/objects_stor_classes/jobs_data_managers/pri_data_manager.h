#ifndef _PRI_DATA_MANAGER_
#define _PRI_DATA_MANAGER_

class pri_data_manager {
       friend class friend_class_of_pri_data_manager_only_for_compiler_happening;
       private:
//external using disabled for singleton
        pri_data_manager() {};
        pri_data_manager(const pri_data_manager&){};
        pri_data_manager& operator=(const pri_data_manager& mngr){return const_cast<pri_data_manager&>(mngr);};
        ~pri_data_manager() {};


        vector<byte> process_command_to_occupate( vector<byte> command);
        vector<byte> process_command_to_free( vector<byte> command);
		public :
           vector<byte> process_command(vector<byte> command);

           static  pri_data_manager& get_instance();
}; 

#endif
