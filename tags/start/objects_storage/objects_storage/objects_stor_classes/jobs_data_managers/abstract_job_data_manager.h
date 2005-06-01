class abstract_job_data_manager {
     public:
       virtual vector<byte> process_command(vector<byte> command) =0;
       virtual ~abstract_job_data_manager() {};  
};

