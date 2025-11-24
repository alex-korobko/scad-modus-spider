#ifndef _PROC_CONTROL_MANAGER_
#define _PROC_CONTROL_MANAGER_

class proc_control_manager {
       friend class friend_class_of_proc_control_manager_only_for_compiler_happening;
       private:
//external using disabled for singleton
        proc_control_manager() {};
        proc_control_manager(const proc_control_manager&){};
        proc_control_manager& operator=(const proc_control_manager& mngr){return const_cast<proc_control_manager&>(mngr);};
        ~proc_control_manager() {};


        vector<byte> process_command_to_query_state_of_channel_b( vector<byte> &command);
		public :
           vector<byte> process_command(vector<byte> &command);

           static  proc_control_manager& get_instance();
}; 

#endif



