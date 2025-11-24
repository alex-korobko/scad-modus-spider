#ifndef __METRO_DEVICES_CONTAINER__
#define __METRO_DEVICES_CONTAINER__

/*****************************************************************************
Container class for pointers of metro devices -
wrapper of STL container  map
*****************************************************************************/

class metro_devices_container {
public:
  enum  device_type {ESCALATOR=0, SHAVR, UDKU, DOOR};

  typedef  hash_map <int, metro_device*> metro_devices_map;
  typedef metro_devices_map::iterator  iterator;
  typedef metro_devices_map::value_type value_type;
  typedef metro_devices_map::size_type size_type;

  iterator current_device;
  iterator current_device_in_directions;
private :
  metro_devices_map	container_metro_devices;

//functor class for generating  commands from
//metro_devices_container
	class make_timer_command{
	private:
		int day_now;
		make_timer_command() {};		
		static make_timer_command* make_timer_command_instance;
	public:
     void operator() (value_type device_container_value) throw (spider_exception);
     void initialize_day();
     static make_timer_command* get_instance();
	};

//functor class for searching in contain_timer_command
//devices, which be started in current hour and minutes and start it.
	class find_timer_command{
		private:
  		int timer_hour, timer_minute, now_execution_mode;
  		find_timer_command(){};
  		static find_timer_command* find_timer_command_instance;
		public:
   		void 	initialize_time_to_find (
                        int new_timer_hour,
                        int new_start_minute);
   		void operator() (timer_command& timer_cmd);
   		static find_timer_command* get_instance();
	};

//functor class for setting time in all devices of 
//metro_devices_container
	class make_set_time{
     public:
     make_set_time() {};	
     void operator() (value_type device_container_value);
	};


	metro_devices_container ():
		current_device(container_metro_devices.end()),
		current_device_in_directions(container_metro_devices.end()){};

static metro_devices_container* metro_devices_container_instance;
public :

//get_ and set_  metods for private data members
iterator get_current_device() {return (current_device);};
void set_current_device(iterator new_current_device) {current_device=new_current_device;};

iterator get_current_device_in_directions() {return (current_device_in_directions);};
void set_current_device_in_directions(iterator new_current_device_in_directions) {current_device_in_directions=new_current_device_in_directions;};

//wrappers of current STL device`s container metods
iterator begin() {return (container_metro_devices.begin());};
iterator end(){return (container_metro_devices.end());};

iterator find(const int key) { return (container_metro_devices.find(key)); };
iterator upper_bound(const int key) { return container_metro_devices.upper_bound(key); };

void erase (iterator iter) { container_metro_devices.erase(iter); };
void erase (iterator iter_beg,
					iterator iter_end) { container_metro_devices.erase(iter_beg, iter_end); };
void clear() {container_metro_devices.clear();};
iterator insert (iterator iter, const value_type& obj){
  return (container_metro_devices.insert(iter, obj));
}

bool empty() const { return (container_metro_devices.empty());}
size_type size() const {return (container_metro_devices.size());};

/*
Other functions
*/
void save_door_parameters () throw (spider_exception);
void save_shavr_parameters () throw (spider_exception);
void save_escalator_parameters () throw (spider_exception);
void save_udku_parameters () throw (spider_exception);
void save (string file_name) throw (spider_exception);

void  load_door_parameters (
                                    int channel) // channel where devices must be attached
                                    throw (spider_exception);

void  load_shavr_parameters (
                                    int channel) // channel where devices must be attached
                                    throw (spider_exception);
void  load_escalator_parameters (
                                    int channel ) // channel where devices must be attached
                                    throw (spider_exception);

void  load_udku_parameters (
                                    int channel ) // channel where devices must be attached
                                    throw (spider_exception);

void load (
    string file_name,
    int channel) // channel where devices must be attached
      throw (spider_exception);

/*
preparing timer_commands from metro_devices_container
*/
void prepare_timer_command() throw (spider_exception);

/*
executing  timer_commands by metro_devices_container
*/
//static functions
static metro_devices_container* get_instance();
static void execute_timer_commands(
                            int start_hour,
                            int start_minute);

static void set_time();

}; // class metro_devices_container
#endif
