#ifndef __ESCALATOR_H__
#define __ESCALATOR_H__

/*
Fields of struct data_block;

status : 
	modbus register 30001 Condition 
	escakator`s status 
	see enum  STATE_UP_...  in system_settings class for values
mode :
	modbus register 30002 Mode
	escakator`s mode
	see enum  MODE_... in system_settings class for values
ready :
	modbus register 30003 Gotov
	escakator`s ready state
	see enum  STATE_MAIN_... in system_settings class for values
block_circuts_status :
	modbus register 30004 BlockCircute
	escalator`s block circuts state
	1-normal 0-not normal Other-not defined
breaking_path_status :
	modbus register 30005 BremseCondition
	escalator`s  breaking path state
	1-normal 0-not normal Other-not defined
channels :
	modbus register 30006 BU1, BU2, BU3
	state of three undepended control channels
	each of channels occupate two binary positions in register
		(in binary) 00-blue 01-green 11-gray //NOTICE: check gray value 
		data of BU1 occupate 0 and 1 positions of register( 2^0 and 2^1)
		data of BU2 occupate 2 and 3 positions of register( 2^2 and 2^3)
		data of BU3 occupate 4 and 5 positions of register( 2^4 and 2^5)
message_number :
	modbus register 30007 Messer
	number of that message in buffer
	max value COUNT_OF_MESSAGES if 0-buffer is empty
count_of_messages :
	modbus register 30008 Upper
	count of messages in buffer (registers 30031-30051)
	max value COUNT_OF_MESSAGES if 0-buffer is empty
reserved1 :
	modbus register 30009 no name
	reserved by future
breaking_path_value:
	modbus registers 30010-30011 Nbremse
	value of breaking path
running_path_value:
	modbus registers 30012-30013 Nprobeg
	value of running path
reserved2 :
	modbus register 30014 no name
	reserved by future
reserved3 :
	modbus register 30015 no name
	reserved by future
signals :
	modbus registers at 30016 
	to 30016
		+COUNT_OF_BLOCK_CIRCUT_REGISTERS
		+COUNT_OF_CONTACTORS_REGISTERS
		+COUNT_OF_AUTOMATS_REGISTERS
		+COUNT_OF_OTHER_REGISTERS
	includes 
		modbus registers 
			30016 -
			30016
			+COUNT_OF_BLOCK_CIRCUT_REGISTERS
			NBlock
			each block circut occupate two binary positions in register
			in each register can contain 8 block circuts
			values (binary)
				00-green 01-blue  10-red 11-grey

		modbus registers
			30016
			+COUNT_OF_BLOCK_CIRCUT_REGISTERS
			+1  -
			30016
			+COUNT_OF_BLOCK_CIRCUT_REGISTERS
			+COUNT_OF_CONTACTORS_REGISTERS 
			NContactor
			each contactor occupate two binary positions in register
			in each register can contain 8 contactors
			values (binary)
				00-green 01-blue  10-red 11-grey

		modbus registers 
			30016
			+COUNT_OF_BLOCK_CIRCUT_REGISTERS
			+COUNT_OF_CONTACTORS_REGISTERS 
			+1 -
			30016
			+COUNT_OF_BLOCK_CIRCUT_REGISTERS
			+COUNT_OF_CONTACTORS_REGISTERS 
			+COUNT_OF_AUTOMATS_REGISTERS
			NAutomat
			each automat occupate two binary positions in register
			in each register can contain 8 automats
			values (binary)
				00-green 01-blue  10-red 11-grey


		modbus registers 
			30016
			+COUNT_OF_BLOCK_CIRCUT_REGISTERS
			+COUNT_OF_CONTACTORS_REGISTERS 
			+COUNT_OF_AUTOMATS_REGISTERS
			+1 -
			30016
			+COUNT_OF_BLOCK_CIRCUT_REGISTERS
			+COUNT_OF_CONTACTORS_REGISTERS 
			+COUNT_OF_AUTOMATS_REGISTERS
			+COUNT_OF_OTHER_REGISTERS
			NOS
			each other parameter occupate two binary positions in register
			in each register can contain 8 other parameters
			values (binary)
				00-green 01-blue  10-red 11-grey

signals :
	modbus registers at
	30031	
	to 
		30031
		+COUNT_OF_MESSAGES
	includes 
		messages 
*/


typedef struct data_block
{
	enum {
	COUNT_OF_CONTACTORS_REGISTERS=1,
	COUNT_OF_AUTOMATS_REGISTERS=1,
	COUNT_OF_OTHER_REGISTERS=1,
	COUNT_OF_MESSAGES=8,
	COUNT_OF_BLOCK_CIRCUT_REGISTERS=9
	};

	vector<word> signals;
	vector<word>  messages;
	
	word 	status;
	word	 mode;
	word  ready;
	word block_circuts_status;
	word breaking_path_status;
	word channels;
	word	 message_number;
	word	 count_of_messages;
	word	 reserved1;
	dword breaking_path_value;
	dword running_path_value;
	word	 reserved2;
	word reserved3;
	
	data_block() :
	signals(COUNT_OF_BLOCK_CIRCUT_REGISTERS+
                          COUNT_OF_CONTACTORS_REGISTERS+
                          COUNT_OF_AUTOMATS_REGISTERS+
                          COUNT_OF_OTHER_REGISTERS
						),
	messages(COUNT_OF_MESSAGES)
	{};	

};

// escalator class
class metro_escalator
{
public:	
	typedef vector<byte> buffer_data_type;
	typedef vector<byte>::iterator	 buffer_data_iterator;

private: 				
   	PtWidget_t *wnd, *arrow, *num_label, *block_label;

   	int			id_station;   	
   	int 			number;
   	int 			type;
   	dword 		id_escalator;

   	pthread_t	tid;   

	int				sleepticks;
   	int 			enabled;
   	int				online;
   	int				direction;
   	int				prefDirection;
	int last_message_remote_id;
	//for storing id of message in remote container (attention! not a self message id!) last recived
	time_t  last_message_remote_time;
	//for storing time of message in remote container (attention! not a self message id!) last recived

    data_block dataBlock;
   	
   	buffer_data_type	outBuffer;
   	buffer_data_type	inBuffer;
   	
   	in_addr_t	ip;
   	
   	pthread_mutex_t		outMutex;
   	pthread_mutex_t		inMutex;

   	int				connection_id;
   	int				channel_id;
   	pid_t			pulse;

public:	

//class for locking mutexes for class metro escalator
	class lock_mutex
	{
	private:
		lock_mutex() ;
		pthread_mutex_t  now_used_mutex;

	public:
		lock_mutex(pthread_mutex_t  mutex_for_lock) 
				{
				now_used_mutex=mutex_for_lock;
				pthread_mutex_lock( &mutex_for_lock);
				};
		~lock_mutex()
				{
				pthread_mutex_unlock( &now_used_mutex);
				};
	};

/*
metro escalator`s metods
*/

/*
constructors and destructors
*/	

metro_escalator(
		int new_id_escalator, 
		int new_id_station,
		int new_type,
		int new_number,
		int new_pref_direction,
		int new_direction,
		int new_enabled,
		in_addr_t	new_ip
		):
	id_station(new_id_station),
	number(new_number),
	type(new_type),
	id_escalator(new_id_escalator),
	
	enabled(new_enabled),
	online(system_settings::OFFLINE),
	direction (new_direction),
	prefDirection(new_pref_direction),
	last_message_remote_id(0),
	
	inBuffer(1024),
	
	ip(new_ip)
{

	wnd = arrow = num_label = block_label = NULL;

	connection_id = 0;	
	pulse = 0;
	sleepticks = 0;

	pthread_mutex_init(&outMutex, NULL);
	pthread_mutex_init(&inMutex, NULL);
	
}


~metro_escalator()
{
	if (pthread_cancel(tid) != EOK)
	{
		string mess="Fail to close escalator's thread ";
		mess+=strerror(errno);
		g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
	}

	pthread_mutex_destroy(&inMutex);
	pthread_mutex_destroy(&outMutex);
}


/*
get_ and set_  metods for private data members
*/
   	
   	PtWidget_t*  get_station_widget();

   PtWidget_t*  get_block_label_widget() {return (block_label);};
   PtWidget_t*  get_arrow_label_widget() {return (arrow);};
   
	dword get_id() { return id_escalator; };
	void set_id (dword new_id) {id_escalator=new_id;};
	
   	int get_number() { return number; }
	void set_number(int new_number) { number=new_number;};

   	int get_type() { return type; };
	void set_type(int new_escalator_type) { type=new_escalator_type;};
	
   	int get_direction() { return direction; };   	
   	void set_direction(int new_dir) { direction=new_dir; };   	

   	int get_pref_direction() { return prefDirection; };   	
   	void set_pref_direction(int new_dir) { prefDirection=new_dir; };   	


   	int get_last_message_remote_id() { return last_message_remote_id; };   	
   	void set_last_message_remote_id(int new_last_message_remote_id) { last_message_remote_id=new_last_message_remote_id; };   	

	int get_sleepticks () {return (sleepticks);};
	void set_sleepticks(int new_sleepticks) {sleepticks=new_sleepticks;};
	void increment_sleepticks() {sleepticks++;};

	int get_connection_id() { return (connection_id);};

   	void get_data();
   	
	in_addr_t get_ip() { return ip; };
	void set_ip(in_addr_t new_ip_address) { ip=new_ip_address; };

	pthread_mutex_t get_out_mutex() {return (outMutex);};
	pthread_mutex_t get_in_mutex() {return (inMutex);};

	int get_enabled() {return(enabled);};
	void set_enabled(int new_enabled_state) {enabled=new_enabled_state;};

	int get_online() {return(online);};
	void set_online(int online_state) {online=online_state;};

	int get_mode() {return (dataBlock.mode);}
	int get_status() {return (dataBlock.status);}
	int get_ready() {return (dataBlock.ready);}
	int get_blocked_status() {return(dataBlock.block_circuts_status);};
	int get_breaking_status() {return(dataBlock.breaking_path_status);};
	
	dword get_breaking_path_value() {return(dataBlock.breaking_path_value);};
	dword get_running_path_value() {return(dataBlock.running_path_value);};

	void swap_in_buffer(buffer_data_type& buffer_to_swap) { inBuffer.swap(buffer_to_swap); };   	
	void append_in_buffer(buffer_data_iterator first_iter, buffer_data_iterator last_iter) { inBuffer.insert(inBuffer.end(), first_iter, last_iter); };   	
	
 	void swap_out_buffer(buffer_data_type& buffer_to_swap) { outBuffer.swap(buffer_to_swap); };   	
 	void append_out_buffer(buffer_data_iterator first_iter, buffer_data_iterator last_iter) { outBuffer.insert(outBuffer.end(), first_iter, last_iter); };   	
   	  	
   	int get_station_id(){return (id_station);};
   	void set_station_id( int new_id_station) { id_station=new_id_station;};
/*
other methods
*/   	

	int update_leds();
	void update_escalator();
	int create_station_window(int x, int y);
	void update_panel();
	void set_state(dword state);
	int set_data();

	int connect_to_channel(int channel);
	int disconnect_from_channel();
	int start();
   	void send_command(byte cmd);   
   	void send_time();   
   	void check_status();
	int confirm_direction(int new_direction);
};

#endif
