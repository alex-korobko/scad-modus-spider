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

#include <vector>
#include <string>
#include <hash_map>
#include <list>
#include <set>
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
#include "line.h"
#include "metro_lines_container.h"
#include "station.h"
#include "metro_stations_container.h"
#include "metro_devices_types_container.h"
#include "metro_device.h"
#include "metro_devices_container.h"
#include "data_block.h"
#include "shavr_data_block.h"
#include "shavr.h"

metro_stations_container* 
        metro_stations_container::metro_stations_container_instance=NULL;

PtWidget_t* metro_stations_container::metro_station_devices_properties_panel=NULL;
PtWidget_t* metro_stations_container::metro_station_devices_properties_raw_list=NULL;

metro_stations_container* metro_stations_container::get_instance(){
    if (metro_stations_container_instance==NULL)
                 metro_stations_container_instance=new metro_stations_container();
   return metro_stations_container_instance;
};

/*
void 
	load_station_parameters () 
	throw (spider_exception);

That function reads station`s parameters from config file 
and insert that station into metro_stations_container. 
	id -  id of station. If station with same id present in g_stadions, 
function write error message and return false.
	name -  name of station. If not present,  
function write error message and return false.
	lineID  - id of line in lines_container. If not present, station id 
must be added to  lines_container.get_current_line(). If 
lines_container.get_current_line()==lines_container.end()  function write error 
message and return false. If lineID is present, but line with  lineID
not present in g_lines, function write  error message and return false.
If lineID present in g_lines, station id  must be added to that
line.
	x - x coordinate of station on main_wnd::Sheme widget. If not present,
function write error message and return false.
	y - y coordinate of station on main_wnd::Sheme widget. If not present,
function write error message and return false.
*/

void metro_stations_container::load_station_parameters () throw (spider_exception) {
enum {ID=0, NAME, LINE_ID, X_COORD, Y_COORD, WAV_FILE, ENTRIES_COUNT};
ostringstream exception_description;
const char *entry_name_c_str;
string entry_name, name_station, wave_file_name;
vector<char> temp_str(512);
vector<string> entries_names(ENTRIES_COUNT);
int id_station(-1), id_line(-1), x_coord(-1), y_coord(-1);
metro_lines_container *metro_lines=metro_lines_container::get_instance();

entries_names[ID]="id";
entries_names[NAME]="name";
entries_names[LINE_ID]="line id";
entries_names[X_COORD]="x";
entries_names[Y_COORD]="y";
entries_names[WAV_FILE]="wav file";

entry_name_c_str=PxConfigNextString(&temp_str[0], 
							                        temp_str.size());

while(entry_name_c_str!=NULL) {
entry_name=entry_name_c_str;

if (entry_name.compare(entries_names[ID])==0) {
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0){
				if (this->find(temp_int)==this->end()){
						id_station=temp_int;
					} else 	{
                       exception_description << "Station with id  "<<id_station<<" already exist in stations container";
                       throw spider_exception(exception_description.str());
					};
			} else {
                       exception_description << "Wrong station id  "<<&temp_str[0];
                       throw spider_exception(exception_description.str());
			};
	
	} else if (entry_name.compare(entries_names[NAME])==0) {
			name_station=&temp_str[0];
	} else if (entry_name.compare(entries_names[WAV_FILE])==0) {
			wave_file_name=&temp_str[0];
	}  else if (entry_name.compare(entries_names[LINE_ID])==0) {
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0){
						if(metro_lines->find(temp_int)!=metro_lines->end()){
								id_line=temp_int;
							} else {
                              exception_description << "Not found station`s line id  "<<temp_int;
                              throw spider_exception(exception_description.str());
							};
			} else {
               exception_description << "Wrong station`s line id "<<&temp_str[0];
               throw spider_exception(exception_description.str());
			};

	}  else if (entry_name.compare(entries_names[X_COORD])==0) {
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0){
						x_coord=temp_int;
			} else {
               exception_description << "Wrong station`s X coordinate "<<temp_int;
               throw spider_exception(exception_description.str());
			};

	}  else if (entry_name.compare(entries_names[Y_COORD])==0) {
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0){
						y_coord=temp_int;
			} else {
               exception_description << "Wrong station`s Y coordinate "<<temp_int;
               throw spider_exception(exception_description.str());
			};

	} else {
           exception_description << "Unrecognized config entry  "<<entry_name;
           throw spider_exception(exception_description.str());
	};
	
entry_name_c_str=PxConfigNextString(&temp_str[0], 
							         			               temp_str.size()
															);
};

if (id_station>0 &&
	 x_coord>0 &&
	 y_coord>0 &&
	 !name_station.empty()){

	 if (id_line<0) {
			if (metro_lines->get_current_line()!=metro_lines->end()){
					id_line=metro_lines->get_current_line()->second.get_id();
				} else {
                   exception_description << "Not found current line id  for station  "<<id_station;
                   throw spider_exception(exception_description.str());
				};
				
	 	};

	 					
	metro_lines_container::iterator lines_iter;
	lines_iter=metro_lines->find(id_line);
	if (lines_iter!=metro_lines->end()){
				lines_iter->second.insert_stations_id(id_station);
				this->set_current_station(
                            insert (upper_bound(id_station),
                            metro_stations_container::value_type (id_station,
                              metro_station (
                                           id_station,
                                           id_line,
                                           x_coord,
                                           y_coord,
                                           name_station,
                                           wave_file_name)                   )
                                       )
                );

			} else { // it`s checked and imosible
                exception_description << "Not found line id  "<<id_line<<" for station "<<id_station;
                throw spider_exception(exception_description.str());
			};		
	 } else {
       exception_description << "Not found all of required entries for station";
       throw spider_exception(exception_description.str());
	 };
};


void metro_stations_container::load ( string file_name) throw (spider_exception) {
 	enum {STATION=0, ENTRIES_COUNT};
	ostringstream exception_description;	
	string	section_name;
	const char *section_name_c_str;
	vector<string> sections_names(ENTRIES_COUNT);

	sections_names[STATION]="station";
 	
	if (PxConfigOpen( file_name.c_str(), PXCONFIG_READ)==Pt_FALSE ){
        exception_description<<"Can`t open config file "<<file_name;
	};

	cont_stations.erase(cont_stations.begin(), cont_stations.end());	
	
	try {	
            section_name_c_str=PxConfigNextSection();
            while (section_name_c_str!=NULL) {
                section_name=section_name_c_str;

                 if (section_name.compare(sections_names[STATION])==0) {
                     load_station_parameters (); 
                 } else {
                     exception_description<<"Unrecognized config section "<<section_name;
                      throw spider_exception(exception_description.str());
                 };
                section_name_c_str=PxConfigNextSection();
           };
   } catch (spider_exception spr_exc) {
        exception_description<<spr_exc.get_description();
         if  (PxConfigClose()!=Pt_TRUE) {
              exception_description<<"\nCan`t close file "<<file_name;
         };
        throw spider_exception(exception_description.str());    
   };

   if  (PxConfigClose()!=Pt_TRUE) {
          exception_description<<"\nCan`t close file "<<file_name;
          throw spider_exception(exception_description.str());    
   };

};
