#ifndef __CONT_LOG_REC_H__
#define __CONT_LOG_REC_H__

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
	log_record(int new_id, int new_msg_id, int new_msg_type, int new_station_id, int new_device_id, time_t new_record_time):
		id(new_id),  msg_id(new_msg_id), msg_type(new_msg_type), 
		station_id(new_station_id), device_id(new_device_id), record_time(new_record_time)
		{
		}
	
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
public:
	typedef  set<int> ids_container;	
	typedef  ids_container::iterator ids_container_iterator;	
	typedef  ids_container::size_type ids_container_size_type;	

private:
	ids_container contain_id_msg_types;
	ids_container contain_id_stations;
	ids_container contain_id_devices;

	time_t	start_time;
	time_t	stop_time;	

	bool all_times, all_devices, all_stations, all_msg_types,  filter_state;
	void set_filter_state (bool new_filter_state) {filter_state=new_filter_state;}

public:
/*
Constructor and destructor
*/

	log_filter() :
		start_time(0), stop_time(0),
		all_times(true), 
		all_devices(true), 
		all_stations(true),
		all_msg_types(true),
		filter_state(false)
	{
	};
	
/*
get_ and set_ metods for private data members
*/	
	bool is_filter_on() {return (filter_state);}

	void reset_to_default()
		{
			all_msg_types=all_devices=all_times=all_stations=true;
		}

	bool get_all_devices() {return(all_devices);};
	void set_all_devices(bool new_all_devices) {all_devices=new_all_devices;};
	void insert_device (int device_id) {contain_id_devices.insert(device_id);};
	void insert_devices_range (ids_container_iterator first_dev_iter, 
												ids_container_iterator last_dev_iter)
											 {contain_id_devices.insert (first_dev_iter,last_dev_iter);};
	void erase_device (int device_id) {contain_id_devices.erase(device_id);};
	void erase_devices_range (ids_container_iterator first_dev_iter, 
												ids_container_iterator last_dev_iter)
											 {contain_id_devices.erase (first_dev_iter,last_dev_iter);};
	ids_container_iterator find_device(int dev_id) {return contain_id_devices.find(dev_id);};
	ids_container_iterator devices_end() {return contain_id_devices.end();};
	ids_container_iterator devices_begin() {return contain_id_devices.begin();};
	ids_container_size_type devices_size() {return contain_id_devices.size();};
	bool devices_empty() {return contain_id_devices.empty();};
	void devices_advance(ids_container_iterator& cursor, ids_container_size_type distance)
												{advance(cursor, distance);};
	ids_container_size_type devices_distance(ids_container_iterator first_dev_iter, 
												ids_container_iterator last_dev_iter)
												{return distance(first_dev_iter, last_dev_iter);};

	bool get_all_stations() {return(all_stations);};
	void set_all_stations(bool new_all_stations) {all_stations=new_all_stations;};
	void insert_station (int station_id) {contain_id_stations.insert(station_id);};
	void erase_station (int station_id) {contain_id_stations.erase(station_id);};
	ids_container_iterator find_station(int station_id) {return contain_id_stations.find(station_id);};
	ids_container_iterator stations_end()  {return contain_id_stations.end();};
	ids_container_iterator stations_begin() {return contain_id_stations.begin();};
	ids_container_size_type stations_size() {return contain_id_stations.size();};
	bool stations_empty() {return contain_id_stations.empty();};
	void stations_advance(ids_container_iterator& cursor, ids_container_size_type distance)
												{return advance(cursor, distance);};
	ids_container_size_type stations_distance(ids_container_iterator first_station_iter, 
												ids_container_iterator last_station_iter)
												{return distance(first_station_iter, last_station_iter);};

	bool get_all_msg_types() {return(all_msg_types);};
	void set_all_msg_types(bool new_all_msg_types) {all_msg_types=new_all_msg_types;};
	void insert_msg_type (int msg_type_id) {contain_id_msg_types.insert(msg_type_id);};
	void erase_msg_types (int msg_type_id) {contain_id_msg_types.erase(msg_type_id);};
	ids_container_iterator find_msg_type(int msg_type_id) {return contain_id_msg_types.find(msg_type_id);};
	ids_container_iterator msg_types_end() {return contain_id_msg_types.end();};
	ids_container_iterator msg_types_begin() {return contain_id_msg_types.begin();};
	ids_container_size_type msg_types_size() {return contain_id_msg_types.size();};
	bool msg_types_empty() {return contain_id_msg_types.empty();};
	void msg_types_advance(ids_container_iterator& cursor, ids_container_size_type distance)
												{return advance(cursor, distance);};
	ids_container_size_type msg_types_distance(ids_container_iterator first_message_iter, 
												ids_container_iterator last_message_iter)
												{return distance(first_message_iter, last_message_iter);};

	bool get_all_times() {return(all_times);};
	void set_all_times(bool new_all_times) {all_times=new_all_times;};
	time_t get_start_time() {return(start_time);};
	void set_start_time(time_t  new_start_time) {start_time=new_start_time;};
	time_t get_stop_time() {return(stop_time);};
	void set_stop_time(time_t  new_stop_time) {stop_time=new_stop_time;};

/*
other
*/

bool operator () (const log_record log_rec) const
	{
	if (! filter_state) {return (true);}
	if (! all_times)
			if (start_time<=log_rec.get_record_time() || stop_time>=log_rec.get_record_time()) return (false);

	if (! all_stations)
			{
			if (contain_id_stations.end()==contain_id_stations.find(log_rec.get_station_id())) {return (false);};

			if (! all_devices)
				{		
				if (contain_id_devices.end()==contain_id_devices.find(log_rec.get_device_id())) {return (false);};
				};

			};
			
			
	if (! all_msg_types)
			{
				if (contain_id_msg_types.end()==contain_id_msg_types.find(log_rec.get_msg_type())) {return (false);};
			};
			
	return(true);
	};

};



class log_records_container {
private :
typedef	list<log_record> contain_log_rec;

public :
typedef  contain_log_rec::iterator iterator_log_rec;

private :
contain_log_rec container_log_rec;
PtWidget_t	 *wnd, *filtration_state_indicator;
tm last_saved_time;

long int records_count; //needed for automaic id generation

iterator_log_rec iter_end;

/*
Class functor for sorting by log_record.get_save_time  with Strict Weak Ordering
*/
class stw_sorter : 
 public binary_function<const  log_record,
 									const  log_record,
 									bool>
{
public:
bool operator () (log_record first_log_rec, log_record second_log_rec) const 
	{
	return (first_log_rec.get_record_time()<second_log_rec.get_record_time());
	}
	
};   

stw_sorter log_rec_sorter;

public :
/*
data members
*/
log_filter filter;

/*
Constructors & destructors
*/
log_records_container(): records_count(0)
	{
		last_saved_time.tm_sec=0;
		last_saved_time.tm_min=0;
		last_saved_time.tm_hour=0;
		last_saved_time.tm_mday=0;
		last_saved_time.tm_mon=0;
		last_saved_time.tm_year=0;
		last_saved_time.tm_wday=0;
		last_saved_time.tm_yday=0;
		last_saved_time.tm_isdst=0;
		
		iter_end=container_log_rec.end();
		wnd=filtration_state_indicator=NULL;
	};


~log_records_container()
{
	PtArg_t arg;

	if (wnd!=NULL) 
		{
				PtSetArg(&arg, 
							Pt_ARG_POINTER , 
							NULL, 
							0);

			 PtSetResources( wnd,
					                    1,
                    						&arg);
		}
}

/*
get_ and set_  metods for private data members
*/
int get_records_autoincrement() {return (records_count);}
void set_records_autoincrement(int new_records_counter) {records_count=new_records_counter;}

tm get_last_saved_time() {return (last_saved_time);}
void set_last_saved_time(tm new_last_saved_time) {last_saved_time=new_last_saved_time;}

PtWidget_t* get_widget() {return (wnd);}
void set_widget(PtWidget_t *new_wnd) 
	{
	if (wnd!=NULL) 
		{
			PtSetResource(wnd, Pt_ARG_POINTER, NULL, 0);				
		}
	
	if (new_wnd!=NULL) 
		{
			PtSetResource(new_wnd, Pt_ARG_POINTER, this, 0);				
		};		
	
	wnd=new_wnd;
	};

PtWidget_t* get_filtration_state_indicator() {return (filtration_state_indicator);}
void set_filtration_state_indicator(PtWidget_t *new_filtration_state_indicator) 
	{
	if (filtration_state_indicator!=NULL) 
		{
			PtSetResource(filtration_state_indicator, Pt_ARG_POINTER, NULL, 0);				
		}
	
	if (new_filtration_state_indicator!=NULL) 
		{
			PtSetResource(new_filtration_state_indicator, Pt_ARG_POINTER, this, 0);				
		};		

	filtration_state_indicator=new_filtration_state_indicator;
	};


/*
Interface for useful metods to STL container vector
*/
iterator_log_rec begin() {return (container_log_rec.begin());};
iterator_log_rec end() {return (iter_end);};

/*
Sort all interval by log_rec_sorter()
unset filter
and set  iter_end to container_log_rec.end()
*/
void sort_all ()
	{
	container_log_rec.sort (log_rec_sorter);

	iter_end=container_log_rec.end();
	}

/*
Insert 
new log_record rec into container
if  rec in filter condition - it was inserted into it`s position in container
otherwise it pushed to back
*/

void insert (log_record rec) 
	{ 
	if (! container_log_rec.empty() && this->filter.is_filter_on() && this->filter(rec))
		{
			container_log_rec.insert(upper_bound(container_log_rec.begin(),
																		iter_end,
																		rec,
																		log_rec_sorter),
												rec);
		}  else {
			container_log_rec.push_back(rec);
		};
		
	}

void erase(iterator_log_rec iter) {container_log_rec.erase(iter);};
void erase(iterator_log_rec iter_beg, iterator_log_rec iter_end) {container_log_rec.erase(iter_beg, iter_end);};
void erase_all() 
	{
		container_log_rec.erase(container_log_rec.begin(), container_log_rec.end());
		contain_log_rec(container_log_rec).swap(container_log_rec);		
		iter_end=container_log_rec.end();		
	};

bool empty() const {return (container_log_rec.empty());}
int size() const{return (container_log_rec.size());};

/*
other metods
*/
bool load (string file_name)
{
	FILE		*logFile;
	vector<char> signature(6);
	string logdat("LOGDAT"), spdlog("SPDLOG"); //LOGDAT is signature of old format log files , SPDLOG is signature of new format log files
	bool new_format_file=false;
	dword	size=0;
	struct stat file_stat;

	long int  tmp_autoincrement;
	int mess_id, station_id, device_id;
	time_t save_time;

	set_filtering(false);
	erase_all();

	if ((logFile = fopen(file_name.c_str(), "r")) ==NULL)
	{
		if ((logFile = fopen(file_name.c_str(), "w")) !=NULL)
		{
		g_system_settings.message_window(system_settings::INFO_MSG, string("Создан новый файл журнала"));

		fwrite(logdat.c_str(),
				  sizeof(string::value_type),	
				  logdat.size(),
				  logFile
				);

		fwrite(&size,
				  sizeof(dword),	
				  1,
				  logFile
				);
			
		fclose(logFile);
		return true;
		} else {
			string mess="Can`t open or create file ";
			mess += file_name;
			mess+=" ";
			mess+=strerror( errno);
		
			g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
			return false;
		};
	};

	if (fread(&signature[0], sizeof(string::value_type) ,  signature.size(), logFile)< signature.size())
	{
	
		g_system_settings.message_window(system_settings::ERROR_MSG, string("Файл журнала поврежден	"));

		fclose (logFile);
		return false;
	};

	if (logdat.compare(0, 
								 logdat.size(),
								 &signature[0],
								 logdat.size()
								 )!=0)
		{
		
		 if (spdlog.compare(0, 
									 spdlog.size(),
									 &signature[0],
									 spdlog.size()
									 )==0) 
			{
				new_format_file=true;
 			} else {
			g_system_settings.message_window(system_settings::ERROR_MSG, string("Файл журнала поврежден - формат файла не установлен"));
			fclose (logFile);
			return false;
			};
		}
		
	if (fread(&size, sizeof(size), 1, logFile)==0 && size<0)
	{	
		g_system_settings.message_window(system_settings::ERROR_MSG, string("Файл журнала поврежден	"));
		
		fclose (logFile);
		return false;
	};
	
	while (size-->0)	
	{
		if (new_format_file) 
			{
			if (fread(&tmp_autoincrement, sizeof(tmp_autoincrement), 1, logFile) <=0)	
				{
					g_system_settings.message_window(system_settings::ERROR_MSG, string("Файл жунала поврежден - ошибка чтения"));

					string mess="Can`t read from file ";
					mess += file_name;
					mess+=" ";
					mess+=strerror( errno);
		
					g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
					fclose (logFile);
					return false;
				};

			} else {
			tmp_autoincrement=records_count+1;
			};
			
		if (fread(&mess_id, sizeof(mess_id), 1, logFile)<=0)
				{
					g_system_settings.message_window(system_settings::ERROR_MSG, string("Файл журнала поврежден - ошибка чтения"));

					string mess="Can`t read from file ";
					mess += file_name;
					mess+=" ";
					mess+=strerror( errno);
		
					g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
					fclose (logFile);
					return false;
				};


		if (fread(&station_id, sizeof(station_id), 1, logFile)<=0)
				{
					g_system_settings.message_window(system_settings::ERROR_MSG, string("Файл журнала поврежден - ошибка чтения"));

					string mess="Can`t read from file ";
					mess += file_name;
					mess+=" ";
					mess+=strerror( errno);
		
					g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
					fclose (logFile);
					return false;
				};


		if (fread(&device_id, sizeof(device_id), 1, logFile)<=0)
				{
					g_system_settings.message_window(system_settings::ERROR_MSG, string("Файл журнала поврежден - ошибка чтения"));

					string mess="Can`t read from file ";
					mess += file_name;
					mess+=" ";
					mess+=strerror( errno);
		
					g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
					fclose (logFile);
					return false;
				};

		if (fread(&save_time, sizeof(save_time), 1, logFile)<=0)
				{
					g_system_settings.message_window(system_settings::ERROR_MSG, string("Файл журнала поврежден - ошибка чтения"));
	
					string mess="Can`t read from file ";
					mess += file_name;
					mess+=" ";
					mess+=strerror( errno);
		
					g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
					fclose (logFile);
					return false;
				};

		msg_dict_container::msg_dict_iterator tmp_msg_iter=g_msgDictionary.find(mess_id);
		if(tmp_msg_iter==g_msgDictionary.end())
			{
				vector<char> tmp_chars(10);
				string mess="Error in  file ";
				mess += file_name;
				mess += "  not found message id ";
				itoa (mess_id, &tmp_chars[0], 10);
				mess +=&tmp_chars[0];

				g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
				fclose (logFile);
				return false;
			};


		int msg_type=tmp_msg_iter->second.get_type();

		set_records_autoincrement(tmp_autoincrement);		
		insert(log_record (
									tmp_autoincrement,
									mess_id,
									msg_type,
									station_id,
									device_id,
									save_time
									)
				);		
	
	};


	if(stat(file_name.c_str(), &file_stat) != -1)
		{
 	     	localtime_r(&file_stat.st_mtime, &last_saved_time);
      	} 	else 	{
			string mess="Can`t get file access time ";
			mess += file_name;
			mess+=" ";
			mess+=strerror( errno);

			g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
			fclose (logFile);
	     };
	
	fclose (logFile);
	return true;
};



bool save (string file_name)
{
	FILE		*logFile;
	string spdlog("SPDLOG"); //LOGDAT is signature of old format log files , SPDLOG is signature of new format log files
	dword	size=container_log_rec.size();
	iterator_log_rec iter_log=container_log_rec.begin();

	if ((logFile = fopen(file_name.c_str(), "w")) ==NULL)
	{
			string mess="Can`t open file for writing ";
			mess += file_name;
			mess+=" ";
			mess+=strerror( errno);

			g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
			return false;
	};
	
	if (fwrite(spdlog.c_str(), sizeof(string::value_type), spdlog.size(), logFile) <=0)	
	{
		string mess="Can`t write to file ";
		mess += file_name;
		mess+=" ";
		mess+=strerror( errno);
		
		g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
		fclose (logFile);
		return false;
	};
	
	if (fwrite(&size, sizeof(size), 1, logFile) <=0)	
	{
		string mess="Can`t write to file ";
		mess += file_name;
		mess+=" ";
		mess+=strerror( errno);
		
		g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
		fclose (logFile);
		return false;
	};
	
	while (iter_log!=container_log_rec.end())
		{
		long tmp_long=iter_log->get_id();
		if (fwrite(&tmp_long, sizeof(tmp_long), 1, logFile) <=0)	
				{
					string mess="Can`t write to file ";
					mess += file_name;
					mess+=" ";
					mess+=strerror( errno);
		
					g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
					fclose (logFile);
					return false;
				};

		int tmp_int=iter_log->get_msg_id();
		if (fwrite(&tmp_int, sizeof(tmp_int), 1, logFile) <=0)	
				{

					string mess="Can`t write to file ";
					mess += file_name;
					mess+=" ";
					mess+=strerror( errno);
		
					g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
					fclose (logFile);
					return false;
				};

		tmp_int=iter_log->get_station_id();
		if (fwrite(&tmp_int, sizeof(tmp_int), 1, logFile) <=0)	
				{
					string mess="Can`t write to file ";
					mess += file_name;
					mess+=" ";
					mess+=strerror( errno);
		
					g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
					fclose (logFile);
					return false;
				};

		tmp_int=iter_log->get_device_id();
		if (fwrite(&tmp_int, sizeof(tmp_int), 1, logFile) <=0)	
				{
					string mess="Can`t write to file ";
					mess += file_name;
					mess+=" ";
					mess+=strerror( errno);
		
					g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
					fclose (logFile);
					return false;
				};

		time_t tmp_time=iter_log->get_record_time();
		if (fwrite(&tmp_time, sizeof(tmp_time), 1, logFile) <=0)	
				{

					string mess="Can`t write to file ";
					mess += file_name;
					mess+=" ";
					mess+=strerror( errno);
		
					g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
					fclose (logFile);
					return false;
				};

		iter_log++;
		};
	fclose (logFile);

return true;
};

void set_filtering( bool new_filtering_state)
{
sort_all();
filter.set_filter_state(new_filtering_state);
if (new_filtering_state) 
	{
	iter_end=stable_partition(container_log_rec.begin(),
											container_log_rec.end(),
											filter
											);
	};

};

bool rotate()
{
	vector<char> tmp_chars(10);
	time_t			curTime;
	struct	tm		localTime;
	string			file_name;

	curTime=time(NULL);
	localtime_r(&curTime, &localTime);
	if (localTime.tm_mon != last_saved_time.tm_mon)
	{

		itoa(last_saved_time.tm_mon, &tmp_chars[0], 10);
		file_name=&tmp_chars[0];
		file_name+="_";		
		itoa((last_saved_time.tm_year + 1900), &tmp_chars[0], 10);
		file_name+=&tmp_chars[0];
		file_name+=".log";		

		if (save(file_name))
			{
				erase_all();
				set_filtering(false);
			} else {
				return false;
			};
		memcpy(&last_saved_time, &localTime, sizeof(last_saved_time));
	}

	return true;
}; 

};

#endif