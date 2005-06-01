using namespace std;

#include <Ph.h>
#include <Ap.h>
#include <Pt.h>
#include <sys/neutrino.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

#include <vector>
#include <string>
#include <hash_map>
#include <set>
#include <list>
#include <functional>
#include <algorithm>
#include <sstream>

#include "defines.h"
#include "spider_exception.h"
#include "system_settings.h"
#include "system_settings_spider.h"

#include "command.h"
#include "data_block.h"
#include "msg_types_container.h"
#include "msg_dict_container.h"
#include "devices_types.h"
#include "timer_command_container.h"
class metro_devices_container;
#include "station.h"
#include "metro_stations_container.h"
#include "metro_devices_types_container.h"
#include "metro_device.h"
#include "metro_devices_container.h"
#include "data_block.h"
#include "shavr_data_block.h"
#include "shavr.h"

#include "log_records_container.h"

extern msg_dict_container *messages;

log_record::log_record(ldword new_id, ldword new_msg_id,
                                 int new_msg_type, int new_station_id,
                                 int new_device_id, time_t new_record_time):
		id(new_id),  msg_id(new_msg_id), msg_type(new_msg_type), 
		station_id(new_station_id), device_id(new_device_id), record_time(new_record_time) {};

log_filter::log_filter() :
		start_time(time(NULL)), stop_time(time(NULL)),
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

		const_iterator tmp_iter;

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


//Sort all interval by log_rec_sorter()
//and set  iter_end to container_log_rec.end()
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
		wnd=NULL;
        filtration_state_indicator=NULL;
        first=NULL;
        toggle_button_filter_turn_off=NULL;
        toggle_button_filter_all_times=NULL;
        toggle_button_filter_all_messages_types=NULL;
        toggle_button_filter_all_stations=NULL;
        toggle_button_filter_all_devices=NULL;
        calendar_begin_time_interval=NULL;
        calendar_end_time_interval=NULL;
        numeric_begin_hour=NULL;
        numeric_end_hour=NULL;
        numeric_begin_minute=NULL;
        numeric_end_minute=NULL;
        messages_types_list=NULL;
        stations_list=NULL;
        station_devices_list=NULL;
	};


void log_records_container::set_widget(PtWidget_t *new_wnd) {

  //ATTENTION: In PtGenListAddItems free memory  of first already present??
         PtGenListItem_t *list_item;
          while (first!=NULL) {
              list_item=first;
              first=first->next;
              free(list_item);
           }; //while (last!=NULL)

	wnd=new_wnd;
	};

void log_records_container::set_filtration_state_indicator(
				PtWidget_t *new_filtration_state_indicator) {
	filtration_state_indicator=new_filtration_state_indicator;
	};

//Insert 
//new log_record rec into container
//if  rec in filter condition - it was inserted into it`s position in container
//otherwise it pushed to back
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

void log_records_container::erase(iterator iter) {
		if (iter_end==iter) iter_end=container_log_rec.end(); //ATTENTION : what can I do ???
		container_log_rec.erase(iter);
};

		
void log_records_container::erase(iterator iter_beg, iterator iter_end) {
		container_log_rec.erase(iter_beg, iter_end);
		contain_log_rec(container_log_rec).swap(container_log_rec);		
		iter_end=container_log_rec.end();		
};

void log_records_container::prepare_to_display(){
    system_settings_spider *sys_sett_spr=system_settings_spider::get_instance();
	PtGenListItem_t *last,
                                *list_item;
	if(wnd==NULL) {
           sys_sett_spr->sys_message(system_settings::ERROR_MSG,
                       "log_records_container::prepare_to_display():wnd==NULL");
               return;
           };
	PtGenListRemoveItems(wnd, NULL,NULL);

	unsigned short size=this->size();

  //ATTENTION: In PtGenListAddItems free memory  of first already present??
    while (first!=NULL) {
              list_item=first;
              first=first->next;
              free(list_item);
           }; //while (first!=NULL)

   if (size==0) return ;


    last=NULL;
    list_item=(PtGenListItem_t*)malloc(1*sizeof(PtGenListItem_t));

	first=list_item;

	first->prev=NULL;
	first->next=NULL;
	first->size.w=0;
	first->size.h=system_settings_spider::ROW_HEIGHT;
	first->flags=0;

	for (int i=1; i<size; i++) {
			last=(PtGenListItem_t*)malloc(1*sizeof(PtGenListItem_t));

			last->prev=list_item;
			last->next=NULL;
			last->size.w=0;
			last->size.h=system_settings_spider::ROW_HEIGHT;
			last->flags=0;
			list_item->next=last;
				
			list_item=last;
		};

	PtGenListAddItems(wnd, first, NULL);

	 unsigned short *visible_count, top_item_pos;
	 PtGetResource(wnd, Pt_ARG_VISIBLE_COUNT, &visible_count, 0);

	if (*visible_count<size){
			top_item_pos=size-*visible_count+1;
			 PtSetResource(wnd, Pt_ARG_TOP_ITEM_POS,  &top_item_pos, 0);
		};
}


void log_records_container::load (
                 string file_name) throw (spider_exception) {
	ostringstream exception_description;
    system_settings_spider *sys_sett=system_settings_spider::get_instance();
    metro_devices_container *devs_cont=metro_devices_container::get_instance();
    metro_devices_types_container *metro_devices_types=metro_devices_types_container::get_instance();
    metro_stations_container *metro_stations=metro_stations_container::get_instance();

	FILE		*logFile;
	vector<char> signature(6);
//LOGDAT is signature of old format log files , SPDLOG is signature of new format log files
	string logdat("LOGDAT"), 
              spdlog("SPDLOG"); 
	bool new_format_file=false;
	struct stat file_stat;

	ldword  tmp_autoincrement;
    ldword message_id;
    ldword second_byte_mask=0;
    second_byte_mask=!second_byte_mask;
    int msg_type=0;

	dword old_mess_id, station_id, device_id, size=0;
	time_t save_time;
    byte tmp_mess_id;

	filter.set_filter_state(false);
	set_filtering();
	container_log_rec.erase(container_log_rec.begin(), container_log_rec.end());

	if ((logFile = fopen(file_name.c_str(), "r")) ==NULL)	{
		if ((logFile = fopen(file_name.c_str(), "w")) !=NULL){
		 if (sys_sett->get_main_window()==NULL ||
               PtWidgetIsRealized(sys_sett->get_main_window())==0) {
		       sys_sett->sys_message(system_settings::INFO_MSG, "New main log file created");
		  } else { //if (PtWidgetIsRealized(sys_sett->get_main_window())
 		       sys_sett->message_window(system_settings::INFO_MSG, string("Создан новый файл журнала"), true);
		  }; //if (PtWidgetIsRealized(sys_sett->get_main_window())

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
		 if (sys_sett->get_main_window()==NULL ||
              PtWidgetIsRealized(sys_sett->get_main_window())==0) {
		     sys_sett->sys_message(system_settings::ERROR_MSG, "Log file corrupted");
		  } else { //if (PtWidgetIsRealized(sys_sett->get_main_window())
               sys_sett->message_window(system_settings::ERROR_MSG, "Файл журнала поврежден	",true);
		  }; //if (PtWidgetIsRealized(sys_sett->get_main_window())

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
                if (sys_sett->get_main_window()==NULL ||
                    PtWidgetIsRealized(sys_sett->get_main_window())==0) {
                    sys_sett->sys_message(system_settings::ERROR_MSG, "Log file corrupted");
                  } else { //if (PtWidgetIsRealized(sys_sett->get_main_window())
                    sys_sett->message_window(system_settings::ERROR_MSG, "Файл журнала поврежден	",true);
                  }; //if (PtWidgetIsRealized(sys_sett->get_main_window())

                exception_description<<"Unknown file signature";
                 if (fclose (logFile)!=0) {
                   exception_description<<"\nCan`t close "<<file_name<<" : "<<strerror( errno);
                   throw spider_exception(exception_description.str());
                  };
                 throw spider_exception(exception_description.str());
			};
		}
		
	if (fread(&size, sizeof(size), 1, logFile)==0 && size<0){
               if (sys_sett->get_main_window()==NULL ||
                    PtWidgetIsRealized(sys_sett->get_main_window())==0) {
                    sys_sett->sys_message(system_settings::ERROR_MSG, "Log file corrupted");
                  } else { //if (PtWidgetIsRealized(sys_sett->get_main_window())
                    sys_sett->message_window(system_settings::ERROR_MSG, "Файл журнала поврежден	",true);
                  }; //if (PtWidgetIsRealized(sys_sett->get_main_window())
 
                 exception_description<<"Can`t read from file "<<file_name<<" : "<<strerror( errno);
                 if (fclose (logFile)!=0) {
                   exception_description<<"\nCan`t close  "<<file_name<<" : "<<strerror( errno);
                  };
                 throw spider_exception(exception_description.str());
	};


	while (size-->0) {
		if (new_format_file) {
			if (fread(&tmp_autoincrement, sizeof(tmp_autoincrement), 1, logFile) <=0)	{
                if (sys_sett->get_main_window()==NULL ||
                    PtWidgetIsRealized(sys_sett->get_main_window())==0) {
                    sys_sett->sys_message(system_settings::ERROR_MSG, "Log file corrupted");
                   } else { //if (PtWidgetIsRealized(sys_sett->get_main_window())
                     sys_sett->message_window(system_settings::ERROR_MSG, "Файл журнала поврежден	",true);
                  }; //if (PtWidgetIsRealized(sys_sett->get_main_window())

                    exception_description<<"Can`t read from file "<<file_name<<" : "<<strerror( errno);
                     if (fclose (logFile)!=0)
                        exception_description<<"\nCan`t close "<<file_name<<" : "<<strerror( errno);

                    throw spider_exception(exception_description.str());
				};

          if (fread(&message_id, sizeof(message_id), 1, logFile)<=0){
               if (sys_sett->get_main_window()==NULL ||
                    PtWidgetIsRealized(sys_sett->get_main_window())==0) {
                    sys_sett->sys_message(system_settings::ERROR_MSG, "Log file corrupted");
                  } else { //if (PtWidgetIsRealized(sys_sett->get_main_window())
                    sys_sett->message_window(system_settings::ERROR_MSG, "Файл журнала поврежден	",true);
                  }; //if (PtWidgetIsRealized(sys_sett->get_main_window())

                    exception_description<<"Can`t read from file "<<file_name<<" : "<<strerror( errno);
                    if (fclose (logFile)!=0) 
                        exception_description<<"\nCan`t close "<<file_name<<" : "<<strerror( errno);

                    throw spider_exception(exception_description.str());
				};

            tmp_mess_id=static_cast<byte>(message_id&0xff);

			} else { //if (new_format_file) {

           if (fread(&old_mess_id, sizeof(old_mess_id), 1, logFile)<=0){
               if (sys_sett->get_main_window()==NULL ||
                    PtWidgetIsRealized(sys_sett->get_main_window())==0) {
                    sys_sett->sys_message(system_settings::ERROR_MSG, "Log file corrupted");
                  } else { //if (PtWidgetIsRealized(sys_sett->get_main_window())
                    sys_sett->message_window(system_settings::ERROR_MSG, "Файл журнала поврежден	",true);
                  }; //if (PtWidgetIsRealized(sys_sett->get_main_window())

                    exception_description<<"Can`t read from file "<<file_name<<" : "<<strerror( errno);
                    if (fclose (logFile)!=0) 
                        exception_description<<"\nCan`t close "<<file_name<<" : "<<strerror( errno);

                    throw spider_exception(exception_description.str());
				};
               message_id=old_mess_id;
               tmp_mess_id=static_cast<byte>(old_mess_id&0xff);

			}; //if (new_format_file) {

		if (fread(&station_id, sizeof(station_id), 1, logFile)<=0){
               if (sys_sett->get_main_window()==NULL ||
                    PtWidgetIsRealized(sys_sett->get_main_window())==0) {
                    sys_sett->sys_message(system_settings::ERROR_MSG, "Log file corrupted");
                  } else { //if (PtWidgetIsRealized(sys_sett->get_main_window())
                    sys_sett->message_window(system_settings::ERROR_MSG, "Файл журнала поврежден	",true);
                  }; //if (PtWidgetIsRealized(sys_sett->get_main_window())

                    exception_description<<"Can`t read from file "<<file_name<<" : "<<strerror( errno);
                     if (fclose (logFile)!=0) {
                        exception_description<<"\nCan`t close "<<file_name<<" : "<<strerror( errno);
                     };
                    throw spider_exception(exception_description.str());
				};

		if (fread(&device_id, sizeof(device_id), 1, logFile)<=0){
               if (sys_sett->get_main_window()==NULL ||
                    PtWidgetIsRealized(sys_sett->get_main_window())==0) {
                    sys_sett->sys_message(system_settings::ERROR_MSG, "Log file corrupted");
                  } else { //if (PtWidgetIsRealized(sys_sett->get_main_window())
                    sys_sett->message_window(system_settings::ERROR_MSG, "Файл журнала поврежден	",true);
                  }; //if (PtWidgetIsRealized(sys_sett->get_main_window())

                    exception_description<<"Can`t read from file "<<file_name<<" : "<<strerror( errno);
                     if (fclose (logFile)!=0) {
                        exception_description<<"\nCan`t close "<<file_name<<" : "<<strerror( errno);
                     };
                    throw spider_exception(exception_description.str());                    
				};

		if (fread(&save_time, sizeof(save_time), 1, logFile)<=0){
               if (sys_sett->get_main_window()==NULL ||
                    PtWidgetIsRealized(sys_sett->get_main_window())==0) {
                    sys_sett->sys_message(system_settings::ERROR_MSG, "Log file corrupted");
                  } else { //if (PtWidgetIsRealized(sys_sett->get_main_window())
                    sys_sett->message_window(system_settings::ERROR_MSG, "Файл журнала поврежден	",true);
                  }; //if (PtWidgetIsRealized(sys_sett->get_main_window())

                    exception_description<<"Can`t read from file "<<file_name<<" : "<<strerror( errno);
                     if (fclose (logFile)!=0) {
                        exception_description<<"\nCan`t close "<<file_name<<" : "<<strerror( errno);
                     };
                    throw spider_exception(exception_description.str());
				};

		//id of message (from messages) in log_record message_id stored only in first byte.
        //Other bits can used or 
        //as breaking_path_value (high 4 bytes)
        // or as block_circut_index (second byte) example 
        //visualization of that parameters depend of %breaking_path and %block_circut 
        //modificators in message text

        //begin : search message with that id in device type. if not found in messages dictionary
        //of device type then find in global messages dictionary;
		//if device id=0 search in messages dictionary
		if (device_id!=0){
                msg_type=0;
				metro_devices_types_container::iterator iter_devices_types;
				metro_devices_container::iterator iter_metro_devices;			

				iter_metro_devices=devs_cont->find(device_id);
				if (iter_metro_devices!=devs_cont->end()){
						iter_devices_types=metro_devices_types->find(iter_metro_devices->second->get_type());
						if (iter_devices_types!=metro_devices_types->end()) {
                                if (new_format_file || (!new_format_file && (message_id&0x8000==0))) {
                                     msg_dict_container::iterator tmp_msg_iter=
                                          iter_devices_types->second->type_messages.find(tmp_mess_id);
                                     if (tmp_msg_iter!=iter_devices_types->second->type_messages.end())
                                                                            msg_type=tmp_msg_iter->second.get_type();
                                 }; // if (new_format_file || (!new_format_file && (message_id&0x8000==0)))
							}  else  { // if (iter_devices_types!=metro_devices_types->end())
					                    exception_description<<"Error in  file "<<file_name;
					                    exception_description<<"  not found device type with id "<<iter_metro_devices->second->get_type();
					                    exception_description<<" for device "<<device_id;
					                     if (fclose (logFile)!=0)
     						                   exception_description<<"\nCan`t close "<<file_name<<" : "<<strerror( errno);

					                    throw spider_exception(exception_description.str());
							};// if (iter_devices_types=metro_devices_types->end())
					} else {
                         exception_description<<"Error in  file "<<file_name;
                         exception_description<<"  not found device with id "<<device_id;
	                     if (fclose (logFile)!=0)
			                   exception_description<<"\nCan`t close "<<file_name<<" : "<<strerror( errno);

	                    throw spider_exception(exception_description.str());
					}; // if (iter_metro_devices!=esc_cont->end())

                }; // if (device_id!=0)

     if (station_id!=0) {
        metro_stations_container::iterator station_iter=metro_stations->find(station_id);
        if (station_iter==metro_stations->end()) {
         exception_description<<"Error in  file "<<file_name;
         exception_description<<"  not found station with id "<<station_id;

         if (fclose (logFile)!=0) {
                exception_description<<"\nCan`t close "<<file_name<<" : "<<strerror( errno);
	         };
	        throw spider_exception(exception_description.str());
          };
      };// if (station_id!=0)

   //if not found in device type messages (or device_id==0), must be found in global messages container
   if (msg_type==0 &&
       (new_format_file || (!new_format_file && (message_id&0x8000==0)
                                         )
       )
       ){ 
			msg_dict_container::iterator tmp_msg_iter=messages->find(tmp_mess_id);
			if(tmp_msg_iter==messages->end())
				{
                   exception_description<<"Error in  file "<<file_name;
                   exception_description<<"  not found message with id "<<message_id;
                    if (fclose (logFile)!=0) 
		                   exception_description<<"\nCan`t close "<<file_name<<" : "<<strerror( errno);

                    throw spider_exception(exception_description.str());
				}; // if(tmp_msg_iter==msg_dict->end())
				//end : search message with that id in device type.
				msg_type=tmp_msg_iter->second.get_type();
			}; //if (msg_type==0 &&

        if (new_format_file) {
              set_records_autoincrement(tmp_autoincrement);
              insert(log_record ( tmp_autoincrement,
                                          message_id,
                                          msg_type,
                                          station_id,
                                          device_id,
                                          save_time));
         } else {
            //in old format log files if mess_id&0x8000!=0 then that is not message it, that is block circut index. So it must be 
            //added to previos message to second byte
            if (old_mess_id&0x8000==0) {
      			tmp_autoincrement=records_count+1;
                //in old format log files in highest 2 bytes (4 bytes size of int) of message placed breaking path value
                //now breaking path value must be in 4 highest bytes (8 bytes size of ldword) of message placed breaking path value
                message_id=(old_mess_id&0xff)|((old_mess_id&0xffff0000)<<16);
                insert(log_record ( tmp_autoincrement,
                                          message_id,
                                          msg_type,
                                          station_id,
                                          device_id,
                                          save_time));
                 } else { //if (old_mess_id&0x8000==0) {
                   contain_log_rec::iterator finded_iter;
                   stw_finder by_ids_finder(tmp_autoincrement);
                   finded_iter=find_if(container_log_rec.begin(), container_log_rec.end(), by_ids_finder);
                   if (finded_iter!=container_log_rec.end()) {
                         //ATTENTION: finded first log record with message_id==tmp_autoincrement
                         message_id=finded_iter->get_msg_id();
                         message_id=(message_id&second_byte_mask)|((old_mess_id&0xff)<<8);
                         finded_iter->set_msg_id(message_id);
                      } else { //if (finded_iter!=container_log_rec.end())
                         ostringstream message;
                         message<<"Not found message with id "<<tmp_autoincrement<<" for message id (block circut) "<<old_mess_id;
                         sys_sett->sys_message(system_settings::ERROR_MSG, message.str());
                      }; //if (finded_iter!=container_log_rec.end())
                }; //if (old_mess_id&0x8000==0) {
         };
	}; //while (size-->0) 

	if(stat(file_name.c_str(), &file_stat) != -1){
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
	iterator iter_log=container_log_rec.begin();
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
	
   ldword tmp_long;
   int tmp_int;
   time_t tmp_time;

	while (iter_log!=container_log_rec.end()){

		tmp_long=iter_log->get_id();
		if (fwrite(&tmp_long, sizeof(tmp_long), 1, logFile) <=0)	{
          exception_description<<"Can`t write to file "<<file_name<<" : "<<strerror( errno);
          if (fclose (logFile)!=0)
                   exception_description<<"\nCan`t close "<<file_name<<" : "<<strerror( errno);
          throw spider_exception(exception_description.str());
		};

		tmp_long=iter_log->get_msg_id();
		if (fwrite(&tmp_long, sizeof(tmp_long), 1, logFile) <=0)	{
          exception_description<<"Can`t write to file "<<file_name<<" : "<<strerror( errno);
          if (fclose (logFile)!=0)
                   exception_description<<"\nCan`t close "<<file_name<<" : "<<strerror( errno);
          throw spider_exception(exception_description.str());
		};

		tmp_int=iter_log->get_station_id();
		if (fwrite(&tmp_int, sizeof(tmp_int), 1, logFile) <=0)	{
          exception_description<<"Can`t write to file "<<file_name<<" : "<<strerror( errno);
          if (fclose (logFile)!=0)
                   exception_description<<"\nCan`t close "<<file_name<<" : "<<strerror( errno);
          throw spider_exception(exception_description.str());
		};

		tmp_int=iter_log->get_device_id();
		if (fwrite(&tmp_int, sizeof(tmp_int), 1, logFile) <=0)	{
          exception_description<<"Can`t write to file "<<file_name<<" : "<<strerror( errno);
          if (fclose (logFile)!=0)
                   exception_description<<"\nCan`t close "<<file_name<<" : "<<strerror( errno);
          throw spider_exception(exception_description.str());
		};

		tmp_time=iter_log->get_record_time();
		if (fwrite(&tmp_time, sizeof(tmp_time), 1, logFile) <=0){
          exception_description<<"Can`t write to file "<<file_name<<" : "<<strerror( errno);
          if (fclose (logFile)!=0)
                   exception_description<<"\nCan`t close "<<file_name<<" : "<<strerror( errno);
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
	if (localTime.tm_mon != last_saved_time.tm_mon){

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
		container_log_rec.erase(container_log_rec.begin(), container_log_rec.end());
		filter.set_filter_state(false);
		set_filtering();

		memcpy(&last_saved_time, &localTime, sizeof(last_saved_time));
	}

	return true;
}; 


