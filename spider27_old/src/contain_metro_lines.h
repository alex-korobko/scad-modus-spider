#ifndef __CONT_METRO_LINES_H__
#define __CONT_METRO_LINES_H__

/*****************************************************************************
Container class for metro lines - 
wrapper of STL container  map
*****************************************************************************/

class metro_lines_container {
private :
struct ltint
	{
		  bool operator() (const int i1, const int i2) const
 			 {
   			 return (i1<i2) ;
  			}
	};
typedef  map <int, metro_line, ltint> metro_lines_map;	
metro_lines_map 	container_metro_lines;

public :
typedef 	metro_lines_map::iterator iterator_metro_lines;	
typedef 	metro_lines_map::value_type pair_metro_lines;	
typedef 	metro_lines_map::size_type size_type_metro_lines;	

private :
int lines_count;
iterator_metro_lines current_line;

public :

metro_lines_container(): lines_count(0){current_line=container_metro_lines.end();};

/*
get_ and set_  metods for private data members
*/
int get_lines_autoincrement() {return (lines_count);}
void set_lines_autoincrement(int new_lines_counter) {lines_count=new_lines_counter;}

iterator_metro_lines get_current_line() {return(current_line);};
void set_current_line (iterator_metro_lines new_current_line) {current_line=new_current_line;};

/*
wrappers of current STL lines`s container metods
*/
iterator_metro_lines begin() {return container_metro_lines.begin();};
iterator_metro_lines end() {return container_metro_lines.end();};

iterator_metro_lines find(const int key) { return container_metro_lines.find(key); };
iterator_metro_lines upper_bound(const int key) { return container_metro_lines.upper_bound(key); };

void erase (iterator_metro_lines iter) { container_metro_lines.erase(iter); };
void erase (iterator_metro_lines iter_beg, 
					iterator_metro_lines iter_end ) { container_metro_lines.erase(iter_beg, iter_end); };

iterator_metro_lines insert (iterator_metro_lines iter, const pair_metro_lines& obj) 
{
  return (container_metro_lines.insert(iter, obj)); 
}

bool empty() const { return container_metro_lines.empty();}
size_type_metro_lines size() const {return container_metro_lines.size();};

/*
bool 
	load_line_parameters ()

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
	load_line_parameters (system_settings *sys_sett_obj)
{
enum {ID=0, NAME, COLOR, ENTRIES_COUNT};
string entry_name, line_name;
const char *entry_name_c_str;
vector<char> temp_str(512);
vector<string> entries_names(ENTRIES_COUNT);
int line_id(-1);
PgColor_t line_color(0);


entries_names[ID]="id";
entries_names[NAME]="name";
entries_names[COLOR]="color";


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
						line_id=temp_int;
					} else 	{
						string message("Line with id  ");
						message+=&temp_str[0];
						message+=" already exist in lines container";
						sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
					};
			} else {
						string message("Wrong line id  ");
						message+=&temp_str[0];
						sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);		
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


if ( line_id>0 &&
	! line_name.empty() ) 
	{
	if (line_color<=0) line_color=metro_line::DEFAULT_COLOR;
	pair_metro_lines tmp_pair (line_id,
											metro_line 
														(
														line_id,
														line_name,
														line_color
														)
											);
		
	this->set_current_line(this->insert(this->upper_bound(line_id), tmp_pair));	
	
	} else {
		sys_sett_obj->sys_message(system_settings::ERROR_MSG, 
															string ("Not found all of required entries for line"));		
		return(false);
	};

 return(true);
};


bool load (system_settings *sys_sett_obj, string file_name)
	{
 	enum {LINE=0, ENTRIES_COUNT};

	string	section_name;
	const char *section_name_c_str;
	vector<string> sections_names(ENTRIES_COUNT);

	sections_names[LINE]="line";

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

			if (section_name.compare(sections_names[LINE])==0) 
			{
				if (! load_line_parameters (sys_sett_obj)) {return(false);};			
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