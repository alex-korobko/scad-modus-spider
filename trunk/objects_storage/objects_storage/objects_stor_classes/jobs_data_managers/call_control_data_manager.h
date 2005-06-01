#ifndef _CALL_CONTROL_MANAGER_
#define _CALL_CONTROL_MANAGER_

class call_control_manager {
       friend class friend_class_of_call_control_manager_only_for_compiler_happening;
       private:
//external using disabled for singleton
        call_control_manager() {};
        call_control_manager(const call_control_manager&){};
        call_control_manager& operator=(const call_control_manager& mngr){return const_cast<call_control_manager&>(mngr);};
        ~call_control_manager() {};


        vector<byte> process_command_to_occupate_channel_b_side_a( vector<byte> &command);
        vector<byte> process_command_to_occupate_channel_b_side_b( vector<byte> &command);
        vector<byte> process_command_to_free_channel_b( vector<byte> &command);
        vector<byte> process_command_to_occupate_call_pointer( vector<byte> &command);
        vector<byte> process_command_to_free_call_pointer( vector<byte> &command);
		public :
           vector<byte> process_command(vector<byte> &command);

           static  call_control_manager& get_instance();
}; 

#endif
