#include <Ap.h>
#include <Ph.h>
#include <Pt.h>

#include <arpa/inet.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/neutrino.h>
#include <sys/syspage.h>
#include <inttypes.h>
#include <fcntl.h>
#include <signal.h>
#include <iostream.h>

#include <hash_map>
#include <set>
#include <vector>
#include <string>
#include <list>
#include <map>
#include <functional>
#include <algorithm>
#include <sstream>

using namespace std;

#include "test_cond.h"

#include "defines.h"
#include <sys/select.h> //#define FD_SETSIZE
#include "spider_exception.h"
#include "system_settings.h"
#include "system_settings_spider.h"

#include "socket_exception.h"
#include "generic_socket.h"
#include "client_socket.h"

#include "command.h"
#include "data_block.h"
#include "escalator_data_block.h"
#include "shavr_data_block.h"

#include "msg_types_container.h"
#include "msg_dict_container.h"
#include "log_records_container.h"

#include "devices_types.h"
#include "metro_devices_types_container.h"
#include "timer_command_container.h"
class metro_devices_container;
#include "line.h"
#include "metro_lines_container.h"
#include "station.h"
#include "metro_stations_container.h"
#include "metro_device.h"
#include "metro_devices_container.h"

extern msg_dict_container *messages;
extern log_records_container *main_log;
extern log_records_container *archive_log;
extern PtWidget_t *main_window;

//see draw_functinos.cpp
void archive_log_raw_list_draw_function( PtWidget_t *widget,
            PtGenListItem_t *item,
            unsigned index,
            unsigned nitems,
            PhRect_t *where );

void main_log_raw_list_draw_function( PtWidget_t *widget,
            PtGenListItem_t *item,
            unsigned index,
            unsigned nitems,
            PhRect_t *where );

//see in_code_attached_callbacks.cpp for close_window body
int
close_window( PtWidget_t *widget, 
                            ApInfo_t *apinfo, 
                           PtCallbackInfo_t *cbinfo );

//static functions
static void prepare_devices_list_in_filter_window (log_records_container *log_rec_cont,
                                                       vector<ushort_t> selected_stations_indexes) {
system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
ostringstream description;
unsigned int list_index=1;
const char* new_list_item[1];
static  string tmp_str;

if (spider_sys_sett==NULL) {
  cout<<"Can`t get instance of system_settings_spider"<<endl;
  return;
};

try {
   if (log_rec_cont==NULL)
        throw spider_exception("log_rec_cont==NULL");

   if (log_rec_cont->get_station_devices_list()==NULL)
        throw spider_exception("log_rec_cont->get_station_devices_list()==NULL");

    PtListDeleteAllItems(log_rec_cont->get_station_devices_list());
    if (selected_stations_indexes.empty()) return;

   metro_devices_container::iterator devices_iter;
   metro_devices_container *devices=metro_devices_container::get_instance();
   if (devices==NULL)
           throw spider_exception("devices==NULL");

   metro_station::iterator_devices_id devices_in_station_ids_iter;
   metro_stations_container::iterator stations_iter;
   metro_stations_container *stations=metro_stations_container::get_instance();
   if (stations==NULL)
           throw spider_exception("stations==NULL");

   metro_devices_types_container::iterator devices_types_iter;
   metro_devices_types_container *devices_types=metro_devices_types_container::get_instance();
   if (devices_types==NULL)
           throw spider_exception("devices_types==NULL");
  
   vector<ushort_t>::iterator stations_sel_idexes_iter=selected_stations_indexes.begin();
  while(stations_sel_idexes_iter!=selected_stations_indexes.end()) {
   stations_iter=stations->begin();
   advance(stations_iter, static_cast<int>(*stations_sel_idexes_iter-1));

        devices_in_station_ids_iter=stations_iter->second.begin_devices_id();
        while(devices_in_station_ids_iter!=stations_iter->second.end_devices_id()) {
              devices_iter=devices->find(*devices_in_station_ids_iter);
             if (devices_iter==devices->end()) {
                 description.str("not found device with id ");
                 description<<*devices_in_station_ids_iter;
                 throw spider_exception (description.str());
             };

          devices_types_iter=devices_types->find(devices_iter->second->get_type());
          if (devices_types_iter==devices_types->end()) {
                     description.str("Not found device type with id ");
                     description<<devices_iter->second->get_type()<<" for device with id "<<*devices_in_station_ids_iter<<" in station with id "<<stations_iter->second.get_id();
                     throw spider_exception(description.str());
                   };

             description.str("");
             description<<stations_iter->second.get_stl_name_string()
                               <<" "<<devices_types_iter->second->get_name();

               if (devices_iter->second->get_type_description()==metro_device_type::DEVICE_TYPE_ESCALATOR ||
                    devices_iter->second->get_type_description()==metro_device_type::DEVICE_TYPE_UDKU)
                    description<<" "<<devices_iter->second->get_number();


            tmp_str=description.str();
             new_list_item[0]=tmp_str.c_str();
            if ( PtListAddItems(log_rec_cont->get_station_devices_list(),
                    new_list_item,
                    1,
                    0)==-1) throw spider_exception("Error on PtListAddItems(...) ");

              if (log_rec_cont->filter.find_device(*devices_in_station_ids_iter)!=log_rec_cont->filter.devices_end())
                     PtListSelectPos(log_rec_cont->get_station_devices_list(),
                      list_index);

              devices_in_station_ids_iter++;
              list_index++;
             }; //while(devices_in_station_ids_iter!=stations_iter->second.end_devices_id())
          stations_sel_idexes_iter++;
      };//  while(stations_sel_idexes_iter!=selected_stations_indexes.end())


} catch (spider_exception spr_exc) {
     spider_sys_sett->sys_message(system_settings::ERROR_MSG, "In prepare_devices_list_in_filter_window(..) "+spr_exc.get_description());
};
};

//local functions
int
selection_on_stations_list(PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo ){
log_records_container *log_rec_cont=NULL;
system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
PtListCallback_t *list_callback_data=static_cast<PtListCallback_t*>(cbinfo->cbdata);
ushort_t *sel_array=list_callback_data->sel_array;
int sel_item_count=list_callback_data->sel_item_count;

vector<ushort_t> selected_idexes(sel_item_count);
memcpy(&selected_idexes[0], sel_array, sel_item_count*sizeof(ushort_t));

try {
PtGetResource(widget,Pt_ARG_POINTER, &log_rec_cont, 0);
if (log_rec_cont==NULL)
     throw spider_exception("log_rec_cont==NULL");

prepare_devices_list_in_filter_window (log_rec_cont, selected_idexes);

} catch (spider_exception spr_exc) {
      spider_sys_sett->sys_message(system_settings::ERROR_MSG, "In selection_on_stations_list(...) "+spr_exc.get_description());
      return Pt_HALT;
};

return Pt_CONTINUE;
};

int
activate_report_button_in_log_window( PtWidget_t *widget, 
                                                                        ApInfo_t *apinfo, 
                                                                        PtCallbackInfo_t *cbinfo ){
return Pt_CONTINUE;
};

int
activate_button_ok_in_filter_log_window( PtWidget_t *widget, 
                                                                ApInfo_t *apinfo, 
                                                                PtCallbackInfo_t *cbinfo ) {
long *internal_flags, flags;
const PtCalendarDate_t *internal_pt_time;
const int *tmp_int_value;
const unsigned short *tmp_internal_ushort_value=NULL;
unsigned short *selection_indexes , tmp_ushort_value;
vector<int> devices_ids;
PtCalendarDate_t pt_time;
tm  time_to_filtered;

system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
if (spider_sys_sett==NULL) {
  cout<<"Can`t get instance of system_settings_spider"<<endl;
  return( Pt_HALT);
};

log_records_container *log_rec_cont=NULL;
try {
   PtGetResource(widget,Pt_ARG_POINTER, &log_rec_cont, 0);

   if (log_rec_cont==NULL)
       throw spider_exception("log_rec_cont==NULL");

  if (log_rec_cont->get_toggle_button_filter_turn_off()!=NULL){
            PtGetResource(log_rec_cont->get_toggle_button_filter_turn_off(),
                                     Pt_ARG_FLAGS, &internal_flags, 0);
             flags=*internal_flags;
             log_rec_cont->filter.set_filter_state((flags&Pt_SET)!=0);
             if (log_rec_cont->get_filtration_state_indicator()!=NULL){
                  if (log_rec_cont->filter.get_filter_state()) {
                        PtSetResource(log_rec_cont->get_filtration_state_indicator(),
                                                 Pt_ARG_TEXT_STRING, "ФИЛЬТР ВКЛ",0);
                        PtSetResource(log_rec_cont->get_filtration_state_indicator(),
                                                 Pt_ARG_LABEL_IMAGE, spider_sys_sett->get_image(system_settings_spider::BLOCK_RED_LED),0);
                    } else { //if (log_rec_cont->filter.get_filter_state())
                        PtSetResource(log_rec_cont->get_filtration_state_indicator(),
                                                 Pt_ARG_TEXT_STRING, "ФИЛЬТР ВЫКЛ",0);
                        PtSetResource(log_rec_cont->get_filtration_state_indicator(),
                                                 Pt_ARG_LABEL_IMAGE, spider_sys_sett->get_image(system_settings_spider::BLOCK_GREY_LED),0);
                    }; //if (log_rec_cont->filter.get_filter_state())
             }; //if (log_rec_cont->get_filtration_state_indicator()!=NULL)
    };// if (log_rec_cont->get_toggle_button_filter_turn_off()!=NULL)

  if ( log_rec_cont->get_toggle_button_filter_all_times()!=NULL) {
           PtGetResource(log_rec_cont->get_toggle_button_filter_all_times(),
                                     Pt_ARG_FLAGS, &internal_flags, 0);
             flags=*internal_flags;
             log_rec_cont->filter.set_all_times((flags&Pt_SET)!=0);
     }; //if ( log_rec_cont->get_toggle_button_filter_all_times()!=NULL) 

   if (log_rec_cont->get_toggle_button_filter_all_messages_types()!=NULL){
            PtGetResource(log_rec_cont->get_toggle_button_filter_all_messages_types(),
                                     Pt_ARG_FLAGS, &internal_flags, 0);
             flags=*internal_flags;
             log_rec_cont->filter.set_all_msg_types((flags&Pt_SET)!=0);
   }; //if (log_rec_cont->get_toggle_button_filter_all_messages_types()!=NULL)

   if (log_rec_cont->get_toggle_button_filter_all_stations()!=NULL ) {
            PtGetResource(log_rec_cont->get_toggle_button_filter_all_stations(),
                                     Pt_ARG_FLAGS, &internal_flags, 0);
             flags=*internal_flags;
             log_rec_cont->filter.set_all_stations((flags&Pt_SET)!=0);
   }; // if (log_rec_cont->set_toggle_button_filter_all_stations()!=NULL )

   if (log_rec_cont->get_toggle_button_filter_all_devices()!=NULL) {
            PtGetResource(log_rec_cont->get_toggle_button_filter_all_devices(),
                                     Pt_ARG_FLAGS, &internal_flags, 0);
             flags=*internal_flags;
             log_rec_cont->filter.set_all_devices((flags&Pt_SET)!=0);
   }; //if (log_rec_cont->get_toggle_button_filter_all_devices()!=NULL)

   if (log_rec_cont->get_messages_types_list()!=NULL) {
       msg_types_container* msg_types=msg_types_container::get_instance();
       log_rec_cont->filter.erase_msg_type (log_rec_cont->filter.msg_types_begin(), log_rec_cont->filter.msg_types_end());
      PtGetResource(log_rec_cont->get_messages_types_list(), Pt_ARG_SELECTION_INDEXES,
                                &selection_indexes, &tmp_internal_ushort_value);
       tmp_ushort_value=*tmp_internal_ushort_value;
        if (tmp_ushort_value!=0) {
              msg_types_container::iterator msg_types_iter;
              for(vector<unsigned short>::size_type i=0; i<tmp_ushort_value; i++) {
                msg_types_iter=msg_types->begin();
                advance(msg_types_iter, selection_indexes[i]-1);
                log_rec_cont->filter.insert_msg_type(msg_types_iter->second.get_type_id());
              };//for(vector<unsigned short>::size_type i=0; i<tmp_ushort_value; i++
         }; //if (*tmp_ushort_value!=0)
   }; // if (log_rec_cont->get_messages_types_list() !=NULL)

   if (log_rec_cont->get_stations_list()!=NULL){
      metro_stations_container* stations=metro_stations_container::get_instance();
      log_rec_cont->filter.erase_station (log_rec_cont->filter.stations_begin(), log_rec_cont->filter.stations_end());
      PtGetResource(log_rec_cont->get_stations_list(), Pt_ARG_SELECTION_INDEXES,
                                &selection_indexes, &tmp_internal_ushort_value);
       tmp_ushort_value=*tmp_internal_ushort_value;
        if (tmp_ushort_value!=0) {
              metro_stations_container::iterator stations_iter;
             for(vector<unsigned short>::size_type i=0; i<tmp_ushort_value; i++) {
                stations_iter=stations->begin();
                advance(stations_iter, selection_indexes[i]-1);
                log_rec_cont->filter.insert_station(stations_iter->second.get_id());
              };//for(vector<unsigned short>::size_type i=0; i<tmp_ushort_value; i++)
         }; //if (tmp_ushort_value!=0)
    }; //if (log_rec_cont->get_stations_list()!=NULL)

   if (log_rec_cont->get_station_devices_list()!=NULL &&
        log_rec_cont->get_stations_list()!=NULL){
       metro_stations_container* stations=metro_stations_container::get_instance();
       log_rec_cont->filter.erase_device (log_rec_cont->filter.devices_begin(), log_rec_cont->filter.devices_end());
       PtGetResource(log_rec_cont->get_stations_list(), Pt_ARG_SELECTION_INDEXES,
                                &selection_indexes, &tmp_internal_ushort_value);
        tmp_ushort_value=*tmp_internal_ushort_value;
         if (tmp_ushort_value!=0) { //for stations list
              metro_stations_container::iterator stations_iter;
             for(vector<unsigned short>::size_type i=0; i<tmp_ushort_value; i++) {
                stations_iter=stations->begin();
                advance(stations_iter, selection_indexes[i]-1);
                devices_ids.insert(devices_ids.end(),
                                              stations_iter->second.begin_devices_id(), 
                                              stations_iter->second.end_devices_id());
              };//for(vector<unsigned short>::size_type i=0; i<tmp_ushort_value; i++)

           PtGetResource(log_rec_cont->get_station_devices_list(), Pt_ARG_SELECTION_INDEXES,
                                &selection_indexes, &tmp_internal_ushort_value);
           tmp_ushort_value=*tmp_internal_ushort_value;
            if (tmp_ushort_value!=0) { //for stations devices list
              for(vector<unsigned short>::size_type i=0; i<tmp_ushort_value; i++) { //for devices selection
                  log_rec_cont->filter.insert_device (devices_ids.at(selection_indexes[i]-1));
              };//for(vector<unsigned short>::size_type i=0; i<selection_indexes.size(){ //for devices selection

            };//if (tmp_ushort_value!=0) { //for stations devices list
         }; //if (tmp_ushort_value!=0) //for stations list
   }; //if (log_rec_cont->get_station_devices_list()!=NULL) &&

   if (log_rec_cont->get_calendar_begin_time_interval()!=NULL &&
        log_rec_cont->get_numeric_begin_hour()!=NULL &&
        log_rec_cont->get_numeric_begin_minute()!=NULL){
			PtGetResource(log_rec_cont->get_calendar_begin_time_interval(), 
                                      Pt_ARG_CALENDAR_DATE, &internal_pt_time,
                                       sizeof(PtCalendarDate_t));
			pt_time=*internal_pt_time;

           memset(&time_to_filtered, 0, sizeof(time_to_filtered));

            time_to_filtered.tm_isdst =daylight;
			time_to_filtered.	tm_sec=0;
			time_to_filtered.	tm_mday=pt_time.day+1;
			time_to_filtered.	tm_mon=pt_time.month;
			time_to_filtered.	tm_year=pt_time.year-1900;

            PtGetResource(log_rec_cont->get_numeric_begin_hour(), 
                                      Pt_ARG_NUMERIC_VALUE, &tmp_int_value, 0);
             time_to_filtered.tm_hour=*tmp_int_value;

			PtGetResource(log_rec_cont->get_numeric_begin_minute(),
                                       Pt_ARG_NUMERIC_VALUE, &tmp_int_value, 0);
			time_to_filtered.	tm_min=*tmp_int_value;
            log_rec_cont->filter.set_start_time(mktime(&time_to_filtered));
   }; //if (log_rec_cont->get_calendar_begin_time_interval()!=NULL) &&

   if (log_rec_cont->get_calendar_end_time_interval()!=NULL &&
        log_rec_cont->get_numeric_end_hour()!=NULL &&
        log_rec_cont->get_numeric_end_minute()!=NULL){
			PtGetResource(log_rec_cont->get_calendar_end_time_interval(), 
                                      Pt_ARG_CALENDAR_DATE, &internal_pt_time,
                                       sizeof(PtCalendarDate_t));
			pt_time=*internal_pt_time;

           memset(&time_to_filtered, 0, sizeof(time_to_filtered));

            time_to_filtered.tm_isdst =daylight;
			time_to_filtered.	tm_sec=0;
			time_to_filtered.	tm_mday=pt_time.day+1;
			time_to_filtered.	tm_mon=pt_time.month;
			time_to_filtered.	tm_year=pt_time.year-1900;

           PtGetResource(log_rec_cont->get_numeric_end_hour(), 
                                      Pt_ARG_NUMERIC_VALUE, &tmp_int_value, 0);
            time_to_filtered.tm_hour=*tmp_int_value;

			PtGetResource(log_rec_cont->get_numeric_end_minute(),
                                       Pt_ARG_NUMERIC_VALUE, &tmp_int_value, 0);
			time_to_filtered.	tm_min=*tmp_int_value;

            log_rec_cont->filter.set_stop_time(mktime(&time_to_filtered));
   }; //if (log_rec_cont->get_calendar_end_time_interval()!=NULL) &&

//check filter settings
	if (log_rec_cont->filter.get_filter_state() &&
        !log_rec_cont->filter.get_all_times() &&
		(log_rec_cont->filter.get_stop_time()<=
		log_rec_cont->filter.get_start_time())){
				spider_sys_sett->message_window(system_settings::ERROR_MSG, 
								"Конечная дата фильтрации должна быть позже, чем начальная",
                                 true);
				return Pt_HALT;
			};


	if ( log_rec_cont->filter.get_filter_state() &&
         !log_rec_cont->filter.get_all_msg_types() &&
         log_rec_cont->filter.msg_types_empty()) {
				spider_sys_sett->message_window(system_settings::ERROR_MSG,
                                               "Не выбран ни один тип сообщения",
                                                true);
				return Pt_HALT;
			};

	if ( log_rec_cont->filter.get_filter_state() &&
          !log_rec_cont->filter.get_all_stations() &&
          log_rec_cont->filter.stations_empty()) {
				spider_sys_sett->message_window(system_settings::ERROR_MSG, 
                                                 "Не выбрана ни одна станция",
                                                  true);
				return Pt_HALT;
			};

	if (log_rec_cont->filter.get_filter_state() &&
        !log_rec_cont->filter.get_all_devices() &&
		log_rec_cont->filter.devices_empty()) 	{
				spider_sys_sett->message_window(system_settings::ERROR_MSG, 
                                   "Не выбрано ни одно из устройств на станциях",
                                    true);
				return Pt_HALT;
			};

//set filtering
	log_rec_cont->set_filtering();
	log_rec_cont->prepare_to_display();

} catch (spider_exception spr_exc) {
      spider_sys_sett->sys_message(system_settings::ERROR_MSG, "In activate_button_ok_in_filter_log_window(...) "+spr_exc.get_description());
      return Pt_HALT;
};
      return Pt_CONTINUE;
};

int
activate_button_chancel_in_filter_log_window( PtWidget_t *widget, 
                                                                ApInfo_t *apinfo, 
                                                                PtCallbackInfo_t *cbinfo ) {
system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
if (spider_sys_sett==NULL) {
  cout<<"Can`t get instance of system_settings_spider"<<endl;
  return( Pt_HALT);
};

log_records_container *log_rec_cont=NULL;
try {
PtGetResource(widget,Pt_ARG_POINTER, &log_rec_cont, 0);

   if (log_rec_cont==NULL)
       throw spider_exception("log_rec_cont==NULL");

   log_rec_cont->set_toggle_button_filter_turn_off(NULL);
   log_rec_cont->set_toggle_button_filter_all_times(NULL);
   log_rec_cont->set_toggle_button_filter_all_messages_types(NULL);
   log_rec_cont->set_toggle_button_filter_all_stations(NULL);
   log_rec_cont->set_toggle_button_filter_all_devices(NULL);
   log_rec_cont->set_messages_types_list(NULL);
   log_rec_cont->set_stations_list(NULL);
   log_rec_cont->set_station_devices_list(NULL);
   log_rec_cont->set_calendar_begin_time_interval(NULL);
   log_rec_cont->set_calendar_end_time_interval(NULL);
   log_rec_cont->set_numeric_begin_hour(NULL);
   log_rec_cont->set_numeric_end_hour(NULL);
   log_rec_cont->set_numeric_begin_minute(NULL);
   log_rec_cont->set_numeric_end_minute(NULL);
} catch (spider_exception spr_exc) {
      spider_sys_sett->sys_message(system_settings::ERROR_MSG, "In activate_button_chancel_in_filter_log_window(...) "+spr_exc.get_description());
      return Pt_HALT;
};

return Pt_CONTINUE;
};

int 
activate_toggle_button_all_devices_in_filter_log_window( PtWidget_t *widget, 
                                                                ApInfo_t *apinfo, 
                                                                PtCallbackInfo_t *cbinfo ) {
vector<PtArg_t> args;
long *internal_flags, flags;

system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
if (spider_sys_sett==NULL) {
  cout<<"Can`t get instance of system_settings_spider"<<endl;
  return( Pt_HALT);
};

log_records_container *log_rec_cont=NULL;

try {
args.clear();
args.resize(2);
PtSetArg(&args[0], Pt_ARG_POINTER, &log_rec_cont, 0);
PtSetArg(&args[1], Pt_ARG_FLAGS, &internal_flags, 0);
PtGetResources(widget, args.size(), &args[0]);
flags=*internal_flags;

if (log_rec_cont==NULL)
       throw spider_exception("log_rec_cont==NULL");

args.clear();
args.resize(2);
if ((flags&Pt_SET) != 0) {
  PtSetArg(&args[0], Pt_ARG_FLAGS, Pt_TRUE, Pt_GHOST);
  PtSetArg(&args[1], Pt_ARG_FLAGS, Pt_TRUE, Pt_BLOCKED);
 } else {
  PtSetArg(&args[0], Pt_ARG_FLAGS, Pt_FALSE, Pt_GHOST);
  PtSetArg(&args[1], Pt_ARG_FLAGS, Pt_FALSE, Pt_BLOCKED);
 };

if (log_rec_cont->get_station_devices_list()!=NULL)
      PtSetResources(log_rec_cont->get_station_devices_list(), args.size(), &args[0]);
 } catch (spider_exception spr_exc) {
    spider_sys_sett->sys_message(system_settings::ERROR_MSG, "In activate_toggle_button_all_devices_in_filter_log_window(..) "+spr_exc.get_description ());
    return Pt_HALT;
 };

return Pt_CONTINUE;
};



int 
activate_toggle_button_all_stations_in_filter_log_window( PtWidget_t *widget, 
                                                                ApInfo_t *apinfo, 
                                                                PtCallbackInfo_t *cbinfo ) {
vector<PtArg_t> args;
long *internal_flags, flags;

system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
if (spider_sys_sett==NULL) {
  cout<<"Can`t get instance of system_settings_spider"<<endl;
  return( Pt_HALT);
};

log_records_container *log_rec_cont=NULL;

try {
args.clear();
args.resize(2);
PtSetArg(&args[0], Pt_ARG_POINTER, &log_rec_cont, 0);
PtSetArg(&args[1], Pt_ARG_FLAGS, &internal_flags, 0);
PtGetResources(widget, args.size(), &args[0]);
flags=*internal_flags;
if (log_rec_cont==NULL)
       throw spider_exception("log_rec_cont==NULL");

args.clear();
args.resize(2);
if ((flags&Pt_SET) != 0) {
  PtSetArg(&args[0], Pt_ARG_FLAGS, Pt_TRUE, Pt_GHOST);
  PtSetArg(&args[1], Pt_ARG_FLAGS, Pt_TRUE, Pt_BLOCKED);
 } else {
  PtSetArg(&args[0], Pt_ARG_FLAGS, Pt_FALSE, Pt_GHOST);
  PtSetArg(&args[1], Pt_ARG_FLAGS, Pt_FALSE, Pt_BLOCKED);
 };

if (log_rec_cont->get_stations_list()!=NULL)
      PtSetResources(log_rec_cont->get_stations_list(), args.size(), &args[0]);

if (log_rec_cont->get_toggle_button_filter_all_devices()!=NULL)
      PtSetResources(log_rec_cont->get_toggle_button_filter_all_devices(), args.size(), &args[0]);

  if (log_rec_cont->get_station_devices_list()!=NULL &&
       log_rec_cont->get_toggle_button_filter_all_devices()!=NULL) {
       PtGetResource(log_rec_cont->get_toggle_button_filter_all_devices(),
                                 Pt_ARG_FLAGS, &internal_flags, 0);
       flags=*internal_flags;
       if ((flags&Pt_SET) != 0) {
          PtSetArg(&args[0], Pt_ARG_FLAGS, Pt_TRUE, Pt_GHOST);
          PtSetArg(&args[1], Pt_ARG_FLAGS, Pt_TRUE, Pt_BLOCKED);
        }; //if ((flags&Pt_SET) != 0)
      PtSetResources(log_rec_cont->get_station_devices_list(), args.size(), &args[0]);
    }; //  if (log_rec_cont->get_station_devices_list()!=NULL)

 } catch (spider_exception spr_exc) {
    spider_sys_sett->sys_message(system_settings::ERROR_MSG, "In activate_toggle_button_all_stations_in_filter_log_window(..) "+spr_exc.get_description ());
    return Pt_HALT;
 };

return Pt_CONTINUE;
};


int
activate_toggle_button_all_msg_types_in_filter_log_window( PtWidget_t *widget, 
                                                                ApInfo_t *apinfo, 
                                                                PtCallbackInfo_t *cbinfo ) {
vector<PtArg_t> args;
long *internal_flags, flags;

system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
if (spider_sys_sett==NULL) {
  cout<<"Can`t get instance of system_settings_spider"<<endl;
  return( Pt_HALT);
};

log_records_container *log_rec_cont=NULL;

try {
args.clear();
args.resize(2);
PtSetArg(&args[0], Pt_ARG_POINTER, &log_rec_cont, 0);
PtSetArg(&args[1], Pt_ARG_FLAGS, &internal_flags, 0);
PtGetResources(widget, args.size(), &args[0]);
flags=*internal_flags;

if (log_rec_cont==NULL)
       throw spider_exception("log_rec_cont==NULL");

args.clear();
args.resize(2);
if ((flags&Pt_SET) != 0) {
  PtSetArg(&args[0], Pt_ARG_FLAGS, Pt_TRUE, Pt_GHOST);
  PtSetArg(&args[1], Pt_ARG_FLAGS, Pt_TRUE, Pt_BLOCKED);
 } else {
  PtSetArg(&args[0], Pt_ARG_FLAGS, Pt_FALSE, Pt_GHOST);
  PtSetArg(&args[1], Pt_ARG_FLAGS, Pt_FALSE, Pt_BLOCKED);
 };

if (log_rec_cont->get_messages_types_list()!=NULL)
      PtSetResources(log_rec_cont->get_messages_types_list(), args.size(), &args[0]);

 } catch (spider_exception spr_exc) {
    spider_sys_sett->sys_message(system_settings::ERROR_MSG, "In activate_toggle_button_all_msg_types_in_filter_log_window(..) "+spr_exc.get_description ());
    return Pt_HALT;
 };

return Pt_CONTINUE;
};


int
activate_toggle_button_all_times_in_filter_log_window( PtWidget_t *widget, 
                                                                ApInfo_t *apinfo, 
                                                                PtCallbackInfo_t *cbinfo ) {
vector<PtArg_t> args;
long *internal_flags, flags;

system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
if (spider_sys_sett==NULL) {
  cout<<"Can`t get instance of system_settings_spider"<<endl;
  return( Pt_HALT);
};

log_records_container *log_rec_cont=NULL;

try {
args.clear();
args.resize(2);
PtSetArg(&args[0], Pt_ARG_POINTER, &log_rec_cont, 0);
PtSetArg(&args[1], Pt_ARG_FLAGS, &internal_flags, 0);
PtGetResources(widget, args.size(), &args[0]);
flags=*internal_flags;

if (log_rec_cont==NULL)
       throw spider_exception("log_rec_cont==NULL");

args.clear();
args.resize(2);
if ((flags&Pt_SET) != 0) {
  PtSetArg(&args[0], Pt_ARG_FLAGS, Pt_TRUE, Pt_GHOST);
  PtSetArg(&args[1], Pt_ARG_FLAGS, Pt_TRUE, Pt_BLOCKED);
 } else {
  PtSetArg(&args[0], Pt_ARG_FLAGS, Pt_FALSE, Pt_GHOST);
  PtSetArg(&args[1], Pt_ARG_FLAGS, Pt_FALSE, Pt_BLOCKED);
 };

if (log_rec_cont->get_calendar_begin_time_interval()!=NULL)
      PtSetResources(log_rec_cont->get_calendar_begin_time_interval(), args.size(), &args[0]);
if (log_rec_cont->get_calendar_end_time_interval()!=NULL)
      PtSetResources(log_rec_cont->get_calendar_end_time_interval(), args.size(), &args[0]);
if (log_rec_cont->get_numeric_begin_hour()!=NULL)
      PtSetResources(log_rec_cont->get_numeric_begin_hour(), args.size(), &args[0]);
if (log_rec_cont->get_numeric_end_hour()!=NULL)
      PtSetResources(log_rec_cont->get_numeric_end_hour(), args.size(), &args[0]);
if (log_rec_cont->get_numeric_begin_minute()!=NULL)
      PtSetResources(log_rec_cont->get_numeric_begin_minute(), args.size(), &args[0]);
if (log_rec_cont->get_numeric_end_minute()!=NULL)
      PtSetResources(log_rec_cont->get_numeric_end_minute(), args.size(), &args[0]);

 } catch (spider_exception spr_exc) {
    spider_sys_sett->sys_message(system_settings::ERROR_MSG, "In activate_toggle_button_all_times_in_filter_log_window(..) "+spr_exc.get_description ());
    return Pt_HALT;
 };

return Pt_CONTINUE;
};

int
activate_filter_button_in_log_window( PtWidget_t *widget, 
                                                                ApInfo_t *apinfo, 
                                                                PtCallbackInfo_t *cbinfo ){
PtWidget_t *dialog_window,*parent_panel,*current_panel, *panel_group, *current_button;
PtWidget_t *toggle_button_filter_turn_off=NULL, *toggle_button_filter_all_times=NULL,
                   *toggle_button_filter_all_messages_types=NULL, *toggle_button_filter_all_stations=NULL, 
                   *toggle_button_filter_all_devices=NULL,
                    *calendar_begin_time_interval=NULL, *calendar_end_time_interval=NULL, 
                    *numeric_begin_hour=NULL, *numeric_end_hour=NULL, *numeric_begin_minute=NULL, *numeric_end_minute=NULL,
                    *messages_types_list=NULL, *stations_list=NULL, *station_devices_list=NULL;

vector<PtCallback_t> callbacks;
PtCallback_t tmp_callback;
vector<PtArg_t> args;
string dialog_title("Фильтр сообщений");
int inside_panel_margin=65, inside_panel_height=40;
int *internal_panel_height, panel_height;

tm *local_time;
time_t local_time_t;

bool disable_all_elemets_in_panel=false;

char *w_days[]={
    "Вс",
    "Пн",
    "Вт",
    "Ср",
    "Чт",
    "Пт",
    "Сб"
};

char *y_months[]={
    "Январь",
    "Февраль",
    "Март",
    "Апрель",
    "Май",
    "Июнь",
    "Июль",
    "Август",
    "Сентябрь",
    "Октябрь",
    "Ноябрь",
    "Декабрь"
 };


system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
if (spider_sys_sett==NULL) {
  cout<<"Can`t get instance of system_settings_spider"<<endl;
  return( Pt_HALT);
};

unsigned int dialog_wnd_height=300,
                      dialog_wnd_width=450,
                      buttons_panel_height=40,
                      button_in_panel_width=80;
PhPoint_t widget_position;
log_records_container *log_rec_cont=NULL;

try {
PtGetResource(widget, Pt_ARG_POINTER, &log_rec_cont, 0);
if (log_rec_cont==NULL)
       throw spider_exception("log_rec_cont==NULL");

args.clear();
args.resize(12);
PtSetArg(&args[0], Pt_ARG_HEIGHT, dialog_wnd_height,0);
PtSetArg(&args[1], Pt_ARG_WIDTH, dialog_wnd_width,0);
PtSetArg(&args[2], Pt_ARG_WINDOW_TITLE, dialog_title.c_str(),0);
PtSetArg(&args[3], Pt_ARG_WINDOW_RENDER_FLAGS,
                 Pt_FALSE, Ph_WM_RENDER_RESIZE);
PtSetArg(&args[4], Pt_ARG_WINDOW_RENDER_FLAGS,
                 Pt_FALSE, Ph_WM_TOBACK);
PtSetArg(&args[5], Pt_ARG_WINDOW_RENDER_FLAGS,
                 Pt_FALSE, Ph_WM_RENDER_MAX);
PtSetArg(&args[6], Pt_ARG_WINDOW_RENDER_FLAGS,
                  Pt_FALSE, Ph_WM_RENDER_MENU);
PtSetArg(&args[7], Pt_ARG_WINDOW_RENDER_FLAGS,
                  Pt_FALSE, Ph_WM_RENDER_COLLAPSE );
PtSetArg(&args[8], Pt_ARG_WINDOW_MANAGED_FLAGS,
                  Pt_FALSE, Ph_WM_MAX );
PtSetArg(&args[9], Pt_ARG_WINDOW_MANAGED_FLAGS,
                  Pt_FALSE, Ph_WM_MENU  );
PtSetArg(&args[10], Pt_ARG_WINDOW_MANAGED_FLAGS,
                  Pt_FALSE, Ph_WM_CLOSE  );
PtSetArg(&args[11], Pt_ARG_WINDOW_MANAGED_FLAGS,
                  Pt_FALSE, Ph_WM_COLLAPSE );

dialog_window=PtCreateWidget(PtWindow,
                                                    spider_sys_sett->get_main_window(),
                                                    args.size(),
                                                    &args[0]);
if (dialog_window==NULL)
             throw spider_exception("dialog_wnd is NULL");

//buttons panel
   args.clear();
   args.resize(3);
   PtSetArg(&args[0], Pt_ARG_HEIGHT, buttons_panel_height,0);
   PtSetArg(&args[1], Pt_ARG_WIDTH, dialog_wnd_width,0);
   widget_position.x=0;
   widget_position.y=dialog_wnd_height-buttons_panel_height;
   PtSetArg(&args[2], Pt_ARG_POS,
                     &widget_position, 0);
   current_panel=PtCreateWidget(PtPane,
                                                  dialog_window,
                                                  args.size(),
                                                  &args[0]);
   if (current_panel==NULL)
            throw spider_exception("buttons_panel is NULL");

         //buttons in button panel
		args.clear();
		args.resize(6);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, buttons_panel_height-8,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, button_in_panel_width,0);
         widget_position.x=dialog_wnd_width-2*button_in_panel_width-20;
         widget_position.y=2;
         PtSetArg(&args[2], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[3], Pt_ARG_TEXT_STRING,
                       "OK", 0);
         PtSetArg(&args[4], Pt_ARG_POINTER,
                        log_rec_cont, 0);

		callbacks.clear();
		tmp_callback.event_f=activate_button_ok_in_filter_log_window;
		callbacks.push_back(tmp_callback);
        tmp_callback.event_f=close_window;
		callbacks.push_back(tmp_callback);
        PtSetArg(&args[5], Pt_CB_ACTIVATE,
                        &callbacks[0],
                        callbacks.size());
         current_button=PtCreateWidget(PtButton,
                                                     current_panel,
                                                     args.size(),
                                                     &args[0]);
         if (current_button==NULL)
                  throw spider_exception("button \"OK\" in buttons_panel is NULL");

		args.clear();
		args.resize(6);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, buttons_panel_height-8,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, button_in_panel_width,0);
         widget_position.x=dialog_wnd_width-button_in_panel_width-10;
         widget_position.y=2;
         PtSetArg(&args[2], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[3], Pt_ARG_TEXT_STRING,
                       "Отмена", 0);
         PtSetArg(&args[4], Pt_ARG_POINTER,
                        log_rec_cont, 0);

		callbacks.clear();
		tmp_callback.event_f=activate_button_chancel_in_filter_log_window;
		callbacks.push_back(tmp_callback);
        tmp_callback.event_f=close_window;
		callbacks.push_back(tmp_callback);
        PtSetArg(&args[5], Pt_CB_ACTIVATE,
                        &callbacks[0],
                        callbacks.size());
         current_button=PtCreateWidget(PtButton,
                                                     current_panel,
                                                     args.size(),
                                                     &args[0]);
         if (current_button==NULL)
                  throw spider_exception("button \"Chancel\" in buttons_panel is NULL");

		args.clear();
		args.resize(4);
         widget_position.x=2;
         widget_position.y=4;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_TEXT_STRING,
                       "Включить фильтрацию сообщений", 0);
         if (log_rec_cont->filter.get_filter_state()) {
  			   PtSetArg(&args[2], Pt_ARG_FLAGS, Pt_TRUE, Pt_SET); 
            } else {
  			   PtSetArg(&args[2], Pt_ARG_FLAGS, Pt_FALSE, Pt_SET); 
            };
         PtSetArg(&args[3], Pt_ARG_COLOR,
                       system_settings_spider::COLOR_RED, 0);
         toggle_button_filter_turn_off=PtCreateWidget(PtToggleButton,
                                                     current_panel,
                                                     args.size(),
                                                     &args[0]);
         if (toggle_button_filter_turn_off==NULL)
                  throw spider_exception("toggle_button_filter_turn_off in buttons_panel is NULL");

		args.clear();
		args.resize(7);
         widget_position.x=0;
         widget_position.y=0;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_HEIGHT,
                           dialog_wnd_height-buttons_panel_height  , 0);
         PtSetArg(&args[2], Pt_ARG_WIDTH,
                           dialog_wnd_width, 0);
         PtSetArg(&args[3], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_LEFT_ANCHORED_LEFT);
         PtSetArg(&args[4], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_RIGHT_ANCHORED_RIGHT);
         PtSetArg(&args[5], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_TOP_ANCHORED_TOP);
         PtSetArg(&args[6], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_BOTTOM_ANCHORED_BOTTOM);

          panel_group=PtCreateWidget(PtPanelGroup,
                                                  dialog_window,
                                                  args.size(),
                                                  &args[0]);
   if (panel_group==NULL)
            throw spider_exception("PanelGroup is NULL");

//BEGIN: time interval panel
		args.clear();
		args.resize(7);
         widget_position.x=0;
         widget_position.y=0;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_LEFT_ANCHORED_LEFT);
         PtSetArg(&args[2], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_RIGHT_ANCHORED_RIGHT);
         PtSetArg(&args[3], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_TOP_ANCHORED_TOP);
         PtSetArg(&args[4], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_BOTTOM_ANCHORED_BOTTOM);
         PtSetArg(&args[5], Pt_ARG_TITLE,
                             "Временной диапазон", 0);
         PtSetArg(&args[6], Pt_ARG_FLAGS,
                             Pt_FALSE, Pt_HIGHLIGHTED);

           current_panel=PtCreateWidget(PtPane,
                                                  panel_group,
                                                  args.size(),
                                                  &args[0]);
        if (current_panel==NULL)
            throw spider_exception("time interval panel is NULL");

		 args.clear();
		 args.resize(5);
         widget_position.x=2;
         widget_position.y=4;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_TEXT_STRING,
                       "Весь временной диапазон", 0);
         if (log_rec_cont->filter.get_all_times()) {
  			   PtSetArg(&args[2], Pt_ARG_FLAGS, Pt_TRUE, Pt_SET);
            } else {
  			   PtSetArg(&args[2], Pt_ARG_FLAGS, Pt_FALSE, Pt_SET); 
            };
         PtSetArg(&args[3], Pt_ARG_POINTER,
                       log_rec_cont, 0);

		callbacks.clear();
		tmp_callback.event_f=activate_toggle_button_all_times_in_filter_log_window;
		callbacks.push_back(tmp_callback);
        PtSetArg(&args[4], Pt_CB_ACTIVATE,
                        &callbacks[0],
                        callbacks.size());

       disable_all_elemets_in_panel=log_rec_cont->filter.get_all_times();

         toggle_button_filter_all_times=PtCreateWidget(PtToggleButton,
                                                     current_panel,
                                                     args.size(),
                                                     &args[0]);
         if (toggle_button_filter_all_times==NULL)
                  throw spider_exception("toggle_button_filter_all_times in time interval panel is NULL");

        parent_panel=current_panel;

		args.clear();
		args.resize(7);
         widget_position.x=0;
         widget_position.y=25;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_WIDTH,
                          static_cast<int>(dialog_wnd_width/2-14),0);
         PtSetArg(&args[2], Pt_ARG_HEIGHT,
                         dialog_wnd_height-buttons_panel_height-inside_panel_margin, 0);
         PtSetArg(&args[3], Pt_ARG_TITLE,
                             "Начальная дата", 0);
         PtSetArg(&args[4], Pt_ARG_CONTAINER_FLAGS,
                            Pt_TRUE, Pt_SHOW_TITLE);
         PtSetArg(&args[5], Pt_ARG_CONTAINER_FLAGS,
                            Pt_TRUE, Pt_GRADIENT_TITLE_AREA);
         PtSetArg(&args[6], Pt_ARG_CONTAINER_FLAGS,
                            Pt_TRUE, Pt_ETCH_TITLE_AREA);

           current_panel=PtCreateWidget(PtPane,
                                                  parent_panel,
                                                  args.size(),
                                                  &args[0]);
        if (current_panel==NULL)
            throw spider_exception("begin of time interval panel is NULL");

        PtGetResource(current_panel, Pt_ARG_HEIGHT, &internal_panel_height, 0);
        panel_height=*internal_panel_height;

		args.clear();
		args.resize(12);
         widget_position.x=0;
         widget_position.y=0;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_WIDTH,
                          static_cast<int>(dialog_wnd_width/2-20),0);
         PtSetArg(&args[2], Pt_ARG_HEIGHT,
                         panel_height-inside_panel_height-22, 0);
         PtSetArg(&args[3], Pt_ARG_CALENDAR_YEAR_BTN_COLOR,
                         0XFF8000, 0);
         PtSetArg(&args[4], Pt_ARG_CALENDAR_MONTH_BTN_COLOR,
                         0XFF8000, 0);
         PtSetArg(&args[5], Pt_ARG_CALENDAR_SEL_COLOR,
                         0X00A000, 0);
         PtSetArg(&args[6], Pt_ARG_FILL_COLOR,
                         0XFFFFBD, 0);
         PtSetArg(&args[7], Pt_ARG_CALENDAR_WDAY_NAMES,
                         w_days, 7);
         PtSetArg(&args[8], Pt_ARG_CALENDAR_MONTH_NAMES,
                         y_months, 12);
         PtSetArg(&args[9], Pt_ARG_CALENDAR_TIME_T,
                         log_rec_cont->filter.get_start_time(), 0);
         if (disable_all_elemets_in_panel) {
                 PtSetArg(&args[10], Pt_ARG_FLAGS,
                            Pt_TRUE, Pt_GHOST);
                 PtSetArg(&args[11], Pt_ARG_FLAGS,
                            Pt_TRUE, Pt_BLOCKED);
                }else{
                  PtSetArg(&args[10], Pt_ARG_FLAGS,
                            Pt_FALSE, Pt_GHOST);
                  PtSetArg(&args[11], Pt_ARG_FLAGS,
                            Pt_FALSE, Pt_BLOCKED);
                 };

         calendar_begin_time_interval=PtCreateWidget(PtCalendar,
                                                  current_panel,
                                                  args.size(),
                                                  &args[0]);
           if (calendar_begin_time_interval==NULL)
               throw spider_exception("begin of time interval calendar is NULL");

		args.clear();
		args.resize(7);
         widget_position.x=0;
         widget_position.y=panel_height-inside_panel_height-24;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_LEFT_ANCHORED_LEFT);
         PtSetArg(&args[2], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_RIGHT_ANCHORED_RIGHT);
         PtSetArg(&args[3], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_TOP_ANCHORED_BOTTOM);
         PtSetArg(&args[4], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_BOTTOM_ANCHORED_BOTTOM);
         PtSetArg(&args[5], Pt_ARG_HEIGHT,
                             inside_panel_height, 0);
         PtSetArg(&args[6], Pt_ARG_WIDTH,
                          static_cast<int>(dialog_wnd_width/2-20), 0);

           current_panel=PtCreateWidget(PtPane,
                                                  current_panel,
                                                  args.size(),
                                                  &args[0]);
        if (current_panel==NULL)
            throw spider_exception("Inside panel for begin hour and minute is NULL");

         args.clear();
         args.resize(2);
         widget_position.x=2;
         widget_position.y=8;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_TEXT_STRING,
                             "Время", 0);

           current_button=PtCreateWidget(PtLabel, //no, that not button, $-)
                                                  current_panel,
                                                  args.size(),
                                                  &args[0]);
        if (current_button==NULL)
            throw spider_exception("Inside label for begin hour and minute is NULL");

       local_time_t=log_rec_cont->filter.get_start_time();
       local_time=localtime(&local_time_t);

		args.clear();
		args.resize(10);
         widget_position.x=45;
         widget_position.y=5;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_NUMERIC_UPDOWN_WIDTH,
                          12, 0);
         PtSetArg(&args[2], Pt_ARG_NUMERIC_MIN,
                          0, 0);
         PtSetArg(&args[3], Pt_ARG_NUMERIC_MAX,
                          23, 0);
         PtSetArg(&args[4], Pt_ARG_NUMERIC_SUFFIX,
                          " ч ", 0);
         PtSetArg(&args[5], Pt_ARG_WIDTH,
                     50, 0);
         PtSetArg(&args[6], Pt_ARG_HEIGHT,
                     inside_panel_height-4, 0);
         PtSetArg(&args[7], Pt_ARG_NUMERIC_VALUE,
                     local_time->tm_hour, 0);
         if (disable_all_elemets_in_panel) {
                 PtSetArg(&args[8], Pt_ARG_FLAGS,
                            Pt_TRUE, Pt_GHOST);
                 PtSetArg(&args[9], Pt_ARG_FLAGS,
                            Pt_TRUE, Pt_BLOCKED);
                }else{
                  PtSetArg(&args[8], Pt_ARG_FLAGS,
                            Pt_FALSE, Pt_GHOST);
                  PtSetArg(&args[9], Pt_ARG_FLAGS,
                            Pt_FALSE, Pt_BLOCKED);
                 };

          numeric_begin_hour=PtCreateWidget(PtNumericInteger,
                                                  current_panel,
                                                  args.size(),
                                                  &args[0]);
        if (numeric_begin_hour==NULL)
            throw spider_exception("Numeric for begin hour is NULL");

		args.clear();
		args.resize(2);
         widget_position.x=102;
         widget_position.y=8;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_TEXT_STRING,
                             ":", 0);

           current_button=PtCreateWidget(PtLabel, //no, that not button, $-)
                                                  current_panel,
                                                  args.size(),
                                                  &args[0]);
        if (current_button==NULL)
            throw spider_exception("Inside label for begin hour and minute is NULL");

		args.clear();
		args.resize(10);
         widget_position.x=110;
         widget_position.y=5;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_NUMERIC_UPDOWN_WIDTH,
                          12, 0);
         PtSetArg(&args[2], Pt_ARG_NUMERIC_MIN,
                          0, 0);
         PtSetArg(&args[3], Pt_ARG_NUMERIC_MAX,
                          59, 0);
         PtSetArg(&args[4], Pt_ARG_NUMERIC_SUFFIX,
                          " мин    ", 0);
         PtSetArg(&args[5], Pt_ARG_WIDTH,
                     60, 0);
         PtSetArg(&args[6], Pt_ARG_HEIGHT,
                     inside_panel_height-4, 0);
         PtSetArg(&args[7], Pt_ARG_NUMERIC_VALUE,
                     local_time->tm_min, 0);
         if (disable_all_elemets_in_panel) {
                 PtSetArg(&args[8], Pt_ARG_FLAGS,
                            Pt_TRUE, Pt_GHOST);
                 PtSetArg(&args[9], Pt_ARG_FLAGS,
                            Pt_TRUE, Pt_BLOCKED);
                }else{
                  PtSetArg(&args[8], Pt_ARG_FLAGS,
                            Pt_FALSE, Pt_GHOST);
                  PtSetArg(&args[9], Pt_ARG_FLAGS,
                            Pt_FALSE, Pt_BLOCKED);
                 };

          numeric_begin_minute=PtCreateWidget(PtNumericInteger,
                                                  current_panel,
                                                  args.size(),
                                                  &args[0]);
        if (numeric_begin_minute==NULL)
            throw spider_exception("Numeric for begin minute is NULL");

		args.clear();
		args.resize(7);
         widget_position.x=static_cast<int>(dialog_wnd_width/2);
         widget_position.y=25;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_WIDTH,
                          static_cast<int>(dialog_wnd_width/2-14),0);
         PtSetArg(&args[2], Pt_ARG_HEIGHT,
                         dialog_wnd_height-buttons_panel_height-inside_panel_margin, 0);
         PtSetArg(&args[3], Pt_ARG_TITLE,
                             "Конечная дата", 0);
         PtSetArg(&args[4], Pt_ARG_CONTAINER_FLAGS,
                            Pt_TRUE, Pt_SHOW_TITLE);
         PtSetArg(&args[5], Pt_ARG_CONTAINER_FLAGS,
                            Pt_TRUE, Pt_GRADIENT_TITLE_AREA);
         PtSetArg(&args[6], Pt_ARG_CONTAINER_FLAGS,
                            Pt_TRUE, Pt_ETCH_TITLE_AREA);

           current_panel=PtCreateWidget(PtPane,
                                                  parent_panel,
                                                  args.size(),
                                                  &args[0]);
        if (current_panel==NULL)
            throw spider_exception("end of time interval panel is NULL");

        PtGetResource(current_panel, Pt_ARG_HEIGHT, &internal_panel_height, 0);
        panel_height=*internal_panel_height;

		args.clear();
		args.resize(12);
         widget_position.x=0;
         widget_position.y=0;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_WIDTH,
                          static_cast<int>(dialog_wnd_width/2-20),0);
         PtSetArg(&args[2], Pt_ARG_HEIGHT,
                         panel_height-inside_panel_height-22, 0);
         PtSetArg(&args[3], Pt_ARG_CALENDAR_YEAR_BTN_COLOR,
                         0XFF8000, 0);
         PtSetArg(&args[4], Pt_ARG_CALENDAR_MONTH_BTN_COLOR,
                         0XFF8000, 0);
         PtSetArg(&args[5], Pt_ARG_CALENDAR_SEL_COLOR,
                         0X00A000, 0);
         PtSetArg(&args[6], Pt_ARG_FILL_COLOR,
                         0XFFFFBD, 0);
         PtSetArg(&args[7], Pt_ARG_CALENDAR_WDAY_NAMES,
                         w_days, 7);
         PtSetArg(&args[8], Pt_ARG_CALENDAR_MONTH_NAMES,
                         y_months, 12);
         PtSetArg(&args[9], Pt_ARG_CALENDAR_TIME_T,
                         log_rec_cont->filter.get_stop_time(), 0);
         if (disable_all_elemets_in_panel) {
                 PtSetArg(&args[10], Pt_ARG_FLAGS,
                            Pt_TRUE, Pt_GHOST);
                 PtSetArg(&args[11], Pt_ARG_FLAGS,
                            Pt_TRUE, Pt_BLOCKED);
                }else{
                  PtSetArg(&args[10], Pt_ARG_FLAGS,
                            Pt_FALSE, Pt_GHOST);
                  PtSetArg(&args[11], Pt_ARG_FLAGS,
                            Pt_FALSE, Pt_BLOCKED);
                 };

         calendar_end_time_interval=PtCreateWidget(PtCalendar,
                                                  current_panel,
                                                  args.size(),
                                                  &args[0]);
           if (calendar_end_time_interval==NULL)
               throw spider_exception("end of time interval calendar is NULL");

		args.clear();
		args.resize(7);
         widget_position.x=0;
         widget_position.y=panel_height-inside_panel_height-24;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_LEFT_ANCHORED_LEFT);
         PtSetArg(&args[2], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_RIGHT_ANCHORED_RIGHT);
         PtSetArg(&args[3], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_TOP_ANCHORED_BOTTOM);
         PtSetArg(&args[4], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_BOTTOM_ANCHORED_BOTTOM);
         PtSetArg(&args[5], Pt_ARG_HEIGHT,
                             inside_panel_height, 0);
         PtSetArg(&args[6], Pt_ARG_WIDTH,
                          static_cast<int>(dialog_wnd_width/2-20), 0);

           current_panel=PtCreateWidget(PtPane,
                                                  current_panel,
                                                  args.size(),
                                                  &args[0]);
        if (current_panel==NULL)
            throw spider_exception("Inside panel for end hour and minute is NULL");

		args.clear();
		args.resize(2);
         widget_position.x=2;
         widget_position.y=8;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_TEXT_STRING,
                             "Время", 0);

           current_button=PtCreateWidget(PtLabel, //no, that not button, $-)
                                                  current_panel,
                                                  args.size(),
                                                  &args[0]);
        if (current_button==NULL)
            throw spider_exception("Inside label for end hour and minute is NULL");

       local_time_t=log_rec_cont->filter.get_stop_time();
       local_time=localtime(&local_time_t);

		args.clear();
		args.resize(10);
         widget_position.x=45;
         widget_position.y=5;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_NUMERIC_UPDOWN_WIDTH,
                          12, 0);
         PtSetArg(&args[2], Pt_ARG_NUMERIC_MIN,
                          0, 0);
         PtSetArg(&args[3], Pt_ARG_NUMERIC_MAX,
                          23, 0);
         PtSetArg(&args[4], Pt_ARG_NUMERIC_SUFFIX,
                          " ч ", 0);
         PtSetArg(&args[5], Pt_ARG_WIDTH,
                     50, 0);
         PtSetArg(&args[6], Pt_ARG_HEIGHT,
                     inside_panel_height-4, 0);
         PtSetArg(&args[7], Pt_ARG_NUMERIC_VALUE,
                     local_time->tm_hour, 0);
         if (disable_all_elemets_in_panel) {
                 PtSetArg(&args[8], Pt_ARG_FLAGS,
                            Pt_TRUE, Pt_GHOST);
                 PtSetArg(&args[9], Pt_ARG_FLAGS,
                            Pt_TRUE, Pt_BLOCKED);
                }else{
                  PtSetArg(&args[8], Pt_ARG_FLAGS,
                            Pt_FALSE, Pt_GHOST);
                  PtSetArg(&args[9], Pt_ARG_FLAGS,
                            Pt_FALSE, Pt_BLOCKED);
                 };

          numeric_end_hour=PtCreateWidget(PtNumericInteger,
                                                  current_panel,
                                                  args.size(),
                                                  &args[0]);
        if (numeric_begin_hour==NULL)
            throw spider_exception("Numeric for end hour is NULL");

		args.clear();
		args.resize(2);
         widget_position.x=102;
         widget_position.y=8;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_TEXT_STRING,
                             ":", 0);

           current_button=PtCreateWidget(PtLabel, //no, that not button, $-)
                                                  current_panel,
                                                  args.size(),
                                                  &args[0]);
        if (current_button==NULL)
            throw spider_exception("Inside label for end hour and minute is NULL");

		args.clear();
		args.resize(10);
         widget_position.x=110;
         widget_position.y=5;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_NUMERIC_UPDOWN_WIDTH,
                          12, 0);
         PtSetArg(&args[2], Pt_ARG_NUMERIC_MIN,
                          0, 0);
         PtSetArg(&args[3], Pt_ARG_NUMERIC_MAX,
                          59, 0);
         PtSetArg(&args[4], Pt_ARG_NUMERIC_SUFFIX,
                          " мин    ", 0);
         PtSetArg(&args[5], Pt_ARG_WIDTH,
                     60, 0);
         PtSetArg(&args[6], Pt_ARG_HEIGHT,
                     inside_panel_height-4, 0);
         PtSetArg(&args[7], Pt_ARG_NUMERIC_VALUE,
                     local_time->tm_min, 0);
         if (disable_all_elemets_in_panel) {
                 PtSetArg(&args[8], Pt_ARG_FLAGS,
                            Pt_TRUE, Pt_GHOST);
                 PtSetArg(&args[9], Pt_ARG_FLAGS,
                            Pt_TRUE, Pt_BLOCKED);
                }else{
                  PtSetArg(&args[8], Pt_ARG_FLAGS,
                            Pt_FALSE, Pt_GHOST);
                  PtSetArg(&args[9], Pt_ARG_FLAGS,
                            Pt_FALSE, Pt_BLOCKED);
                 };

          numeric_end_minute=PtCreateWidget(PtNumericInteger,
                                                  current_panel,
                                                  args.size(),
                                                  &args[0]);
        if (numeric_end_minute==NULL)
            throw spider_exception("Numeric for end minute is NULL");

//END time interval panel
//BEGIN messages types panel
		args.clear();
		args.resize(7);
         widget_position.x=0;
         widget_position.y=0;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_LEFT_ANCHORED_LEFT);
         PtSetArg(&args[2], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_RIGHT_ANCHORED_RIGHT);
         PtSetArg(&args[3], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_TOP_ANCHORED_TOP);
         PtSetArg(&args[4], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_BOTTOM_ANCHORED_BOTTOM);
         PtSetArg(&args[5], Pt_ARG_TITLE,
                             "Типы сообщений", 0);
         PtSetArg(&args[6], Pt_ARG_FLAGS,
                             Pt_FALSE, Pt_HIGHLIGHTED);

           current_panel=PtCreateWidget(PtPane,
                                                  panel_group,
                                                  args.size(),
                                                  &args[0]);
        if (current_panel==NULL)
            throw spider_exception("Messages types panel is NULL");

		args.clear();
		args.resize(5);
         widget_position.x=2;
         widget_position.y=4;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_TEXT_STRING,
                       "Все типы сообщений", 0);
        if (log_rec_cont->filter.get_all_msg_types()) {
  			   PtSetArg(&args[2], Pt_ARG_FLAGS, Pt_TRUE, Pt_SET); 
            } else {
  			   PtSetArg(&args[2], Pt_ARG_FLAGS, Pt_FALSE, Pt_SET); 
            };
         PtSetArg(&args[3], Pt_ARG_POINTER,
                       log_rec_cont, 0);

		callbacks.clear();
		tmp_callback.event_f=activate_toggle_button_all_msg_types_in_filter_log_window;
		callbacks.push_back(tmp_callback);
        PtSetArg(&args[4], Pt_CB_ACTIVATE,
                        &callbacks[0],
                        callbacks.size());

       disable_all_elemets_in_panel=log_rec_cont->filter.get_all_msg_types();

       toggle_button_filter_all_messages_types=PtCreateWidget(PtToggleButton,
                                                     current_panel,
                                                     args.size(),
                                                     &args[0]);
         if (toggle_button_filter_all_times==NULL)
                  throw spider_exception("toggle_button_filter_all_messages_types in time interval panel is NULL");

		args.clear();
		args.resize(6);
         widget_position.x=0;
         widget_position.y=25;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_WIDTH,
                          (dialog_wnd_width-14),0);
         PtSetArg(&args[2], Pt_ARG_HEIGHT,
                         dialog_wnd_height-buttons_panel_height-70, 0);
         PtSetArg(&args[3], Pt_ARG_SELECTION_MODE,
                             Pt_SELECTION_MODE_MULTIPLE, 0);
         if (disable_all_elemets_in_panel) {
                 PtSetArg(&args[4], Pt_ARG_FLAGS,
                            Pt_TRUE, Pt_GHOST);
                 PtSetArg(&args[5], Pt_ARG_FLAGS,
                            Pt_TRUE, Pt_BLOCKED);
                }else{
                  PtSetArg(&args[4], Pt_ARG_FLAGS,
                            Pt_FALSE, Pt_GHOST);
                  PtSetArg(&args[5], Pt_ARG_FLAGS,
                            Pt_FALSE, Pt_BLOCKED);
                 };

           messages_types_list=PtCreateWidget(PtList,
                                                  current_panel,
                                                  args.size(),
                                                  &args[0]);
        if (messages_types_list==NULL)
            throw spider_exception("messages types list is NULL");

          msg_types_container *mst_types_inst=msg_types_container::get_instance();
          if (mst_types_inst==NULL)
            throw spider_exception("mst_types_inst is NULL");

          vector<const char*> item_cont(1);
          vector<unsigned short> sel_indexes;
          unsigned short list_index_counter=1;

          msg_types_container::iterator msg_types_iter=mst_types_inst->begin();
          while(msg_types_iter!=mst_types_inst->end()) {
                 item_cont[0]=msg_types_iter->second.get_type_description().c_str();
                  if( PtListAddItems( messages_types_list,
                    &item_cont[0],
                    item_cont.size(), 0)!=0)
                            throw spider_exception("error in PtListAddItems(...) for messages_types_list");
                   if (log_rec_cont->filter.find_msg_type(msg_types_iter->second.get_type_id())!=
                        log_rec_cont->filter.msg_types_end())
                                sel_indexes.push_back(list_index_counter);
                 msg_types_iter++;
                 list_index_counter++;
            };
       if (!sel_indexes.empty())
            PtSetResource(messages_types_list, Pt_ARG_SELECTION_INDEXES, &sel_indexes[0], sel_indexes.size());

		args.clear();
		args.resize(7);
         widget_position.x=0;
         widget_position.y=0;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_LEFT_ANCHORED_LEFT);
         PtSetArg(&args[2], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_RIGHT_ANCHORED_RIGHT);
         PtSetArg(&args[3], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_TOP_ANCHORED_TOP);
         PtSetArg(&args[4], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_BOTTOM_ANCHORED_BOTTOM);
         PtSetArg(&args[5], Pt_ARG_TITLE,
                             "Станции и оборудование", 0);
         PtSetArg(&args[6], Pt_ARG_FLAGS,
                             Pt_FALSE, Pt_HIGHLIGHTED);

         current_panel=PtCreateWidget(PtPane,
                                                  panel_group,
                                                  args.size(),
                                                  &args[0]);
        if (current_panel==NULL)
            throw spider_exception("Stations panel is NULL");

         args.clear();
         args.resize(5);
         widget_position.x=2;
         widget_position.y=4;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_TEXT_STRING,
                       "Все станции", 0);
         if (log_rec_cont->filter.get_all_stations()) {
  			   PtSetArg(&args[2], Pt_ARG_FLAGS, Pt_TRUE, Pt_SET); 
            } else {
  			   PtSetArg(&args[2], Pt_ARG_FLAGS, Pt_FALSE, Pt_SET); 
            };

         PtSetArg(&args[3], Pt_ARG_POINTER,
                       log_rec_cont, 0);

		callbacks.clear();
		tmp_callback.event_f=activate_toggle_button_all_stations_in_filter_log_window;
		callbacks.push_back(tmp_callback);
        PtSetArg(&args[4], Pt_CB_ACTIVATE,
                        &callbacks[0],
                        callbacks.size());

        disable_all_elemets_in_panel=log_rec_cont->filter.get_all_stations();

         toggle_button_filter_all_stations=PtCreateWidget(PtToggleButton,
                                                     current_panel,
                                                     args.size(),
                                                     &args[0]);
         if (toggle_button_filter_all_stations==NULL)
                  throw spider_exception("toggle_button_filter_all_stations in time interval panel is NULL");

        parent_panel=current_panel;

		args.clear();
		args.resize(7);
         widget_position.x=0;
         widget_position.y=25;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_WIDTH,
                          static_cast<int>(dialog_wnd_width/2-14),0);
         PtSetArg(&args[2], Pt_ARG_HEIGHT,
                         dialog_wnd_height-buttons_panel_height-inside_panel_margin, 0);
         PtSetArg(&args[3], Pt_ARG_TITLE,
                             "Станция", 0);
         PtSetArg(&args[4], Pt_ARG_CONTAINER_FLAGS,
                            Pt_TRUE, Pt_SHOW_TITLE);
         PtSetArg(&args[5], Pt_ARG_CONTAINER_FLAGS,
                            Pt_TRUE, Pt_GRADIENT_TITLE_AREA);
         PtSetArg(&args[6], Pt_ARG_CONTAINER_FLAGS,
                            Pt_TRUE, Pt_ETCH_TITLE_AREA);

           current_panel=PtCreateWidget(PtPane,
                                                  parent_panel,
                                                  args.size(),
                                                  &args[0]);
        if (current_panel==NULL)
            throw spider_exception("station panel is NULL");

        PtGetResource(current_panel, Pt_ARG_HEIGHT, &internal_panel_height, 0);
        panel_height=*internal_panel_height;

		args.clear();
		args.resize(8);
         widget_position.x=0;
         widget_position.y=0;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_WIDTH,
                          static_cast<int>(dialog_wnd_width/2-18),0);
         PtSetArg(&args[2], Pt_ARG_HEIGHT,
                         panel_height-25, 0);
         PtSetArg(&args[3], Pt_ARG_SELECTION_MODE,
                             Pt_SELECTION_MODE_MULTIPLE, 0);

		callbacks.clear();
		tmp_callback.event_f=selection_on_stations_list;
		callbacks.push_back(tmp_callback);
        PtSetArg(&args[4], Pt_CB_SELECTION,
                        &callbacks[0],
                        callbacks.size());
         PtSetArg(&args[5], Pt_ARG_POINTER,
                        log_rec_cont, 0);


         if (disable_all_elemets_in_panel) {
                 PtSetArg(&args[6], Pt_ARG_FLAGS,
                            Pt_TRUE, Pt_GHOST);
                 PtSetArg(&args[7], Pt_ARG_FLAGS,
                            Pt_TRUE, Pt_BLOCKED);
                }else{
                  PtSetArg(&args[6], Pt_ARG_FLAGS,
                            Pt_FALSE, Pt_GHOST);
                  PtSetArg(&args[7], Pt_ARG_FLAGS,
                            Pt_FALSE, Pt_BLOCKED);
                 };

           stations_list=PtCreateWidget(PtList,
                                                  current_panel,
                                                  args.size(),
                                                  &args[0]);
        if (stations_list==NULL)
            throw spider_exception("stations list is NULL");

          metro_stations_container *stations_inst=metro_stations_container::get_instance();
          if (stations_inst==NULL)
            throw spider_exception("stations_inst is NULL");

          item_cont.clear();
          item_cont.resize(1);
          sel_indexes.clear();
          list_index_counter=1;

          metro_stations_container::iterator stations_iter=stations_inst->begin();
          while(stations_iter!=stations_inst->end()) {
                 item_cont[0]=stations_iter->second.get_c_name_string();
                  if( PtListAddItems( stations_list,
                    &item_cont[0],
                    item_cont.size(), 0)!=0)
                            throw spider_exception("error in PtListAddItems(...) for stations_list");
                   if (log_rec_cont->filter.find_station(stations_iter->second.get_id())!=
                        log_rec_cont->filter.stations_end())
                                sel_indexes.push_back(list_index_counter);
                 stations_iter++;
                 list_index_counter++;
            };
       if (!sel_indexes.empty())
            PtSetResource(stations_list, Pt_ARG_SELECTION_INDEXES, &sel_indexes[0], sel_indexes.size());

		args.clear();
		args.resize(7);
         widget_position.x=static_cast<int>(dialog_wnd_width/2-10);
         widget_position.y=25;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_WIDTH,
                          static_cast<int>(dialog_wnd_width/2),0);
         PtSetArg(&args[2], Pt_ARG_HEIGHT,
                         dialog_wnd_height-buttons_panel_height-inside_panel_margin, 0);
         PtSetArg(&args[3], Pt_ARG_TITLE,
                             "Оборудование станции", 0);
         PtSetArg(&args[4], Pt_ARG_CONTAINER_FLAGS,
                            Pt_TRUE, Pt_SHOW_TITLE);
         PtSetArg(&args[5], Pt_ARG_CONTAINER_FLAGS,
                            Pt_TRUE, Pt_GRADIENT_TITLE_AREA);
         PtSetArg(&args[6], Pt_ARG_CONTAINER_FLAGS,
                            Pt_TRUE, Pt_ETCH_TITLE_AREA);

           current_panel=PtCreateWidget(PtPane,
                                                  parent_panel,
                                                  args.size(),
                                                  &args[0]);
        if (current_panel==NULL)
            throw spider_exception("station devices panel is NULL");

         args.clear();
         args.resize(7);
         widget_position.x=2;
         widget_position.y=4;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_TEXT_STRING,
                       "Всё оборудование", 0);
         if (log_rec_cont->filter.get_all_devices()) {
  			   PtSetArg(&args[2], Pt_ARG_FLAGS, Pt_TRUE, Pt_SET); 
            } else {
  			   PtSetArg(&args[2], Pt_ARG_FLAGS, Pt_FALSE, Pt_SET); 
            };

         PtSetArg(&args[3], Pt_ARG_POINTER,
                       log_rec_cont, 0);

		callbacks.clear();
		tmp_callback.event_f=activate_toggle_button_all_devices_in_filter_log_window;
		callbacks.push_back(tmp_callback);
        PtSetArg(&args[4], Pt_CB_ACTIVATE,
                        &callbacks[0],
                        callbacks.size());

         if (disable_all_elemets_in_panel) {
                 PtSetArg(&args[5], Pt_ARG_FLAGS,
                            Pt_TRUE, Pt_GHOST);
                 PtSetArg(&args[6], Pt_ARG_FLAGS,
                            Pt_TRUE, Pt_BLOCKED);
                }else{
                  PtSetArg(&args[5], Pt_ARG_FLAGS,
                            Pt_FALSE, Pt_GHOST);
                  PtSetArg(&args[6], Pt_ARG_FLAGS,
                            Pt_FALSE, Pt_BLOCKED);
                 };


      if (! disable_all_elemets_in_panel)
        disable_all_elemets_in_panel=log_rec_cont->filter.get_all_devices();

         toggle_button_filter_all_devices=PtCreateWidget(PtToggleButton,
                                                     current_panel,
                                                     args.size(),
                                                     &args[0]);
         if (toggle_button_filter_all_devices==NULL)
                  throw spider_exception("toggle_button_filter_all_devices in devices panel is NULL");

        PtGetResource(current_panel, Pt_ARG_HEIGHT, &internal_panel_height, 0);
        panel_height=*internal_panel_height;

         args.clear();
         args.resize(8);
         widget_position.x=0;
         widget_position.y=25;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_WIDTH,
                          static_cast<int>(dialog_wnd_width/2-4),0);
         PtSetArg(&args[2], Pt_ARG_HEIGHT,
                         panel_height-50, 0);
         PtSetArg(&args[3], Pt_ARG_SELECTION_MODE,
                             Pt_SELECTION_MODE_MULTIPLE, 0);
         if (disable_all_elemets_in_panel) {
                 PtSetArg(&args[4], Pt_ARG_FLAGS,
                            Pt_TRUE, Pt_GHOST);
                 PtSetArg(&args[5], Pt_ARG_FLAGS,
                            Pt_TRUE, Pt_BLOCKED);
                }else{
                  PtSetArg(&args[4], Pt_ARG_FLAGS,
                            Pt_FALSE, Pt_GHOST);
                  PtSetArg(&args[5], Pt_ARG_FLAGS,
                            Pt_FALSE, Pt_BLOCKED);
                 };

         //ATTENTION!! If Pt_ARG_ITEMS is NULL redriwing of items not worked - bug??
         const  char* test_chars[]={"-"};
         PtSetArg(&args[6], Pt_ARG_ITEMS,
                         test_chars, 1);
         PtSetArg(&args[7], Pt_ARG_SELECTION_INDEXES,
                         NULL, 0);

           station_devices_list=PtCreateWidget(PtList,
                                                  current_panel,
                                                  args.size(),
                                                  &args[0]);
        if (station_devices_list==NULL)
            throw spider_exception("stations devices list is NULL");

   log_rec_cont->set_toggle_button_filter_turn_off(toggle_button_filter_turn_off);
   log_rec_cont->set_toggle_button_filter_all_times(toggle_button_filter_all_times);
   log_rec_cont->set_toggle_button_filter_all_messages_types(toggle_button_filter_all_messages_types);
   log_rec_cont->set_toggle_button_filter_all_stations(toggle_button_filter_all_stations);
   log_rec_cont->set_toggle_button_filter_all_devices(toggle_button_filter_all_devices);
   log_rec_cont->set_messages_types_list(messages_types_list);
   log_rec_cont->set_stations_list(stations_list);
   log_rec_cont->set_station_devices_list(station_devices_list);
   log_rec_cont->set_calendar_begin_time_interval(calendar_begin_time_interval);
   log_rec_cont->set_calendar_end_time_interval(calendar_end_time_interval);
   log_rec_cont->set_numeric_begin_hour(numeric_begin_hour);
   log_rec_cont->set_numeric_end_hour(numeric_end_hour);
   log_rec_cont->set_numeric_begin_minute(numeric_begin_minute);
   log_rec_cont->set_numeric_end_minute(numeric_end_minute);

   PtRealizeWidget(dialog_window);

   prepare_devices_list_in_filter_window (log_rec_cont,  sel_indexes);

   } catch (spider_exception spr_exc) {
       spider_sys_sett->sys_message(system_settings::ERROR_MSG, "In activate_filter_button_in_log_window(...)"+spr_exc.get_description ());
       return Pt_HALT;
  };

return( Pt_CONTINUE);
};

int
activate_exit_button_in_main_log_window( PtWidget_t *widget, 
                                                              ApInfo_t *apinfo, 
                                                              PtCallbackInfo_t *cbinfo ){
     main_log->set_widget(NULL);
return( Pt_CONTINUE);
};

int
activate_exit_button_in_archive_log_window( PtWidget_t *widget, 
                                                              ApInfo_t *apinfo, 
                                                              PtCallbackInfo_t *cbinfo ){
     archive_log->set_widget(NULL);
return( Pt_CONTINUE);
};


int
activate_archive_log_button_in_main_window( PtWidget_t *widget, 
                                                                                     ApInfo_t *apinfo, 
                                                                   PtCallbackInfo_t *cbinfo ){
PtWidget_t *dialog_window,*buttons_panel, *raw_list, *divider_in_raw_list, *current_button;
vector<PtCallback_t> callbacks;
PtCallback_t tmp_callback;
vector<PtArg_t> args;
PhPoint_t widget_position;
PtFileSelectionInfo_t info;
string dialog_title("Архив журналов сообщений") , filter_button_text("Фильтр");
unsigned int dialog_wnd_height=450,
                      dialog_wnd_width=850,
                      buttons_panel_height=40,
                      divider_height=35,
                      button_in_panel_width=180;

system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
if (spider_sys_sett==NULL) {
  cout<<"Can`t get instance of system_settings_spider"<<endl;
  return( Pt_HALT);
};

if (archive_log->get_widget()!=NULL) {
            close_window(archive_log->get_widget(), NULL, NULL);
            archive_log->set_widget(NULL);
       };

 memset( &info, 0x0, sizeof(PtFileSelectionInfo_t) );


if ( PtFileSelection( main_window, 
                     NULL,
                     "Открыть файл архива", 
                     ".",
                     "*.log", 
                     "Открыть",
                     "Отмена", 
                     NULL, 
                     &info, 
                    Pt_FSR_NO_NEW | Pt_FSR_NO_NEW_BUTTON | Pt_FSR_NO_DELETE | Pt_FSR_NO_UP_BUTTON |
                     Pt_FSR_DONT_SHOW_DIRS | Pt_FSR_NO_SEEK_KEY | Pt_FSR_NO_FSPEC | Pt_FSR_NO_ROOT_DISPLAY  )!=0)
           return( Pt_HALT);

 if ( info.ret == Pt_FSDIALOG_BTN2 ) 
           return( Pt_HALT);

archive_log->load(info.path);

try {
		args.clear();
		args.resize(13);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, dialog_wnd_height,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, dialog_wnd_width,0);
		PtSetArg(&args[2], Pt_ARG_WINDOW_TITLE, dialog_title.c_str(),0);
		PtSetArg(&args[3], Pt_ARG_WINDOW_RENDER_FLAGS,
                      Pt_FALSE, Ph_WM_RENDER_RESIZE);
		PtSetArg(&args[4], Pt_ARG_WINDOW_RENDER_FLAGS,
                      Pt_FALSE, Ph_WM_TOBACK);
		PtSetArg(&args[5], Pt_ARG_WINDOW_RENDER_FLAGS,
                      Pt_FALSE, Ph_WM_RENDER_MAX);
		PtSetArg(&args[6], Pt_ARG_WINDOW_RENDER_FLAGS,
                      Pt_FALSE, Ph_WM_RENDER_MENU);
		PtSetArg(&args[7], Pt_ARG_WINDOW_RENDER_FLAGS,
                      Pt_FALSE, Ph_WM_RENDER_COLLAPSE );
		PtSetArg(&args[8], Pt_ARG_WINDOW_MANAGED_FLAGS,
                      Pt_FALSE, Ph_WM_MAX );
		PtSetArg(&args[9], Pt_ARG_WINDOW_MANAGED_FLAGS,
                      Pt_FALSE, Ph_WM_MENU  );
		PtSetArg(&args[10], Pt_ARG_WINDOW_MANAGED_FLAGS,
                      Pt_FALSE, Ph_WM_CLOSE  );
		PtSetArg(&args[11], Pt_ARG_WINDOW_MANAGED_FLAGS,
                      Pt_FALSE, Ph_WM_COLLAPSE );
		PtSetArg(&args[12], Pt_ARG_POINTER,
                        archive_log, 0);

/*
//that`s present in activate close window button
		callbacks.clear();
		tmp_callback.event_f=activate_exit_button_in_log_window;
         tmp_callback.data=NULL;
		callbacks.push_back(tmp_callback);
		tmp_callback.event_f=close_window;
         tmp_callback.data=NULL;
		callbacks.push_back(tmp_callback);

        PtSetArg(&args[13],  Pt_CB_WINDOW_CLOSING,
			           &callbacks[0],
                         callbacks.size());
*/

		dialog_window=PtCreateWidget(PtWindow,
                                                     spider_sys_sett->get_main_window(),
                                                     args.size(),
                                                     &args[0]);
         if (dialog_window==NULL)
                  throw spider_exception("dialog_wnd is NULL");
        //buttons panel
		args.clear();
		args.resize(3);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, buttons_panel_height,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, dialog_wnd_width,0);
          widget_position.x=0;
          widget_position.y=dialog_wnd_height-buttons_panel_height;
         PtSetArg(&args[2], Pt_ARG_POS,
                     &widget_position, 0);
         buttons_panel=PtCreateWidget(PtPane,
                                                     dialog_window,
                                                     args.size(),
                                                     &args[0]);
         if (buttons_panel==NULL)
                  throw spider_exception("buttons_panel is NULL");

          //raw list with messages
		args.clear();
		args.resize(8);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, dialog_wnd_height-buttons_panel_height,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, dialog_wnd_width,0);
          widget_position.x=0;
          widget_position.y=0;
         PtSetArg(&args[2], Pt_ARG_POS,
                       &widget_position, 0);
         PtSetArg(&args[3], Pt_ARG_RAWLIST_DRAW_F,
                       archive_log_raw_list_draw_function, 0);
		PtSetArg(&args[4], Pt_ARG_LIST_FLAGS,
                      Pt_TRUE, Pt_LIST_SCROLLBAR_ALWAYS);
		PtSetArg(&args[5], Pt_ARG_LIST_FLAGS,
                      Pt_TRUE, Pt_LIST_SCROLLBAR_AUTORESIZE);
		PtSetArg(&args[6], Pt_ARG_LIST_FLAGS,
                      Pt_TRUE, Pt_LIST_BALLOONS_IN_COLUMNS);
		PtSetArg(&args[7], Pt_ARG_POINTER,
                        archive_log, 0);

         raw_list=PtCreateWidget(PtRawList,
                                                   dialog_window,
                                                   args.size(),
                                                   &args[0]);
         if (raw_list==NULL)
                 throw spider_exception("raw_list is NULL");

         //divider in raw list
		args.clear();
		args.resize(6);
         PtSetArg(&args[0], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_LEFT_ANCHORED_LEFT);
         PtSetArg(&args[1], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_RIGHT_ANCHORED_RIGHT);
         PtSetArg(&args[2], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_TOP_ANCHORED_TOP);
         PtSetArg(&args[3], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_BOTTOM_ANCHORED_TOP);
         PtSetArg(&args[4], Pt_ARG_DIVIDER_FLAGS, Pt_TRUE, Pt_DIVIDER_NORESIZE);
         PtSetArg(&args[5], Pt_ARG_DIVIDER_FLAGS, Pt_FALSE, Pt_DIVIDER_RESIZE_BOTH);

         divider_in_raw_list=PtCreateWidget(PtDivider,
                                                     raw_list,
                                                     args.size(),
                                                     &args[0]);
         if (divider_in_raw_list==NULL)
                  throw spider_exception("divider_in_raw_list is NULL");

          //buttons in raw list
		args.clear();
		args.resize(3);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, divider_height-2,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, (dialog_wnd_width-2)/8,0);
         PtSetArg(&args[2], Pt_ARG_TEXT_STRING,
                       "Дата", 0);
         current_button=PtCreateWidget(PtButton,
                                                     divider_in_raw_list,
                                                     args.size(),
                                                     &args[0]);
         if (current_button==NULL)
                  throw spider_exception("button \"Date\" in raw list header is NULL");

		args.clear();
		args.resize(3);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, divider_height-2,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, (dialog_wnd_width-2)/8,0);
         PtSetArg(&args[2], Pt_ARG_TEXT_STRING,
                       "Время", 0);
         current_button=PtCreateWidget(PtButton,
                                                     divider_in_raw_list,
                                                     args.size(),
                                                     &args[0]);
         if (current_button==NULL)
                  throw spider_exception("button \"Time\" in raw list header is NULL");

		args.clear();
		args.resize(3);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, divider_height-2,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, (dialog_wnd_width-2)/8,0);
         PtSetArg(&args[2], Pt_ARG_TEXT_STRING,
                       "Станция", 0);
         current_button=PtCreateWidget(PtButton,
                                                     divider_in_raw_list,
                                                     args.size(),
                                                     &args[0]);
         if (current_button==NULL)
                  throw spider_exception("button \"Station\" in raw list header is NULL");

		args.clear();
		args.resize(3);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, (dialog_wnd_width-2)/8,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, 200,0);
         PtSetArg(&args[2], Pt_ARG_TEXT_STRING,
                       "Устройство", 0);
         current_button=PtCreateWidget(PtButton,
                                                     divider_in_raw_list,
                                                     args.size(),
                                                     &args[0]);
         if (current_button==NULL)
                  throw spider_exception("button \"Device\" in raw list header is NULL");

		args.clear();
		args.resize(3);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, (dialog_wnd_width-2)/2,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, 200,0);
         PtSetArg(&args[2], Pt_ARG_TEXT_STRING,
                       "Сообщение", 0);
         current_button=PtCreateWidget(PtButton,
                                                     divider_in_raw_list,
                                                     args.size(),
                                                     &args[0]);
         if (current_button==NULL)
                  throw spider_exception("button \"Message\" in raw list header is NULL");

           archive_log->set_widget(raw_list);
           archive_log->prepare_to_display();

         //buttons in button panel
		args.clear();
		args.resize(9);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, buttons_panel_height-8,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, button_in_panel_width,0);
          widget_position.x=5;
          widget_position.y=2;
         PtSetArg(&args[2], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[3], Pt_ARG_LABEL_TYPE,
                       Pt_TEXT_IMAGE , 0);
         PtSetArg(&args[4], Pt_ARG_TEXT_IMAGE_SPACING,
                       5, 0);
         if (archive_log->filter.get_filter_state()) { // true mean is filtered
                  PtSetArg(&args[5], Pt_ARG_LABEL_IMAGE,
                         spider_sys_sett->get_image(system_settings_spider::BLOCK_RED_LED), 0);
                    filter_button_text+=" ВКЛ";
             } else { // if (archive_log->filter.get_filter_state())
                  PtSetArg(&args[5], Pt_ARG_LABEL_IMAGE,
                          spider_sys_sett->get_image(system_settings_spider::BLOCK_GREY_LED), 0);
                   filter_button_text+=" ВЫКЛ";
             };// if (archive_log->filter.get_filter_state())
         PtSetArg(&args[6], Pt_ARG_TEXT_STRING,
                       filter_button_text.c_str(), 0);
         PtSetArg(&args[7], Pt_ARG_POINTER,
                        archive_log, 0);

		callbacks.clear();
		tmp_callback.event_f=activate_filter_button_in_log_window;
         tmp_callback.data=raw_list;
		callbacks.push_back(tmp_callback);
         PtSetArg(&args[8], Pt_CB_ACTIVATE,
                        &callbacks[0],
                        callbacks.size());
         current_button=PtCreateWidget(PtButton,
                                                     buttons_panel,
                                                     args.size(),
                                                     &args[0]);
         if (current_button==NULL)
                  throw spider_exception("button \"Filter\" in buttons_panel is NULL");

        archive_log->set_filtration_state_indicator(current_button);

/*
		args.clear();
		args.resize(6);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, buttons_panel_height-8,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, button_in_panel_width,0);
          widget_position.x=button_in_panel_width+10;
          widget_position.y=2;
         PtSetArg(&args[2], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[3], Pt_ARG_TEXT_STRING,
                       "Отчет", 0);
         PtSetArg(&args[4], Pt_ARG_POINTER,
                        archive_log, 0);
		callbacks.clear();
		tmp_callback.event_f=activate_report_button_in_log_window;
         tmp_callback.data=raw_list;
		callbacks.push_back(tmp_callback);
         PtSetArg(&args[5], Pt_CB_ACTIVATE,
                        &callbacks[0],
                        callbacks.size());
         current_button=PtCreateWidget(PtButton,
                                                     buttons_panel,
                                                     args.size(),
                                                     &args[0]);
         if (current_button==NULL)
                  throw spider_exception("button \"Report\" in buttons_panel is NULL");
*/
		args.clear();
		args.resize(6);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, buttons_panel_height-8,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, button_in_panel_width,0);
          widget_position.x=dialog_wnd_width-button_in_panel_width-5;
          widget_position.y=2;
         PtSetArg(&args[2], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[3], Pt_ARG_TEXT_STRING,
                       "Закрыть окно", 0);
         PtSetArg(&args[4], Pt_ARG_POINTER,
                       archive_log, 0);
		callbacks.clear();
		tmp_callback.event_f=activate_exit_button_in_archive_log_window;
         tmp_callback.data=NULL;
		callbacks.push_back(tmp_callback);
		tmp_callback.event_f=close_window;
         tmp_callback.data=NULL;
		callbacks.push_back(tmp_callback);
         PtSetArg(&args[5], Pt_CB_ACTIVATE,
                        &callbacks[0],
                        callbacks.size());
         current_button=PtCreateWidget(PtButton,
                                                     buttons_panel,
                                                     args.size(),
                                                     &args[0]);
         if (current_button==NULL)
                  throw spider_exception("button \"Close\" in buttons_panel is NULL");

       if ( PtRealizeWidget(dialog_window)!=0) 
                   throw spider_exception("Can`t realize dialog window");
   		args.clear();
		args.resize(2);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, divider_height,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, dialog_wnd_width-4,0);
         PtSetResources(divider_in_raw_list, args.size(), &args[0]);

} catch (spider_exception spr_exc) { 
    spider_sys_sett->sys_message(system_settings::ERROR_MSG, 
         "In activate_main_log_button_in_main_window: "+spr_exc.get_description());
     return( Pt_HALT);
}; // catch (spider_exception spr_exc)

return( Pt_CONTINUE);
};

int
activate_main_log_button_in_main_window( PtWidget_t *widget, 
                                                                         ApInfo_t *apinfo, 
                                                                         PtCallbackInfo_t *cbinfo ){
PtWidget_t *dialog_window,*buttons_panel, *raw_list, *divider_in_raw_list, *current_button;
vector<PtCallback_t> callbacks;
PtCallback_t tmp_callback;
vector<PtArg_t> args;
PhPoint_t widget_position;
string dialog_title("Журнал сообщений") , filter_button_text("Фильтр");
unsigned int dialog_wnd_height=450,
                      dialog_wnd_width=850,
                      buttons_panel_height=40,
                      divider_height=35,
                      button_in_panel_width=180;

system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
if (spider_sys_sett==NULL) {
  cout<<"Can`t get instance of system_settings_spider"<<endl;
  return( Pt_HALT);
};

if (main_log->get_widget()!=NULL) {
            close_window(main_log->get_widget(), NULL, NULL);
            main_log->set_widget(NULL);
         };

try {
		args.clear();
		args.resize(13);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, dialog_wnd_height,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, dialog_wnd_width,0);
		PtSetArg(&args[2], Pt_ARG_WINDOW_TITLE, dialog_title.c_str(),0);
		PtSetArg(&args[3], Pt_ARG_WINDOW_RENDER_FLAGS,
                      Pt_FALSE, Ph_WM_RENDER_RESIZE);
		PtSetArg(&args[4], Pt_ARG_WINDOW_RENDER_FLAGS,
                      Pt_FALSE, Ph_WM_TOBACK);
		PtSetArg(&args[5], Pt_ARG_WINDOW_RENDER_FLAGS,
                      Pt_FALSE, Ph_WM_RENDER_MAX);
		PtSetArg(&args[6], Pt_ARG_WINDOW_RENDER_FLAGS,
                      Pt_FALSE, Ph_WM_RENDER_MENU);
		PtSetArg(&args[7], Pt_ARG_WINDOW_RENDER_FLAGS,
                      Pt_FALSE, Ph_WM_RENDER_COLLAPSE );
		PtSetArg(&args[8], Pt_ARG_WINDOW_MANAGED_FLAGS,
                      Pt_FALSE, Ph_WM_MAX );
		PtSetArg(&args[9], Pt_ARG_WINDOW_MANAGED_FLAGS,
                      Pt_FALSE, Ph_WM_MENU  );
		PtSetArg(&args[10], Pt_ARG_WINDOW_MANAGED_FLAGS,
                      Pt_FALSE, Ph_WM_CLOSE  );
		PtSetArg(&args[11], Pt_ARG_WINDOW_MANAGED_FLAGS,
                      Pt_FALSE, Ph_WM_COLLAPSE );
		PtSetArg(&args[12], Pt_ARG_POINTER,
                        main_log, 0);

		dialog_window=PtCreateWidget(PtWindow,
                                                     spider_sys_sett->get_main_window(),
                                                     args.size(),
                                                     &args[0]);
         if (dialog_window==NULL)
                  throw spider_exception("dialog_wnd is NULL");
        //buttons panel
		args.clear();
		args.resize(3);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, buttons_panel_height,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, dialog_wnd_width,0);
          widget_position.x=0;
          widget_position.y=dialog_wnd_height-buttons_panel_height;
         PtSetArg(&args[2], Pt_ARG_POS,
                     &widget_position, 0);
         buttons_panel=PtCreateWidget(PtPane,
                                                     dialog_window,
                                                     args.size(),
                                                     &args[0]);
         if (buttons_panel==NULL)
                  throw spider_exception("buttons_panel is NULL");

          //raw list with messages
		args.clear();
		args.resize(8);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, dialog_wnd_height-buttons_panel_height,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, dialog_wnd_width,0);
          widget_position.x=0;
          widget_position.y=0;
         PtSetArg(&args[2], Pt_ARG_POS,
                       &widget_position, 0);
         PtSetArg(&args[3], Pt_ARG_RAWLIST_DRAW_F, 
                       main_log_raw_list_draw_function, 0);
		PtSetArg(&args[4], Pt_ARG_LIST_FLAGS,
                      Pt_TRUE, Pt_LIST_SCROLLBAR_ALWAYS);
		PtSetArg(&args[5], Pt_ARG_LIST_FLAGS,
                      Pt_TRUE, Pt_LIST_SCROLLBAR_AUTORESIZE);
		PtSetArg(&args[6], Pt_ARG_LIST_FLAGS,
                      Pt_TRUE, Pt_LIST_BALLOONS_IN_COLUMNS);
		PtSetArg(&args[7], Pt_ARG_POINTER,
                        main_log, 0);

         raw_list=PtCreateWidget(PtRawList,
                                                   dialog_window,
                                                   args.size(),
                                                   &args[0]);
         if (raw_list==NULL)
                 throw spider_exception("raw_list is NULL");

          //devider in raw list
		args.clear();
		args.resize(6);
         PtSetArg(&args[0], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_LEFT_ANCHORED_LEFT);
         PtSetArg(&args[1], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_RIGHT_ANCHORED_RIGHT);
         PtSetArg(&args[2], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_TOP_ANCHORED_TOP);
         PtSetArg(&args[3], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_BOTTOM_ANCHORED_TOP);
         PtSetArg(&args[4], Pt_ARG_DIVIDER_FLAGS, Pt_TRUE, Pt_DIVIDER_NORESIZE);
         PtSetArg(&args[5], Pt_ARG_DIVIDER_FLAGS, Pt_FALSE, Pt_DIVIDER_RESIZE_BOTH);

         divider_in_raw_list=PtCreateWidget(PtDivider,
                                                     raw_list,
                                                     args.size(),
                                                     &args[0]);
         if (divider_in_raw_list==NULL)
                  throw spider_exception("divider_in_raw_list is NULL");

          //buttons in raw list
		args.clear();
		args.resize(3);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, divider_height-2,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, (dialog_wnd_width-2)/8,0);
         PtSetArg(&args[2], Pt_ARG_TEXT_STRING,
                       "Дата", 0);
         current_button=PtCreateWidget(PtButton,
                                                     divider_in_raw_list,
                                                     args.size(),
                                                     &args[0]);
         if (current_button==NULL)
                  throw spider_exception("button \"Date\" in raw list header is NULL");

		args.clear();
		args.resize(3);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, divider_height-2,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, (dialog_wnd_width-2)/8,0);
         PtSetArg(&args[2], Pt_ARG_TEXT_STRING,
                       "Время", 0);
         current_button=PtCreateWidget(PtButton,
                                                     divider_in_raw_list,
                                                     args.size(),
                                                     &args[0]);
         if (current_button==NULL)
                  throw spider_exception("button \"Time\" in raw list header is NULL");

		args.clear();
		args.resize(3);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, divider_height-2,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, (dialog_wnd_width-2)/8,0);
         PtSetArg(&args[2], Pt_ARG_TEXT_STRING,
                       "Станция", 0);
         current_button=PtCreateWidget(PtButton,
                                                     divider_in_raw_list,
                                                     args.size(),
                                                     &args[0]);
         if (current_button==NULL)
                  throw spider_exception("button \"Station\" in raw list header is NULL");

		args.clear();
		args.resize(3);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, (dialog_wnd_width-2)/8,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, 200,0);
         PtSetArg(&args[2], Pt_ARG_TEXT_STRING,
                       "Устройство", 0);
         current_button=PtCreateWidget(PtButton,
                                                     divider_in_raw_list,
                                                     args.size(),
                                                     &args[0]);
         if (current_button==NULL)
                  throw spider_exception("button \"Device\" in raw list header is NULL");

		args.clear();
		args.resize(3);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, (dialog_wnd_width-2)/2,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, 200,0);
         PtSetArg(&args[2], Pt_ARG_TEXT_STRING,
                       "Сообщение", 0);
         current_button=PtCreateWidget(PtButton,
                                                     divider_in_raw_list,
                                                     args.size(),
                                                     &args[0]);
         if (current_button==NULL)
                  throw spider_exception("button \"Message\" in raw list header is NULL");

           main_log->set_widget(raw_list);
           main_log->prepare_to_display();

         //buttons in button panel
		args.clear();
		args.resize(9);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, buttons_panel_height-8,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, button_in_panel_width,0);
          widget_position.x=5;
          widget_position.y=2;
         PtSetArg(&args[2], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[3], Pt_ARG_LABEL_TYPE,
                       Pt_TEXT_IMAGE , 0);
         PtSetArg(&args[4], Pt_ARG_TEXT_IMAGE_SPACING,
                       5, 0);
         if (main_log->filter.get_filter_state()) { // true mean is filtered
                   PtSetArg(&args[5], Pt_ARG_LABEL_IMAGE,
                          spider_sys_sett->get_image(system_settings_spider::BLOCK_RED_LED), 0);
                    filter_button_text+=" ВКЛ";
             } else { // if (main_log->filter.get_filter_state())
                  PtSetArg(&args[5], Pt_ARG_LABEL_IMAGE,
                          spider_sys_sett->get_image(system_settings_spider::BLOCK_GREY_LED), 0);
                   filter_button_text+=" ВЫКЛ";
             };// if (main_log->filter.get_filter_state())

         PtSetArg(&args[6], Pt_ARG_TEXT_STRING,
                       filter_button_text.c_str(), 0);
         PtSetArg(&args[7], Pt_ARG_POINTER,
                        main_log, 0);
		callbacks.clear();
		tmp_callback.event_f=activate_filter_button_in_log_window;
         tmp_callback.data=raw_list;
		callbacks.push_back(tmp_callback);
         PtSetArg(&args[8], Pt_CB_ACTIVATE,
                        &callbacks[0],
                        callbacks.size());
         current_button=PtCreateWidget(PtButton,
                                                     buttons_panel,
                                                     args.size(),
                                                     &args[0]);
         if (current_button==NULL)
                  throw spider_exception("button \"Filter\" in buttons_panel is NULL");

          main_log->set_filtration_state_indicator(current_button);

/*
		args.clear();
		args.resize(6);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, buttons_panel_height-8,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, button_in_panel_width,0);
          widget_position.x=button_in_panel_width+10;
          widget_position.y=2;
         PtSetArg(&args[2], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[3], Pt_ARG_TEXT_STRING,
                       "Отчет", 0);
         PtSetArg(&args[4], Pt_ARG_POINTER,
                        main_log, 0);
		callbacks.clear();
		tmp_callback.event_f=activate_report_button_in_log_window;
         tmp_callback.data=raw_list;
		callbacks.push_back(tmp_callback);
         PtSetArg(&args[5], Pt_CB_ACTIVATE,
                        &callbacks[0],
                        callbacks.size());
         current_button=PtCreateWidget(PtButton,
                                                     buttons_panel,
                                                     args.size(),
                                                     &args[0]);
         if (current_button==NULL)
                  throw spider_exception("button \"Report\" in buttons_panel is NULL");
*/

		args.clear();
		args.resize(6);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, buttons_panel_height-8,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, button_in_panel_width,0);
          widget_position.x=dialog_wnd_width-button_in_panel_width-5;
          widget_position.y=2;
         PtSetArg(&args[2], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[3], Pt_ARG_TEXT_STRING,
                       "Закрыть окно", 0);
         PtSetArg(&args[4], Pt_ARG_POINTER,
                       main_log, 0);
		callbacks.clear();
		tmp_callback.event_f=activate_exit_button_in_main_log_window;
         tmp_callback.data=NULL;
		callbacks.push_back(tmp_callback);

		tmp_callback.event_f=close_window;
         tmp_callback.data=NULL;
		callbacks.push_back(tmp_callback);
         PtSetArg(&args[5], Pt_CB_ACTIVATE,
                        &callbacks[0],
                        callbacks.size());
         current_button=PtCreateWidget(PtButton,
                                                     buttons_panel,
                                                     args.size(),
                                                     &args[0]);
      if (current_button==NULL)
                  throw spider_exception("button \"Close\" in buttons_panel is NULL");

       if ( PtRealizeWidget(dialog_window)!=0) 
                   throw spider_exception("Can`t realize dialog window");
   		args.clear();
		args.resize(2);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, divider_height,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, dialog_wnd_width-4,0);
         PtSetResources(divider_in_raw_list, args.size(), &args[0]);

} catch (spider_exception spr_exc) { 
    spider_sys_sett->sys_message(system_settings::ERROR_MSG, 
         "In activate_main_log_button_in_main_window: "+spr_exc.get_description());
     return( Pt_HALT);
}; // catch (spider_exception spr_exc)

return( Pt_CONTINUE);
};


