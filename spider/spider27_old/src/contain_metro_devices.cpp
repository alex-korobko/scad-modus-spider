#include "global.h"
#include "system_settings.h"
#include "spider_exception.h"
#include "contain_metro_devices.h"

metro_devices_container::make_timer_command::make_timer_command(
                           contain_timer_command *new_cont_timer_command,
                           metro_devices_container *new_devices) :
			cont_timer_command(new_cont_timer_command),
			devices(new_devices) {

			time_t time_now=time(NULL);
			struct tm that_day_tm;
			localtime_r(&time_now, &that_day_tm);

			that_day_tm.tm_mday%2==0 ?
			day_now=system_settings::START_DAY_MODE_EVEN :
			day_now=system_settings::START_DAY_MODE_ODD;
		};

void  metro_devices_container::make_timer_command::operator() 
              (int device_id) throw (spider_exception) {
			ostringstream exception_description;
			bool start_enabled=true;

			metro_devices_container::iterator_metro_devices	 iter_metro_device=
															devices->find(device_id);

			if (iter_metro_device==devices->end()) {
                       exception_description<<"make_timer_command::operator(): not found esc id ";
                       exception_description<<device_id;
					    throw spider_exception(exception_description.str());
					};
								
			if (! iter_metro_device->second.get_enabled()) return;

			int execution_mode=iter_metro_device->second.get_execution_mode();

			if (execution_mode==
				system_settings::START_DAY_MODE_NEVER ||
				(execution_mode!=
				system_settings::START_DAY_MODE_EVERYDAY &&
				execution_mode!=day_now) 
				 ) start_enabled=false; 

/*	
			int  direction=iter_metro_device->second.get_pref_direction();
			if (direction!=system_settings::DIRECTION_UP &&
				direction!=system_settings::DIRECTION_DOWN)
				{
					direction=iter_metro_device->second.get_direction();
					if (direction!=system_settings::DIRECTION_UP &&
						direction!=system_settings::DIRECTION_DOWN)
							start_enabled=false; // what can I do ??
				};
*/
			cont_timer_command->insert(cont_timer_command->end(),
												  timer_command(
												  		
/*												  
												  		iter_metro_device->second.get_id(),
																		   iter_metro_device->second.get_pref_direction(),
   																		   iter_metro_device->second.get_direction(),
 		                 												   iter_metro_device->second.get_start_hour(),
																		   iter_metro_device->second.get_start_minute(),
*/
																		   execution_mode,
																		   start_enabled
																			)
													);
	};


metro_devices_container::find_timer_command::find_timer_command
								(metro_devices_container *new_dev_container,
									 int new_timer_hour,
									 int new_start_minute) :
									timer_hour(new_start_hour),
									timer_minute(new_start_minute)
  {dev_container=new_dev_container; };


void metro_devices_container::find_timer_command::operator() 
		(timer_command& timer_cmd){
			if (timer_cmd.get_timer_hour()!=timer_hour ||
				morn_start.get_timer_minute()!=timer_minute) return;
		
			metro_devices_container::iterator_metro_devices iter_esc=
					esc_container->find(morn_start.get_device_id() );
					
			if (iter_esc!=esc_container->end())
				{
					int direction=morn_start.get_device_direction();

					if (direction==system_settings::DIRECTION_UP)
							iter_esc->second.send_command(system_settings::COMMAND_UP);

					if (direction==system_settings::DIRECTION_DOWN)
							iter_esc->second.send_command(system_settings::COMMAND_DOWN);
					
				};
		};


void 	metro_devices_container::save_device_parameters (
				system_settings *sys_sett_obj)  throw (spider_exception) {

enum {ID=0, STATION_ID, TYPE, ENABLED, NUMBER,
			IP_ADDRESS, DIRECTION, START_DAY_MODE, START_HOUR, START_MINUTE, 
			PREDEFINED_DIRECTION, ENTRIES_COUNT};

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
entries_names[IP_ADDRESS]="ip";
entries_names[DIRECTION]="direction";
entries_names[START_DAY_MODE]="start day mode";
entries_names[START_HOUR]="start hour";
entries_names[START_MINUTE]="start minute";
entries_names[PREDEFINED_DIRECTION]="predef_direction";

if (PxConfigReadInt( NULL, 
			                     entries_names[ID].c_str(), 
			                     -1, 
			                     &id_device)!=Pt_TRUE) {
        exception_description<<"save_device_parameters: can`t read entry - id";
	    throw spider_exception(exception_description.str());
	};

	iterator_metro_devices	 iter_esc=this->find(id_device);
	if (iter_esc==this->end()){
	       exception_description<<"save_device_parameters: iter_esc==this->end() esc_id ";
	       exception_description<<id_device;
            throw spider_exception(exception_description.str());
   };

	if (PxConfigWriteInt( NULL, 
			                    entries_names[START_HOUR].c_str(), 
   				                PXCONFIG_FMT_INT_DECIMAL, 
			                    iter_esc->second.get_start_hour())!=Pt_TRUE) {
	       exception_description<<"save_device_parameters: PxConfigWriteInt START_HOUR !=Pt_TRUE esc_id ";
	       exception_description<<id_device;
           throw spider_exception(exception_description.str());
           };

	if (PxConfigWriteInt( NULL, 
			                    entries_names[START_MINUTE].c_str(), 
   				                PXCONFIG_FMT_INT_DECIMAL, 
			                    iter_esc->second.get_start_minute())!=Pt_TRUE){
	       exception_description<<"save_device_parameters: PxConfigWriteInt START_MINUTE !=Pt_TRUE esc_id ";
	       exception_description<<id_device;
           throw spider_exception(exception_description.str());
           };
	
	if (PxConfigWriteString( NULL, 
			                    entries_names[START_DAY_MODE].c_str(), 
   				                PXCONFIG_FMT_STRING, 
			                    start_days_modes_strings_engl[iter_esc->second.get_start_day_mode()].c_str())!=Pt_TRUE){
	       exception_description<<"save_device_parameters: PxConfigWriteString START_DAY_MODE !=Pt_TRUE esc_id ";
	       exception_description<<id_device;
           throw spider_exception(exception_description.str());
          };

	 if (PxConfigWriteString( NULL, 
			                    entries_names[DIRECTION].c_str(), 
   				                PXCONFIG_FMT_STRING, 
			                    directions_strings_engl[iter_esc->second.get_direction()].c_str())!=Pt_TRUE){
	       exception_description<<"save_device_parameters: PxConfigWriteString DIRECTION !=Pt_TRUE esc_id ";
	       exception_description<<id_device;
           throw spider_exception(exception_description.str());
           };

	 if (PxConfigWriteString( NULL, 
			                    entries_names[PREDEFINED_DIRECTION].c_str(), 
   				                PXCONFIG_FMT_STRING, 
			                    directions_strings_engl[iter_esc->second.get_pref_direction()].c_str())!=Pt_TRUE){
	       exception_description<<"save_device_parameters: PxConfigWriteString PREDEFINED_DIRECTION !=Pt_TRUE esc_id ";
	       exception_description<<id_device;
           throw spider_exception(exception_description.str());
           };
}


void metro_devices_container::save (string file_name,
															system_settings *sys_sett_obj)
         throw (spider_exception) {
 	enum {ESCALATOR=0, ENTRIES_COUNT};

	ostringstream exception_description;
	string	section_name;
	const char *section_name_c_str;
	vector<string> sections_names(ENTRIES_COUNT);
	sections_names[ESCALATOR]="device";
	
	if (PxConfigOpen( file_name.c_str(), PXCONFIG_READ|PXCONFIG_WRITE)==Pt_FALSE ){
	       exception_description<<"Can`t open config file "<<file_name;
           throw spider_exception(exception_description.str());
	};

	section_name_c_str=PxConfigNextSection();
	try{
          while (section_name_c_str!=NULL) {
                 section_name=section_name_c_str;

                  if (section_name.compare(sections_names[ESCALATOR])==0) {
                         save_device_parameters(sys_sett_obj);
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
	metro_devices_container::load_device_parameters (
                                    metro_stations_container *metro_stat_obj,
                                    metro_devices_types_container *metro_dev_types,
                                    system_settings *sys_sett_obj) 
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
	ip - ip address of device.  If not present  
function write error message and return false.
	directoin - last used direction of device.
See system_settings::DIRECTION_... for details. If not present
or not recognized (not ...STOP or .._DOWN or ..._UP) 
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
	metro_devices_container::load_device_parameters (
                                    metro_stations_container *metro_stat_obj,
                                    metro_devices_types_container *metro_dev_types,
                                    system_settings *sys_sett_obj) 
                                    throw (spider_exception) {

enum {ID=0, STATION_ID, TYPE, ENABLED, NUMBER,
			IP_ADDRESS, DIRECTION, START_DAY_MODE, START_HOUR, START_MINUTE, 
			PREDEFINED_DIRECTION, ENTRIES_COUNT};

ostringstream exception_description;
const char *entry_name_c_str;
string entry_name;

vector<char> temp_str(512);
vector<string> entries_names(ENTRIES_COUNT);

int id_device=-1, id_station=-1, esc_type=-1, number=-1,
	predef_direction=-1, direction=-1, start_day_mode=-1, 
	start_hour=-1, start_minute=-1,  enabled=-1;

in_addr_t ip=INADDR_NONE;

system_settings::strings_container directions_strings_engl=sys_sett_obj->get_directions_engl_strings();
system_settings::strings_container start_days_modes_strings_engl=sys_sett_obj->get_start_days_modes_engl_strings();
system_settings::strings_container outer_states_strings=sys_sett_obj->get_outer_states_strings();

entries_names[ID]="id";
entries_names[STATION_ID]="stationID";
entries_names[TYPE]="type";
entries_names[ENABLED]="enabled";
entries_names[NUMBER]="num";
entries_names[IP_ADDRESS]="ip";
entries_names[DIRECTION]="direction";
entries_names[START_DAY_MODE]="start day mode";
entries_names[START_HOUR]="start hour";
entries_names[START_MINUTE]="start minute";
entries_names[PREDEFINED_DIRECTION]="predef_direction";

entry_name_c_str=PxConfigNextString(&temp_str[0], 
							                        temp_str.size()
							                      );

while(entry_name_c_str!=NULL)
{
entry_name=entry_name_c_str;

if (entry_name.compare(entries_names[ID])==0) 
	{

			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0)
			{
				if (this->find(temp_int)==this->end())
					{
						id_device=temp_int;
					} else 	{
                       exception_description<<"Escalator with id  "<<&temp_str[0];
                       exception_description<<" already exist in devices container";
                       throw spider_exception(exception_description.str());
					};
			} else {
                       exception_description<<"Wrong device id  ";
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
                       exception_description<<"Not found device`s station id  ";
                       exception_description<<&temp_str[0];
                       throw spider_exception(exception_description.str());
					};
			} else {
                       exception_description<<"Wrong  device`s station id ";
                       exception_description<<&temp_str[0];
                       throw spider_exception(exception_description.str());
			};
			

	}else if (entry_name.compare(entries_names[TYPE])==0) {

			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0)
			{
				if (metro_dev_types->find(temp_int)!=metro_dev_types->end())
					{
						esc_type=temp_int;
					} else 	{
                       exception_description<<"Not found device`s type id  ";
                       exception_description<<&temp_str[0];
                       throw spider_exception(exception_description.str());
					};
			} else {
                       exception_description<<"Wrong  device`s type id ";
                       exception_description<<&temp_str[0];
                       throw spider_exception(exception_description.str());
			};
			
	}else if (entry_name.compare(entries_names[ENABLED])==0) { 	
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
                           exception_description<<"Wrong device enabled state (enabled/disabled)  ";
                           exception_description<<&temp_str[0];
                           throw spider_exception(exception_description.str());
						};
				};	

	}else if (entry_name.compare(entries_names[NUMBER])==0) { 	
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0)
			{
						number=temp_int;
			} else {
                   exception_description<<"Wrong device number  ";
                   exception_description<<&temp_str[0];
                    throw spider_exception(exception_description.str());
			};


	}else if (entry_name.compare(entries_names[IP_ADDRESS])==0) {
	
			ip=inet_addr(&temp_str[0]);
			if (ip==INADDR_NONE)
				{
                   exception_description<<"Wrong device ip  ";
                   exception_description<<&temp_str[0];
                   throw spider_exception(exception_description.str());
				};
				 
			
	}else if (entry_name.compare(entries_names[DIRECTION])==0) { 	
			int temp_int;

			temp_int=system_settings::DIRECTION_UP;
			if (directions_strings_engl[temp_int].compare(&temp_str[0])==0) 
				{
					direction = temp_int;					
				} else {
					temp_int=system_settings::DIRECTION_DOWN;
					if (directions_strings_engl[temp_int].compare(&temp_str[0])==0) 
						{
							direction = temp_int;					
						} else {
                            exception_description<<"Wrong device direction  ";
                            exception_description<<&temp_str[0];
                            throw spider_exception(exception_description.str());
						};
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
                                            exception_description<<"Wrong device start day mode  ";
                                            exception_description<<&temp_str[0];
                                             throw spider_exception(exception_description.str());
										};
								};
						};				
				};

	}else if (entry_name.compare(entries_names[START_HOUR])==0) { 	
			int temp_int = atoi(&temp_str[0]);
			if (	temp_int>=system_settings::START_HOUR_MIN &&
				temp_int<=system_settings::START_HOUR_MAX)
			{
						start_hour=temp_int;
			} else {
                          exception_description<<"Wrong device start hour  ";
                          exception_description<<&temp_str[0];
                          throw spider_exception(exception_description.str());
			};

	}else if (entry_name.compare(entries_names[START_MINUTE])==0) { 	
			int temp_int = atoi(&temp_str[0]); 
			if (	temp_int>=0 &&  //ATTENTION !!! CAVEATS !!! atoi return 0 if string contain 0 or if string not correct.... device may be started in not correct time.
				temp_int<60)
			{
						start_minute=temp_int;
			} else {
                   exception_description<<"Wrong device start minute  ";
                   exception_description<<&temp_str[0];
                   throw spider_exception(exception_description.str());
			};

	}else if (entry_name.compare(entries_names[PREDEFINED_DIRECTION])==0) { 	
			int temp_int;

			temp_int=system_settings::DIRECTION_UP;
			if (directions_strings_engl[temp_int].compare(&temp_str[0])==0) 
				{
					predef_direction = temp_int;					
				} else {
					temp_int=system_settings::DIRECTION_DOWN;
					if (directions_strings_engl[temp_int].compare(&temp_str[0])==0) 
						{
							predef_direction = temp_int;					
						} else {
							temp_int=system_settings::DIRECTION_REVERSE;
							if (directions_strings_engl[temp_int].compare(&temp_str[0])==0) 
								{
									predef_direction = temp_int;					
								} else {
                                    exception_description<<"Wrong device predefined direction  ";
                                    exception_description<<&temp_str[0];
                                    throw spider_exception(exception_description.str());
								};
						};				
				};

	} else {
            exception_description<<"Unrecognized config entry  ";
            exception_description<<&temp_str[0];
            throw spider_exception(exception_description.str());
	};
	
entry_name_c_str=PxConfigNextString(&temp_str[0], 
							                        temp_str.size()
							                      );
}; //while 

if (id_device>0 && 
	esc_type>0 &&
	number>0 && 
	start_day_mode>=0 &&
	start_hour>0 &&
	start_minute>=0 &&
	predef_direction>0 &&
	direction>0 &&
	ip!=INADDR_NONE)
	{
	if (id_station<0) 
			{
			if (metro_stat_obj->get_current_station()!=metro_stat_obj->end())
				{
					id_station=metro_stat_obj->get_current_station()->second.get_id();
				} else {
                    exception_description<<"Not found current station id for device ";
                    exception_description<<&temp_str[0];
                    throw spider_exception(exception_description.str());
				};
			};

	if (enabled<0) 	enabled=system_settings::DISABLED;

	metro_stations_container::iterator_metro_stations iter_stat;
	iter_stat=metro_stat_obj->find(id_station);
	if (iter_stat!=metro_stat_obj->end())
			{ 
			
			pair_metro_devices tmp_pair
												(id_device,
												 metro_device
																	 (	id_device, 
																		id_station,
																		esc_type,
																		number,
																		predef_direction,
																		direction,
																		start_day_mode,
																		start_hour,
																		start_minute,
																		enabled,
																		ip
																	)
													);

				iter_stat->second.push_back_devices_id(id_device); 
				this->set_current_device(this->insert(this->upper_bound(id_device), tmp_pair));	
			} else { // it`s checked and imosible
                 exception_description<<"Not found station id  "<<id_station;
                 exception_description<<" for device "<<id_device;
                  throw spider_exception(exception_description.str());
			};		

	}  else {//if (id_device>0 &&
           exception_description<<"Not found all of required entries for device";
           throw spider_exception(exception_description.str());
	};
};

void metro_devices_container::load (
        string file_name,
        metro_stations_container *metro_stations_cont,
        metro_devices_types_container *metro_dev_types,
        system_settings *sys_sett_obj ) throw (spider_exception) {

 	enum {ESCALATOR=0, ENTRIES_COUNT};
	ostringstream exception_description;
	string	section_name;
	const char *section_name_c_str;
	vector<string> sections_names(ENTRIES_COUNT);

	sections_names[ESCALATOR]="device";

	this->erase(this->begin(), this->end());
 	
	if (PxConfigOpen( file_name.c_str(), PXCONFIG_READ)==Pt_FALSE )
	{
         exception_description<<"Can`t open config file "<<file_name;
         throw spider_exception(exception_description.str());
	};

    try {
          section_name_c_str=PxConfigNextSection();
          while (section_name_c_str!=NULL) {
              section_name=section_name_c_str;

              if (section_name.compare(sections_names[ESCALATOR])==0) {
                   load_device_parameters (metro_stations_cont,
                                                               metro_dev_types,
                                                               sys_sett_obj);
                 } else {
                     exception_description<<"Unrecognized config section "<<section_name;
                     throw spider_exception(exception_description.str());
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

void metro_devices_container::prepare_morning_start(
                            contain_morning_start *cont_morn_start,
                            metro_station::devices_id_container *devices_id_for_morning_start)
                             throw (spider_exception) {
	make_morning_start maker_of_morning_start(cont_morn_start,
																			this);

	cont_morn_start->erase_all();

	for_each(devices_id_for_morning_start->begin(),
				devices_id_for_morning_start->end(),
				maker_of_morning_start);

	cont_morn_start->sort_all();				
};

void metro_devices_container::execute_morning_start(
                           contain_morning_start *cont_morn_start,
                            int start_hour,
                            int start_minute){
	find_morning_start finder_morning_start(this,
																	start_hour,
																	start_minute);

	for_each(cont_morn_start->begin(),
				   cont_morn_start->end(),
				   finder_morning_start);

};
