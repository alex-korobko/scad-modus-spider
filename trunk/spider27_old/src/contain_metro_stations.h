#ifndef __CONT_METRO_STATIONS_H__
#define __CONT_METRO_STATIONS_H__

/*****************************************************************************
Container class for metro stations - 
wrapper of STL container  map
*****************************************************************************/

class metro_stations_container {
private :
struct ltint
	{
		  bool operator() (const int i1, const int i2) const
 			 {
   			 return (i1<i2) ;
  			}
	};
typedef  map <int,  metro_station, ltint> metro_stations_map;	
metro_stations_map	container_metro_stations;
typedef 	metro_stations_map::iterator iterator_metro_stations;	
typedef 	metro_stations_map::value_type pair_metro_stations;	
typedef 	metro_stations_map::size_type size_type_metro_stations;	

private :
int stations_count;
iterator_metro_stations current_station;

public :
metro_stations_container () : stations_count(0){current_station=container_metro_stations.end();};

/*
get_ and set_  metods for private data members
*/
int get_stations_autoincrement() {return (stations_count);}
void set_stations_autoincrement(int new_stations_count) {stations_count=new_stations_count;}

iterator_metro_stations get_current_station() {return(current_station);};
void set_current_station(iterator_metro_stations new_current_station) { current_station=new_current_station;};

/*
wrappers of current STL stations`s container metods
*/
iterator_metro_stations begin() {return container_metro_stations.begin();};
iterator_metro_stations end() {return container_metro_stations.end();};

iterator_metro_stations find(const int key) { return container_metro_stations.find(key); };
iterator_metro_stations upper_bound(const int key) { return container_metro_stations.upper_bound(key); };

void erase (iterator_metro_stations iter) { container_metro_stations.erase(iter); };
void erase (iterator_metro_stations iter_beg,
					iterator_metro_stations iter_end) { container_metro_stations.erase(iter_beg, iter_end); };

iterator_metro_stations insert (iterator_metro_stations iter, const pair_metro_stations& obj) 
{
  return (container_metro_stations.insert(iter,obj)); 
}

bool empty() const { return container_metro_stations.empty();}
size_type_metro_stations size() const {return container_metro_stations.size();};

/*
bool 
	load_station_parameters ()

That function reads station`s parameters from config file 
and insert that station into g_stations. 
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

bool 
	load_station_parameters (system_settings *sys_sett_obj, metro_lines_container* metro_lines_obj)
{
enum {ID=0, NAME, LINE_ID, X_COORD, Y_COORD, WAV_FILE, ENTRIES_COUNT};

const char *entry_name_c_str;
string entry_name, name_station, wave_file_name;
vector<char> temp_str(512);
vector<string> entries_names(ENTRIES_COUNT);
int id_station(-1), id_line(-1), x_coord(-1), y_coord(-1);

entries_names[ID]="id";
entries_names[NAME]="name";
entries_names[LINE_ID]="lineID";
entries_names[X_COORD]="x";
entries_names[Y_COORD]="y";
entries_names[WAV_FILE]="wav file";

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
						id_station=temp_int;
					} else 	{
						string message("Station with id  ");
						message+=&temp_str[0];
						message+=" already exist in stations container";
						sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
					};
			} else {
						string message("Wrong station id  ");
						message+=&temp_str[0];
						sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
			};
	
	} else if (entry_name.compare(entries_names[NAME])==0) {
			name_station=&temp_str[0];
	} else if (entry_name.compare(entries_names[WAV_FILE])==0) {
			wave_file_name=&temp_str[0];
	}  else if (entry_name.compare(entries_names[LINE_ID])==0) {
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0)
			{
						if(metro_lines_obj->find(temp_int)!=metro_lines_obj->end())
							{
								id_line=temp_int;
							} else {
								string message("Not found station`s line id  ");
								message+=&temp_str[0];
								sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);		
								return(false);
							};
			} else {
						string message("Wrong station`s line id ");
						message+=&temp_str[0];
						sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);		
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
						sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);		
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
						sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
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
};

if (id_station>0 &&
	 x_coord>0 &&
	 y_coord>0 &&
	 !name_station.empty())
	 {

	 if (id_line<0) 
	 	{
			if (metro_lines_obj->get_current_line()!=metro_lines_obj->end())
				{
					id_line=metro_lines_obj->get_current_line()->second.get_id();
				} else {
					vector<char> tmp_chars(10);
					string message("Not found current line id  for station  ");
					itoa(id_station, &tmp_chars[0], 10);
					message+=&tmp_chars[0];
					sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);		
				};
				
	 	};

	 					
	metro_lines_container::iterator_metro_lines iter_line;
	iter_line=metro_lines_obj->find(id_line);
	if (iter_line!=metro_lines_obj->end())
			{ 
				pair_metro_stations tmp_pair
														(id_station,
														 metro_station
																		 (	id_station,
																		 	id_line,
																			x_coord,
																			y_coord,											 
																			name_station,
																			wave_file_name
																		)
														);

				iter_line->second.push_back_stations_id(id_station); 
				this->set_current_station(this->insert(this->upper_bound(id_station), tmp_pair));	
			} else { // it`s checked and imosible
				vector<char> tmp_chars(10);
				string message("Not found line id  ");
				itoa(id_line, &tmp_chars[0], 10);
				message+=&tmp_chars[0];
				message+=" for station ";
				itoa(id_station, &tmp_chars[0], 10);
				message+=&tmp_chars[0];
				sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);		
				return(false);
			};		

	 					
	 					
	 } else {
		sys_sett_obj->sys_message(system_settings::ERROR_MSG, 
															string ("Not found all of required entries for station"));		
		return(false);
	 };

return(true);
};


bool load (system_settings *sys_sett_obj, metro_lines_container *metro_lines_cont, string file_name)
	{
 	enum {STATION=0, ENTRIES_COUNT};

	string	section_name;
	const char *section_name_c_str;
	vector<string> sections_names(ENTRIES_COUNT);

	sections_names[STATION]="station";
 	
	if (PxConfigOpen( file_name.c_str(), PXCONFIG_READ)==Pt_FALSE )
	{
		string message("Can`t open config file ");
		message+=file_name.c_str();
		sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);

		return 0;
	};

	erase(begin(), end());	

	section_name_c_str=PxConfigNextSection();
	
	while (section_name_c_str!=NULL) 
	{

	section_name=section_name_c_str;

			if (section_name.compare(sections_names[STATION])==0) 
			{
				if (! load_station_parameters (sys_sett_obj, metro_lines_cont)) {return(false);};			
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


};

#endif