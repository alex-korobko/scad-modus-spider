#ifndef __MSG_TYPES_CONT_H__
#define __MSG_TYPES_CONT_H__

class msg_type
{
private:
int id_msg_type;
string descr_msg_type;
PgColor_t color_msg_type;

msg_type();

public:
msg_type (int new_id_msg_type,
				string new_descr_msg_type,
				PgColor_t new_color_msg_type
				) :
				id_msg_type(new_id_msg_type),
				descr_msg_type(new_descr_msg_type),	
				color_msg_type(new_color_msg_type)
				{};

string get_type_description() {return descr_msg_type;};
int get_type_id() {return id_msg_type;};
int get_type_color() {return color_msg_type;};
};


/*****************************************************************************
Container class for messages types - 
wrapper of STL container  map
*****************************************************************************/

class msg_types_container {
private :
struct ltint
	{
		  bool operator() (const int i1, const int i2) const
 			 {
   			 return (i1<i2) ;
  			}
	};

typedef  map <int, msg_type, ltint> msg_types_cont;	
typedef  msg_types_cont::iterator msg_types_iterator;	
typedef  msg_types_cont::value_type msg_types_pair;	
typedef  msg_types_cont::size_type msg_types_size_type;	

msg_types_cont	container_msg_types;

public :
enum {DEFAULT_COLOR=Pg_BLACK};

msg_types_iterator begin() {return container_msg_types.begin();};
msg_types_iterator end() {return container_msg_types.end();};

msg_types_iterator find(const int key) { return container_msg_types.find(key); };
msg_types_iterator upper_bound(const int key) { return container_msg_types.upper_bound(key); };
void erase (msg_types_iterator iter) { container_msg_types.erase(iter); };
void erase (msg_types_iterator iter_beg,
					msg_types_iterator iter_end) { container_msg_types.erase(iter_beg, iter_end); };

msg_types_iterator insert (msg_types_iterator iter, const msg_types_pair& obj) 
{
  return (container_msg_types.insert(iter, obj)); 
}

bool empty() const { return container_msg_types.empty();}
msg_types_container::msg_types_size_type size() const {return container_msg_types.size();};

bool load_type_parameters (system_settings *sys_sett_obj)
{
enum {ID=0, DESCRIPTION, COLOR, ENTRIES_COUNT};
string entry_name, msg_type_description;
const char *entry_name_c_str;
vector<char> temp_str(512);
vector<string> entries_names(ENTRIES_COUNT);
int msg_type_id(-1);
PgColor_t msg_type_color(0);

entries_names[ID]="id";
entries_names[DESCRIPTION]="description";
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
						msg_type_id=temp_int;
					} else 	{
						string message("Message type with id  ");
						message+=&temp_str[0];
						message+=" already exist";
						sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
					};
			} else {
						string message("Wrong msg_type_id  ");
						message+=&temp_str[0];
						sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
			};
	
	} else if (entry_name.compare(entries_names[DESCRIPTION])==0) {
		msg_type_description=&temp_str[0];
	}  else if (entry_name.compare(entries_names[COLOR])==0) {
			long temp_int = strtol(&temp_str[0], NULL, 16);
			if (temp_int>0)
			{
				msg_type_color=static_cast<PgColor_t>(temp_int);
			} else {
						string message("Wrong msg_type_color  ");
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


	if ( msg_type_id>0 &&
	! msg_type_description.empty() ) 
	{
	if (msg_type_color<=0) msg_type_color=msg_types_container::DEFAULT_COLOR;

	this->insert(this->upper_bound(msg_type_id), 
						msg_types_container::msg_types_pair(msg_type_id, 
																					msg_type(msg_type_id, msg_type_description, msg_type_color)
																					)
						);
	
	} else {
		sys_sett_obj->sys_message(system_settings::ERROR_MSG, 
															string ("Not found all of required entries for msg_types_container"));		
		return(false);
	};

	return true;
}

bool load (system_settings *sys_sett_obj, string file_name)
	{
 	enum {TYPE=0, ENTRIES_COUNT};

	string	section_name;
	const char *section_name_c_str;
	vector<string> sections_names(ENTRIES_COUNT);

	sections_names[TYPE]="type";

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

			if (section_name.compare(sections_names[TYPE])==0) 
			{
				if (! load_type_parameters (sys_sett_obj)) {return(false);};			
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