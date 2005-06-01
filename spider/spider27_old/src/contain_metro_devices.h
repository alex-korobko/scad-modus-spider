#ifndef __CONT_METRO_DEVICES__
#define  __CONT_METRO_DEVICES__

/*****************************************************************************
Container class for pointers of metro devices - 
wrapper of STL container  map
*****************************************************************************/

class metro_devices_container {
private :

struct ltint {
		  bool operator() (const int i1, const int i2) const{
   			 return (i1<i2) ;
  			}
	};

typedef  map <int, metro_device*, ltint> metro_devices_map;	

metro_devices_map	container_metro_devices;

typedef metro_devices_map::iterator  iterator_metro_devices;	
typedef metro_devices_map::value_type pair_metro_devices;	
typedef metro_devices_map::size_type size_type_metro_devices;	

iterator_metro_devices current_device;
iterator_metro_devices current_device_in_directions;

/*
functor class for creating contain_morning_start from 
metro_devices_container
*/	
	class make_timer_command{
	private:
		contain_timer_command *cont_timer_command;
		metro_devices_container *devices;
		int day_now;

		make_timer_command();
	public:
	make_timer_command(
                           contain_timer_command *new_cont_timer_command,
                           metro_devices_container *new_devices);

     void operator() (int device_id) throw (spider_exception);

	};
/*
functor class for searching in contain_timer_command
devices, wich be started in current hour and minutes and start it.
*/	

	class find_timer_command{
		private:
		metro_devices_container *dev_container;
		int timer_hour, timer_minute;
		
		find_timer_command();
		public:

		find_timer_command (metro_devices_container *new_dev_container,
									 int new_timer_hour,
									 int new_start_minute);

		void operator() (timer_command& timer_cmd);
	};

public :
	metro_devices_container (): 
		current_device(container_metro_devices.end()), 
		current_device_in_directions(container_metro_devices.end()){};
	
/*
get_ and set_  metods for private data members
*/

iterator_metro_devices get_current_device() {return (current_device);};
void set_current_device(iterator_metro_devices new_current_device) {current_device=new_current_device;};

iterator_metro_devices get_current_device_in_directions() {return (current_device_in_directions);};
void set_current_device_in_directions(iterator_metro_devices new_current_device_in_directions) {current_device_in_directions=new_current_device_in_directions;};

/*
wrappers of current STL device`s container metods
*/

iterator_metro_devices begin() {return (container_metro_devices.begin());};
iterator_metro_devices end(){return (container_metro_devices.end());};

iterator_metro_devices find(const int key) { return (container_metro_devices.find(key)); };
iterator_metro_devices upper_bound(const int key) { return container_metro_devices.upper_bound(key); };

void erase (iterator_metro_devices iter) { container_metro_devices.erase(iter); };
void erase (iterator_metro_devices iter_beg,
					iterator_metro_devices iter_end) { container_metro_devices.erase(iter_beg, iter_end); };
void clear() {container_metro_devices.clear();};
iterator_metro_devices insert (iterator_metro_devices iter, const pair_metro_devices& obj){
  return (container_metro_devices.insert(iter, obj)); 
}

bool empty() const { return (container_metro_devices.empty());}
size_type_metro_devices size() const {return (container_metro_devices.size());};

/*
Other functions
*/
void save_device_parameters (system_settings *sys_sett_obj) throw (spider_exception);

void save (string file_name,
				system_settings *sys_sett_obj) throw (spider_exception);

void  load_device_parameters (metro_stations_container *metro_stat_obj,
                                    metro_devices_types_container *metro_dev_types,
                                    system_settings *sys_sett_obj)  
                                    throw (spider_exception);
void load (
    string file_name,
    metro_stations_container *metro_stations_cont,
    metro_devices_types_container *metro_dev_types,
    system_settings *sys_sett_obj) throw (spider_exception);

/*
preparing contain_morning_start from metro_devices_container
*/
void prepare_timer_command(contain_timer_command *cont_timer_cmd, 
											metro_station::devices_id_container *devices_id_for_morning_start)
											 throw (spider_exception);

/*
executing  contain_morning_start by metro_devices_container
*/
void execute_morning_start(contain_timer_command *cont_timer_command,
											int timer_hour,
											int timer_minute);

}; // class metro_devices_container
#endif