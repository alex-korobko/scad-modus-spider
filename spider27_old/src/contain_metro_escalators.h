#ifndef __CONT_METRO_ESCALATORS_H__
#define  __CONT_METRO_ESCALATORS_H__

/*****************************************************************************
Container class for metro escalators - 
wrapper of STL container  map
*****************************************************************************/

class metro_escalators_container {
private :
struct ltint
	{
		  bool operator() (const int i1, const int i2) const
 			 {
   			 return (i1<i2) ;
  			}
	};

typedef  map <int, metro_escalator, ltint> metro_escalators_map;	

metro_escalators_map	container_metro_escalators;

typedef metro_escalators_map::iterator  iterator_metro_escalators;	
typedef metro_escalators_map::value_type pair_metro_escalators;	
typedef metro_escalators_map::size_type size_type_metro_escalators;	

iterator_metro_escalators current_escalator;
iterator_metro_escalators current_escalator_in_directions;

system_settings *sys_sett_obj;

/*
functor class for creating contain_morning_start from 
metro_escalators_container
*/	
	class make_morning_start
	{
	private:
		contain_morning_start *cont_morn_start;
		metro_escalators_container *escalators;
		system_settings *sys_sett_obj;
		int day_now;

		make_morning_start();
	public:
		make_morning_start(contain_morning_start *new_cont_morn_start, 
										metro_escalators_container *new_escalators, 
										system_settings *new_sys_sett_obj) :
			cont_morn_start(new_cont_morn_start),
			escalators(new_escalators),
			sys_sett_obj(new_sys_sett_obj)
		{
			time_t time_now=time(NULL);
			struct tm that_day_tm;
			localtime_r(&time_now, &that_day_tm);

			that_day_tm.tm_mday%2==0 ?
			day_now=system_settings::START_DAY_MODE_EVEN :
			day_now=system_settings::START_DAY_MODE_ODD;
		}
		
		void operator() (int escalator_id)
		{
			bool start_enabled=true;
			metro_escalators_container::iterator_metro_escalators	 iter_metro_escalator=
															escalators->find(escalator_id);

			if (iter_metro_escalator==escalators->end()) 
					{
						vector<char> temp_chars(10);
						string message("make_morning_start::operator(): not found esc id ");
						itoa(escalator_id, &temp_chars[0], 10);
						message+=&temp_chars[0];
						sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);		
						return;
					};
								
			if (iter_metro_escalator->second.get_enabled()==
				system_settings::DISABLED) return;

			int esc_start_day_mode=iter_metro_escalator->second.get_start_day_mode();

			if (esc_start_day_mode==
				system_settings::START_DAY_MODE_NEVER ||
				(esc_start_day_mode!=
				system_settings::START_DAY_MODE_EVERYDAY &&
				esc_start_day_mode!=day_now) 
				 ) start_enabled=false; 
	
			int  direction=iter_metro_escalator->second.get_pref_direction();
			if (direction!=system_settings::DIRECTION_UP &&
				direction!=system_settings::DIRECTION_DOWN)
				{
					direction=iter_metro_escalator->second.get_direction();
					if (direction!=system_settings::DIRECTION_UP &&
						direction!=system_settings::DIRECTION_DOWN)
							start_enabled=false; // what can I do ??
				};

			cont_morn_start->insert(cont_morn_start->end(),
												  escalator_start(iter_metro_escalator->second.get_id(),
																		   iter_metro_escalator->second.get_pref_direction(),
   																		   iter_metro_escalator->second.get_direction(),
 		                 												   iter_metro_escalator->second.get_start_hour(),
																		   iter_metro_escalator->second.get_start_minute(),
																		   esc_start_day_mode,
																		   start_enabled
																			)
													);
		};
	};


/*
functor class for searching in contain_morning_start
escalators, wich be started in current hour and minutes and start it.
*/	

	class find_morning_start
	{
		private:
		metro_escalators_container *esc_container;
		int start_hour, start_minute;
		
		find_morning_start();
		public:
		find_morning_start(metro_escalators_container *new_esc_container,
									 int new_start_hour,
									 int new_start_minute
									) : start_hour(new_start_hour),
										start_minute(new_start_minute)
									{ esc_container=new_esc_container; };
		
		void operator() (escalator_start& morn_start)
		{
			if (morn_start.get_start_hour()!=start_hour ||
				morn_start.get_start_minute()!=start_minute) return;
		
			metro_escalators_container::iterator_metro_escalators iter_esc=
					esc_container->find(morn_start.get_escalator_id() );
					
			if (iter_esc!=esc_container->end())
				{
					int direction=morn_start.get_escalator_direction();

					if (direction==system_settings::DIRECTION_UP)
							iter_esc->second.send_command(system_settings::COMMAND_UP);

					if (direction==system_settings::DIRECTION_DOWN)
							iter_esc->second.send_command(system_settings::COMMAND_DOWN);
					
				};

		};
	};

public :

explicit metro_escalators_container (system_settings *new_sys_sett_obj): 
		current_escalator(container_metro_escalators.end()), 
		current_escalator_in_directions(container_metro_escalators.end())
	{ sys_sett_obj=new_sys_sett_obj;};

/*
get_ and set_  metods for private data members
*/

iterator_metro_escalators get_current_escalator() {return (current_escalator);};
void set_current_escalator(iterator_metro_escalators new_current_escalator) {current_escalator=new_current_escalator;};

iterator_metro_escalators get_current_escalator_in_directions() {return (current_escalator_in_directions);};
void set_current_escalator_in_directions(iterator_metro_escalators new_current_escalator_in_directions) {current_escalator_in_directions=new_current_escalator_in_directions;};

/*
wrappers of current STL escalator`s container metods
*/

iterator_metro_escalators begin() {return (container_metro_escalators.begin());};
iterator_metro_escalators end(){return (container_metro_escalators.end());};

iterator_metro_escalators find(const int key) { return (container_metro_escalators.find(key)); };
iterator_metro_escalators upper_bound(const int key) { return container_metro_escalators.upper_bound(key); };

void erase (iterator_metro_escalators iter) { container_metro_escalators.erase(iter); };
void erase (iterator_metro_escalators iter_beg,
					iterator_metro_escalators iter_end) { container_metro_escalators.erase(iter_beg, iter_end); };
iterator_metro_escalators insert (iterator_metro_escalators iter, const pair_metro_escalators& obj) 
{
  return (container_metro_escalators.insert(iter, obj)); 
}

bool empty() const { return (container_metro_escalators.empty());}
size_type_metro_escalators size() const {return (container_metro_escalators.size());};

/*
Other functions
*/
bool 
	save_escalator_parameters ()
{
enum {ID=0, STATION_ID, TYPE, ENABLED, NUMBER,
			IP_ADDRESS, DIRECTION, START_DAY_MODE, START_HOUR, START_MINUTE, 
			PREDEFINED_DIRECTION, ENTRIES_COUNT};

string entry_name;

vector<char> temp_str(512);
vector<string> entries_names(ENTRIES_COUNT);

int id_escalator=-1;

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
			                     &id_escalator)!=Pt_TRUE)
	{
		sys_sett_obj->sys_message(system_settings::ERROR_MSG,
													"save_escalator_parameters: can`t read entry - id");
		return false;
	};

	iterator_metro_escalators	 iter_esc=this->find(id_escalator);
	if (iter_esc==this->end())
		{
			string mess("save_escalator_parameters: iter_esc==this->end() esc_id ");
			vector<char> tmp_chars(10);
			itoa(id_escalator, &tmp_chars[0], 10);
			mess+=&tmp_chars[0];
			sys_sett_obj->sys_message(system_settings::ERROR_MSG,
															mess);		
			return false;
		};

	if (PxConfigWriteInt( NULL, 
			                    entries_names[START_HOUR].c_str(), 
   				                PXCONFIG_FMT_INT_DECIMAL, 
			                    iter_esc->second.get_start_hour())!=Pt_TRUE)
			                    {
			                    		vector<char> tmp_chars(10);
									itoa(id_escalator, &tmp_chars[0], 10);
									string mess("save_escalator_parameters: PxConfigWriteInt START_HOUR !=Pt_TRUE esc_id ");
									mess+=&tmp_chars[0];
		                    			sys_sett_obj->sys_message(system_settings::ERROR_MSG,
																				mess);		

			                    		return false;
			                    };

	if (PxConfigWriteInt( NULL, 
			                    entries_names[START_MINUTE].c_str(), 
   				                PXCONFIG_FMT_INT_DECIMAL, 
			                    iter_esc->second.get_start_minute())!=Pt_TRUE)
			                    {
			                    		vector<char> tmp_chars(10);
									itoa(id_escalator, &tmp_chars[0], 10);
									string mess("save_escalator_parameters: PxConfigWriteInt START_MINUTE !=Pt_TRUE esc_id ");
									mess+=&tmp_chars[0];
		                    			sys_sett_obj->sys_message(system_settings::ERROR_MSG,
																				mess);		
			                    		return false;
			                    };
	
	if (PxConfigWriteString( NULL, 
			                    entries_names[START_DAY_MODE].c_str(), 
   				                PXCONFIG_FMT_STRING, 
			                    start_days_modes_strings_engl[iter_esc->second.get_start_day_mode()].c_str())!=Pt_TRUE)
			                    {
			                    		vector<char> tmp_chars(10);
									itoa(id_escalator, &tmp_chars[0], 10);
									string mess("save_escalator_parameters: PxConfigWriteString START_DAY_MODE !=Pt_TRUE esc_id ");
									mess+=&tmp_chars[0];
		                    			sys_sett_obj->sys_message(system_settings::ERROR_MSG,
																				mess);		
			                    		return false;
			                    };

	 if (PxConfigWriteString( NULL, 
			                    entries_names[DIRECTION].c_str(), 
   				                PXCONFIG_FMT_STRING, 
			                    directions_strings_engl[iter_esc->second.get_direction()].c_str())!=Pt_TRUE)
			                    {
			                    		vector<char> tmp_chars(10);
									itoa(id_escalator, &tmp_chars[0], 10);
									string mess("save_escalator_parameters: PxConfigWriteString DIRECTION !=Pt_TRUE esc_id ");
									mess+=&tmp_chars[0];
		                    			sys_sett_obj->sys_message(system_settings::ERROR_MSG,
																				mess);		
			                    		return false;
			                    };

	 if (PxConfigWriteString( NULL, 
			                    entries_names[PREDEFINED_DIRECTION].c_str(), 
   				                PXCONFIG_FMT_STRING, 
			                    directions_strings_engl[iter_esc->second.get_pref_direction()].c_str())!=Pt_TRUE)
			                    {
			                    		vector<char> tmp_chars(10);
									itoa(id_escalator, &tmp_chars[0], 10);
									string mess("save_escalator_parameters: PxConfigWriteString PREDEFINED_DIRECTION !=Pt_TRUE esc_id ");
									mess+=&tmp_chars[0];
		                    			sys_sett_obj->sys_message(system_settings::ERROR_MSG,
																				mess);		
			                    		return false;
			                    };

return true;
}

bool save (string file_name)
{
 	enum {ESCALATOR=0, ENTRIES_COUNT};

	string	section_name;
	const char *section_name_c_str;
	vector<string> sections_names(ENTRIES_COUNT);
	sections_names[ESCALATOR]="escalator";

	if (PxConfigOpen( file_name.c_str(), PXCONFIG_READ|PXCONFIG_WRITE)==Pt_FALSE )
	{
		string message("Can`t open config file ");
		message+=file_name.c_str();
		sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);

		return false;
	};

	section_name_c_str=PxConfigNextSection();
	while (section_name_c_str!=NULL) 
	{

	section_name=section_name_c_str;

			if (section_name.compare(sections_names[ESCALATOR])==0) 
			{
				if (!save_escalator_parameters()) {return(false);};			
			};
			
			section_name_c_str=PxConfigNextSection();
	};	

	if  (PxConfigClose()) 
	{return (true);}
	else
	{
		string message("Can`t close config file ");
		message+=file_name.c_str();
		sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);

		return (false);
	};
	

return (true);
};

/*
bool 
	load_escalator_parameters ()

That function reads escalator`s parameters from config file 
and insert that escalator into g_escalators. 
	id -  id of escalator. If escalator with same id present in g_escalators, 
function write error message and return false.
	stationID  - id of station in stations container. If not present, escalator id 
must be added to  g_stations.get_current_station(). If 
g_stations.get_current_station()==g_stations.end()  function write error 
message and return false. If stationID present, but station with  stationID
not present in g_stations, function write  error message and return false.
If stationID present in g_stations, escalator id  must be added to that
station.
	type - type of escalator in g_metro_escalator_types. If not present  
function write error message and return false. If type present, 
but in g_metro_escalator_types not present type with id type, 
function write error message and return false.
	enabed  - enabled state of escalator. 0 - disabled (default). 
1 - enabled.  If not present  set disabled.
	num - number of escalator in station.  If not present  
function write error message and return false.
	ip - ip address of escalator.  If not present  
function write error message and return false.
	directoin - last used direction of escalator.
See system_settings::DIRECTION_... for details. If not present
or not recognized (not ...STOP or .._DOWN or ..._UP) 
function write error message and return false.
   day_start mode or morning start  - everyday, never,
even days, odd days. See system_settings::START_DAY_MODE_...
for emplimination details.
	start_hour hour of morning start.
   	start_min minute of morning start.
	predef_directoin - predefined direction of escalator.
See system_settings::DIRECTION_... for details. If not present
or not recognized (not ...REVERSE or .._DOWN or ..._UP)  
function write error message and return false.
*/

bool 
	load_escalator_parameters (metro_stations_container *metro_stat_obj)
{
enum {ID=0, STATION_ID, TYPE, ENABLED, NUMBER,
			IP_ADDRESS, DIRECTION, START_DAY_MODE, START_HOUR, START_MINUTE, 
			PREDEFINED_DIRECTION, ENTRIES_COUNT};

const char *entry_name_c_str;
string entry_name;

vector<char> temp_str(512);
vector<string> entries_names(ENTRIES_COUNT);

int id_escalator=-1, id_station=-1, esc_type=-1, number=-1,
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
						id_escalator=temp_int;
					} else 	{
						string message("Escalator with id  ");
						message+=&temp_str[0];
						message+=" already exist in escalators container";
						sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
					};
			} else {
						string message("Wrong escalator id  ");
						message+=&temp_str[0];
						sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
			};
			
	
	}else if (entry_name.compare(entries_names[STATION_ID])==0) {
	
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0)
			{
				if (metro_stat_obj->find(temp_int)!=metro_stat_obj->end())
					{
						id_station=temp_int;
					} else 	{
						string message("Not found escalator`s station id  ");
						message+=&temp_str[0];
						sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
					};
			} else {
						string message("Wrong  escalator`s station id ");
						message+=&temp_str[0];
						sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
			};
			

	}else if (entry_name.compare(entries_names[TYPE])==0) {

			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0)
			{
				if (g_metro_devices_types.find(temp_int)!=g_metro_devices_types.end())
					{
						esc_type=temp_int;
					} else 	{
						string message("Not found escalator`s type id  ");
						message+=&temp_str[0];
						sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
					};
			} else {
						string message("Wrong  escalator`s type id ");
						message+=&temp_str[0];
						sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
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
							string message("Wrong escalator enabled state (enabled/disabled)  ");
							message+=&temp_str[0];
							sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);		
						};
				};	

	}else if (entry_name.compare(entries_names[NUMBER])==0) { 	
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0)
			{
						number=temp_int;
			} else {
						string message("Wrong escalator number  ");
						message+=&temp_str[0];
						sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
			};


	}else if (entry_name.compare(entries_names[IP_ADDRESS])==0) {
	
			ip=inet_addr(&temp_str[0]);
			if (ip==INADDR_NONE)
				{
					string message("Wrong escalator ip  ");
					message+=&temp_str[0];
					sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);		
					return(false);
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
							string message("Wrong escalator direction  ");
							message+=&temp_str[0];
							sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);		

							return(false);
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
											string message("Wrong escalator start day mode  ");
											message+=&temp_str[0];
											sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);		
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
						string message("Wrong escalator start hour  ");
						message+=&temp_str[0];
						sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
			};

	}else if (entry_name.compare(entries_names[START_MINUTE])==0) { 	
			int temp_int = atoi(&temp_str[0]); 
			if (	temp_int>=0 &&  //ATTENTION !!! CAVEATS !!! atoi return 0 if string contain 0 or if string not correct.... escalator may be started in not correct time.
				temp_int<60)
			{
						start_minute=temp_int;
			} else {
						string message("Wrong escalator start minute  ");
						message+=&temp_str[0];
						sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
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
									string message("Wrong escalator predefined direction  ");
									message+=&temp_str[0];
									sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);		

									return(false);
								};
						};				
				};

	} else {
			string message("Unrecognized config entry  ");
			message+=entry_name;
			sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);		
			
			return (false);
	};
	
entry_name_c_str=PxConfigNextString(&temp_str[0], 
							                        temp_str.size()
							                      );
}; //while 

if (id_escalator>0 && 
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
					vector<char> tmp_chars(10);
					string message("Not found current station id for escalator ");
					itoa(id_escalator, &tmp_chars[0], 10);
					message+=&tmp_chars[0];
					sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);		
				};
			};

	if (enabled<0) 	enabled=system_settings::DISABLED;

	metro_stations_container::iterator_metro_stations iter_stat;
	iter_stat=metro_stat_obj->find(id_station);
	if (iter_stat!=metro_stat_obj->end())
			{ 
			
			pair_metro_escalators tmp_pair
												(id_escalator,
												 metro_escalator
																	 (	sys_sett_obj,	
																	 	id_escalator, 
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

				iter_stat->second.push_back_escalators_id(id_escalator); 
				this->set_current_escalator(this->insert(this->upper_bound(id_escalator), tmp_pair));	
			} else { // it`s checked and imosible
				vector<char> tmp_chars(10);
				string message("Not found station id  ");
				itoa(id_station, &tmp_chars[0], 10);
				message+=&tmp_chars[0];
				message+=" for escalator ";
				itoa(id_escalator, &tmp_chars[0], 10);
				message+=&tmp_chars[0];
				sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);		
				return(false);
			};		

	}  else {//if (id_escalator>0 &&
		sys_sett_obj->sys_message(system_settings::ERROR_MSG, 
															string ("Not found all of required entries for escalator"));		
		return(false);

	};

return(true);
};


bool load (metro_stations_container *metro_stations_cont, string file_name)
	{
 	enum {ESCALATOR=0, ENTRIES_COUNT};

	string	section_name;
	const char *section_name_c_str;
	vector<string> sections_names(ENTRIES_COUNT);

	sections_names[ESCALATOR]="escalator";

	erase(begin(), end());
 	
	if (PxConfigOpen( file_name.c_str(), PXCONFIG_READ)==Pt_FALSE )
	{
		string message("Can`t open config file ");
		message+=file_name.c_str();
		sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);

		return 0;
	};

	section_name_c_str=PxConfigNextSection();
	while (section_name_c_str!=NULL) 
	{

	section_name=section_name_c_str;

			if (section_name.compare(sections_names[ESCALATOR])==0) 
			{
				if (! load_escalator_parameters (metro_stations_cont)) {return(false);};			
			} else {
				string message("Unrecognized config section ");
				message+=section_name;
				sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);
				return 0;
			};
			
			section_name_c_str=PxConfigNextSection();
	};	

	if  (PxConfigClose()) 
	{return (true);}
	else
	{
		string message("Can`t close config file ");
		message+=file_name.c_str();
		sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);

		return (false);
	};
	
	return (true);

	}; 


/*
preparing contain_morning_start from metro_escalators_container
*/
void prepare_morning_start(contain_morning_start *cont_morn_start, 
											metro_station::escalators_id_container *escalators_id_for_morning_start,
											system_settings *sys_sett_obj)
{
	make_morning_start maker_of_morning_start(cont_morn_start,
																			this,
																			sys_sett_obj);

	cont_morn_start->erase_all();

	for_each(escalators_id_for_morning_start->begin(),
				escalators_id_for_morning_start->end(),
				maker_of_morning_start);

	cont_morn_start->sort_all();				
};

/*
executing  contain_morning_start by metro_escalators_container
*/
void execute_morning_start(contain_morning_start *cont_morn_start,
											int start_hour,
											int start_minute)
{
	find_morning_start finder_morning_start(this,
																	start_hour,
																	start_minute);

	for_each(cont_morn_start->begin(),
				   cont_morn_start->end(),
				   finder_morning_start);

};



}; // class metro_escalators_container
#endif