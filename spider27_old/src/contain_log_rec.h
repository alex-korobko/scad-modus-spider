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
	int station_id;
	int device_id;
	time_t record_time;   
	
	log_record();
public:
	log_record(int new_id, int new_msg_id, int new_station_id, int new_device_id, time_t new_record_time):
		id(new_id),  msg_id(new_msg_id), station_id(new_station_id), device_id(new_device_id), record_time(new_record_time)
		{
		}
	
/*
get_ and set_  metods for private data members
*/

	long int get_id () {return (id);};
	void set_id ( long int new_id) {id=new_id;};
	
	int get_msg_id () {return (msg_id);};
	void set_msg_id( int new_msg_id) { msg_id=new_msg_id;};
	
	int get_station_id () {return (station_id);};
	void set_station_id( int new_station_id) { station_id=new_station_id;};
	
	int get_device_id () {return (device_id);};
	void set_device_id( int new_device_id) { device_id=new_device_id;};

	time_t get_record_time () {return (record_time);};
	void set_record_time( time_t new_record_time) { record_time=new_record_time;};
};


class log_filter : public unary_function<log_record, bool>
{
private:
	int		first_escalator_num;
	int		last_escalator_num;
	time_t	start_time;
	time_t	stop_time;	
	int		station_id;
	int messages;

	int	 all_times;

	bool filter_state;
public:
/*
enumerate message types
*/

	enum { MSG_SYSTEM=1,
				MSG_ALERT=2,
				MSG_BLOCK=4,
				MSG_LOCAL=8,
				MSG_NONE=0,
			};
/*
Constructor and destructor
*/

	log_filter() :
		first_escalator_num(0), last_escalator_num(0),
		start_time(0), stop_time(0),
		station_id(0),
		messages(~MSG_NONE),
		all_times(0),
		filter_state(false)
	{
	};
	
/*
get_ and set_ metods for private data members
*/	
	void set_filter_on() {filter_state=true;}
	void set_filter_off() {filter_state=false;}
	bool is_filter_on() {return (filter_state);}

	int get_station_id() {return(station_id);};
	void set_station_id (int new_station_id) {station_id=new_station_id;};

	int get_first_escalator_num() {return(first_escalator_num);};
	void set_first_escalator_num( int new_first_escalator_num) {first_escalator_num=new_first_escalator_num;};

	int get_last_escalator_num() {return(last_escalator_num);};
	void set_last_escalator_num( int new_last_escalator_num) {last_escalator_num=new_last_escalator_num;};

	time_t get_start_time() {return(start_time);};
	void set_start_time(time_t  new_start_time) {start_time=new_start_time;};

	time_t get_all_times() {return(all_times);};
	void set_all_times(time_t  new_all_times) {all_times=new_all_times;};

	time_t get_stop_time() {return(stop_time);};
	void set_stop_time(time_t  new_stop_time) {stop_time=new_stop_time;};

	int get_messages() {return(messages);};
	void set_messages( int new_messages) {messages=new_messages;};
/*
other
*/

bool operator () (log_record log_rec) const
	{
	if (!filter_state) {return (true);}
	if (station_id != log_rec.get_station_id()) {return (false);}
	if (start_time>log_rec.get_record_time() || stop_time<log_rec.get_record_time()) { return (false);}

//NOTICE: messages types not used!!!	becouse I dont uderstud what it defind - range id - stuped desigion
// if (!(log_rec.get_msg_id() & messages)) return (false);
		
		metro_escalators_container::iterator_metro_escalators iter_esc;
		iter_esc=g_escalators.find(log_rec.get_device_id());
		if (iter_esc==g_escalators.end()) return (false);
		if (first_escalator_num>iter_esc->second.get_number() || last_escalator_num < iter_esc->second.get_number() ) return (false);

	return(true);
	};

};



class log_records_container {
private :
typedef	vector<log_record> log_rec_vector;

public :
typedef  log_rec_vector::iterator iterator_log_rec;

private :
log_rec_vector container_log_rec;
PtWidget_t*		wnd;
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
enum msg_type {INFO=0x1, ALARM=0x2, BLOCK_C=0x4, LOCAL=0x8};

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
		wnd=NULL;
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
	
	if (new_wnd!=NULL) 
		{
			PtSetArg(&arg, 
							Pt_ARG_POINTER , 
							this, 
							0);
			 PtSetResources( new_wnd,
					                    1,
                    						&arg);
		};		
	
	wnd=new_wnd;
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
	filter.set_filter_off();

	sort (container_log_rec.begin(), 
			container_log_rec.end(),
			log_rec_sorter);

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
	if (this->filter(rec))
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
		log_rec_vector(container_log_rec).swap(container_log_rec);		
		iter_end=container_log_rec.end();		
	};

bool empty() const {return (container_log_rec.empty());}
int size() const{return (container_log_rec.size());};
/*
other metods
*/
int load (string file_name)
{
/*
	int		logFile;
	dword	size;
	char	signature[7];
	LogRecord record;
	int	msgID, stationID, escalatorID;
	time_t	saveTime;
	time_t	curTime;
	struct	stat fileStat;
	int		justCreated = 0;

	SysMessage(INFO_MSG, "In Log::Load");
	
	if ((logFile = open(fileName, O_RDONLY, O_BINARY)) < 0)
	{
		if ((logFile = open(fileName, O_CREAT, O_BINARY)) < 0)
		{
			SysMessage(ERROR_MSG, "can't create log file [%s]", strerror(errno));
			return 0;
		}
		else
			INFO_BOX("Создан новый файл журнала");
		curTime = time(NULL);
		localtime_r(&curTime, &lastAccessTime); 
		justCreated = 1;
	}
	if(stat(fileName, &fileStat) != -1)
      	localtime_r(&fileStat.st_mtime, &lastAccessTime);
	else
		SysMessage(ERROR_MSG, "can't get log file access time [%s]", strerror(errno));

	read(logFile, signature, 6);
	signature[6] = '\0';
	if (!justCreated)
	{
		if (!strcmp(signature, "LOGDAT"))
		{
			read(logFile, &size, sizeof(size));
		
			for(dword i=0; i<size; i++)
			{
				read(logFile, &msgID, sizeof(msgID));
				read(logFile, &stationID, sizeof(stationID));
				read(logFile, &escalatorID, sizeof(escalatorID));
				read(logFile, &saveTime, sizeof(saveTime));
				AddMessage(msgID, stationID, escalatorID, &saveTime);
			}
		}
		else
		{
			ERROR_BOX("Файл журнала поврежден");
			SysMessage(ERROR_MSG, "Incompatible log file format expected");
			return 0;
		}
	}

	SysMessage(INFO_MSG, "In Log::Load - leaved");
	close(logFile);
	
	return 1;

*/
return (1);
};



int save (string file_name)
{
/*
	int		logFile;
	dword	size;
	char	signature[] = "LOGDAT";
	CLogRecContainer::LogRecIterator log_rec_iterator;
	
	printf ("\nSaving journal\n");
	
	if ((logFile = open(fileName, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH)) < 0)
	{
		SysMessage(ERROR_MSG, "Fail to open log file [%s]", strerror(errno));
		return 0;
	}
		
	write(logFile, signature, strlen(signature));
	size = records.size();
	write(logFile, &size, sizeof(size));
	log_rec_iterator=records.begin();
	while(log_rec_iterator!= records.end())
	{
		write(logFile, &(*log_rec_iterator).msgID, sizeof((*log_rec_iterator).saveTime));
		write(logFile, &(*log_rec_iterator).stationID, sizeof((*log_rec_iterator).saveTime));
		write(logFile, &(*log_rec_iterator).escalatorID, sizeof((*log_rec_iterator).saveTime));
		write(logFile, &(*log_rec_iterator).saveTime, sizeof((*log_rec_iterator).saveTime));			
		log_rec_iterator++;
	}
	close(logFile);
	SysMessage(INFO_MSG, "Save current log");

	return 1;
*/
return (1);
};


void rotate()
{
/*
	time_t			curTime;
	struct	tm		localTime;
	char			fileName[11];

	curTime = time(NULL);
	localtime_r(&curTime, &localTime);
	if ((localTime.tm_mon != lastAccessTime.tm_mon) && !(localTime.tm_mon % 3))
	{
		int quartal = int(lastAccessTime.tm_mon / 3) + 1;
		sprintf(fileName, "%d-%d.log", quartal, lastAccessTime.tm_year + 1900);
		Save(fileName);
		memcpy(&lastAccessTime, &localTime, sizeof(lastAccessTime));
		Clear();
		SysMessage(INFO_MSG, "Rotating log");
	}

	return 1;
*/
}; 




void set_filter()
{
/*
if  data member class log_filter set to default settings sort by id and set data member iter_end to container_log_rec.end()
else partition_stable and set iter_end to returned iterartor (sort before ???)
*/
};

};

#endif