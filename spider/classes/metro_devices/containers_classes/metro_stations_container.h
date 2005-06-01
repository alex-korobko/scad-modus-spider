#ifndef __METRO_STATIONS_CONTAINER__
#define __METRO_STATIONS_CONTAINER_
class metro_stations_container  {
private:
typedef hash_map<int , metro_station > container_metro_stations;
typedef container_metro_stations::iterator iterator;
typedef container_metro_stations::value_type value_type;
typedef container_metro_stations::size_type size_type;

typedef vector<int> devices_id_container;

container_metro_stations cont_stations;
devices_id_container devices_ids;
iterator current_station_iter;

metro_stations_container(){};

//static members
static metro_stations_container* metro_stations_container_instance;
static PtWidget_t *metro_station_devices_properties_panel;
static PtWidget_t *metro_station_devices_properties_raw_list;
public:

void set_current_station(iterator new_current_station_iter) {
  current_station_iter=new_current_station_iter;
 };

iterator get_current_station() {
return current_station_iter;
};

iterator upper_bound (const int key) { return cont_stations.upper_bound(key);};

iterator insert (iterator station_iterator, value_type station_value)
   { return cont_stations.insert (station_iterator, station_value);  };
void erase (iterator begin_iter, iterator end_iter) {cont_stations.erase(begin_iter, end_iter);};

iterator find (const int key)  {return cont_stations.find(key);} 
iterator begin ()  {return cont_stations.begin();} 
iterator end ()  {return cont_stations.end();} 

void push_back_devices_id(int device_id) {devices_ids.push_back(device_id);};

void load_station_parameters () throw (spider_exception);
void load ( string file_name) throw (spider_exception);

//static metods
static metro_stations_container* get_instance();

static  PtWidget_t* get_metro_station_devices_properties_panel() {return metro_station_devices_properties_panel;}; 
static  void set_metro_station_devices_properties_panel(PtWidget_t* new_devices_properties_panel) {metro_station_devices_properties_panel=new_devices_properties_panel;};


static  PtWidget_t* get_metro_station_devices_properties_raw_list() {return metro_station_devices_properties_raw_list;}; 
static  void set_metro_station_devices_properties_raw_list(PtWidget_t* new_metro_station_devices_properties_raw_list)
                          {metro_station_devices_properties_raw_list=new_metro_station_devices_properties_raw_list;};

};
#endif
