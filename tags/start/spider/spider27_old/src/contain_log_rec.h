#ifndef __CONT_LOG_REC__
#define __CONT_LOG_REC__

/*****************************************************************************
*   	     Container class for log records - wrapper of STL container vector 			*
*****************************************************************************/

class log_record
{
private:
	long int id;
	int msg_id;
	int msg_type;
	int station_id;
	int device_id;
	time_t record_time;   
	
	log_record();
public:
	log_record(int new_id, 
                      int new_msg_id,
                      int new_msg_type,
                      int new_station_id,
                      int new_device_id,
                      time_t new_record_time);
	
/*
get_ and set_  metods for private data members
*/
	long int get_id () const {return (id);};
	void set_id ( long int new_id) {id=new_id;};
	
	int get_msg_id () const {return (msg_id);};
	void set_msg_id( int new_msg_id) { msg_id=new_msg_id;};

	int get_msg_type () const {return (msg_type);};
	void set_msg_type( int new_msg_type) { msg_type=new_msg_type;};
	
	int get_station_id () const {return (station_id);};
	void set_station_id( int new_station_id) { station_id=new_station_id;};
	
	int get_device_id () const {return (device_id);};
	void set_device_id( int new_device_id) { device_id=new_device_id;};

	time_t get_record_time () const {return (record_time);};
	void set_record_time( time_t new_record_time) { record_time=new_record_time;};

};


class log_filter : public unary_function<log_record, bool>
{
friend class log_records_container;
private:
	typedef  set<int> ids_container;	
	typedef  ids_container::iterator ids_container_iterator;	
	typedef  ids_container::const_iterator const_ids_container_iterator;	
	typedef  ids_container::size_type ids_container_size_type;	


	ids_container contain_id_msg_types;
	ids_container contain_id_stations;
	ids_container contain_id_devices;

	time_t	start_time;
	time_t	stop_time;	

	bool all_times, all_devices, all_stations, all_msg_types,  filter_state;
public:
/*
Constructor and destructor
*/

	log_filter();
	log_filter (const log_filter& filter_to_copy);

/*
get_ and set_ metods for private data members
*/	
	bool get_all_devices() {return(all_devices);};
	void set_all_devices(bool new_all_devices) {all_devices=new_all_devices;};
	void insert_device (int device_id) {contain_id_devices.insert(device_id);};
	void insert_devices_range (ids_container_iterator first_dev_iter, 
												ids_container_iterator last_dev_iter)
											 {contain_id_devices.insert (first_dev_iter,last_dev_iter);};
	void erase_device (int device_id) {contain_id_devices.erase(device_id);};
	void erase_devices_range (ids_container_iterator first_dev_iter, 
												ids_container_iterator last_dev_iter)
											 {contain_id_devices.erase (first_dev_iter, 
											 											last_dev_iter);};
	ids_container_iterator find_device(int dev_id) {return contain_id_devices.find(dev_id);};
	ids_container_iterator devices_end() {return contain_id_devices.end();};
	ids_container_iterator devices_begin() {return contain_id_devices.begin();};
	ids_container_size_type devices_size() {return contain_id_devices.size();};
	bool devices_empty() {return contain_id_devices.empty();};

	bool get_all_stations() {return(all_stations);};
	void set_all_stations(bool new_all_stations) {all_stations=new_all_stations;};
	void insert_station (int station_id) {contain_id_stations.insert(station_id);};
	void insert_stations_range (ids_container_iterator begin_stations_id_iter,
									ids_container_iterator end_stations_id_iter) 
									{contain_id_stations.insert(begin_stations_id_iter,
																				end_stations_id_iter);};

	void erase_station (int station_id) {contain_id_stations.erase(station_id);};
	void erase_stations_range (ids_container_iterator begin_stations_id_iter,
									ids_container_iterator end_stations_id_iter)
									{contain_id_stations.erase(begin_stations_id_iter,
																				end_stations_id_iter);};

	ids_container_iterator find_station(int station_id) {return contain_id_stations.find(station_id);};
	ids_container_iterator stations_end()  {return contain_id_stations.end();};
	ids_container_iterator stations_begin() {return contain_id_stations.begin();};
	ids_container_size_type stations_size() {return contain_id_stations.size();};
	bool stations_empty() {return contain_id_stations.empty();};

	bool get_all_msg_types() {return(all_msg_types);};
	void set_all_msg_types(bool new_all_msg_types) {all_msg_types=new_all_msg_types;};
	void insert_msg_type (int msg_type_id) {contain_id_msg_types.insert(msg_type_id);};
	
	void erase_msg_type (int msg_type_id) {contain_id_msg_types.erase(msg_type_id);};
	void erase_msg_types_range (ids_container_iterator begin_msg_type_id_iter,
									ids_container_iterator end_msg_type_id_iter)
					{contain_id_msg_types.erase(begin_msg_type_id_iter, end_msg_type_id_iter);};
					
	ids_container_iterator find_msg_type(int msg_type_id) {return contain_id_msg_types.find(msg_type_id);};
	ids_container_iterator msg_types_end() {return contain_id_msg_types.end();};
	ids_container_iterator msg_types_begin() {return contain_id_msg_types.begin();};
	ids_container_size_type msg_types_size() {return contain_id_msg_types.size();};
	bool msg_types_empty() {return contain_id_msg_types.empty();};

	bool get_all_times() {return(all_times);};
	void set_all_times(bool new_all_times) {all_times=new_all_times;};
	time_t get_start_time() {return(start_time);};
	void set_start_time(time_t  new_start_time) {start_time=new_start_time;};
	time_t get_stop_time() {return(stop_time);};
	void set_stop_time(time_t  new_stop_time) {stop_time=new_stop_time;};

	bool get_filter_state() {return (filter_state);}
	void set_filter_state(bool new_filter_state) {filter_state=new_filter_state;}

	void reset_to_default(){
			all_msg_types=all_devices=all_times=all_stations=true;
		}

/*
other
*/

log_filter& operator = (log_filter& filter_to_assigment);

bool operator == (const log_filter filter_to_compare) const;

bool operator () (const log_record log_rec) const;

};



class log_records_container {
private :
typedef	list<log_record> contain_log_rec;
typedef  contain_log_rec::iterator iterator_log_rec;
typedef  contain_log_rec::size_type size_type_log_rec;

contain_log_rec container_log_rec;
PtWidget_t	 *wnd, *filtration_state_indicator;
tm last_saved_time;

long int records_count; //needed for automaic id generation

iterator_log_rec iter_end;

/*
Class functor for sorting by log_record.get_save_time  with Strict Weak Ordering
*/
class stw_sorter: 
 public binary_function<const  log_record,
 									const  log_record,
 									bool> {
public:
bool operator () (log_record first_log_rec, log_record second_log_rec) const {
	return (first_log_rec.get_record_time()<second_log_rec.get_record_time() && first_log_rec.get_id()<second_log_rec.get_id());
	}
};

stw_sorter log_rec_sorter;

void sort_all ();

public :
/*
data members
*/
log_filter filter;

/*
Constructors & destructors
*/
log_records_container();

/*
get_ and set_  metods for private data members
*/
int get_records_autoincrement() {return (records_count);}
void set_records_autoincrement(int new_records_counter) {records_count=new_records_counter;}

tm get_last_saved_time() {return (last_saved_time);}
void set_last_saved_time(tm new_last_saved_time) {last_saved_time=new_last_saved_time;}

PtWidget_t* get_widget() {return (wnd);}
void set_widget(PtWidget_t *new_wnd);

PtWidget_t* get_filtration_state_indicator() {return (filtration_state_indicator);}
void set_filtration_state_indicator(PtWidget_t *new_filtration_state_indicator);

/*
Interface for useful metods to STL container vector
*/
iterator_log_rec begin() {return (container_log_rec.begin());};
iterator_log_rec end() {return (iter_end);};

void insert (log_record rec);
void erase(iterator_log_rec iter);

void erase_all();

bool empty() const {return (container_log_rec.empty());}
size_type_log_rec size()  {return distance(container_log_rec.begin(), iter_end);};

/*
other metods
*/
void prepare_to_display();

void log_records_container::load (
                 string file_name,
                 system_settings *sys_sett,
                 metro_devices_container *devs_cont,
                 metro_devices_types_container *metro_devices_types,
                 msg_dict_container *msg_dict) throw (spider_exception);

void save (string file_name) throw (spider_exception);

void set_filtering();

bool rotate() throw(spider_exception);
};

#endif