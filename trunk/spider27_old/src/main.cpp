/* Y o u r   D e s c r i p t i o n                       */
/*                            AppBuilder Photon Code Lib */
/*                                         Version 2.01  */

#include "global.h"

system_settings g_system_settings;
alerts_container g_alerts;
metro_stations_container g_stations;
metro_escalators_container g_escalators;
cmd_pool_container g_CommandPool;
esc_types_container g_metro_escalator_types;
metro_lines_container g_lines;
msg_dict_container g_msgDictionary;
router g_router;
log_records_container  g_main_log, g_archive_log;

int g_chanID;
pthread_t		pingTID, timerTID;
sig_atomic_t SigpipeCount;

/* Application Options string */

const char ApOptions[] =
	AB_OPTIONS "";

/* Add your options in the "" */

void SigpipeHandler (int SignalNumber)
{
++SigpipeCount;
};
	
void* PingThread(void* arg)
{
metro_escalators_container::iterator_metro_escalators iter_esc;

	while(1)
	{
		if (SigpipeCount>0) {
		g_system_settings.sys_message(system_settings::ERROR_MSG, string ("Recived SIGPIPE"));
		--SigpipeCount;
										};
		iter_esc=g_escalators.begin();
		while (iter_esc!=g_escalators.end())
	 	{
	 		if ((iter_esc->second.get_online() ==system_settings::ONLINE) && iter_esc->second.get_enabled())
	 		{
	 			if (iter_esc->second.get_sleepticks() >= 1)
	 			{
//	 				iter_esc->second.check_status();	
					iter_esc->second.get_data();	
	 				iter_esc->second.set_sleepticks (0);
	 			}
	 			else
	 				iter_esc->second.increment_sleepticks();
	 		}
	 		else if (iter_esc->second.get_enabled())
	 		{
				if (iter_esc->second.get_sleepticks() >= 5)
	 			{
//	 				iter_esc->second.check_status();	
					iter_esc->second.get_data();
	 				iter_esc->second.set_sleepticks (0);
	 			}
	 			else
	 				iter_esc->second.increment_sleepticks();
	 				
	 		iter_esc++;		
	 		}
			
 		}

 		usleep(system_settings::PING_TIMEOUT);
 	}
}

void* TimerThread(void *arg)
{
	struct _pulse   pulse;
	timer_t         timerID;
	struct sigevent event;
	struct itimerspec timer;
	int  conID,  timeoutChannel, recvID;
	
	if ((timeoutChannel = ChannelCreate(0)) == -1)
	{
		string message("Can't create timeout channel ");
		message+=strerror(errno);
		g_system_settings.sys_message(system_settings::ERROR_MSG,  message);

		return 0;
	}
	
	
	conID = ConnectAttach(0, 0, timeoutChannel, 0, 0);
	if (conID == -1) {
		string message("Attach channel ");
		message+=strerror(errno);
		g_system_settings.sys_message(system_settings::ERROR_MSG,  message);

		return 0;
	}
	
	
	SIGEV_PULSE_INIT(&event, conID, SIGEV_PULSE_PRIO_INHERIT, system_settings::SYSTEM_TIMER, 0);

	if (timer_create(CLOCK_REALTIME, &event, &timerID) == -1)
	{
		string message("Fail to create system timer  ");
		message+=strerror(errno);
		g_system_settings.sys_message(system_settings::ERROR_MSG,  message);
	}

	
	timer.it_value.tv_sec = 1;
	timer.it_value.tv_nsec = 0;
	timer.it_interval.tv_sec = system_settings::SYSTEM_TICK;
	timer.it_interval.tv_nsec = 0;

	timer_settime(timerID, 0, &timer, NULL);

	while (1)
	{
		recvID = MsgReceive(timeoutChannel, &pulse, sizeof(pulse), NULL);
		
		if (recvID==-1)
			{
				string message("MsgReceive ");
				message+=strerror(errno);
				g_system_settings.sys_message(system_settings::ERROR_MSG,  message);

			};
 		if (recvID == 0 && pulse.code == system_settings::SYSTEM_TIMER) g_main_log.rotate();
	}

	return 0;
}


/*
bool 
	get_escalator_parameters ()

That function reads escalator`s parameters from config file 
and insert that escalator into g_escalators. 
	id -  id of escalator. If escalator with same id present in g_escalators, 
function write error message and return false.
	stationID  - id of station in g_stations. If not present, escalator id 
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
set system_settings::DIRECTION_STOP.
	predef_directoin - predefined direction of escalator.
See system_settings::DIRECTION_... for details. If not present
or not recognized (not ...REVERSE or .._DOWN or ..._UP)  
set system_settings::DIRECTION_REVERSE.
*/
bool 
	get_escalator_parameters ()
{
enum {ID=0, STATION_ID, TYPE, ENABLED, NUMBER, IP_ADDRESS, DIRECTION, PREDEFINED_DIRECTION, ENTRIES_COUNT};

string entry_name;
vector<char> temp_str(512);
vector<string> entries_names(ENTRIES_COUNT);
int id_escalator=-1, id_station=-1, esc_type=-1, number=-1,
	predef_direction=-1, direction=-1, enabled=-1;
in_addr_t ip=INADDR_NONE;

entries_names[ID]="id";
entries_names[STATION_ID]="stationID";
entries_names[TYPE]="type";
entries_names[ENABLED]="enabled";
entries_names[NUMBER]="num";
entries_names[IP_ADDRESS]="ip";
entries_names[DIRECTION]="direction";
entries_names[PREDEFINED_DIRECTION]="predef_direction";

entry_name=PxConfigNextString(&temp_str[0], 
							                        temp_str.size()
							                      );

while(!entry_name.empty())
{

if (entry_name.compare(entries_names[ID])==0) 
	{

			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0)
			{
				if (g_escalators.find(temp_int)==g_escalators.end())
					{
						id_escalator=temp_int;
					} else 	{
						string message("Escalator with id  ");
						message+=&temp_str[0];
						message+=" already exist in g_escalators";
						g_system_settings.sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
					};
			} else {
						string message("Wrong escalator id  ");
						message+=&temp_str[0];
						g_system_settings.sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
			};
			
	
	}else if (entry_name.compare(entries_names[STATION_ID])==0) {
	
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0)
			{
				if (g_stations.find(temp_int)!=g_stations.end())
					{
						id_station=temp_int;
					} else 	{
						string message("Not found escalator`s station id  ");
						message+=&temp_str[0];
						g_system_settings.sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
					};
			} else {
						string message("Wrong  escalator`s station id ");
						message+=&temp_str[0];
						g_system_settings.sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
			};
			

	}else if (entry_name.compare(entries_names[TYPE])==0) {

			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0)
			{
				if (g_metro_escalator_types.find(temp_int)!=g_metro_escalator_types.end())
					{
						esc_type=temp_int;
					} else 	{
						string message("Not found escalator`s type id  ");
						message+=&temp_str[0];
						g_system_settings.sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
					};
			} else {
						string message("Wrong  escalator`s type id ");
						message+=&temp_str[0];
						g_system_settings.sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
			};
			
	}else if (entry_name.compare(entries_names[ENABLED])==0) { 	
			int temp_int = atoi(&temp_str[0]);
			if (temp_int==system_settings::ENABLED)
			{
				enabled=system_settings::ENABLED;
			} else {
				enabled=system_settings::DISABLED;
			};

	}else if (entry_name.compare(entries_names[NUMBER])==0) { 	
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0)
			{
						number=temp_int;
			} else {
						string message("Wrong escalator number  ");
						message+=&temp_str[0];
						g_system_settings.sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
			};


	}else if (entry_name.compare(entries_names[IP_ADDRESS])==0) {
	
			ip=inet_addr(&temp_str[0]);
			if (ip==INADDR_NONE)
				{
					string message("Wrong escalator ip  ");
					message+=&temp_str[0];
					g_system_settings.sys_message(system_settings::ERROR_MSG,  message);		
					return(false);
				};
				 
			
	}else if (entry_name.compare(entries_names[DIRECTION])==0) { 	
			int temp_int=atoi(&temp_str[0]);
			if (temp_int==system_settings::DIRECTION_STOP  ||
				temp_int==system_settings::DIRECTION_UP ||
				temp_int==system_settings::DIRECTION_DOWN)
				{
					direction = temp_int;
				} else {
				
					string message("Wrong escalator direction  ");
					message+=&temp_str[0];
					g_system_settings.sys_message(system_settings::ERROR_MSG,  message);		

					direction = system_settings::DIRECTION_STOP;
				};
			
	}else if (entry_name.compare(entries_names[PREDEFINED_DIRECTION])==0) { 	
			int temp_int=atoi(&temp_str[0]);
			if (temp_int==system_settings::DIRECTION_UP ||
				temp_int==system_settings::DIRECTION_DOWN ||
				temp_int==system_settings::DIRECTION_REVERSE)
				{
					predef_direction=temp_int;
				} else {
					string message("Wrong escalator predefined direction  ");
					message+=&temp_str[0];
					g_system_settings.sys_message(system_settings::ERROR_MSG,  message);		

					direction = system_settings::DIRECTION_REVERSE;
				};

	} else {
			string message("Unrecognized config entry  ");
			message+=entry_name;
			g_system_settings.sys_message(system_settings::ERROR_MSG,  message);		
			
			return (false);
	};
	
entry_name=PxConfigNextString(&temp_str[0], 
							                        temp_str.size()
							                      );
}; //while 

if (id_escalator>0 && 
	esc_type>0 &&
	number>0 && 
	ip!=INADDR_NONE)
	{
	if (id_station<0) 
			{
			if (g_stations.get_current_station()!=g_stations.end())
				{
					id_station=g_stations.get_current_station()->second.get_id();
				} else {
					vector<char> tmp_chars(10);
					string message("Not found current station id for escalator ");
					itoa(id_escalator, &tmp_chars[0], 10);
					message+=&tmp_chars[0];
					g_system_settings.sys_message(system_settings::ERROR_MSG,  message);		
				};
			};
	if (predef_direction<0) predef_direction=system_settings::DIRECTION_REVERSE;
	if (direction<0) direction=system_settings::DIRECTION_STOP;
	if (enabled<0) 	enabled=system_settings::DISABLED;

	metro_stations_container::iterator_metro_stations iter_stat;
	iter_stat=g_stations.find(id_station);
	if (iter_stat!=g_stations.end())
			{ 
			
			metro_escalators_container::pair_metro_escalators tmp_pair
																						(id_escalator,
																						 metro_escalator
																								 (	id_escalator, 
																									id_station,
																									esc_type,
																									number,
																									predef_direction,
																									direction,
																									enabled,
																									ip
																								)
																						);

				iter_stat->second.push_back_escalators_id(id_escalator); 
				g_escalators.set_current_escalator(g_escalators.insert(g_escalators.upper_bound(id_escalator), tmp_pair));	
			} else { // it`s checked and imosible
				vector<char> tmp_chars(10);
				string message("Not found station id  ");
				itoa(id_station, &tmp_chars[0], 10);
				message+=&tmp_chars[0];
				message+=" for escalator ";
				itoa(id_escalator, &tmp_chars[0], 10);
				message+=&tmp_chars[0];
				g_system_settings.sys_message(system_settings::ERROR_MSG,  message);		
				return(false);
			};		

	}  else {//if (id_escalator>0 &&
		g_system_settings.sys_message(system_settings::ERROR_MSG, 
															string ("Not found all of required entries for escalator"));		
		return(false);

	};

return(true);
};


/*
bool 
	get_station_parameters ()

That function reads station`s parameters from config file 
and insert that station into g_stations. 
	id -  id of station. If station with same id present in g_stadions, 
function write error message and return false.
	name -  name of station. If not present,  
function write error message and return false.
	lineID  - id of line in g_lines. If not present, station id 
must be added to  g_lines.get_current_line(). If 
g_lines.get_current_line()==g_lines.end()  function write error 
message and return false. If lineID is present, but line with  lineID
not present in g_lines, function write  error message and return false.
If lineID present in g_lines, station id  must be added to that
line.
	x - x coordinate of station on main_wnd::Sheme widget. If not present,
function write error message and return false.
	y - y coordinate of station on main_wnd::Sheme widget. If not present,
function write error message and return false.
	order  - order station in line`s stations.
*/

bool 
	get_station_parameters ()
{
enum {ID=0, NAME, LINE_ID, X_COORD, Y_COORD, ORDER,  ENTRIES_COUNT};

string entry_name, name_station;
vector<char> temp_str(512);
vector<string> entries_names(ENTRIES_COUNT);
int id_station(-1), id_line(-1), x_coord(-1), y_coord(-1), order(-1);

entries_names[ID]="id";
entries_names[NAME]="name";
entries_names[LINE_ID]="lineID";
entries_names[X_COORD]="x";
entries_names[Y_COORD]="y";
entries_names[ORDER]="order";

entry_name=PxConfigNextString(&temp_str[0], 
							                        temp_str.size()
							                      );

while(!entry_name.empty())
{

if (entry_name.compare(entries_names[ID])==0) 
	{
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0)
			{
				if (g_stations.find(temp_int)==g_stations.end())
					{
						id_station=temp_int;
					} else 	{
						string message("Station with id  ");
						message+=&temp_str[0];
						message+=" already exist in g_stations";
						g_system_settings.sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
					};
			} else {
						string message("Wrong station id  ");
						message+=&temp_str[0];
						g_system_settings.sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
			};
	
	} else if (entry_name.compare(entries_names[NAME])==0) {
			name_station=&temp_str[0];
	}  else if (entry_name.compare(entries_names[LINE_ID])==0) {
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0)
			{
						if(g_lines.find(temp_int)!=g_lines.end())
							{
								id_line=temp_int;
							} else {
								string message("Not found station`s line id  ");
								message+=&temp_str[0];
								g_system_settings.sys_message(system_settings::ERROR_MSG,  message);		
								return(false);
							};
			} else {
						string message("Wrong station`s line id ");
						message+=&temp_str[0];
						g_system_settings.sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
			};

	}  else if (entry_name.compare(entries_names[X_COORD])==0) {
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0)
			{
						x_coord=temp_int;
			} else {
						string message("Wrong station`s X coordinate ");
						message+=&temp_str[0];
						g_system_settings.sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
			};

	}  else if (entry_name.compare(entries_names[Y_COORD])==0) {
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0)
			{
						y_coord=temp_int;
			} else {
						string message("Wrong station`s Y coordinate ");
						message+=&temp_str[0];
						g_system_settings.sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
			};
	}  else if (entry_name.compare(entries_names[ORDER])==0) {
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0)
			{
						order=temp_int;
			} else {
						string message("Wrong station`s order value ");
						message+=&temp_str[0];
						g_system_settings.sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
			};

		} else {
			string message("Unrecognized config entry  ");
			message+=entry_name;
			g_system_settings.sys_message(system_settings::ERROR_MSG,  message);		

			return (false);
	};
	
entry_name=PxConfigNextString(&temp_str[0], 
							                        temp_str.size()
							                      );
};

if (id_station>0 &&
	 x_coord>0 &&
	 y_coord>0 &&
	 !name_station.empty())
	 {

	 if (id_line<0) 
	 	{
			if (g_lines.get_current_line()!=g_lines.end())
				{
					id_line=g_lines.get_current_line()->second.get_id();
				} else {
					vector<char> tmp_chars(10);
					string message("Not found current line id  for station  ");
					itoa(id_station, &tmp_chars[0], 10);
					message+=&tmp_chars[0];
					g_system_settings.sys_message(system_settings::ERROR_MSG,  message);		
				};
				
	 	};

	if (order<0) {
				order=1;
				}
	 					
	metro_lines_container::iterator_metro_lines iter_line;
	iter_line=g_lines.find(id_line);
	if (iter_line!=g_lines.end())
			{ 
				metro_stations_container::pair_metro_stations tmp_pair
																						(id_station,
																						 metro_station
																							 (	id_station,
																							 	id_line,
																								x_coord,
																								y_coord,											 
																								name_station,
																								order																							 	
																							)
																						);
			
				iter_line->second.push_back_stations_id(id_station); 
				g_stations.set_current_station(g_stations.insert(g_stations.upper_bound(id_station), tmp_pair));	
			} else { // it`s checked and imosible
				vector<char> tmp_chars(10);
				string message("Not found line id  ");
				itoa(id_line, &tmp_chars[0], 10);
				message+=&tmp_chars[0];
				message+=" for station ";
				itoa(id_station, &tmp_chars[0], 10);
				message+=&tmp_chars[0];
				g_system_settings.sys_message(system_settings::ERROR_MSG,  message);		
				return(false);
			};		

	 					
	 					
	 } else {
		g_system_settings.sys_message(system_settings::ERROR_MSG, 
															string ("Not found all of required entries for station"));		
		return(false);
	 };

return(true);
};

/*
bool 
	get_line_parameters ()

That function reads line`s parameters from config file 
and insert that line into g_lines. 
	id -  id of line. If line with same id present in g_lines, 
function write error message and return false.
	name -  name of line. If not present,  
function write error message and return false.
	color  -  color of line. If not present,  
sets metro_line::DEFAULT_COLOR. Don`t set color ==0!!!
*/

bool 
	get_line_parameters ()
{
enum {ID=0, NAME, COLOR, ENTRIES_COUNT};
string entry_name, line_name;
vector<char> temp_str(512);
vector<string> entries_names(ENTRIES_COUNT);
int line_id(-1);
PgColor_t line_color(0);


entries_names[ID]="id";
entries_names[NAME]="name";
entries_names[COLOR]="color";


entry_name=PxConfigNextString(&temp_str[0], 
							                        temp_str.size()
							                      );

while(!entry_name.empty())
{

if (entry_name.compare(entries_names[ID])==0) 
	{
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0)
			{
				if (g_lines.find(temp_int)==g_lines.end())
					{
						line_id=temp_int;
					} else 	{
						string message("Line with id  ");
						message+=&temp_str[0];
						message+=" already exist in g_lines";
						g_system_settings.sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
					};
			} else {
						string message("Wrong line id  ");
						message+=&temp_str[0];
						g_system_settings.sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
			};
	
	} else if (entry_name.compare(entries_names[NAME])==0) {
		line_name=&temp_str[0];
	}  else if (entry_name.compare(entries_names[COLOR])==0) {
			long temp_int = strtol(&temp_str[0], NULL, 16);
			if (temp_int>0)
			{
				line_color=static_cast<PgColor_t>(temp_int);
			} else {
						string message("Wrong line color  ");
						message+=&temp_str[0];
						g_system_settings.sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
			};
			
	} else {
			string message("Unrecognized config entry  ");
			message+=entry_name;
			g_system_settings.sys_message(system_settings::ERROR_MSG,  message);		

			return (false);
	};
	
entry_name=PxConfigNextString(&temp_str[0], 
							                        temp_str.size()
							                      );
};


if ( line_id>0 &&
	! line_name.empty() ) 
	{
	if (line_color<=0) line_color=metro_line::DEFAULT_COLOR;
	metro_lines_container::pair_metro_lines tmp_pair (line_id,
																				metro_line 
																					(
																					line_id,
																					line_name,
																					line_color
																					)
																				);
		
	g_lines.set_current_line(g_lines.insert(g_lines.upper_bound(line_id), tmp_pair));	
	
	} else {
		g_system_settings.sys_message(system_settings::ERROR_MSG, 
															string ("Not found all of required entries for station"));		
		return(false);
	};

 return(true);
};



bool load_map(string file_name)
{
 	enum {LINE=0, STATION, ESCALATOR, ENTRIES_COUNT};

	string	section_name;
	vector<string> sections_names(ENTRIES_COUNT);

	sections_names[LINE]="line";
	sections_names[STATION]="station";
 	sections_names[ESCALATOR]="escalator";
 	
	if (PxConfigOpen( file_name.c_str(), PXCONFIG_READ)==Pt_FALSE )
	{
		string message("Can`t open config file ");
		message+=file_name.c_str();
		g_system_settings.sys_message(system_settings::ERROR_MSG,  message);

		return 0;
	};
	section_name=PxConfigNextSection();
	while (!section_name.empty()) 
	{
			if (section_name.compare(sections_names[LINE])==0) 
			{
				if (! get_line_parameters ()) {return(false);};
			} else if (section_name.compare(sections_names[STATION])==0) {
				if (! get_station_parameters ()) {return(false);};
			} else if (section_name.compare(sections_names[ESCALATOR])==0) {
				if (! get_escalator_parameters () ) {return(false);};
			} else {
				string message("Unrecognized config section ");
				message+=section_name.c_str();
				g_system_settings.sys_message(system_settings::ERROR_MSG,  message);
				return 0;
			};
			
			section_name=PxConfigNextSection();
	};	

	g_lines.set_current_line(g_lines.end());
	g_stations.set_current_station(g_stations.end());
	g_escalators.set_current_escalator(g_escalators.end());

	if  (PxConfigClose()) 
	{return (true);}
	else
	{
		string message("Can`t close config file ");
		message+=file_name.c_str();
		g_system_settings.sys_message(system_settings::ERROR_MSG,  message);

		return (false);
	};
	
	return (true);
}

int CreateScheme()
{
	PhDim_t		*size;
	vector<PtArg_t>		args(3);

    PtGetResource(ABW_Scheme, Pt_ARG_DIM, &size, 0);
 
	metro_lines_container::iterator_metro_lines   temp_lines_iterator;   
	
	temp_lines_iterator=g_lines.begin();
	
	while (temp_lines_iterator!=g_lines.end())
	{
		vector<PhPoint_t>  points;
		
		temp_lines_iterator->second.sort_stations_id();
		metro_line::iterator_stations_id temp_stations_id_iterator=temp_lines_iterator->second.begin_stations_id();
		while(temp_stations_id_iterator!=temp_lines_iterator->second.end_stations_id())
		{
			metro_stations_container::iterator_metro_stations temp_metro_stations_iterator=g_stations.find(*temp_stations_id_iterator);
			if(temp_metro_stations_iterator!=g_stations.end())
				{
					PhPoint_t temp_point;
					temp_point.x=temp_metro_stations_iterator->second.get_x_coord()*size->w/100;
					temp_point.y= temp_metro_stations_iterator->second.get_y_coord()*size->h/100;
					points.push_back(temp_point);
				}; 
		
			temp_stations_id_iterator++;
		};

	 	PtSetArg(&args[0], Pt_ARG_COLOR, temp_lines_iterator->second.get_color(), 0);
	 	PtSetArg(&args[1], Pt_ARG_POINTS, &points[0], points.size());	 	
	 	PtSetArg(&args[2], Pt_ARG_LINE_WIDTH, 6, 0);
	    temp_lines_iterator->second.set_line_widget( PtCreateWidget(PtPolygon,
	    																									ABW_Scheme,
	    																									args.size(),
	    																									&args[0])
	    																		);
        	if (temp_lines_iterator->second.get_line_widget()!=NULL
        		&& PtRealizeWidget(temp_lines_iterator->second.get_line_widget())!=0 ) 
        		{
			vector<char> tmp_chars(10);
			string message("Can`t create widget for line ");
			itoa (temp_lines_iterator->second.get_id(), &tmp_chars[0], 10);
			g_system_settings.sys_message(system_settings::ERROR_MSG,  message);

        			return 0;
        		};

		temp_stations_id_iterator=temp_lines_iterator->second.begin_stations_id();
		while(temp_stations_id_iterator!=temp_lines_iterator->second.end_stations_id())
		{
			metro_stations_container::iterator_metro_stations temp_metro_stations_iterator=g_stations.find(*temp_stations_id_iterator);
			if(temp_metro_stations_iterator!=g_stations.end())
				{
					temp_metro_stations_iterator->second.create_wnd(ABW_Scheme, size ->w, size ->h);
				}; 
		
			temp_stations_id_iterator++;
		};

    	
		temp_lines_iterator++;
	};
	return 1;
}

int ResizeScheme( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	PtArg_t		arg;
	PhDim_t		*wnd_size;
	PtContainerCallback_t	*data = (PtContainerCallback_t*)cbinfo->cbdata;
	vector<PhPoint_t>  points;

	metro_lines_container::iterator_metro_lines   temp_lines_iterator;   
	
	temp_lines_iterator=g_lines.begin();
	while (temp_lines_iterator!=g_lines.end())
	{
		vector<PhPoint_t>  points;

		metro_line::iterator_stations_id temp_stations_id_iterator=temp_lines_iterator->second.begin_stations_id();
		while(temp_stations_id_iterator!=temp_lines_iterator->second.end_stations_id())
		{
			metro_stations_container::iterator_metro_stations temp_metro_stations_iterator=g_stations.find(*temp_stations_id_iterator);
			if(temp_metro_stations_iterator!=g_stations.end())
				{
					PhPoint_t temp_point;
					
					temp_point.x=temp_metro_stations_iterator->second.get_x_coord()*data->new_dim.w/100;
					temp_point.y= temp_metro_stations_iterator->second.get_y_coord()*data->new_dim.h/100;
					
					temp_metro_stations_iterator->second.set_x_coord(temp_point.x);
					temp_metro_stations_iterator->second.set_y_coord(temp_point.y);
					
					points.push_back(temp_point);
			
					if (temp_metro_stations_iterator->second.get_wnd()!=NULL)
						{		
							PtSetArg(&arg, Pt_ARG_DIM, &wnd_size, 0);
							PtGetResources(temp_metro_stations_iterator->second.get_wnd(), 1, &arg);

							temp_point.x = temp_metro_stations_iterator->second.get_x_coord() - wnd_size->w/2;
							temp_point.y = temp_metro_stations_iterator->second.get_y_coord() - wnd_size->h/2;
	
					 		PtSetArg(&arg, Pt_ARG_POS, &temp_point, 0);
					 		PtSetResources(temp_metro_stations_iterator->second.get_wnd(), 1, &arg);		
					 	};

					
				}; 
			temp_stations_id_iterator++;
		};
		if (temp_lines_iterator->second.get_line_widget()!=NULL) 
			{
		 	PtSetArg(&arg, Pt_ARG_POINTS, &points[0], points.size());	 	
		 	PtSetResources(temp_lines_iterator->second.get_line_widget(), 1, &arg);
		 	};
    	
		temp_lines_iterator++;
	};

	return( Pt_CONTINUE );
}

// PtInputCallbackProcF_t PulseReceiver;

int PulseReceiver(void *data, int rcvid, void *message, size_t mbsize )
{
	struct _pulse* pulse;

	pulse = (struct _pulse*)message;	
//	printf("Code %d, value %d\n", pulse->code, pulse->value.sival_int);
	
	metro_escalators_container::iterator_metro_escalators temp_escalators_iterator=g_escalators.find(pulse->value.sival_int);
	if(temp_escalators_iterator!=g_escalators.end())
		{
			//	printf("Code %d, value %d ESCALATOR %p on %d\n", pulse->code, pulse->value.sival_int, temp_escalators_iterator->second.get_number(), temp_escalators_iterator->second.get_online());
			switch(pulse->code)
				{
					case 0:
					case 1:
					case 2:
//						printf("In PulseReceiver\n");
						temp_escalators_iterator->second.set_state(pulse->code);			
						break;
					case 3:
						temp_escalators_iterator->second.set_data();
						break;
					default:
						break;
				};
	
			PtFlush();

		
		} else {
			g_system_settings.sys_message(system_settings::ERROR_MSG,  
																string("Can`t find current escalator"));

			return Pt_HALT;
		};

	return Pt_HALT;
}



bool load_dictionary(string fileName)
{
/*
	FILE*		file;
	char		str[256];
	char		delims[] = "\t";
	char*	token;
	int			count = 0;
	CMsgDictContainer::MsgDictPair DictPair;
	
	file = fopen(fileName, "rt");
	if (file)
	{
		while(fgets(str, 256, file))
		{
			str[strlen(str) - 1] = 0;
				count = 0;
				token = strtok(str, delims);
			    while(token)
			    {
					 switch(count)
					 {
					 	case 0:		 		
				 			DictPair.second.number = atoi(token);
				 			DictPair.first=atoi(token);				 			
					 		break;
					 	case 1:
					 		DictPair.second.text = new char[(strlen(token) + 5) & ~3];
					 		if (DictPair.second.text)
					 		{
					 			strcpy(DictPair.second.text, token);
					 			strcat(DictPair.second.text, "\n");
					 		}
					 		else
					 		{
					 			SysMessage(ERROR_MSG, "Memory allocation");
					 			fclose(file);
					 			return 0;
					 		}
					 		break;
					 	case 2:
					 		if (!strcmp(token, "BLACK"))
					 			DictPair.second.color = system_settings::COLOR_BLACK;
					 		else if (!strcmp(token, "RED"))
					 			DictPair.second.color = system_settings::COLOR_RED;
					 		else if (!strcmp(token, "BLUE"))
					 			DictPair.second.color = system_settings::COLOR_BLUE;
					 		else if (!strcmp(token, "GREEN"))
					 			DictPair.second.color = system_settings::COLOR_GREEN;
					 		break;
					 	default:
					 		break;
					 }
      				token = strtok( NULL, delims );
      				count++;
    				}
    				g_msgDictionary.insert(DictPair);
		}
		fclose(file);
	}
	else
	{
		SysMessage(ERROR_MSG, "fail to open '%s' dictionary file", fileName);
		return 0;
	}

	SysMessage(INFO_MSG, "Dictionary loaded");
*/	
	return true;
}



bool load_escalators_types(string fileName)
{
/*
	Block*		curBlock = NULL;
	Type*		curType = NULL;
	Signal*		curSignal = NULL;
	char	buffer[256];
	char	temp[40];
	FILE*	handle;
   	char 	*val_name, *val_data;
    char  	tokens[] = "=";
	int	   	mode = 0;
	int 	   	intTemp, i;
	int		typeCount = -1, blockCount = -1, signalCount = -1;

	handle = fopen(fileName, "rt");
	if (!handle)
	{
		SysMessage(ERROR_MSG, "can't open escalators' library [%s]", strerror(errno));
		return 0;
	}
		
	while(fgets(buffer, 256, handle))
	{
		if (extract_string(buffer, temp, '[', ']'))
		{
			if (!strcmp("type", temp))
			{
				mode = 1;
				typeCount++;
				if (typeCount >= g_escTypeNum)
				{
					typeCount = -1;
					mode = 0;
					SysMessage(ERROR_MSG, "Parsing escalator library file");
					fclose(handle);
					return 0;
				}
				signalCount = -1;
				blockCount = -1;
			}
			else if (!strcmp("block", temp))
			{
				mode = 2;
				blockCount++;
				signalCount = -1;
			}
			else if (!strcmp("signal", temp))
			{
				mode = 3;
				signalCount++;
			}
		}
		else if (buffer[0] != '#')
		{
			val_name = strtok(buffer, tokens);
			val_data = strtok(NULL, tokens);
			if (val_name && val_data)
			{
				if (!strcmp("typeNum", val_name))
				{					
					g_escTypeNum = atoi(val_data);
					if (g_escTypeNum)
						g_escTypes = new Type[g_escTypeNum];
				}
				else if (!strcmp("blockNum", val_name))
				{	
					if (g_escTypes)
					{
						g_escTypes[typeCount].size = atoi(val_data);
						g_escTypes[typeCount].blocks = new Block[g_escTypes[typeCount].size];
					}
				}
				else if (!strcmp("signalNum", val_name))
				{					
					if (g_escTypes[typeCount].blocks)
					{
						g_escTypes[typeCount].blocks[blockCount].size = atoi(val_data);
						g_escTypes[typeCount].blocks[blockCount].signals = new Signal[g_escTypes[typeCount].blocks[blockCount].size];
					}
				}
				else if (!strcmp("num", val_name))
				{					
					g_escTypes[typeCount].blocks[blockCount].signals[signalCount].index = atoi(val_data);
				}
				else if (!strcmp("description", val_name))
				{					
					g_escTypes[typeCount].blocks[blockCount].signals[signalCount].hint.Set(val_data);
				}
				else if (!strcmp("name", val_name))
				{
					switch(mode)
					{
						case 1:
							g_escTypes[typeCount].name.Set(val_data);
							break;
						case 2:
							g_escTypes[typeCount].blocks[blockCount].name.Set(val_data);
							break;
						case 3:
							g_escTypes[typeCount].blocks[blockCount].signals[signalCount].name.Set(val_data);
							break;
						default:
							break;
					}
				}
			}
		}
	}

	fclose(handle);
*/	
	return true;
}



int Initialize( int argc, char *argv[] )
{
	PtInputId_t *InputRes=NULL;
	ApDBase_t *widgets_dbase;
	
	struct sigaction sa_sig_pipe;
	memset (&sa_sig_pipe, 0, sizeof(sa_sig_pipe));
	sa_sig_pipe.sa_handler=&SigpipeHandler;
	
	sigaction(SIGPIPE,&sa_sig_pipe,NULL);
   
	// загружаем библиотеку виджетов

	widgets_dbase=ApOpenDBase(ABM_interface);
  	g_system_settings.prepare_visualization(widgets_dbase);	    
	
	g_system_settings.sys_message(system_settings::INFO_MSG, 
														string("Spider: startup"));

    
	// файл сообщений для журнала
    	if (! load_dictionary(g_system_settings.get_global_messages_name()))
	{
		string mess("Файл сообщений журнала\n");
		mess+=g_system_settings.get_global_messages_name();
		mess+="\nне найден или поврежден";
		g_system_settings.message_window(system_settings::ERROR_MSG,
																   mess);

		PtExit(EXIT_FAILURE);
	}

	// инициализируем звук
/*    	if (!g_sound.Init())
	{
		WARN_BOX("Не инициализирован аудио драйвер");
	}*/
            
	if (! load_escalators_types(g_system_settings.get_escalator_types_name()))
	{
		string mess("Файл библиотеки эскалаторов\n");
		mess+=g_system_settings.get_escalator_types_name();
		mess+="\nне найден или поврежден";

		g_system_settings.message_window(system_settings::ERROR_MSG,
																   mess);

		PtExit(EXIT_FAILURE);
	}

	// загузка файла хемы
	if (! load_map(g_system_settings.get_devices_config_name()))
    	{
		string mess("Файл схем метрополина\n");
		mess+=g_system_settings.get_devices_config_name();
		mess+="\nне найден или поврежден";

		g_system_settings.message_window(system_settings::ERROR_MSG,
																   mess);

		PtExit(EXIT_FAILURE);
    	}
    
	
	// описание роутинга
	if (!g_router.load_routing(g_system_settings.get_routing_name()))
    	{
		string mess("Файл конфигурации роутинга\n");
		mess+=g_system_settings.get_routing_name();
		mess+="\nне найден или поврежден";

 		g_system_settings.message_window(system_settings::ERROR_MSG,
																   mess);
    	} else {
		// запускаем поток роуте
		g_router.start();
    	};
	
	// канал для пульсов
	g_chanID = PhChannelAttach(0, -1, NULL);
	if (g_chanID < 0)
		{
 		g_system_settings.message_window(system_settings::ERROR_MSG,
																   string("Ошибка содания канала IPC"));
	
		PtExit(EXIT_FAILURE);
		} 	else {
		string message ("Created channel. Id ");
		vector<char> tmp_chars(10);
		itoa (g_chanID, &tmp_chars[0], 10);
		message+=&tmp_chars[0];
		g_system_settings.sys_message(system_settings::INFO_MSG,
															message);
		};

	InputRes=PtAppAddInput(NULL, 0, PulseReceiver, NULL);
    	if (InputRes== NULL)
    	{
		g_system_settings.message_window(system_settings::ERROR_MSG,
																   string("Ошибка регистрации обработчика пульсов !"));

          PtExit(EXIT_FAILURE);
    	}
 
 
 	metro_escalators_container::iterator_metro_escalators iter_esc = g_escalators.begin();
	while (iter_esc != g_escalators.end())
	{
		iter_esc->second.connect_to_channel(g_chanID);	
		iter_esc->second.start();
		iter_esc++;
	}

	// запуск задачи опроса устройств
	if (pthread_create(&pingTID, NULL, &PingThread, NULL) != EOK)
	{
		g_system_settings.message_window(system_settings::ERROR_MSG,
																   string("Ошибка  запуска менеджера устройств!"));

		PtExit(EXIT_FAILURE);
	} 	else {
		g_system_settings.sys_message(system_settings::INFO_MSG, 
															string("Start ping thread"));

	};

	if (pthread_create(&timerTID, NULL, &TimerThread, NULL) != EOK)
	{
		g_system_settings.message_window(system_settings::ERROR_MSG,
																   string("Ошибка запуска системного таймера"));

		PtExit(EXIT_FAILURE);
	} 	else {
		g_system_settings.sys_message(system_settings::INFO_MSG, 
															string("Start system timer thread"));
	};
		
//	g_sound.Play("startup.wav");
//	g_sound.Play("test.wav");
	
	return( Pt_CONTINUE );
}

int Uninitialize(PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo)
{
	if (pthread_cancel(pingTID) != EOK)
		{
		string  message ("In closing ping thread error - ");
		message+=strerror(errno);
		g_system_settings.sys_message(system_settings::ERROR_MSG, 
															message);

		};

	if (pthread_cancel(timerTID) != EOK)
		{
		string  message ("In closing system timer thread error - ");
		message+=strerror(errno);
		g_system_settings.sys_message(system_settings::ERROR_MSG, 
															message);

		};
	
 	metro_escalators_container::iterator_metro_escalators iter_esc = g_escalators.begin();
	while (iter_esc != g_escalators.end())
	{
		iter_esc->second.disconnect_from_channel();
		iter_esc++;
	}

	// зыкрваем канал
	ChannelDestroy(g_chanID);
	// сохраняем лог
 	g_main_log.save(g_system_settings.get_main_log_name());

	// закрываем блоиотеку виджетов
	if (g_system_settings.get_widgets_dbase()!=NULL)
		{
			ApCloseDBase(g_system_settings.get_widgets_dbase());
		};
		
	return( Pt_CONTINUE );
}

int RealizeMainWnd( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	g_system_settings.set_main_window(widget);
	// создаем схему
	CreateScheme();
	// загружаем журнал
 	g_main_log.load(g_system_settings.get_main_log_name());
	
	// добавляем сообщение
 	g_main_log.set_records_autoincrement(g_main_log.get_records_autoincrement()+1); 
	g_main_log.insert(log_record (
												 g_main_log.get_records_autoincrement(),
												 143, //	message_id  Program started
												 0, // station_id -  broadcast
												 0, // device_id -  broadcast
												 time(NULL)
												)
							);

    	return( Pt_CONTINUE );	
}

int CloseMainWnd( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{

	g_system_settings.set_main_window(NULL);

	// добавляем сообщение
	g_main_log.set_records_autoincrement(g_main_log.get_records_autoincrement()+1);
	g_main_log.insert(log_record (
												 g_main_log.get_records_autoincrement(),
												 143, //	message_id Program closed
												 0, // station_id -  broadcast
												 0, // device_id -  broadcast
												 time(NULL)
												)
							);


	return( Pt_CONTINUE );
}

int FillEscConfList( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
		vector<const char*> list_strings;
		enum{UNDEFINED=0, UP, DOWN, REVERSE, ITEMS_COUNT};
		vector<string> directions_strings(ITEMS_COUNT);
				
		directions_strings[UNDEFINED]="не определено";
		directions_strings[UP]="вверх";
		directions_strings[DOWN]="вниз";
		directions_strings[REVERSE]="реверсивный";
	
		string item_string;
		vector<char> tmp_chars(8);
		int tmp_int;
		
		metro_stations_container::iterator_metro_stations iter_stat;
		metro_escalators_container::iterator_metro_escalators iter_esc=g_escalators.begin();
		
		while(iter_esc!=g_escalators.end())		
		{
			iter_stat=g_stations.find(iter_esc->second.get_station_id());		
			if (iter_stat==g_stations.end()) 
				{
					item_string="нет станции !\t";
				} else {
					item_string=iter_stat->second.get_stl_name_string();
					item_string="\t";
				};
			itoa(iter_esc->second.get_number(), &tmp_chars[0],10);
			item_string+=&tmp_chars[0];
			item_string+="\t";
			tmp_int=iter_esc->second.get_pref_direction();
			if (tmp_int<=UNDEFINED ||
				tmp_int>=ITEMS_COUNT) tmp_int=UNDEFINED;
				
			item_string+=directions_strings[tmp_int];
			list_strings.push_back(item_string.c_str());
				
			iter_esc++;
		};
	
		PtListAddItems(ABW_EscDirectionList, &list_strings[0], list_strings.size(), 0);	
		
		return( Pt_CONTINUE );
}


int
OpenArchive( PtWidget_t *link_instance, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

 	PtWidget_t*	wnd= g_archive_log.get_widget();

	if (wnd!=NULL)
	{
		PtWindowToFront(wnd);
		return (Pt_IGNORE);
	}

	return( Pt_CONTINUE );

	}

