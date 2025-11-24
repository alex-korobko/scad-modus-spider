#include "global.h"
#include "system_settings.h"
#include "spider_exception.h"
#include "data_block.h"
#include "metro_device.h"
#include "contain_msg_types.h"
#include "contain_msg_dict.h"
#include "devices_types.h"
#include "contain_metro_devices_types.h"
#include "contain_morning_start.h"
#include "line.h"
#include "contain_metro_lines.h"
#include "station.h"
#include "contain_metro_stations.h"
#include "escalator.h"
#include "contain_metro_escalators.h"
#include "contain_log_rec.h"

log_record::log_record(int new_id, int new_msg_id, int new_msg_type, int new_station_id, int new_device_id, time_t new_record_time):
		id(new_id),  msg_id(new_msg_id), msg_type(new_msg_type), 
		station_id(new_station_id), device_id(new_device_id), record_time(new_record_time) {};

log_filter::log_filter() :
		start_time(0), stop_time(0),
		all_times(true), 
		all_devices(true), 
		all_stations(true),
		all_msg_types(true),
		filter_state(false) {};

log_filter::log_filter (const log_filter& filter_to_copy)  :
		contain_id_msg_types (filter_to_copy.contain_id_msg_types),
		contain_id_stations (filter_to_copy.contain_id_stations),
		contain_id_devices(filter_to_copy.contain_id_devices),
		start_time(filter_to_copy.start_time), 
		stop_time(filter_to_copy.stop_time),
		all_times(filter_to_copy.all_times), 
		all_devices(filter_to_copy.all_devices), 
		all_stations(filter_to_copy.all_stations),
		all_msg_types(filter_to_copy.all_msg_types),
		filter_state(filter_to_copy.filter_state){};

log_filter& log_filter::operator = (log_filter& filter_to_assigment) {
		if (this== &filter_to_assigment) return *this;
		
		start_time=filter_to_assigment.start_time;
		stop_time=filter_to_assigment.stop_time;
		
		all_times=filter_to_assigment.all_times;
		all_devices=filter_to_assigment.all_devices;
		all_stations=filter_to_assigment.all_stations;
		all_msg_types=filter_to_assigment.all_msg_types;
		filter_state=filter_to_assigment.filter_state;

		contain_id_msg_types = filter_to_assigment.contain_id_msg_types;
		contain_id_stations = filter_to_assigment.contain_id_stations;
		contain_id_devices = filter_to_assigment.contain_id_devices;

		return *this;
	};

bool log_filter::operator == (const log_filter filter_to_compare) const {
		if (this == &filter_to_compare) return true;
		
		if (filter_state!=filter_to_compare.filter_state) return false;

		if (all_times!=filter_to_compare.all_times) return false;
		if (all_devices!=filter_to_compare.all_devices) return false;
		if (all_stations!=filter_to_compare.all_stations) return false;
		if (all_msg_types!=filter_to_compare.all_msg_types) return false;
	
		if (start_time!=filter_to_compare.start_time) return false;
		if (stop_time!=filter_to_compare.stop_time) return false;

		if (contain_id_msg_types.size()!= 
			 filter_to_compare.contain_id_msg_types.size()) return false;
		if (contain_id_devices.size()!= 
			 filter_to_compare.contain_id_devices.size()) return false;
		if (contain_id_stations.size()!= 
			 filter_to_compare.contain_id_stations.size()) return false;

		const_ids_container_iterator tmp_iter;
	
		tmp_iter=contain_id_msg_types.begin();
		while (tmp_iter!=contain_id_msg_types.end())
			{
			if (filter_to_compare.contain_id_msg_types.find(*tmp_iter)==
				filter_to_compare.contain_id_msg_types.end()) return false;
			tmp_iter++;
			};

		tmp_iter=contain_id_stations.begin();
		while (tmp_iter!=contain_id_stations.end())
			{
			if (filter_to_compare.contain_id_stations.find(*tmp_iter)==
				filter_to_compare.contain_id_stations.end()) return false;
			tmp_iter++;
			};

		tmp_iter=contain_id_devices.begin();
		while (tmp_iter!=contain_id_devices.end())
			{
			if (filter_to_compare.contain_id_devices.find(*tmp_iter)==
				filter_to_compare.contain_id_devices.end()) return false;
			tmp_iter++;
			};

		return true;
	};


bool log_filter::operator () (const log_record log_rec) const {
	if (! filter_state) {return (true);}

	if (! all_msg_types)
			if (contain_id_msg_types.end()==contain_id_msg_types.find(log_rec.get_msg_type())) {return (false);};

	if (! all_times)
			if (log_rec.get_record_time()<start_time ||
				log_rec.get_record_time()>stop_time) return (false);

	if (! all_stations)
			{
			if (contain_id_stations.end()==contain_id_stations.find(log_rec.get_station_id())) {return (false);};

			if (! all_devices)
				if (contain_id_devices.end()==contain_id_devices.find(log_rec.get_device_id())) {return (false);};

			};
			
	return(true);
	};

/*
Sort all interval by log_rec_sorter()
and set  iter_end to container_log_rec.end()
*/
void log_records_container::sort_all (){
	 container_log_rec.sort (log_rec_sorter);
	 iter_end=container_log_rec.end();
	}

log_records_container::log_records_container(): records_count(0) {
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


void log_records_container::set_widget(PtWidget_t *new_wnd) {
	if (wnd!=NULL) {
			PtSetResource(wnd, Pt_ARG_POINTER, NULL, 0);				
		}
	
	if (new_wnd!=NULL) {
			PtSetResource(new_wnd, Pt_ARG_POINTER, this, 0);				
		};		
	
	wnd=new_wnd;
	};

void log_records_container::set_filtration_state_indicator(
				PtWidget_t *new_filtration_state_indicator) {
	if (filtration_state_indicator!=NULL) {
			PtSetResource(filtration_state_indicator, Pt_ARG_POINTER, NULL, 0);				
		}
	
	if (new_filtration_state_indicator!=NULL) {
			PtSetResource(new_filtration_state_indicator, Pt_ARG_POINTER, this, 0);				
		};		

	filtration_state_indicator=new_filtration_state_indicator;
	};

/*
Insert 
new log_record rec into container
if  rec in filter condition - it was inserted into it`s position in container
otherwise it pushed to back
*/

void log_records_container::insert (log_record rec) { 
	if (! container_log_rec.empty() && this->filter.get_filter_state() && this->filter(rec))
		{
			container_log_rec.insert(upper_bound(container_log_rec.begin(),
																		iter_end,
																		rec,
																		log_rec_sorter),
												rec);
		}  else {
			container_log_rec.push_back(rec);
		};
		
};

void log_records_container::erase(iterator_log_rec iter) {
		if (iter_end==iter) iter_end=container_log_rec.end(); //ATTENTION : what can I do ???
		container_log_rec.erase(iter);
};

		
void log_records_container::erase_all() {
		container_log_rec.erase(container_log_rec.begin(), container_log_rec.end());
		contain_log_rec(container_log_rec).swap(container_log_rec);		
		iter_end=container_log_rec.end();		
};

void log_records_container::prepare_to_display(){
	if(wnd==NULL) return;

	PtGenListRemoveItems(wnd, NULL,NULL);

	unsigned short size=distance(begin(), 
													end());

   if (size==0) return ;

	PtGenListItem_t *first, *last,  *list_item=new(PtGenListItem_t);

	first=list_item;

	first->prev=NULL;
	first->next=NULL;
	first->size.w=0;
	first->size.h=system_settings::ROW_HEIGHT;
	first->flags=0;

	for (int i=1; i<size; i++)
		{
			last=	new(PtGenListItem_t);
			last->prev=list_item;
			last->next=NULL;
			last->size.w=0;
			last->size.h=system_settings::ROW_HEIGHT;
			last->flags=0;
			list_item->next=last;
				
			list_item=last;
		};

	 PtGenListAddItems(wnd, first, NULL);

	 unsigned short *visible_count, top_item_pos;
	 
	 PtGetResource(wnd, Pt_ARG_VISIBLE_COUNT, &visible_count, 0);

	if (*visible_count<size)
		{
			top_item_pos=size-*visible_count+1;
			 PtSetResource(wnd, Pt_ARG_TOP_ITEM_POS,  &top_item_pos, 0);
		};
}


void log_records_container::load (
                 string file_name,
                 system_settings *sys_sett,
                 metro_devices_container *devs_cont,
                 metro_devices_types_container *metro_devices_types,
                 msg_dict_container *msg_dict) throw (spider_exception) {
	ostringstream exception_description;

	FILE		*logFile;
	vector<char> signature(6);
//LOGDAT is signature of old format log files , SPDLOG is signature of new format log files
	string logdat("LOGDAT"), 
              spdlog("SPDLOG"); 
	bool new_format_file=false;
	dword	size=0;
	struct stat file_stat;

	long int  tmp_autoincrement;
	int mess_id, station_id, device_id;
	time_t save_time;

	filter.set_filter_state(false);
	set_filtering();
	erase_all();

	if ((logFile = fopen(file_name.c_str(), "r")) ==NULL)
	{
		if ((logFile = fopen(file_name.c_str(), "w")) !=NULL)
		{
		sys_sett->message_window(system_settings::INFO_MSG, string("Создан новый файл журнала"));

		if (fwrite(logdat.c_str(),
				  sizeof(string::value_type),	
				  logdat.size(),
				  logFile ) <=0)	{
					exception_description<<"Can`t write to file "<<file_name<<" : "<<strerror( errno);
					if (fclose (logFile)!=0){
					    exception_description<<"\nCan`t close "<<file_name<<" : "<<strerror( errno);
					};
                    throw spider_exception(exception_description.str());
				};

		if (fwrite(&size,
				  sizeof(dword),	
				  1,
				  logFile	)<=0){
					exception_description<<"Can`t write to file "<<file_name<<" : "<<strerror( errno);
					if (fclose (logFile)!=0){
					    exception_description<<"\nCan`t close "<<file_name<<" : "<<strerror( errno);
					};
                    throw spider_exception(exception_description.str());
				};

		if (fclose (logFile)!=0) {
           exception_description<<"Can`t close  "<<file_name<<" : "<<strerror( errno);
           throw spider_exception(exception_description.str());
		};
		return;
		} else {

		exception_description<<"Can`t close "<<file_name<<" : "<<strerror( errno);
         throw spider_exception(exception_description.str());
		};
	};

	if (fread(&signature[0], sizeof(string::value_type), signature.size(), logFile)< signature.size()){
       sys_sett->message_window(system_settings::ERROR_MSG, "Файл журнала поврежден	");

		if (fclose (logFile)!=0) {
           exception_description<<"Can`t close "<<file_name<<" : "<<strerror( errno);
           throw spider_exception(exception_description.str());
		};
		return;
	};

	if (logdat.compare(0, 
								 logdat.size(),
								 &signature[0],
								 logdat.size()
								 )!=0){
		
		 if (spdlog.compare(0, 
									 spdlog.size(),
									 &signature[0],
									 spdlog.size()
									 )==0) {
				new_format_file=true;
 			} else {
               sys_sett->message_window(system_settings::ERROR_MSG, "Файл журнала поврежден	");
                exception_description<<"Unknown file signature";
                 if (fclose (logFile)!=0) {
                   exception_description<<"\nCan`t close "<<file_name<<" : "<<strerror( errno);
                   throw spider_exception(exception_description.str());
                  };
                 throw spider_exception(exception_description.str());
			};
		}
		
	if (fread(&size, sizeof(size), 1, logFile)==0 && size<0){	
               sys_sett->message_window(system_settings::ERROR_MSG, "Файл журнала поврежден	");
                 exception_description<<"Can`t read from file "<<file_name<<" : "<<strerror( errno);
                 if (fclose (logFile)!=0) {
                   exception_description<<"\nCan`t close  "<<file_name<<" : "<<strerror( errno);
                  };
                 throw spider_exception(exception_description.str());
	};
	
	while (size-->0)	
	{
		if (new_format_file) 
			{
			if (fread(&tmp_autoincrement, sizeof(tmp_autoincrement), 1, logFile) <=0)	{
                    sys_sett->message_window(system_settings::ERROR_MSG, "Файл жунала поврежден - ошибка чтения");
                    exception_description<<"Can`t read from file "<<file_name<<" : "<<strerror( errno);
                     if (fclose (logFile)!=0) {
                        exception_description<<"\nCan`t close "<<file_name<<" : "<<strerror( errno);
                     };
                    throw spider_exception(exception_description.str());
				};

			} else {
			tmp_autoincrement=records_count+1;
			};
			
		if (fread(&mess_id, sizeof(mess_id), 1, logFile)<=0){
                    sys_sett->message_window(system_settings::ERROR_MSG, "Файл жунала поврежден - ошибка чтения");
                    exception_description<<"Can`t read from file "<<file_name<<" : "<<strerror( errno);
                    if (fclose (logFile)!=0) {
                        exception_description<<"\nCan`t close "<<file_name<<" : "<<strerror( errno);
                     };
                    throw spider_exception(exception_description.str());
				};


		if (fread(&station_id, sizeof(station_id), 1, logFile)<=0)
				{
                    sys_sett->message_window(system_settings::ERROR_MSG, "Файл жунала поврежден - ошибка чтения");
                    exception_description<<"Can`t read from file "<<file_name<<" : "<<strerror( errno);
                     if (fclose (logFile)!=0) {
                        exception_description<<"\nCan`t close "<<file_name<<" : "<<strerror( errno);
                     };
                    throw spider_exception(exception_description.str());
				};


		if (fread(&device_id, sizeof(device_id), 1, logFile)<=0)
				{
                    sys_sett->message_window(system_settings::ERROR_MSG, "Файл жунала поврежден - ошибка чтения");
                    exception_description<<"Can`t read from file "<<file_name<<" : "<<strerror( errno);
                     if (fclose (logFile)!=0) {
                        exception_description<<"\nCan`t close "<<file_name<<" : "<<strerror( errno);
                     };
                    throw spider_exception(exception_description.str());                    
				};

		if (fread(&save_time, sizeof(save_time), 1, logFile)<=0)
				{
                    sys_sett->message_window(system_settings::ERROR_MSG, "Файл жунала поврежден - ошибка чтения");
                    exception_description<<"Can`t read from file "<<file_name<<" : "<<strerror( errno);
                     if (fclose (logFile)!=0) {
                        exception_description<<"\nCan`t close "<<file_name<<" : "<<strerror( errno);
                     };
                    throw spider_exception(exception_description.str());
				};

		int msg_type=0;
		int tmp_mess_id=mess_id&0x7FFF;
		//what`s up, Doc?? see draw_log_item@draw.cpp for more details about information stored in message id
		//begin : search message with that id in device type. if not found in messages dictionary;
		//if device id=0 search in messages dictionary
		if (device_id!=0)
			{
				metro_devices_types_container::iterator_devices_types iter_devices_types;
				metro_devices_container::iterator_metro_devices iter_metro_devices;			

				iter_metro_devices=devs_cont->find(device_id);
				if (iter_metro_devices!=devs_cont->end()){
						iter_devices_types=metro_devices_types->find(iter_metro_devices->second->get_type());
						if (iter_devices_types!=metro_devices_types->end()) {
								msg_dict_container::msg_dict_iterator tmp_msg_iter=
									iter_devices_types->second.type_messages.find(tmp_mess_id);
								if (tmp_msg_iter!=
									iter_devices_types->second.type_messages.end()) {
										msg_type=tmp_msg_iter->second.get_type();
									};
							}  else  {// if (iter_devices_types!=metro_devices_types->end())
					                    exception_description<<"Error in  file "<<file_name;
					                    exception_description<<"  not found device type with id "<<iter_metro_devices->second.get_type();
					                    exception_description<<" for device "<<device_id;

					                     if (fclose (logFile)!=0) {
     						                   exception_description<<"\nCan`t close "<<file_name<<" : "<<strerror( errno);
					                     };
					                    throw spider_exception(exception_description.str());
							};// if (iter_devices_types=metro_devices_types->end())
					} else {
	                    exception_description<<"Error in  file "<<file_name;
	                    exception_description<<"  not found device with id "<<device_id;

	                     if (fclose (logFile)!=0) {
  						                   exception_description<<"\nCan`t close "<<file_name<<" : "<<strerror( errno);
	                     };
	                    throw spider_exception(exception_description.str());
					}; // if (iter_metro_devices!=esc_cont->end())
			};

		if (msg_type==0)
			{
			msg_dict_container::msg_dict_iterator tmp_msg_iter=msg_dict->find(tmp_mess_id);
			if(tmp_msg_iter==msg_dict->end())
				{
                   exception_description<<"Error in  file "<<file_name;
                   exception_description<<"  not found message with id "<<mess_id;

                    if (fclose (logFile)!=0) {
		                   exception_description<<"\nCan`t close "<<file_name<<" : "<<strerror( errno);
	                     };
                    throw spider_exception(exception_description.str());
				}; // if(tmp_msg_iter==msg_dict->end())
				//end : search message with that id in device type.
				msg_type=tmp_msg_iter->second.get_type();
			};

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
              exception_description<<"Can`t get file access time "<<file_name<<" : "<<strerror( errno);

              if (fclose (logFile)!=0) {
                   exception_description<<"\nCan`t close "<<file_name<<" : "<<strerror( errno);
	                     };
              throw spider_exception(exception_description.str());
	     };

   if (fclose (logFile)!=0) {
       exception_description<<"Can`t close "<<file_name<<" : "<<strerror( errno);
        throw spider_exception(exception_description.str());
    };
};

void log_records_container::save (string file_name) 
	throw (spider_exception){
	FILE		*logFile;
	string spdlog("SPDLOG"); //LOGDAT is signature of old format log files , SPDLOG is signature of new format log files
	dword	size=container_log_rec.size();
	iterator_log_rec iter_log=container_log_rec.begin();
	ostringstream exception_description;

	if ((logFile = fopen(file_name.c_str(), "w")) ==NULL){
       exception_description<<"Can`t open file for writing "<<file_name<<" : "<<strerror( errno);
        throw spider_exception(exception_description.str());
	};
	
	if (fwrite(spdlog.c_str(), sizeof(string::value_type), spdlog.size(), logFile) <=0)	{
          exception_description<<"Can`t write to file "<<file_name<<" : "<<strerror( errno);

          if (fclose (logFile)!=0) {
                   exception_description<<"\nCan`t close "<<file_name<<" : "<<strerror( errno);
	                     };
          throw spider_exception(exception_description.str());
	};
	
	if (fwrite(&size, sizeof(size), 1, logFile) <=0)	{
          exception_description<<"Can`t write to file "<<file_name<<" : "<<strerror( errno);

          if (fclose (logFile)!=0) {
                   exception_description<<"\nCan`t close "<<file_name<<" : "<<strerror( errno);
	                     };
          throw spider_exception(exception_description.str());
	};
	
	while (iter_log!=container_log_rec.end()){
		long tmp_long=iter_log->get_id();
		if (fwrite(&tmp_long, sizeof(tmp_long), 1, logFile) <=0)	{
          exception_description<<"Can`t write to file "<<file_name<<" : "<<strerror( errno);
          if (fclose (logFile)!=0) {
                   exception_description<<"\nCan`t close "<<file_name<<" : "<<strerror( errno);
	                     };
          throw spider_exception(exception_description.str());
		};

		int tmp_int=iter_log->get_msg_id();
		if (fwrite(&tmp_int, sizeof(tmp_int), 1, logFile) <=0)	{
          exception_description<<"Can`t write to file "<<file_name<<" : "<<strerror( errno);
          if (fclose (logFile)!=0) {
                   exception_description<<"\nCan`t close "<<file_name<<" : "<<strerror( errno);
	                     };
          throw spider_exception(exception_description.str());
		};

		tmp_int=iter_log->get_station_id();
		if (fwrite(&tmp_int, sizeof(tmp_int), 1, logFile) <=0)	{
          exception_description<<"Can`t write to file "<<file_name<<" : "<<strerror( errno);
          if (fclose (logFile)!=0) {
                   exception_description<<"\nCan`t close "<<file_name<<" : "<<strerror( errno);
	                     };
          throw spider_exception(exception_description.str());
		};

		tmp_int=iter_log->get_device_id();
		if (fwrite(&tmp_int, sizeof(tmp_int), 1, logFile) <=0)	{
          exception_description<<"Can`t write to file "<<file_name<<" : "<<strerror( errno);
          if (fclose (logFile)!=0) {
                   exception_description<<"\nCan`t close "<<file_name<<" : "<<strerror( errno);
	                     };
          throw spider_exception(exception_description.str());
		};

		time_t tmp_time=iter_log->get_record_time();
		if (fwrite(&tmp_time, sizeof(tmp_time), 1, logFile) <=0){
          exception_description<<"Can`t write to file "<<file_name<<" : "<<strerror( errno);
          if (fclose (logFile)!=0) {
                   exception_description<<"\nCan`t close "<<file_name<<" : "<<strerror( errno);
	                     };
          throw spider_exception(exception_description.str());
		};

		iter_log++;
		};

    if (fclose (logFile)!=0) {
        exception_description<<"Can`t close "<<file_name<<" : "<<strerror( errno);
        throw spider_exception(exception_description.str());
    };
};

void log_records_container::set_filtering(){
sort_all();
if (filter.get_filter_state()) {
	iter_end=stable_partition(container_log_rec.begin(),
											container_log_rec.end(),
											filter);
	};

};

bool log_records_container::rotate() throw(spider_exception){
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

		try{
            save(file_name);
		} catch (spider_exception spr_exc) {
           return false;			
		};
		erase_all();
		filter.set_filter_state(false);
		set_filtering();

		memcpy(&last_saved_time, &localTime, sizeof(last_saved_time));
	}

	return true;
}; 





