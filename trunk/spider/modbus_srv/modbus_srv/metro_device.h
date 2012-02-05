#ifndef __METRO_DEVICE__
#define __METRO_DEVICE__

class metro_device {
private:
  typedef	vector<byte> messages_container;
  typedef	vector<byte> command_data;
  typedef	vector<command_data> command_data_container;

 pthread_cond_t  data_transfer_process_cond_var;
 pthread_mutex_t  data_transfer_process_mutex;

 pthread_mutex_t  default_answer_to_socket_mutex;
 pthread_mutex_t  sockets_to_device_queue_mutex;

 int device_number;
 byte upper_message_id;
 messages_container messages;
 word processed_function_number;

command_data answer_to_socket;
command_data_container request_to_com_port;

command_data default_answer_to_socket;

bool log_packets;

     class metro_device_exception {
       private:
          string test_exc_descr;

        public:
           explicit metro_device_exception (string description) : 
                   test_exc_descr (description) {};

            string get_description () {return test_exc_descr;};
       };
protected:

void set_command_request_to_comport_buffer(command_data_container new_request_to_com_port) {
         request_to_com_port=new_request_to_com_port;
};

public:

metro_device(int new_device_number, bool new_log_packets) 
                throw (metro_device_exception);

virtual ~metro_device();
/*
getters and setters
*/

void set_command_answer_to_socket_buffer(command_data new_answer_to_socket) { 
                  answer_to_socket.swap(new_answer_to_socket);
};


void set_default_answer_to_socket() {
        default_answer_to_socket = answer_to_socket;
};


/*
    getters
*/
pthread_cond_t* get_data_transfer_process_cond_var() { return &data_transfer_process_cond_var;}
pthread_mutex_t* get_data_transfer_process_mutex() { return &data_transfer_process_mutex;}
pthread_mutex_t* get_sockets_to_device_queue_mutex() {return &sockets_to_device_queue_mutex;};
pthread_mutex_t* get_default_answer_to_socket_mutex() {return &default_answer_to_socket_mutex;};

int get_number() {return device_number;};

bool is_packet_logging() const {return log_packets;};

command_data  get_command_answer_to_socket_buffer() { 
     return answer_to_socket;
 };

command_data get_default_answer_to_socket_buffer() {
      return default_answer_to_socket;
};

command_data_container get_command_request_to_comport_buffer() {
     return request_to_com_port;
};

void clear_command_request_to_comport_buffer() {
     request_to_com_port.clear();
};

/*
    messages process
*/
void add_messages_fifo(messages_container messages_from_device); //from comport
messages_container get_messages_fifo(){ return messages;}; //for socket
byte get_upper_message_id() { return upper_message_id;};
byte metro_device::get_messages_count(){ 
      return messages.size();
};

/*
    virtual functions
*/
virtual void 
     put_command_answer_from_comport
                  (metro_device::command_data_container answer_from_com_port)=0;

virtual void 
    put_command_request_from_socket
   (command_data request_from_socket)=0;

virtual command_data_container
   get_command_request_to_comport
        ()=0;

virtual command_data_container
    get_default_command_request_to_comport
                     ()=0;

virtual metro_device::command_data
     get_default_command_request_from_socket()=0;

/*
static functions
*/
static command_data
generate_error_answer_for_function
                             ( int device_number,
                               int function_code,
                               int exception_code);
};

#endif
