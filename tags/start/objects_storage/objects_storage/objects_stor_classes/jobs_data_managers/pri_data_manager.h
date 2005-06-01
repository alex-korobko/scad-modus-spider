class pri_data_manager :
                 public  abstract_job_data_manager {
       private:
        vector<byte> process_command_to_occupate( vector<byte> command);
        vector<byte> process_command_to_free( vector<byte> command);
      
        pri_data_manager() {};
        static pri_data_manager* pri_mgr_instance;
		public :
           virtual vector<byte> process_command(vector<byte> command);

           virtual ~pri_data_manager() {};
           static int data_manager_type;
           static  pri_data_manager* get_instance();
}; 
   
