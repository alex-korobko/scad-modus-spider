#ifndef __MSG_DICT_CONT_H__
#define __MSG_DICT_CONT_H__

class	 dictionary_message
{
private:
	int		id_msg;
	int 	type_msg;
	string	text_msg;
	
	dictionary_message();
public:
	dictionary_message(int new_number, 
									int new_msg_type,
									string new_text) : 
	id_msg(new_number),
	type_msg(new_msg_type),
	text_msg(new_text) 
	{};

	string get_text () {return (text_msg);}
	int get_id () {return( id_msg);}
	int get_type () {return( type_msg);}
};

/*****************************************************************************
Container class for messages dictionary - 
wrapper of STL container  map
*****************************************************************************/

class msg_dict_container {
private :
struct ltint
	{
		  bool operator() (const int i1, const int i2) const
 			 {
   			 return (i1<i2) ;
  			}
	};

typedef  map <int, dictionary_message, ltint> msg_dictionary;	

msg_dictionary	container_msg_dict;

bool get_message_parameters (system_settings *sys_sett_obj, msg_types_container *msg_types_cont)
{
enum {ID=0, TYPE, TEXT, ENTRIES_COUNT};
string entry_name, msg_text;
const char *entry_name_c_str;
vector<char> temp_str(512);
vector<string> entries_names(ENTRIES_COUNT);
int msg_id(-1), msg_type_id(-1);

entries_names[ID]="id";
entries_names[TYPE]="type";
entries_names[TEXT]="text";


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
						msg_id=temp_int;
					} else 	{
						string message("Message with id  ");
						message+=&temp_str[0];
						message+=" already exist";
						sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
					};
			} else {
						string message("Wrong id_msg  ");
						message+=&temp_str[0];
						sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
			};
	
	} else if (entry_name.compare(entries_names[TEXT])==0) {
		msg_text=&temp_str[0];
	}  else if (entry_name.compare(entries_names[TYPE])==0) {

			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0)
			{
					if (msg_types_cont->end()==msg_types_cont->find(temp_int))
						{
							string message("Not found type_msg id  ");
							message+=&temp_str[0];
							sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);		
							return(false);
						};

						msg_type_id=temp_int;
			} else {
						string message("Wrong type_msg  ");
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


	if (msg_id>0 &&
		msg_type_id>0 &&
		! msg_text.empty() ) 
	{

	this->insert(this->upper_bound(msg_id), 
							msg_dict_pair(msg_id, 
											dictionary_message(msg_id, msg_type_id, msg_text)
											)
						);
	
	} else {
		sys_sett_obj->sys_message(system_settings::ERROR_MSG, 
															string ("Not found all of required entries for msg_dict_container"));		
		return(false);
	};

	return true;
}

public :
typedef  msg_dictionary::iterator msg_dict_iterator;	
typedef  msg_dictionary::value_type msg_dict_pair;	

enum {MSG_PROGRAM_STARTED=143, 
			MSG_PROGRAM_CLOSED=144,
			MESSAGES_COUNT
			};

msg_dict_iterator begin() {return container_msg_dict.begin();};
msg_dict_iterator end() {return container_msg_dict.end();};

msg_dict_iterator find(const int key) { return container_msg_dict.find(key); };
msg_dict_iterator upper_bound(const int key) { return container_msg_dict.upper_bound(key); };

void erase (msg_dict_iterator iter) { container_msg_dict.erase(iter); };
msg_dict_iterator insert (msg_dict_iterator iter, const msg_dict_pair& obj) 
{
  return (container_msg_dict.insert(iter, obj)); 
}

bool empty() const { return container_msg_dict.empty();}
int size() const {return container_msg_dict.size();};

bool load(system_settings *sys_sett_obj, msg_types_container *msg_types_cont, string file_name)
	{
 	enum {MESSAGE=0, ENTRIES_COUNT};

	string	section_name;
	const char *section_name_c_str;
	vector<string> sections_names(ENTRIES_COUNT);

	sections_names[MESSAGE]="message";
 	
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

			if (section_name.compare(sections_names[MESSAGE])==0) 
			{
				if (! get_message_parameters (sys_sett_obj, msg_types_cont)) {return(false);};			
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