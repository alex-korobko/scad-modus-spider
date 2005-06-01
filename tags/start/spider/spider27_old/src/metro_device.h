#ifndef __METRO_DEVICE__
#define __METRO_DEVICE__

class metro_device {
private:
	typedef vector<byte> buffer_data_type;
	typedef vector<byte>::iterator buffer_data_iterator;

//class for locking mutexes for class metro escalator
	class lock_mutex {
	private:
		lock_mutex();
		pthread_mutex_t  now_used_mutex;

	public:
		lock_mutex(pthread_mutex_t  mutex_for_lock) {
				now_used_mutex=mutex_for_lock;
				pthread_mutex_lock( &mutex_for_lock);
		};

		~lock_mutex(){
				pthread_mutex_unlock( &now_used_mutex);
		};
	};

   	PtWidget_t *wnd;

   	int 		id;
   	int			id_station;
   	int			number;
   	int			type;
	int			sleepticks;
   	bool		enabled;
   	bool		online;

	int last_message_remote_id;
	//for storing id of message in remote container (attention! not a self message id!) last recived
	time_t  last_message_remote_time;
	//for storing time of message in remote container (attention! not a self message id!) last recived

   	in_addr_t	ip;

   	buffer_data_type	out_buffer;
   	buffer_data_type	in_buffer;

   	pthread_mutex_t		out_mutex;
   	pthread_mutex_t		in_mutex;

   	int				connection_id;
   	int				channel_id;
   	pid_t			pulse;

public:

	metro_device( int  id,
                   int	  id_station,
                   int number,
                   int	 type,
                   bool enabled,
                   in_addr_t	ip,
                   int channel)  throw (spider_exception);

	virtual ~metro_device();


/*
get_ and set_  metods for private data members
*/
	PtWidget_t*  get_station_widget();
	
	int create_station_window(int x, int y);
	
	int get_id() { return id; };
	void set_id (dword new_id) {id=new_id;};

	int get_type() { return type; };
	void set_type(int new_type) { type=new_type;};

   	int get_number() { return number; }
	void set_number(int new_number) { number=new_number;};

	int get_last_message_remote_id() { return last_message_remote_id; };
	void set_last_message_remote_id(int new_last_message_remote_id) { last_message_remote_id=new_last_message_remote_id; };

	int get_sleepticks () {return (sleepticks);};
	void set_sleepticks(int new_sleepticks) {sleepticks=new_sleepticks;};
	void increment_sleepticks() {sleepticks++;};

	int get_connection_id() { return (connection_id);};
  	
	in_addr_t get_ip() { return ip; };
	void set_ip(in_addr_t new_ip_address) { ip=new_ip_address; };

	pthread_mutex_t get_out_mutex() {return (out_mutex);};
	pthread_mutex_t get_in_mutex() {return (in_mutex);};

	bool get_enabled() {return(enabled);};
	void set_enabled(bool new_enabled_state) {enabled=new_enabled_state;};
	
	bool get_online() {return(online);};
 	void set_online(bool online_state) {online=online_state;};

	void swap_in_buffer(buffer_data_type& buffer_to_swap) { in_buffer.swap(buffer_to_swap); };
	void append_in_buffer(buffer_data_iterator first_iter, buffer_data_iterator last_iter) { in_buffer.insert(in_buffer.end(), first_iter, last_iter); };
 	void swap_out_buffer(buffer_data_type& buffer_to_swap) { out_buffer.swap(buffer_to_swap); };
 	void append_out_buffer(buffer_data_iterator first_iter, buffer_data_iterator last_iter) { out_buffer.insert(out_buffer.end(), first_iter, last_iter); };
   	  	
   	int get_station_id(){return (id_station);};
  	void set_station_id( int new_id_station) { id_station=new_id_station;};
/*
other metods
*/  	
virtual void send_command(command cmd);
};

#endif