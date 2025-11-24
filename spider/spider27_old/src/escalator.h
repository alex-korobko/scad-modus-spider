#ifndef _ESCALATOR_
#define _ESCALATOR_

class metro_escalator : public metro_device {
private: 				
   	PtWidget_t *arrow, 
   					*num_label, 
   					*block_label;
	system_settings *sys_sett_obj;   	
   	pthread_t		tid;
   	int				direction;

   	int				start_day_mode; //see system.h for enum START_DAY_MODE_ 
   	int				start_hour;
   	int				start_minute;

   	int				pref_direction;

    ::data_block data_block;
    
public:	

/*
metro escalator`s metods
*/

/*
constructors and destructors
*/	
metro_escalator(
		system_settings *sys_sett_object,
        int id, 
		int id_station,
		int type,
		int number,
		int pref_direction,
		int direction,
	   	int	 start_day_mode,
	   	int	 start_hour,
	   	int	 start_minute,
		bool enabled,
		in_addr_t	new_ip ) throw (spider_exception);

virtual ~metro_escalator();

/*
get_ and set_  metods for private data members
*/
   PtWidget_t*  get_block_label_widget() {return (block_label);};
   PtWidget_t*  get_arrow_label_widget() {return (arrow);};

   	int get_pref_direction() { return pref_direction; };   	
   	void set_pref_direction(int new_dir) { pref_direction=new_dir; };   	
	
   	int get_direction() { return direction; };   	
   	void set_direction(int new_dir) { direction=new_dir; };   	

	int get_start_day_mode() {return start_day_mode;};
	void set_start_day_mode(int new_start_day_mode) {start_day_mode=new_start_day_mode;};
	
	int get_start_hour() {return start_hour;};
	void set_start_hour(int new_start_hour) {start_hour=new_start_hour;};

	int get_start_minute(){ return start_minute;}
	void set_start_minute(int new_start_minute) {start_minute=new_start_minute;};

	int get_mode() {return (data_block.mode);}
	int get_status() {return (data_block.status);}
	int get_ready() {return (data_block.ready);}
	int get_blocked_status() {return(data_block.block_circuts_status);};
	int get_breaking_status() {return(data_block.breaking_path_status);};
	
	dword get_breaking_path_value() {return(data_block.breaking_path_value);};
	dword get_running_path_value() {return(data_block.running_path_value);};

/*
other methods
*/   	
void update_leds(
       metro_devices_types_container *dev_types)  throw (spider_exception);
void update_escalator();
void update_panel();

int create_station_window(int x, int y);

void set_state(
		dword state,
		alerts_container *alerts_cont);

void set_data(
	msg_dict_container *msg_dict,
	log_records_container *log_records,
	metro_devices_types_container *dev_types) throw (spider_exception);
	
void get_data();
   	
void start() throw (spider_exception);
void send_command(byte cmd);   
void send_time();   
void check_status();
int confirm_direction(int new_direction);

};

#endif
