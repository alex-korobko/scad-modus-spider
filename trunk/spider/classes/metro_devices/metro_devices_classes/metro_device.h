#ifndef __METRO_DEVICE__
#define __METRO_DEVICE__

class metro_device {
private:
  typedef vector<byte> buffer_data_type;
  typedef vector<int> related_devices_ids;

    related_devices_ids devices_ids;

//class for locking mutexes for class metro escalator
	class lock_mutex {
	private:
		lock_mutex();
		pthread_mutex_t  *now_used_mutex;

	public:
		lock_mutex(pthread_mutex_t *mutex_for_lock) {
				now_used_mutex=mutex_for_lock;
				pthread_mutex_lock(now_used_mutex);
		};

		~lock_mutex(){
				pthread_mutex_unlock( now_used_mutex);
		};
	};

   	PtWidget_t *device_widget, *station_widget;

   	int 		id;
   	int			id_station;
   	int			number;
   	int			type;

   	int			execution_mode; //see enum system_settings::START_DAY_MODE_ 
   	int			start_hour;
   	int			start_minute;

	int			sleepticks;
   	bool		enabled;
   	bool		must_set_time;

//count of last failures, if  reading data is success so last_failures_count=0
    int last_failures_count;
	int last_message_remote_id;
	//for storing id of message in remote container (attention! not a self message id!) last recived
	time_t  last_message_remote_time;
	//for storing time of message in remote container (attention! not a self message id!) last recived

   	in_addr_t	ip;

   	buffer_data_type	answer_from_device;
   	buffer_data_type	request_for_send_to_device;
   	buffer_data_type	current_request_to_device;

   	pthread_mutex_t answer_from_device_mutex;
   	pthread_mutex_t	request_to_device_mutex;
   	pthread_mutex_t	time_setting_flag_mutex;

   	int				connection_id;
   	int				channel_id;

public:

	//metro device states
	
	metro_device( int  id,
                   int	  id_station,
                   int number,
                   int	 type,
                   int	 start_day_mode,
                   int	 start_hour,
                   int	 start_minute,
                   bool enabled,
                   in_addr_t	ip,
                   int channel)  throw (spider_exception);

	virtual ~metro_device();

/*
get_ and set_  metods for private data members
*/
	PtWidget_t*  get_station_widget() {return station_widget;};
	void  set_station_widget(PtWidget_t *station_wgt ) {station_widget=station_wgt;};

	PtWidget_t*  get_device_widget(){return device_widget;};
	void  set_device_widget(PtWidget_t *device_wgt ){device_widget=device_wgt;};

	int get_id() { return id; };
	void set_id (dword new_id) {id=new_id;};

	int get_type() { return type; };
	void set_type(int new_type) { type=new_type;};

    int get_number() { return number; }
	void set_number(int new_number) { number=new_number;};

	int get_failures_count() { return last_failures_count; };
	void set_failures_count(int failures_count) { last_failures_count=failures_count;};

	int get_last_message_remote_id() { return last_message_remote_id; };
	void set_last_message_remote_id(int new_last_message_remote_id) { last_message_remote_id=new_last_message_remote_id; };

	int get_sleepticks () {return (sleepticks);};
	void set_sleepticks(int new_sleepticks) {sleepticks=new_sleepticks;};
	void increment_sleepticks() {sleepticks++;};

	int get_connection_id() { return (connection_id);};
  	
	in_addr_t get_ip() { return ip; };
	void set_ip(in_addr_t new_ip_address) { ip=new_ip_address; };

	pthread_mutex_t* get_answer_from_device_mutex() {return &answer_from_device_mutex;};
	pthread_mutex_t* get_request_to_device_mutex() {return &request_to_device_mutex;};

	bool get_enabled() {return(enabled);};
	void set_enabled(bool new_enabled_state) {enabled=new_enabled_state;};

	int get_execution_mode() {return execution_mode;};
	void set_execution_mode(int new_execution_mode) {execution_mode=new_execution_mode;};
	
	int get_start_hour() {return start_hour;};
	void set_start_hour(int new_start_hour) {start_hour=new_start_hour;};

	int get_start_minute(){ return start_minute;}
	void set_start_minute(int new_start_minute) {start_minute=new_start_minute;};

   	  	
   	int get_station_id(){return (id_station);};
  	void set_station_id( int new_id_station) { id_station=new_id_station;};

  void set_answer_from_device_buffer (const buffer_data_type answer);
  buffer_data_type get_answer_from_device_buffer();

  void set_request_to_device_buffer (const buffer_data_type request);
  void set_current_request_to_device_buffer (const metro_device::buffer_data_type request);
   buffer_data_type get_request_for_send_to_device_buffer();
   buffer_data_type  get_current_request_to_device_buffer();

//must set time flag setting, checking and unsetting
   	bool	is_must_set_time() { 
           bool tmp_must_set_time;
            {
              lock_mutex mutex_to_lock(&time_setting_flag_mutex);
              tmp_must_set_time=must_set_time;
             }
           return tmp_must_set_time;};

   	void set_time() {
                  lock_mutex mutex_to_lock(&time_setting_flag_mutex);
                  must_set_time=true;};
   	void time_already_setted() {
                  lock_mutex mutex_to_lock(&time_setting_flag_mutex);
                  must_set_time=false;};

/*
related devices ids
*/
    related_devices_ids::iterator devices_ids_begin(){return devices_ids.begin();};
    related_devices_ids::iterator devices_ids_end(){return devices_ids.end();};
    related_devices_ids::size_type devices_ids_size(){return devices_ids.size();}; 
    bool devices_ids_empty(){return devices_ids.empty();}; 

    void push_back_device_id(int device_id){devices_ids.push_back(device_id);};

/*
virtual metods
*/  	
virtual int get_type_description()=0;
virtual void decode_answer_from_device_to_data_block()=0;

virtual buffer_data_type get_request_for_send_to_device()=0;
virtual void send_command(command cmd)=0;
virtual void create_device_widget(int x,
                                                  int y,
                                                  ApDBase_t*  widgets_dbase,
                                                  PtWidget_t *station_widget,
                                                  vector<PtCallback_t> activate_callbacks,
                                                  vector<PtCallback_t> menu_callbacks) 
           throw (spider_exception)=0;

virtual void update_device_widget() throw (spider_exception)=0;
virtual  PhRect_t get_device_widget_extent() throw (spider_exception)=0;
virtual void update_device_panel(metro_device_type *dev_type) throw (spider_exception)=0;
virtual data_block* get_data_block()=0;

virtual command get_device_pref_command() throw (spider_exception)=0;

virtual vector<command> get_appropriated_commands() =0;


virtual void create_properties_widgets(PtWidget_t *parent_widget) =0;
/*
	static 
*/
	static int paneled_device_id;
	static PtWidget_t* device_panel;
    static void create_device_panel(ApDBase_t *widgets_db,
                                           string panel_title,
                                           metro_device_type *dev_type,
                                           data_block *data_blk,
                                           int device_id,
                                           int device_number,
                                           vector<PtCallback_t> callbacks) throw (spider_exception);

    static buffer_data_type
      generate_error_answer_for_function
                             ( int device_number,
                               int function_code,
                               int exception_code);


};

#endif
