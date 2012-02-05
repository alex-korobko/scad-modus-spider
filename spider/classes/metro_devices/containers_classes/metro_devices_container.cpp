#include <vector>
#include <string>
#include <hash_map>
#include <list>
#include <algorithm>
#include <functional>
#include <sstream>

#include <Ph.h>
#include <Ap.h>
#include <Pt.h>
#include <pthread.h>
#include <process.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/neutrino.h>
#include <sys/syspage.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>
#include <inttypes.h>
#include <fcntl.h>

#include <iostream.h>

using namespace std;

#include "defines.h"
#include <sys/select.h> //#define FD_SETSIZE
#include "spider_exception.h"
#include "system_settings.h"
#include "system_settings_spider.h"

#include "msg_types_container.h"
#include "msg_dict_container.h"

class metro_devices_container;

#include "station.h"
#include "metro_stations_container.h"

#include "command.h"
#include "data_block.h"
#include "escalator_data_block.h"
#include "shavr_data_block.h"
#include "udku_data_block.h"
#include "door_data_block.h"
#include "devices_types.h"

#include "timer_command_container.h"
#include "metro_devices_types_container.h"
#include "metro_device.h"
#include "escalator.h"
#include "shavr.h"
#include "udku.h"
#include "door.h"
#include "metro_devices_container.h"

extern bool sending_commands_disabled;

metro_devices_container* metro_devices_container::metro_devices_container_instance=NULL;
metro_devices_container::make_timer_command* 
          metro_devices_container::make_timer_command::make_timer_command_instance=NULL;
metro_devices_container::find_timer_command* 
           metro_devices_container::find_timer_command::find_timer_command_instance=NULL;

metro_devices_container* metro_devices_container::get_instance(){
     if (metro_devices_container_instance==NULL) 
        metro_devices_container_instance=new metro_devices_container();
     return metro_devices_container_instance;
};

metro_devices_container::make_timer_command* 
        metro_devices_container::make_timer_command::get_instance() {
       if (make_timer_command_instance==NULL)
             make_timer_command_instance=new make_timer_command();
       return make_timer_command_instance;
};

metro_devices_container::find_timer_command* 
      metro_devices_container::find_timer_command::get_instance(){
       if (find_timer_command_instance==NULL)
             find_timer_command_instance=new find_timer_command();
       return find_timer_command_instance;
};

void metro_devices_container::make_set_time::operator()
          (metro_devices_container::value_type device_container_value) {
       device_container_value.second->set_time();
};


void metro_devices_container::make_timer_command::initialize_day(){
			time_t time_now=time(NULL);
			struct tm that_day_tm;
			localtime_r(&time_now, &that_day_tm);

			that_day_tm.tm_mday%2==0 ?
			day_now=system_settings::START_DAY_MODE_EVEN :
			day_now=system_settings::START_DAY_MODE_ODD;
		};

void  metro_devices_container::make_timer_command::operator() 
              (value_type device_container_value) throw (spider_exception) {
			ostringstream exception_description;
			bool start_enabled=true;
            timer_command_container *cont_timer_command=timer_command_container::get_instance();

			if (! device_container_value.second->get_enabled()) return;
			int execution_mode=device_container_value.second->get_execution_mode();

			if (execution_mode==
				system_settings::START_DAY_MODE_NEVER ||
				(execution_mode!=
				system_settings::START_DAY_MODE_EVERYDAY &&
				execution_mode!=day_now) 
				 ) start_enabled=false; 

           try {
                if (start_enabled) { //only if start enabled, we can insert it
        			cont_timer_command->insert(
                              cont_timer_command->end(),
        			                  timer_command(
                                               device_container_value.second->get_device_start_command(),
       										   device_container_value.second->get_start_hour(),
								               device_container_value.second->get_start_minute(),
							     						   execution_mode,
												     	   start_enabled )
													);
                     }; //if (start_enabled) {
            } catch (spider_exception) {
             //It`s normally situation 
            };
	};


void metro_devices_container::
           find_timer_command::
             initialize_time_to_find ( int new_timer_hour,
                              int new_timer_minute) {	
						timer_hour=new_timer_hour;
					timer_minute=new_timer_minute;
};

//ATTENTION !!!  COMMAND AFTER EXECUTION MUST BE DELETED
void metro_devices_container::find_timer_command::operator() 
		(timer_command& timer_cmd){
		 metro_devices_container* dev_container=metro_devices_container::get_instance();
// timer_command_container previosly created with only started now commands
// in  void  metro_devices_container::make_timer_command::operator() 
			if (timer_cmd.get_timer_hour()!=timer_hour ||
   				timer_cmd.get_timer_minute()!=timer_minute) return;
			metro_devices_container::iterator iter_dev=
					dev_container->find(timer_cmd.get_command().get_device_id() );
					
			if (iter_dev!=dev_container->end())
			        iter_dev->second->send_command(timer_cmd.get_command());
};

void 	metro_devices_container::save_door_parameters ()  throw (spider_exception) {
};

void 	metro_devices_container::save_shavr_parameters ()  throw (spider_exception) {
};

void 	metro_devices_container::save_escalator_parameters ()  throw (spider_exception) {
system_settings_spider *sys_sett_obj=	system_settings_spider::get_instance();
enum {ID=0, STATION_ID, TYPE, ENABLED, NUMBER, MODBUS_NUMBER,
			IP_ADDRESS, START_DAY_MODE, START_HOUR, START_MINUTE, 
			PREDEFINED_DIRECTION, START_DIRECTION, OFFLINE_DELAY,
			CONDUCTION_DELAY, LOG_PACKETS, ENTRIES_COUNT};

string entry_name;
ostringstream exception_description;

vector<char> temp_str(512);
vector<string> entries_names(ENTRIES_COUNT);

int id_device=-1;

system_settings::strings_container directions_strings_engl=sys_sett_obj->get_directions_engl_strings();
system_settings::strings_container start_days_modes_strings_engl=sys_sett_obj->get_start_days_modes_engl_strings();

entries_names[ID]="id";
entries_names[STATION_ID]="stationID";
entries_names[TYPE]="type";
entries_names[ENABLED]="enabled";
entries_names[NUMBER]="num";
entries_names[MODBUS_NUMBER]="modbus num";
entries_names[IP_ADDRESS]="ip";
entries_names[START_DAY_MODE]="start day mode";
entries_names[START_HOUR]="start hour";
entries_names[START_MINUTE]="start minute";
entries_names[PREDEFINED_DIRECTION]="predef_direction";
entries_names[START_DIRECTION]="start_direction";
entries_names[OFFLINE_DELAY]="after_offline_delay";
entries_names[CONDUCTION_DELAY] = "conduct_notify_delay";
entries_names[LOG_PACKETS] = "log_packets";

if (PxConfigReadInt( NULL, 
			                     entries_names[ID].c_str(), 
			                     -1, 
			                     &id_device)!=Pt_TRUE) {
        exception_description<<"save_escalator_parameters: can`t read entry - id";
	    throw spider_exception(exception_description.str());
	};

	iterator	 iter_dev=this->find(id_device);
	if (iter_dev==this->end()){
	       exception_description<<"save_escalator_parameters: iter_dev==this->end() dev_id ";
	       exception_description<<id_device;
            throw spider_exception(exception_description.str());
   };

    if (iter_dev->second->get_type_description()!=metro_device_type::DEVICE_TYPE_ESCALATOR) {
	       exception_description<<"save_escalator_parameters: device with id  "
	                                         <<id_device<<" in config declared as escalator but in program is not!";
           throw spider_exception(exception_description.str());                            
    };

    metro_device *metro_device=iter_dev->second;
	metro_escalator *esc = dynamic_cast<metro_escalator*> (metro_device);

	if (PxConfigWriteInt( NULL, 
			                    entries_names[START_HOUR].c_str(), 
   				                PXCONFIG_FMT_INT_DECIMAL, 
			                    esc->get_start_hour())!=Pt_TRUE) {
	       exception_description<<"save_escalator_parameters: PxConfigWriteInt START_HOUR !=Pt_TRUE device_id ";
	       exception_description<<id_device;
           throw spider_exception(exception_description.str());
           };

	if (PxConfigWriteInt( NULL, 
			                    entries_names[START_MINUTE].c_str(), 
   				                PXCONFIG_FMT_INT_DECIMAL, 
			                    esc->get_start_minute())!=Pt_TRUE){
	       exception_description<<"save_escalator_parameters: PxConfigWriteInt START_MINUTE !=Pt_TRUE device_id ";
	       exception_description<<id_device;
           throw spider_exception(exception_description.str());
           };
	
	if (PxConfigWriteString( NULL, 
			                    entries_names[START_DAY_MODE].c_str(), 
   				                PXCONFIG_FMT_STRING, 
			                    start_days_modes_strings_engl[esc->get_execution_mode()].c_str())!=Pt_TRUE){
	       exception_description<<"save_escalator_parameters: PxConfigWriteString START_DAY_MODE !=Pt_TRUE device_id ";
	       exception_description<<id_device;
           throw spider_exception(exception_description.str());
          };

	 if (PxConfigWriteString( NULL, 
			                    entries_names[PREDEFINED_DIRECTION].c_str(), 
   				                PXCONFIG_FMT_STRING, 
			                    directions_strings_engl[esc->get_pref_direction()].c_str())!=Pt_TRUE){
	       exception_description<<"save_escalator_parameters: PxConfigWriteString PREDEFINED_DIRECTION !=Pt_TRUE device_id ";
	       exception_description<<id_device;
           throw spider_exception(exception_description.str());
           };

	 if (PxConfigWriteString( NULL, 
			                    entries_names[START_DIRECTION].c_str(), 
   				                PXCONFIG_FMT_STRING, 
			                    directions_strings_engl[esc->get_start_direction()].c_str())!=Pt_TRUE){
	       exception_description<<"save_escalator_parameters: PxConfigWriteString START_DIRECTION !=Pt_TRUE device_id ";
	       exception_description<<id_device;
           throw spider_exception(exception_description.str());
           };

}


void 	metro_devices_container::save_udku_parameters ()  throw (spider_exception) {

system_settings_spider *sys_sett_obj=	system_settings_spider::get_instance();
enum {ID=0, STATION_ID, TYPE, ENABLED, NUMBER, MODBUS_NUMBER,
			IP_ADDRESS, START_DAY_MODE, START_HOUR, START_MINUTE, 
			PREDEFINED_DIRECTION, START_DIRECTION, OFFLINE_DELAY,
			CONDUCTION_DELAY, LOG_PACKETS, ENTRIES_COUNT};

string entry_name;
ostringstream exception_description;

vector<char> temp_str(512);
vector<string> entries_names(ENTRIES_COUNT);

int id_device=-1;

system_settings::strings_container directions_strings_engl=sys_sett_obj->get_directions_engl_strings();
system_settings::strings_container start_days_modes_strings_engl=sys_sett_obj->get_start_days_modes_engl_strings();

entries_names[ID]="id";
entries_names[STATION_ID]="stationID";
entries_names[TYPE]="type";
entries_names[ENABLED]="enabled";
entries_names[NUMBER]="num";
entries_names[MODBUS_NUMBER]="modbus num";
entries_names[IP_ADDRESS]="ip";
entries_names[START_DAY_MODE]="start day mode";
entries_names[START_HOUR]="start hour";
entries_names[START_MINUTE]="start minute";
entries_names[PREDEFINED_DIRECTION]="predef_direction";
entries_names[START_DIRECTION]="start_direction";
entries_names[OFFLINE_DELAY]="after_offline_delay";
entries_names[CONDUCTION_DELAY] = "conduct_notify_delay";
entries_names[LOG_PACKETS] = "log_packets";

if (PxConfigReadInt( NULL, 
			                     entries_names[ID].c_str(), 
			                     -1, 
			                     &id_device)!=Pt_TRUE) {
        exception_description<<"save_escalator_parameters: can`t read entry - id";
	    throw spider_exception(exception_description.str());
	};

	iterator	 iter_dev=this->find(id_device);
	if (iter_dev==this->end()){
	       exception_description<<"save_udku_parameters: iter_dev==this->end() dev_id ";
	       exception_description<<id_device;
            throw spider_exception(exception_description.str());
   };

    if (iter_dev->second->get_type_description()!=metro_device_type::DEVICE_TYPE_UDKU) {
	       exception_description<<"save_udku_parameters: device with id  "
	                                         <<id_device<<" in config declared as udku but in program is not!";
           throw spider_exception(exception_description.str());                            
    };

    metro_device *metro_device=iter_dev->second;
	metro_udku *udku = dynamic_cast<metro_udku*> (metro_device);

	if (PxConfigWriteInt( NULL, 
			                    entries_names[START_HOUR].c_str(), 
   				                PXCONFIG_FMT_INT_DECIMAL, 
			                    udku->get_start_hour())!=Pt_TRUE) {
	       exception_description<<"save_udku_parameters: PxConfigWriteInt START_HOUR !=Pt_TRUE device_id ";
	       exception_description<<id_device;
           throw spider_exception(exception_description.str());
           };

	if (PxConfigWriteInt( NULL, 
			                    entries_names[START_MINUTE].c_str(), 
   				                PXCONFIG_FMT_INT_DECIMAL, 
			                    udku->get_start_minute())!=Pt_TRUE){
	       exception_description<<"save_udku_parameters: PxConfigWriteInt START_MINUTE !=Pt_TRUE device_id ";
	       exception_description<<id_device;
           throw spider_exception(exception_description.str());
           };
	
	if (PxConfigWriteString( NULL, 
			                    entries_names[START_DAY_MODE].c_str(), 
   				                PXCONFIG_FMT_STRING, 
			                    start_days_modes_strings_engl[udku->get_execution_mode()].c_str())!=Pt_TRUE){
	       exception_description<<"save_udku_parameters: PxConfigWriteString START_DAY_MODE !=Pt_TRUE device_id ";
	       exception_description<<id_device;
           throw spider_exception(exception_description.str());
          };

	 if (PxConfigWriteString( NULL, 
			                    entries_names[PREDEFINED_DIRECTION].c_str(), 
   				                PXCONFIG_FMT_STRING, 
			                    directions_strings_engl[udku->get_pref_direction()].c_str())!=Pt_TRUE){
	       exception_description<<"save_udku_parameters: PxConfigWriteString PREDEFINED_DIRECTION !=Pt_TRUE device_id ";
	       exception_description<<id_device;
           throw spider_exception(exception_description.str());
           };

	 if (PxConfigWriteString( NULL, 
			                    entries_names[START_DIRECTION].c_str(), 
   				                PXCONFIG_FMT_STRING, 
			                    directions_strings_engl[udku->get_start_direction()].c_str())!=Pt_TRUE){
	       exception_description<<"save_udku_parameters: PxConfigWriteString START_DIRECTION !=Pt_TRUE device_id ";
	       exception_description<<id_device;
           throw spider_exception(exception_description.str());
           };

}


void metro_devices_container::save (string file_name)
         throw (spider_exception) {
 	enum {ESCALATOR=0, UDKU,  SHAVR, DOOR, ENTRIES_COUNT};

	ostringstream exception_description;
	string	section_name;
	const char *section_name_c_str;
	vector<string> sections_names(ENTRIES_COUNT);
	sections_names[ESCALATOR]="escalator";
	sections_names[UDKU]="udku";
	sections_names[SHAVR]="shavr";
	sections_names[DOOR]="door";

	if (PxConfigOpen( file_name.c_str(), PXCONFIG_READ|PXCONFIG_WRITE)==Pt_FALSE ){
	       exception_description<<"Can`t open config file "<<file_name;
           throw spider_exception(exception_description.str());
	};

	section_name_c_str=PxConfigNextSection();
	try{
          while (section_name_c_str!=NULL) {
                 section_name=section_name_c_str;

                  if (section_name.compare(sections_names[ESCALATOR])==0) {
                         save_escalator_parameters();
			          };

                  if (section_name.compare(sections_names[UDKU])==0) {
                         save_udku_parameters();
			          };

                  if (section_name.compare(sections_names[SHAVR])==0) {
                         save_shavr_parameters();
			          };

                  if (section_name.compare(sections_names[DOOR])==0) {
                         save_door_parameters();
			          };

			      section_name_c_str=PxConfigNextSection();
	       };	
    } catch (spider_exception spr_exc) {
       exception_description<<spr_exc.get_description();
       if  (PxConfigClose()==Pt_FALSE) {
 	       exception_description<<"\nCan`t close config file "<<file_name;
		 };
		throw spider_exception(exception_description.str());
  };


	if  (PxConfigClose()==Pt_FALSE) {
	       exception_description<<"Can`t close config file "<<file_name;
           throw spider_exception(exception_description.str());
	};
};

/*
void 
	metro_devices_container::load_DEVICE_parameters (int channel) 
                                    throw (spider_exception)

That function reads device`s parameters from config file 
and insert that device into g_devices. 
	id -  id of device. If device with same id present in g_devices, 
function write error message and return false.
	stationID  - id of station in stations container. If not present, device id 
must be added to  g_stations.get_current_station(). If 
metro_stat_obj->get_current_station()==metro_stat_obj->end()  function write error 
message and return false. If stationID present, but station with  stationID
not present in g_stations, function write  error message and return false.
If stationID present in g_stations, device id  must be added to that
station.
	type - type of device in g_metro_device_types. If not present  
function write error message and return false. If type present, 
but in g_metro_device_types not present type with id type, 
function write error message and return false.
	enabed  - enabled state of device. 0 - disabled (default). 
1 - enabled.  If not present  set disabled.
	num - number of device in station.  If not present  
function write error message and return false.
	modbus_num - number of device in  modbus protocol station.  If not present  
function write error message and return false.
	ip - ip address of device.  If not present  
function write error message and return false.
   day_start mode or morning start  - everyday, never,
even days, odd days. See system_settings::START_DAY_MODE_...
for emplimination details.
	start_hour hour of morning start.
   	start_min minute of morning start.
	predef_directoin - predefined direction of device.
See system_settings::DIRECTION_... for details. If not present
or not recognized (not ...REVERSE or .._DOWN or ..._UP)  
function write error message and return false.
*/

void 
	metro_devices_container::load_door_parameters (
                                    int channel) // channel where devices must be attached
                                    throw (spider_exception) {

metro_stations_container *metro_stat_obj=metro_stations_container::get_instance();
metro_devices_types_container *metro_dev_types=metro_devices_types_container::get_instance();
system_settings_spider *sys_sett_obj=system_settings_spider::get_instance();

enum {ID=0, STATION_ID, TYPE, ENABLED, NUMBER, MODBUS_NUMBER,
			IP_ADDRESS, OFFLINE_DELAY, LOG_PACKETS, ENTRIES_COUNT};

ostringstream exception_description;
const char *entry_name_c_str;
string entry_name;

vector<char> temp_str(512);
vector<string> entries_names(ENTRIES_COUNT);

system_settings::strings_container outer_states_strings=sys_sett_obj->get_outer_states_strings();

int id_device=-1, id_station=-1, dev_type=-1, number=-1, modbus_number=-1, enabled=system_settings::DISABLED, log_enabled = system_settings::DISABLED;
in_addr_t ip=INADDR_NONE;
time_t offline_delay = system_settings::OFFLINE_DELAY;

entries_names[ID]="id";
entries_names[STATION_ID]="stationID";
entries_names[TYPE]="type";
entries_names[ENABLED]="enabled";
entries_names[NUMBER]="num";
entries_names[MODBUS_NUMBER]="modbus num";
entries_names[IP_ADDRESS]="ip";
entries_names[OFFLINE_DELAY]="after_offline_delay";
entries_names[LOG_PACKETS]="log_packets";

entry_name_c_str=PxConfigNextString(&temp_str[0], 
							                        temp_str.size()
							                      );

while(entry_name_c_str!=NULL) {
entry_name=entry_name_c_str;

if (entry_name.compare(entries_names[ID])==0) 
	{
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0) {
				if (this->find(temp_int)==this->end())
					{
						id_device=temp_int;
					} else 	{
                       exception_description<<"Device with id  "<<&temp_str[0];
                       exception_description<<" for door already exist in devices container";
                       throw spider_exception(exception_description.str());
					};
			} else {
                       exception_description<<"In load_door_parameters: Wrong device id  ";
                       exception_description<<&temp_str[0];
                       throw spider_exception(exception_description.str());
			};
			
	
	}else if (entry_name.compare(entries_names[STATION_ID])==0) {
	
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0)
			{
				if (metro_stat_obj->find(temp_int)!=metro_stat_obj->end())
					{
						id_station=temp_int;
					} else 	{
                       exception_description<<"In load_door_parameters: Not found device`s station id  ";
                       exception_description<<&temp_str[0];
                       throw spider_exception(exception_description.str());
					};
			} else {
                       exception_description<<"In load_door_parameters: Wrong  device`s station id ";
                       exception_description<<&temp_str[0];
                       throw spider_exception(exception_description.str());
			};
			

	}else if (entry_name.compare(entries_names[TYPE])==0) {

			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0)
			{
				if (metro_dev_types->find(temp_int)!=metro_dev_types->end())
					{
						dev_type=temp_int;
					} else 	{
                       exception_description<<"In load_door_parameters: Not found device`s type id  ";
                       exception_description<<&temp_str[0];
                       throw spider_exception(exception_description.str());
					};
			} else {
                       exception_description<<"In load_door_parameters: Wrong  device`s type id ";
                       exception_description<<&temp_str[0];
                       throw spider_exception(exception_description.str());
			};
			
	} else if (entry_name.compare(entries_names[ENABLED])==0) { 	
			int temp_int;

			temp_int=system_settings::DISABLED;
			if (outer_states_strings[temp_int].compare(&temp_str[0])==0) 
				{
					 enabled= temp_int;					
				} else {
					temp_int=system_settings::ENABLED;
					if (outer_states_strings[temp_int].compare(&temp_str[0])==0) {
							enabled = temp_int;					
						} else {
                           exception_description<<"In load_door_parameters: Wrong device enabled state (enabled/disabled)  ";
                           exception_description<<&temp_str[0];
                           throw spider_exception(exception_description.str());
						};
				};	

	}else if (entry_name.compare(entries_names[NUMBER])==0) { 	
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0) {
						number=temp_int;
			} else {
                   exception_description<<"In load_door_parameters: Wrong device number  ";
                   exception_description<<&temp_str[0];
                    throw spider_exception(exception_description.str());
			};

	}else if (entry_name.compare(entries_names[MODBUS_NUMBER])==0) { 	
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0) {
						modbus_number=temp_int;
			} else {
                   exception_description<<"In load_door_parameters: Wrong device modbus number  ";
                   exception_description<<&temp_str[0];
                    throw spider_exception(exception_description.str());
			};

	}else if (entry_name.compare(entries_names[IP_ADDRESS])==0) {
			ip=inet_addr(&temp_str[0]);
			if (ip==INADDR_NONE) {
                   exception_description<<"In load_door_parameters: Wrong device ip  ";
                   exception_description<<&temp_str[0];
                   throw spider_exception(exception_description.str());
				};

	}else if (entry_name.compare(entries_names[OFFLINE_DELAY])==0) {
			time_t temp_time = static_cast<time_t>(atoi(&temp_str[0]));
			if (temp_time>0) {
						offline_delay=temp_time;
			} else {
                   exception_description<<"In load_door_parameters: Wrong after offline delay value  ";
                   exception_description<<&temp_str[0];
                    throw spider_exception(exception_description.str());
			};

	} else if (entry_name.compare(entries_names[LOG_PACKETS])==0) { 	
			int temp_int;

			temp_int=system_settings::DISABLED;
			if (outer_states_strings[temp_int].compare(&temp_str[0])==0) 
				{
					 log_enabled= temp_int;					
				} else {
					temp_int=system_settings::ENABLED;
					if (outer_states_strings[temp_int].compare(&temp_str[0])==0) {
							log_enabled = temp_int;					
						} else {
                           exception_description<<"In load_door_parameters: Wrong device enabled state (enabled/disabled)  ";
                           exception_description<<&temp_str[0];
                           throw spider_exception(exception_description.str());
						};
				};	

	} else {
            exception_description<<"In load_door_parameters: Unrecognized config entry  ";
            exception_description<<entry_name;
            if (id_device>0) exception_description<<" id="<<id_device;
            throw spider_exception(exception_description.str());
	};
	
entry_name_c_str=PxConfigNextString(&temp_str[0], 
							                        temp_str.size()
							                      );
}; //while 

if (id_device>0 && 
	dev_type>0 &&
	number>0 && 
	modbus_number>0 && 
	ip!=INADDR_NONE &&
	offline_delay > 0 ){
 	if (id_station<0) {
			if (metro_stat_obj->get_current_station()!=metro_stat_obj->end()){
					id_station=metro_stat_obj->get_current_station()->second.get_id();
				} else { //	if (id_station<0) {
                    exception_description<<"In load_door_parameters: Not found current station id for device ";
                     exception_description<<&temp_str[0];
                    if (id_device>0) exception_description<<" id="<<id_device;
                    throw spider_exception(exception_description.str());
				}; // if (id_station<0) 
	}; //if (id_device>0 && 

	metro_stations_container::iterator iter_stat;
	iter_stat=metro_stat_obj->find(id_station);
	if (iter_stat!=metro_stat_obj->end()) {
                  value_type insert_dev_value(id_device,
					                 new metro_door
										 (	id_device,
											id_station,
											number,
											modbus_number,
											dev_type,
											channel,
											enabled,
											ip,
											offline_delay,
											sending_commands_disabled,
											log_enabled) );

				iter_stat->second.push_back_devices_id(id_device); 
				this->set_current_device(this->insert(this->upper_bound(id_device), insert_dev_value));	
			} else { // it`s checked and imosible
                 exception_description<<"Not found station id  "<<id_station;
                 exception_description<<" for device "<<id_device;
                  throw spider_exception(exception_description.str());
			};		

	}  else {//if (id_device>0 &&
           exception_description<<"In load_door_parameters: Not found all of required entries for device";
           throw spider_exception(exception_description.str());
	};
};

void 
	metro_devices_container::load_escalator_parameters (
                                    int channel) // channel where devices must be attached
                                    throw (spider_exception) {

metro_stations_container *metro_stat_obj=metro_stations_container::get_instance();
metro_devices_types_container *metro_dev_types=metro_devices_types_container::get_instance();
system_settings_spider *sys_sett_obj=system_settings_spider::get_instance();

enum {ID=0, STATION_ID, TYPE, ENABLED, NUMBER, MODBUS_NUMBER,
			IP_ADDRESS, START_DAY_MODE, START_HOUR, START_MINUTE, 
			PREDEFINED_DIRECTION, START_DIRECTION, OFFLINE_DELAY,
			CONDUCTION_DELAY, LOG_PACKETS, ENTRIES_COUNT};

ostringstream exception_description;
const char *entry_name_c_str;
string entry_name;

vector<char> temp_str(512);
vector<string> entries_names(ENTRIES_COUNT);

int id_device=-1, id_station=-1, dev_type=-1, number=-1, modbus_number=-1,
	predef_direction=-1, start_direction=-1, start_day_mode=-1, 
	start_hour=-1, start_minute=-1,  enabled=system_settings::DISABLED, log_enabled = system_settings::DISABLED;

in_addr_t ip=INADDR_NONE;
time_t offline_delay = system_settings::OFFLINE_DELAY;
time_t conduct_notif_delay = system_settings::CONDUCTION_DELAY;

system_settings::strings_container directions_strings_engl=sys_sett_obj->get_directions_engl_strings();
system_settings::strings_container start_days_modes_strings_engl=sys_sett_obj->get_start_days_modes_engl_strings();
system_settings::strings_container outer_states_strings=sys_sett_obj->get_outer_states_strings();

entries_names[ID]="id";
entries_names[STATION_ID]="stationID";
entries_names[TYPE]="type";
entries_names[ENABLED]="enabled";
entries_names[NUMBER]="num";
entries_names[MODBUS_NUMBER]="modbus num";
entries_names[IP_ADDRESS]="ip";
entries_names[START_DAY_MODE]="start day mode";
entries_names[START_HOUR]="start hour";
entries_names[START_MINUTE]="start minute";
entries_names[PREDEFINED_DIRECTION]="predef_direction";
entries_names[START_DIRECTION]="start_direction";
entries_names[OFFLINE_DELAY] = "after_offline_delay";
entries_names[CONDUCTION_DELAY] = "conduct_notify_delay";
entries_names[LOG_PACKETS] = "log_packets";

entry_name_c_str=PxConfigNextString(&temp_str[0], 
							                        temp_str.size()
							                      );

while(entry_name_c_str!=NULL)
{
entry_name=entry_name_c_str;

if (entry_name.compare(entries_names[ID])==0) 
	{
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0) {
				if (this->find(temp_int)==this->end()) {
						id_device=temp_int;
					} else 	{
                       exception_description<<"Device with id  "<<&temp_str[0];
                       exception_description<<" for escalator already exist in devices container";
                       throw spider_exception(exception_description.str());
					};
			} else {
                       exception_description<<"In load_escalator_parameters: Wrong device id  ";
                       exception_description<<&temp_str[0];
                       throw spider_exception(exception_description.str());
			};
			
	
	}else if (entry_name.compare(entries_names[STATION_ID])==0) {
	
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0)
			{
				if (metro_stat_obj->find(temp_int)!=metro_stat_obj->end()) {
						id_station=temp_int;
					} else 	{
                       exception_description<<"In load_escalator_parameters: Not found device`s station id  ";
                       exception_description<<&temp_str[0];
                       throw spider_exception(exception_description.str());
					};
			} else {
                       exception_description<<"In load_escalator_parameters: Wrong  device`s station id ";
                       exception_description<<&temp_str[0];
                       throw spider_exception(exception_description.str());
			};
			

	} else if (entry_name.compare(entries_names[TYPE])==0) {

			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0)
			{
				if (metro_dev_types->find(temp_int)!=metro_dev_types->end())
					{
						dev_type=temp_int;
					} else 	{
                       exception_description<<"In load_escalator_parameters: Not found device`s type id  ";
                       exception_description<<&temp_str[0];
                       throw spider_exception(exception_description.str());
					};
			} else {
                       exception_description<<"In load_escalator_parameters: Wrong  device`s type id ";
                       exception_description<<&temp_str[0];
                       throw spider_exception(exception_description.str());
			};

	} else if (entry_name.compare(entries_names[ENABLED])==0) { 	
			int temp_int;

			temp_int=system_settings::DISABLED;
			if (outer_states_strings[temp_int].compare(&temp_str[0])==0) 
				{
					 enabled= temp_int;					
				} else {
					temp_int=system_settings::ENABLED;
					if (outer_states_strings[temp_int].compare(&temp_str[0])==0) 
						{
							enabled = temp_int;					
						} else {
                           exception_description<<"In load_escalator_parameters: Wrong device enabled state (enabled/disabled)  ";
                           exception_description<<&temp_str[0];
                           throw spider_exception(exception_description.str());
						};
				};	

	} else if (entry_name.compare(entries_names[NUMBER])==0) { 	
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0) {
						number=temp_int;
			} else {
                   exception_description<<"In load_escalator_parameters: Wrong device number  ";
                   exception_description<<&temp_str[0];
                    throw spider_exception(exception_description.str());
			};

	} else if (entry_name.compare(entries_names[MODBUS_NUMBER])==0) { 	
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0) {
				modbus_number=temp_int;
			} else {
                exception_description<<"In load_escalator_parameters: Wrong device modbus number  ";
                exception_description<<&temp_str[0];
                throw spider_exception(exception_description.str());
			};

	}else if (entry_name.compare(entries_names[IP_ADDRESS])==0) {
	
			ip=inet_addr(&temp_str[0]);
			if (ip==INADDR_NONE)
				{
                   exception_description<<"In load_escalator_parameters: Wrong device ip  ";
                   exception_description<<&temp_str[0];
                   throw spider_exception(exception_description.str());
				};

	}else if (entry_name.compare(entries_names[START_DAY_MODE])==0) { 	
			int temp_int;

			temp_int=system_settings::START_DAY_MODE_NEVER;
			if (start_days_modes_strings_engl[temp_int].compare(&temp_str[0])==0) 
				{
					start_day_mode = temp_int;					
				} else {
					temp_int=system_settings::START_DAY_MODE_EVERYDAY;
					if (start_days_modes_strings_engl[temp_int].compare(&temp_str[0])==0) 
						{
							start_day_mode = temp_int;					
						} else {
							temp_int=system_settings::START_DAY_MODE_EVEN;
							if (start_days_modes_strings_engl[temp_int].compare(&temp_str[0])==0) 
								{
									start_day_mode = temp_int;					
								} else {
										temp_int=system_settings::START_DAY_MODE_ODD;
										if (start_days_modes_strings_engl[temp_int].compare(&temp_str[0])==0) 
										{
											start_day_mode = temp_int;					
										} else {
                                            exception_description<<"In load_escalator_parameters: Wrong device start day mode  ";
                                            exception_description<<&temp_str[0];
                                             throw spider_exception(exception_description.str());
										};
								};
						};				
				};

	}else if (entry_name.compare(entries_names[START_HOUR])==0) { 	
			int temp_int = atoi(&temp_str[0]);
			if (	temp_int>=system_settings::START_HOUR_MIN &&
				temp_int<=system_settings::START_HOUR_MAX){
						start_hour=temp_int;
			} else {
                          exception_description<<"In load_escalator_parameters: Wrong device start hour  ";
                          exception_description<<&temp_str[0];
                          throw spider_exception(exception_description.str());
			};

	}else if (entry_name.compare(entries_names[START_MINUTE])==0) { 	
			int temp_int = atoi(&temp_str[0]); 
			if (	temp_int>=0 &&  //ATTENTION !!! CAVEATS !!! atoi return 0 if string contain 0 or if string not correct.... device may be started in not correct time.
				temp_int<60){
						start_minute=temp_int;
			} else {
                   exception_description<<"In load_escalator_parameters: Wrong device start minute  ";
                   exception_description<<&temp_str[0];
                   throw spider_exception(exception_description.str());
			};

	}else if (entry_name.compare(entries_names[PREDEFINED_DIRECTION])==0) { 	
			int temp_int;

			temp_int=system_settings::DIRECTION_UP;
			if (directions_strings_engl[temp_int].compare(&temp_str[0])==0) {
					predef_direction = temp_int;					
				} else {
					temp_int=system_settings::DIRECTION_DOWN;
					if (directions_strings_engl[temp_int].compare(&temp_str[0])==0) {
							predef_direction = temp_int;					
						} else {
							temp_int=system_settings::DIRECTION_REVERSE;
							if (directions_strings_engl[temp_int].compare(&temp_str[0])==0) {
									predef_direction = temp_int;					
								} else {
                                    exception_description<<"In load_escalator_parameters: Wrong device predefined direction  ";
                                    exception_description<<&temp_str[0];
                                    throw spider_exception(exception_description.str());
								};
						};				
				};

	}else if (entry_name.compare(entries_names[START_DIRECTION])==0) { 	
			int temp_int;

			temp_int=system_settings::DIRECTION_UP;
			if (directions_strings_engl[temp_int].compare(&temp_str[0])==0) {
					start_direction = temp_int;	
				} else {
					temp_int=system_settings::DIRECTION_DOWN;
					if (directions_strings_engl[temp_int].compare(&temp_str[0])==0) {
							start_direction = temp_int;					
						} else {
                             exception_description<<"In load_escalator_parameters: Wrong device start direction  ";
                             exception_description<<&temp_str[0];
                             throw spider_exception(exception_description.str());
						};				
				};
	}else if (entry_name.compare(entries_names[OFFLINE_DELAY])==0) {
			time_t temp_time = static_cast<time_t>(atoi(&temp_str[0]));
			if (temp_time>0) {
						offline_delay=temp_time;
			} else {
                   exception_description<<"In load_escalator_parameters: Wrong after offline delay value  ";
                   exception_description<<&temp_str[0];
                    throw spider_exception(exception_description.str());
			};
	}else if (entry_name.compare(entries_names[CONDUCTION_DELAY])==0) {
			time_t temp_time = static_cast<time_t>(atoi(&temp_str[0]));
			if (temp_time>0) {
						conduct_notif_delay=temp_time;
			} else {
                   exception_description<<"In load_escalator_parameters: Wrong after conduction notification delay value  ";
                   exception_description<<&temp_str[0];
                    throw spider_exception(exception_description.str());
			};

	} else if (entry_name.compare(entries_names[LOG_PACKETS])==0) { 	
			int temp_int;

			temp_int=system_settings::DISABLED;
			if (outer_states_strings[temp_int].compare(&temp_str[0])==0) 
				{
					 log_enabled= temp_int;					
				} else {
					temp_int=system_settings::ENABLED;
					if (outer_states_strings[temp_int].compare(&temp_str[0])==0) {
							log_enabled = temp_int;					
						} else {
                           exception_description<<"In load_door_parameters: Wrong device enabled state (enabled/disabled)  ";
                           exception_description<<&temp_str[0];
                           throw spider_exception(exception_description.str());
						};
				};	


	} else {
            exception_description<<"In load_escalator_parameters: Unrecognized config entry  ";
            exception_description<<entry_name;
            if (id_device>0) exception_description<<" id="<<id_device;
            throw spider_exception(exception_description.str());
	};
	
entry_name_c_str=PxConfigNextString(&temp_str[0], 
							                        temp_str.size()
							                      );
}; //while 

if (id_device>0 && 
	dev_type>0 &&
	number>0 && 
	modbus_number>0 && 
	start_day_mode>=0 &&
	start_hour>0 &&
	start_minute>=0 &&
	predef_direction>0 &&
	start_direction>0 &&
	ip!=INADDR_NONE &&
	offline_delay > 0 &&
	conduct_notif_delay > 0){
 	if (id_station<0) {
			if (metro_stat_obj->get_current_station()!=metro_stat_obj->end()){
					id_station=metro_stat_obj->get_current_station()->second.get_id();
				} else { //	if (id_station<0) {
                    exception_description<<"In load_escalator_parameters: Not found current station id for device ";
                     exception_description<<&temp_str[0];
                    throw spider_exception(exception_description.str());
				}; // if (id_station<0) 
	}; //if (id_device>0 && 

	metro_stations_container::iterator iter_stat;
	iter_stat=metro_stat_obj->find(id_station);
	if (iter_stat!=metro_stat_obj->end()) {
                  value_type insert_dev_value(id_device,
					                 new metro_escalator
										 (	id_device,
											id_station,
											number,
											modbus_number,
											dev_type,
											predef_direction,
                                            start_day_mode,
											start_hour,
											start_minute,
											start_direction,
											channel,
											enabled,
											ip,
											offline_delay,
											conduct_notif_delay,
											sending_commands_disabled,
											log_enabled) );

				iter_stat->second.push_back_devices_id(id_device); 
				this->set_current_device(this->insert(this->upper_bound(id_device), insert_dev_value));	
			} else { // it`s checked and imosible
                 exception_description<<"In load_escalator_parameters: Not found station id  "<<id_station;
                 exception_description<<" for device "<<id_device;
                  throw spider_exception(exception_description.str());
			};		

	}  else {//if (id_device>0 &&
           exception_description<<"In load_escalator_parameters: Not found all of required entries for device";
           throw spider_exception(exception_description.str());
	};
};

void 
	metro_devices_container::load_shavr_parameters (
                                    int channel ) // channel where devices must be attached
                                    throw (spider_exception) {

metro_stations_container *metro_stat_obj=metro_stations_container::get_instance();
metro_devices_types_container *metro_dev_types=metro_devices_types_container::get_instance();
system_settings_spider *sys_sett_obj=system_settings_spider::get_instance();

enum {ID=0, STATION_ID, TYPE, ENABLED, NUMBER, MODBUS_NUMBER,  ESCALATOR_ID,
			IP_ADDRESS, START_DAY_MODE, START_HOUR, START_MINUTE, OFFLINE_DELAY,
			LOG_PACKETS,  ENTRIES_COUNT};

ostringstream exception_description;
const char *entry_name_c_str;
string entry_name;

vector<char> temp_str(512);
vector<string> entries_names(ENTRIES_COUNT);

int id_device=-1, id_station=-1, dev_type=-1, number=-1, modbus_number=-1,
	 start_day_mode=-1, start_hour=-1, start_minute=-1,  enabled=system_settings::DISABLED, 
	 log_enabled=system_settings::DISABLED;

in_addr_t ip=INADDR_NONE;
time_t offline_delay = system_settings::OFFLINE_DELAY;

metro_device::related_devices_ids escalators_ids(0);

system_settings::strings_container directions_strings_engl=sys_sett_obj->get_directions_engl_strings();
system_settings::strings_container start_days_modes_strings_engl=sys_sett_obj->get_start_days_modes_engl_strings();
system_settings::strings_container outer_states_strings=sys_sett_obj->get_outer_states_strings();

entries_names[ID]="id";
entries_names[STATION_ID]="stationID";
entries_names[TYPE]="type";
entries_names[ENABLED]="enabled";
entries_names[NUMBER]="num";
entries_names[MODBUS_NUMBER]="modbus num";
entries_names[ESCALATOR_ID]="escalator id";
entries_names[IP_ADDRESS]="ip";
entries_names[START_DAY_MODE]="start day mode";
entries_names[START_HOUR]="start hour";
entries_names[START_MINUTE]="start minute";
entries_names[OFFLINE_DELAY] = "after_offline_delay";
entries_names[LOG_PACKETS] = "log_packets";

entry_name_c_str=PxConfigNextString(&temp_str[0], 
							                        temp_str.size()
							                      );

while(entry_name_c_str!=NULL)
{
entry_name=entry_name_c_str;

if (entry_name.compare(entries_names[ID])==0) {
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0) {
				if (this->find(temp_int)==this->end())
					{
						id_device=temp_int;
					} else 	{
                       exception_description<<"In load_shavr_parameters: Device with id  "<<&temp_str[0];
                       exception_description<<" for shavr already exist in devices container";
                       throw spider_exception(exception_description.str());
					};
			} else {
                       exception_description<<"In load_shavr_parameters: Wrong device id  ";
                       exception_description<<&temp_str[0];
                       throw spider_exception(exception_description.str());
			};
			
	
	} else if (entry_name.compare(entries_names[STATION_ID])==0) {
	
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0)
			{
				if (metro_stat_obj->find(temp_int)!=metro_stat_obj->end())
					{
						id_station=temp_int;
					} else 	{
                       exception_description<<"In load_shavr_parameters: Not found device`s station id  ";
                       exception_description<<&temp_str[0];
                       throw spider_exception(exception_description.str());
					};
			} else {
                       exception_description<<"In load_shavr_parameters: Wrong  device`s station id ";
                       exception_description<<&temp_str[0];
                       throw spider_exception(exception_description.str());
			};
			

	} else if (entry_name.compare(entries_names[TYPE])==0) {
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0) {
				if (metro_dev_types->find(temp_int)!=metro_dev_types->end())
					{
						dev_type=temp_int;
					} else 	{
                       exception_description<<"In load_shavr_parameters: Not found device`s type id  ";
                       exception_description<<&temp_str[0];
                       throw spider_exception(exception_description.str());
					};
			} else {
                       exception_description<<"In load_shavr_parameters: Wrong  device`s type id ";
                       exception_description<<&temp_str[0];
                       throw spider_exception(exception_description.str());
			};
			
	} else if (entry_name.compare(entries_names[ENABLED])==0) { 	
			int temp_int;

			temp_int=system_settings::DISABLED;
			if (outer_states_strings[temp_int].compare(&temp_str[0])==0) 
				{
					 enabled= temp_int;					
				} else {
					temp_int=system_settings::ENABLED;
					if (outer_states_strings[temp_int].compare(&temp_str[0])==0) 
						{
							enabled = temp_int;					
						} else {
                           exception_description<<"In load_shavr_parameters: Wrong device enabled state (enabled/disabled)  ";
                           exception_description<<&temp_str[0];
                           throw spider_exception(exception_description.str());
						};
				};	

	} else if (entry_name.compare(entries_names[ESCALATOR_ID])==0) { 	

			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0)
			{
				if (this->find(temp_int)!=this->end()){
						escalators_ids.push_back(temp_int);
					} else 	{
                       exception_description<<"In load_shavr_parameters: Not found related device`s id  ";
                       exception_description<<&temp_str[0];
                       throw spider_exception(exception_description.str());
					};
			} else {
                       exception_description<<"In load_shavr_parameters: Wrong related device`s id ";
                       exception_description<<&temp_str[0];
                       throw spider_exception(exception_description.str());
			};

	} else if (entry_name.compare(entries_names[NUMBER])==0) { 	
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0){
						number=temp_int;
			} else {
                   exception_description<<"In load_shavr_parameters: Wrong device number  ";
                   exception_description<<&temp_str[0];
                    throw spider_exception(exception_description.str());
			};

	} else if (entry_name.compare(entries_names[MODBUS_NUMBER])==0) { 	
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0){
				modbus_number=temp_int;
			} else {
                exception_description<<"In load_shavr_parameters: Wrong device modbus number  ";
                exception_description<<&temp_str[0];
                 throw spider_exception(exception_description.str());
			};
	}else if (entry_name.compare(entries_names[IP_ADDRESS])==0) {
			ip=inet_addr(&temp_str[0]);
			if (ip==INADDR_NONE) {
                   exception_description<<"In load_shavr_parameters: Wrong device ip  ";
                   exception_description<<&temp_str[0];
                   throw spider_exception(exception_description.str());
				};

	}else if (entry_name.compare(entries_names[START_DAY_MODE])==0) { 	
			int temp_int;

			temp_int=system_settings::START_DAY_MODE_NEVER;
			if (start_days_modes_strings_engl[temp_int].compare(&temp_str[0])==0) 
				{
					start_day_mode = temp_int;					
				} else {
					temp_int=system_settings::START_DAY_MODE_EVERYDAY;
					if (start_days_modes_strings_engl[temp_int].compare(&temp_str[0])==0) 
						{
							start_day_mode = temp_int;					
						} else {
							temp_int=system_settings::START_DAY_MODE_EVEN;
							if (start_days_modes_strings_engl[temp_int].compare(&temp_str[0])==0) 
								{
									start_day_mode = temp_int;					
								} else {
										temp_int=system_settings::START_DAY_MODE_ODD;
										if (start_days_modes_strings_engl[temp_int].compare(&temp_str[0])==0) 
										{
											start_day_mode = temp_int;					
										} else {
                                            exception_description<<"In load_shavr_parameters: Wrong device start day mode  ";
                                            exception_description<<&temp_str[0];
                                             throw spider_exception(exception_description.str());
										};
								};
						};				
				};

	} else if (entry_name.compare(entries_names[START_HOUR])==0) { 	
			int temp_int = atoi(&temp_str[0]);
			if (	temp_int>=system_settings::START_HOUR_MIN &&
				temp_int<=system_settings::START_HOUR_MAX) {
						start_hour=temp_int;
			} else {
                          exception_description<<"In load_shavr_parameters: Wrong device start hour  ";
                          exception_description<<&temp_str[0];
                          throw spider_exception(exception_description.str());
			};

	} else if (entry_name.compare(entries_names[START_MINUTE])==0) { 	
			int temp_int = atoi(&temp_str[0]); 
			if (	temp_int>=0 &&  //ATTENTION !!! CAVEATS !!! atoi return 0 if string contain 0 or if string not correct.... device may be started in not correct time.
				temp_int<60)
			{
						start_minute=temp_int;
			} else {
                   exception_description<<"In load_shavr_parameters: Wrong device start minute  ";
                   exception_description<<&temp_str[0];
                   throw spider_exception(exception_description.str());
			};

	}else if (entry_name.compare(entries_names[OFFLINE_DELAY])==0) {
			time_t temp_time = static_cast<time_t>(atoi(&temp_str[0]));
			if (temp_time>0) {
						offline_delay=temp_time;
			} else {
                   exception_description<<"In load_shavr_parameters: Wrong after offline delay value  ";
                   exception_description<<&temp_str[0];
                    throw spider_exception(exception_description.str());
			};

	} else if (entry_name.compare(entries_names[LOG_PACKETS])==0) { 	
			int temp_int;

			temp_int=system_settings::DISABLED;
			if (outer_states_strings[temp_int].compare(&temp_str[0])==0) 
				{
					 log_enabled= temp_int;					
				} else {
					temp_int=system_settings::ENABLED;
					if (outer_states_strings[temp_int].compare(&temp_str[0])==0) {
							log_enabled = temp_int;					
						} else {
                           exception_description<<"In load_door_parameters: Wrong device enabled state (enabled/disabled)  ";
                           exception_description<<&temp_str[0];
                           throw spider_exception(exception_description.str());
						};
				};	

	} else {
            exception_description<<"In load_shavr_parameters: Unrecognized config entry  ";
            exception_description<<entry_name;
            throw spider_exception(exception_description.str());
	};
	
entry_name_c_str=PxConfigNextString(&temp_str[0], 
							                        temp_str.size()
							                      );
}; //while 

if (id_device>0 && 
	dev_type>0 &&
	number>0 && 
    modbus_number>0 &&
	start_day_mode>=0 &&
	start_hour>0 &&
	start_minute>=0 &&
	ip!=INADDR_NONE &&
	offline_delay > 0) {
	if (id_station<0) 
			{
			if (metro_stat_obj->get_current_station()!=metro_stat_obj->end())
				{
					id_station=metro_stat_obj->get_current_station()->second.get_id();
				} else {
                    exception_description<<"In load_shavr_parameters: Not found current station id for device ";
                     exception_description<<&temp_str[0];
                    throw spider_exception(exception_description.str());
				};
			};

	metro_stations_container::iterator iter_stat;
	iter_stat=metro_stat_obj->find(id_station);
	if (iter_stat!=metro_stat_obj->end()) {
                      value_type ins_device_value (
                                           id_device,
								           new metro_shavr
														 (	id_device, 
															id_station,
															number,
                                                            modbus_number,
															dev_type,
															start_day_mode,
															start_hour,
															start_minute,
															channel,
															enabled,
															ip,
															offline_delay,
															sending_commands_disabled,
															log_enabled) );

				iter_stat->second.push_back_devices_id(id_device); 
				this->set_current_device(this->insert(this->upper_bound(id_device), ins_device_value));
            	metro_devices_container::iterator curr_dev_iter= this->get_current_device();
                for (metro_device::related_devices_ids::size_type i=0;
                                                                    i<escalators_ids.size();
                                                                   i++) 
                        curr_dev_iter->second->push_back_device_id(escalators_ids[i]);

			} else { // it`s checked and imosible
                 exception_description<<"In load_shavr_parameters: Not found station id  "<<id_station;
                 exception_description<<" for device "<<id_device;
                  throw spider_exception(exception_description.str());
			};		

	}  else {//if (id_device>0 &&
           exception_description<<"In load_shavr_parameters: Not found all of required entries for device";
           throw spider_exception(exception_description.str());
	};
};


void 
	metro_devices_container::load_udku_parameters (
                                    int channel) // channel where devices must be attached
                                    throw (spider_exception) {

metro_stations_container *metro_stat_obj=metro_stations_container::get_instance();
metro_devices_types_container *metro_dev_types=metro_devices_types_container::get_instance();
system_settings_spider *sys_sett_obj=system_settings_spider::get_instance();

enum {ID=0, STATION_ID, TYPE, ENABLED, NUMBER, MODBUS_NUMBER,
			IP_ADDRESS, START_DAY_MODE, START_HOUR, START_MINUTE, 
			PREDEFINED_DIRECTION, START_DIRECTION, OFFLINE_DELAY, 
			CONDUCTION_DELAY, LOG_PACKETS, ENTRIES_COUNT};

ostringstream exception_description;
const char *entry_name_c_str;
string entry_name;

vector<char> temp_str(512);
vector<string> entries_names(ENTRIES_COUNT);

int id_device=-1, id_station=-1, dev_type=-1, number=-1, modbus_number=-1,
	predef_direction=-1, start_day_mode=-1, 
	start_hour=-1, start_minute=-1, start_direction=-1, enabled=system_settings::DISABLED,
	log_enabled=system_settings::DISABLED;

in_addr_t ip=INADDR_NONE;
time_t offline_delay = system_settings::OFFLINE_DELAY;
time_t conduct_notif_delay = system_settings::CONDUCTION_DELAY;

system_settings::strings_container directions_strings_engl=sys_sett_obj->get_directions_engl_strings();
system_settings::strings_container start_days_modes_strings_engl=sys_sett_obj->get_start_days_modes_engl_strings();
system_settings::strings_container outer_states_strings=sys_sett_obj->get_outer_states_strings();

entries_names[ID]="id";
entries_names[STATION_ID]="stationID";
entries_names[TYPE]="type";
entries_names[ENABLED]="enabled";
entries_names[NUMBER]="num";
entries_names[MODBUS_NUMBER]="modbus num";
entries_names[IP_ADDRESS]="ip";
entries_names[START_DAY_MODE]="start day mode";
entries_names[START_HOUR]="start hour";
entries_names[START_MINUTE]="start minute";
entries_names[PREDEFINED_DIRECTION]="predef_direction";
entries_names[START_DIRECTION]="start_direction";
entries_names[OFFLINE_DELAY]= "after_offline_delay";
entries_names[CONDUCTION_DELAY] = "conduct_notify_delay";
entries_names[LOG_PACKETS] = "log_packets";

entry_name_c_str=PxConfigNextString(&temp_str[0], 
							                        temp_str.size() );

while(entry_name_c_str!=NULL) {
entry_name=entry_name_c_str;

if (entry_name.compare(entries_names[ID])==0) {
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0) {
				if (this->find(temp_int)==this->end()) {
						id_device=temp_int;
					} else 	{
                       exception_description<<"In load_udku_parameters: Device with id  "<<&temp_str[0];
                       exception_description<<" for udku already exist in devices container";
                       throw spider_exception(exception_description.str());
					};
			} else {
                       exception_description<<"In load_udku_parameters: Wrong device id  ";
                       exception_description<<&temp_str[0];
                       throw spider_exception(exception_description.str());
			};
			
	
	} else if (entry_name.compare(entries_names[STATION_ID])==0) {
	
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0) {
				if (metro_stat_obj->find(temp_int)!=metro_stat_obj->end()) {
						id_station=temp_int;
					} else 	{
                       exception_description<<"In load_udku_parameters: Not found device`s station id  ";
                       exception_description<<&temp_str[0];
                       throw spider_exception(exception_description.str());
					};
			} else {
                       exception_description<<"In load_udku_parameters: Wrong  device`s station id ";
                       exception_description<<&temp_str[0];
                       throw spider_exception(exception_description.str());
			};

	} else if (entry_name.compare(entries_names[TYPE])==0) {

			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0) {
				if (metro_dev_types->find(temp_int)!=metro_dev_types->end()) {
						dev_type=temp_int;
					} else 	{
                       exception_description<<"In load_udku_parameters: Not found device`s type id  ";
                       exception_description<<&temp_str[0];
                       throw spider_exception(exception_description.str());
					};
			} else {
                       exception_description<<"In load_udku_parameters: Wrong  device`s type id ";
                       exception_description<<&temp_str[0];
                       throw spider_exception(exception_description.str());
			};
			
	} else if (entry_name.compare(entries_names[ENABLED])==0) { 	
			int temp_int;

			temp_int=system_settings::DISABLED;
			if (outer_states_strings[temp_int].compare(&temp_str[0])==0) {
					 enabled= temp_int;					
				} else {
					temp_int=system_settings::ENABLED;
					if (outer_states_strings[temp_int].compare(&temp_str[0])==0) {
							enabled = temp_int;					
						} else {
                           exception_description<<"In load_udku_parameters: Wrong device enabled state (enabled/disabled)  ";
                           exception_description<<&temp_str[0];
                           throw spider_exception(exception_description.str());
						};
				};	

	} else if (entry_name.compare(entries_names[NUMBER])==0) {
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0) {
						number=temp_int;
			} else {
                   exception_description<<"In load_udku_parameters: Wrong device number  ";
                   exception_description<<&temp_str[0];
                    throw spider_exception(exception_description.str());
			};

	} else if (entry_name.compare(entries_names[MODBUS_NUMBER])==0) {
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0) {
						modbus_number=temp_int;
			} else {
                   exception_description<<"In load_udku_parameters: Wrong device modbus number  ";
                   exception_description<<&temp_str[0];
                    throw spider_exception(exception_description.str());
			};

	} else if (entry_name.compare(entries_names[IP_ADDRESS])==0) {
			ip=inet_addr(&temp_str[0]);
			if (ip==INADDR_NONE) {
                   exception_description<<"In load_udku_parameters: Wrong device ip  ";
                   exception_description<<&temp_str[0];
                   throw spider_exception(exception_description.str());
				};

	} else if (entry_name.compare(entries_names[START_DAY_MODE])==0) { 	
			int temp_int;

			temp_int=system_settings::START_DAY_MODE_NEVER;
			if (start_days_modes_strings_engl[temp_int].compare(&temp_str[0])==0) {
					start_day_mode = temp_int;					
				} else {
					temp_int=system_settings::START_DAY_MODE_EVERYDAY;
					if (start_days_modes_strings_engl[temp_int].compare(&temp_str[0])==0) {
							start_day_mode = temp_int;					
						} else {
							temp_int=system_settings::START_DAY_MODE_EVEN;
							if (start_days_modes_strings_engl[temp_int].compare(&temp_str[0])==0) {
									start_day_mode = temp_int;					
								} else {
										temp_int=system_settings::START_DAY_MODE_ODD;
										if (start_days_modes_strings_engl[temp_int].compare(&temp_str[0])==0) {
											start_day_mode = temp_int;					
										} else {
                                            exception_description<<"In load_udku_parameters: Wrong device start day mode  ";
                                            exception_description<<&temp_str[0];
                                             throw spider_exception(exception_description.str());
										};
								};
						};				
				};

	} else if (entry_name.compare(entries_names[START_HOUR])==0) { 	
			int temp_int = atoi(&temp_str[0]);
			if (	temp_int>=system_settings::START_HOUR_MIN &&
				temp_int<=system_settings::START_HOUR_MAX) {
						start_hour=temp_int;
			} else {
                          exception_description<<"In load_udku_parameters: Wrong device start hour  ";
                          exception_description<<&temp_str[0];
                          throw spider_exception(exception_description.str());
			};

	} else if (entry_name.compare(entries_names[START_MINUTE])==0) {
			int temp_int = atoi(&temp_str[0]); 
			if (	temp_int>=0 &&  //ATTENTION !!! CAVEATS !!! atoi return 0 if string contain 0 or if string not correct.... device may be started in not correct time.
				temp_int<60) {
						start_minute=temp_int;
			} else {
                   exception_description<<"In load_udku_parameters: Wrong device start minute  ";
                   exception_description<<&temp_str[0];
                   throw spider_exception(exception_description.str());
			};

	} else if (entry_name.compare(entries_names[PREDEFINED_DIRECTION])==0) {
			int temp_int;

			temp_int=system_settings::DIRECTION_UP;
			if (directions_strings_engl[temp_int].compare(&temp_str[0])==0) {
					predef_direction = temp_int;					
				} else {
					temp_int=system_settings::DIRECTION_DOWN;
					if (directions_strings_engl[temp_int].compare(&temp_str[0])==0) {
							predef_direction = temp_int;					
						} else {
							temp_int=system_settings::DIRECTION_REVERSE;
							if (directions_strings_engl[temp_int].compare(&temp_str[0])==0) {
									predef_direction = temp_int;					
								} else {
                                    exception_description<<"In load_udku_parameters: Wrong device predefined direction  ";
                                    exception_description<<&temp_str[0];
                                    throw spider_exception(exception_description.str());
								};
						};				
				};

	} else if (entry_name.compare(entries_names[START_DIRECTION])==0) {
			int temp_int;
			temp_int=system_settings::DIRECTION_UP;
			if (directions_strings_engl[temp_int].compare(&temp_str[0])==0) {
					start_direction = temp_int;	
				} else {
					temp_int=system_settings::DIRECTION_DOWN;
					if (directions_strings_engl[temp_int].compare(&temp_str[0])==0) {
							start_direction = temp_int;
                         } else {
                               exception_description<<"In load_udku_parameters: Wrong device start direction  ";
                               exception_description<<&temp_str[0];
                                throw spider_exception(exception_description.str());
						};				
				};
	}else if (entry_name.compare(entries_names[OFFLINE_DELAY])==0) {
			time_t temp_time = static_cast<time_t>(atoi(&temp_str[0]));
			if (temp_time>0) {
						offline_delay=temp_time;
			} else {
                   exception_description<<"In load_udku_parameters: Wrong after offline delay value  ";
                   exception_description<<&temp_str[0];
                    throw spider_exception(exception_description.str());
			};

	}else if (entry_name.compare(entries_names[CONDUCTION_DELAY])==0) {
			time_t temp_time = static_cast<time_t>(atoi(&temp_str[0]));
			if (temp_time>0) {
						conduct_notif_delay=temp_time;
			} else {
                   exception_description<<"In load_udku_parameters: Wrong after conduction notification delay value  ";
                   exception_description<<&temp_str[0];
                    throw spider_exception(exception_description.str());
			};

	} else if (entry_name.compare(entries_names[LOG_PACKETS])==0) { 	
			int temp_int;

			temp_int=system_settings::DISABLED;
			if (outer_states_strings[temp_int].compare(&temp_str[0])==0) 
				{
					 log_enabled= temp_int;					
				} else {
					temp_int=system_settings::ENABLED;
					if (outer_states_strings[temp_int].compare(&temp_str[0])==0) {
							log_enabled = temp_int;					
						} else {
                           exception_description<<"In load_door_parameters: Wrong device enabled state (enabled/disabled)  ";
                           exception_description<<&temp_str[0];
                           throw spider_exception(exception_description.str());
						};
				};	

	} else {
            exception_description<<"In load_udku_parameters: Unrecognized config entry  ";
            exception_description<<entry_name;
            if (id_device>0) exception_description<<" id="<<id_device;
            throw spider_exception(exception_description.str());
	};
	
entry_name_c_str=PxConfigNextString(&temp_str[0], 
							                        temp_str.size()
							                      );
}; //while 

if (id_device>0 && 
	dev_type>0 &&
	number>0 && 
	modbus_number>0 && 
	start_day_mode>=0 &&
	start_hour>0 &&
	start_minute>=0 &&
	predef_direction>0 &&
	start_direction>0 &&
	ip!=INADDR_NONE &&
	offline_delay > 0 &&
	conduct_notif_delay > 0 ){
 	if (id_station<0) {
			if (metro_stat_obj->get_current_station()!=metro_stat_obj->end()){
					id_station=metro_stat_obj->get_current_station()->second.get_id();
				} else { //	if (id_station<0) {
                    exception_description<<"In load_udku_parameters: Not found current station id for device ";
                     exception_description<<&temp_str[0];
                    throw spider_exception(exception_description.str());
				}; // if (id_station<0) 
	}; //if (id_device>0 && 

	metro_stations_container::iterator iter_stat;
	iter_stat=metro_stat_obj->find(id_station);
	if (iter_stat!=metro_stat_obj->end()) {
                  value_type insert_dev_value(id_device,
					                 new metro_udku
										 (	id_device,
											id_station,
											number,
											modbus_number,
											dev_type,
											predef_direction,
                                            start_day_mode,
											start_hour,
											start_minute,
											start_direction,
											channel,
											enabled,
											ip,
											offline_delay,
											conduct_notif_delay,
											sending_commands_disabled,
											log_enabled) );

				iter_stat->second.push_back_devices_id(id_device); 
				this->set_current_device(this->insert(this->upper_bound(id_device), insert_dev_value));	
			} else { // it`s checked and imosible
                 exception_description<<"In load_udku_parameters: Not found station id  "<<id_station;
                 exception_description<<" for device "<<id_device;
                  throw spider_exception(exception_description.str());
			};		

	}  else {//if (id_device>0 &&
           exception_description<<"In load_udku_parameters: Not found all of required entries for device";
           throw spider_exception(exception_description.str());
	};
};


//In spider program load now not used, see load_map in main.cpp
void metro_devices_container::load (
        string file_name,
        int channel) throw (spider_exception) {

 	enum {ESCALATOR=0, SHAVR, UDKU, DOOR, ENTRIES_COUNT};
	ostringstream exception_description;
	string	section_name;
	const char *section_name_c_str;
	vector<string> sections_names(ENTRIES_COUNT);

	sections_names[ESCALATOR]="escalator";
	sections_names[SHAVR]="shavr";
	sections_names[UDKU]="udku";
	sections_names[DOOR]="door";

	container_metro_devices.clear();

	if (PxConfigOpen( file_name.c_str(), PXCONFIG_READ)==Pt_FALSE ){
         exception_description<<"Can`t open config file "<<file_name;
         throw spider_exception(exception_description.str());
	};

    try {
          section_name_c_str=PxConfigNextSection();
          while (section_name_c_str!=NULL) {
              section_name=section_name_c_str;
              if (section_name.compare(sections_names[ESCALATOR])==0) {
                   load_escalator_parameters ( channel);
              } else  if (section_name.compare(sections_names[SHAVR])==0)  {
                   load_shavr_parameters ( channel);
              } else  if (section_name.compare(sections_names[UDKU])==0)  {
                   load_udku_parameters ( channel);
              } else  if (section_name.compare(sections_names[DOOR])==0)  {
                   load_door_parameters ( channel);
               } else {
                     exception_description<<"Unrecognized config section "<<section_name;
                     throw spider_exception(exception_description.str());
                 };
                  section_name_c_str=PxConfigNextSection();
              }; //	 while (section_name_c_str!=NULL)

   } catch (spider_exception spr_exc) {
    	 exception_description<<spr_exc.get_description();
         if  (PxConfigClose()==Pt_FALSE) {
              exception_description<<"\nCan`t close config file "<<file_name;
          };
         throw spider_exception(exception_description.str());                
   };

	if  (PxConfigClose()==Pt_FALSE) {
        exception_description<<"Can`t close config file "<<file_name;
        throw spider_exception(exception_description.str());
	};
}; 

void metro_devices_container::prepare_timer_command()throw (spider_exception){
timer_command_container* timer_cmd_cont=timer_command_container::get_instance();
	make_timer_command *maker_of_timer_commands=	make_timer_command::get_instance();
	maker_of_timer_commands->initialize_day();
	timer_cmd_cont->erase_all();
	for_each(container_metro_devices.begin(),
              container_metro_devices.end(),
              *maker_of_timer_commands);
	timer_cmd_cont->sort_all();
};

void metro_devices_container::execute_timer_commands(
                            int start_hour,
                            int start_minute){
timer_command_container* timer_cmd_cont=timer_command_container::get_instance();
	find_timer_command 
         *finder_timer_commands=find_timer_command::get_instance();
	finder_timer_commands->initialize_time_to_find (
                                start_hour,
								start_minute);

	for_each(timer_cmd_cont->begin(),
				   timer_cmd_cont->end(),
				   *finder_timer_commands);
};

void metro_devices_container::set_time() {
metro_devices_container *devices=get_instance();
make_set_time set_time_functor;
if (devices!=NULL) {
  	  for_each(devices->begin(),
                     devices->end(),
 				      set_time_functor);
       } ; //if (devices!=NULL)
};
