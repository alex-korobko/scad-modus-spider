#ifndef __CONT_DEVICES_TYPES_H_
#define __CONT_DEVICES_TYPES_H_


/*****************************************************************************
Container class for metro devices types - 
wrapper of STL container  map
*****************************************************************************/

class devices_types_container {
private :
struct ltint
	{
		  bool operator() (const int i1, const int i2) const
 			 {
   			 return (i1<i2) ;
  			}
	};

typedef  map<int, metro_device_type, ltint> metro_devices_type_container;	
typedef  metro_devices_type_container::iterator iterator_devices_types;	
typedef  metro_devices_type_container::value_type pair_devices_types;	
typedef  metro_devices_type_container::size_type size_devices_types;	

metro_devices_type_container	container_devices_types;

iterator_devices_types current_devices_type;
metro_device_type::iterator_blocks current_device_type_block;

public :

iterator_devices_types begin() {return container_devices_types.begin();};
iterator_devices_types end() {return container_devices_types.end();};

iterator_devices_types find(const int key) { return container_devices_types.find(key); };
iterator_devices_types upper_bound(const int key) { return container_devices_types.upper_bound(key); };

void erase (iterator_devices_types iter) { container_devices_types.erase(iter); };
void erase (iterator_devices_types iter_beg,
					iterator_devices_types iter_end) { container_devices_types.erase(iter_beg, iter_end); };

iterator_devices_types insert (iterator_devices_types iter, const pair_devices_types& obj) 
{
  return (container_devices_types.insert(iter, obj)); 
}

bool empty() const { return (container_devices_types.empty());}
size_devices_types size() const {return (container_devices_types.size());};

/*
load parameters from config file
*/
bool load_message_parameters (system_settings *sys_sett_obj,
													msg_types_container *msg_types_cont)

{
enum {ID=0, TYPE, MSG_TYPE,  TEXT, ENTRIES_COUNT};
string entry_name, message_text;
const char *entry_name_c_str;
vector<char> temp_str(512);
vector<string> entries_names(ENTRIES_COUNT);
int message_id(-1), type_id(-1), message_type(-1);

entries_names[ID]="id";
entries_names[TYPE]="type";
entries_names[MSG_TYPE]="message type";
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
				message_id=temp_int;

			} else {
						string message("Wrong message_id  ");
						message+=&temp_str[0];
						sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
			};

	} else if (entry_name.compare(entries_names[TYPE])==0) 
	{
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0)
			{
						type_id=temp_int;
			} else {
						string message("Wrong type_id  ");
						message+=&temp_str[0];
						sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
			};

	} else if (entry_name.compare(entries_names[MSG_TYPE])==0) 
	{
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0)
			{
					message_type=temp_int;
			} else {
						string message("Wrong message_type id  ");
						message+=&temp_str[0];
						sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
			};

	
	} else if (entry_name.compare(entries_names[TEXT])==0) {
		message_text=&temp_str[0];
			
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


	if ( message_id>0 &&
 		 message_type>0 &&	
		! message_text.empty()) 
	{
	iterator_devices_types tmp_iter;

	if (type_id<0)
		{
			tmp_iter=current_devices_type;
		} else {
			tmp_iter=this->find(type_id);
		};

	if ( tmp_iter==this->end())
		{
					sys_sett_obj->sys_message(system_settings::ERROR_MSG,
																string("Not found devices_type for message "));		

					return (false);
		};

	msg_dict_container *tmp_msg_dict_cont=tmp_iter->second.get_messages();

	if (tmp_msg_dict_cont->find(message_id)!=
		tmp_msg_dict_cont->end()) 
		{
					string mess("Message with id ");
					vector<char> tmp_chars(8);
					itoa(message_id, &tmp_chars[0], 10);
					mess+=&tmp_chars[0];
					mess+=" already exist";
					
					sys_sett_obj->sys_message(system_settings::ERROR_MSG, mess);		
					return (false);
		};

	if (msg_types_cont->find(message_type)==
		msg_types_cont->end())
			{
					string mess("Not found message type with id ");
					vector<char> tmp_chars(8);
					itoa(message_type, &tmp_chars[0], 10);
					mess+=&tmp_chars[0];
					
					sys_sett_obj->sys_message(system_settings::ERROR_MSG, mess);		
					return (false);
			};

	tmp_msg_dict_cont->insert(tmp_msg_dict_cont->upper_bound(message_id), 
	
					msg_dict_container::msg_dict_pair(message_id, 
																	 dictionary_message(message_id,
																	 								message_type, 
																	 								message_text)
																	)
												);

	} else {
		sys_sett_obj->sys_message(system_settings::ERROR_MSG, 
															string ("Not found all of required entries for messages"));		
		return(false);
	};

	return true;
}

bool load_signal_parameters (system_settings *sys_sett_obj)
{
enum {INDEX=0, TYPE, BLOCK,  NAME, HINT, ENTRIES_COUNT};
string entry_name, signal_name, signal_hint;
const char *entry_name_c_str;
vector<char> temp_str(512);
vector<string> entries_names(ENTRIES_COUNT);
int signal_index(-1), block_id(-1), type_id(-1);

entries_names[INDEX]="index";
entries_names[TYPE]="type";
entries_names[BLOCK]="block";
entries_names[NAME]="name";
entries_names[HINT]="hint";

entry_name_c_str=PxConfigNextString(&temp_str[0], 
							                        temp_str.size()
							                      );


while(entry_name_c_str!=NULL)
{
entry_name=entry_name_c_str;
if (entry_name.compare(entries_names[INDEX])==0) 
	{
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0)
			{
				signal_index=temp_int;

			} else {
						string message("Wrong signal_index  ");
						message+=&temp_str[0];
						sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
			};

	} else if (entry_name.compare(entries_names[TYPE])==0) 
	{
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0)
			{
						type_id=temp_int;
			} else {
						string message("Wrong type_id  ");
						message+=&temp_str[0];
						sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
			};

	} else if (entry_name.compare(entries_names[BLOCK])==0) 
	{
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0)
			{
						block_id=temp_int;
			} else {
						string message("Wrong block_id  ");
						message+=&temp_str[0];
						sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
			};
	
	} else if (entry_name.compare(entries_names[NAME])==0) {
		signal_name=&temp_str[0];

	} else if (entry_name.compare(entries_names[HINT])==0) {
		signal_hint=&temp_str[0];
			
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


	if ( signal_index>0 &&
		! signal_name.empty() &&
 		! signal_hint.empty() ) 
	{
	
	iterator_devices_types tmp_iter_device;
	if (type_id<0)
		{
			tmp_iter_device=current_devices_type;
		} else {
			tmp_iter_device=this->find(type_id);
		};
	if ( tmp_iter_device==this->end())
		{
					sys_sett_obj->sys_message(system_settings::ERROR_MSG,
																string("Not found devices_type for signal "));		
					return (false);
		};

	if (block_id<0)
		{
			block_id=current_device_type_block->second.get_id();
		};
		
	metro_device_type::iterator_blocks tmp_iter_blocks=
									tmp_iter_device->second.blocks_find(block_id);

	if (	tmp_iter_blocks==tmp_iter_device->second.blocks_end()) 
			{
					string mess("Not found block id for signal id ");

					vector<char> tmp_chars(8);
					itoa(signal_index, &tmp_chars[0], 10);
					mess+=&tmp_chars[0];

					sys_sett_obj->sys_message(system_settings::ERROR_MSG,
																mess);		
					return (false);
			};

		if (tmp_iter_device->second.signals_find(signal_index)!=
			tmp_iter_device->second.signals_end())
				{
						string mess("Signal with index ");

						vector<char> tmp_chars(8);
						itoa(signal_index, &tmp_chars[0], 10);
						mess+=&tmp_chars[0];
						mess+=" already present in type ";
						
						itoa(tmp_iter_device->second.get_id(), &tmp_chars[0], 10);
						mess+=&tmp_chars[0];

						sys_sett_obj->sys_message(system_settings::ERROR_MSG,
																mess);

						return (false);			
				};
	

		tmp_iter_device->second.signals_insert (tmp_iter_device->second.signals_upper_bound(signal_index), 
																							metro_device_type::pair_signals(signal_index, 
																																				device_signal(signal_index, 
																																									signal_name,
																																									signal_hint)
																																				)
																							);
		tmp_iter_blocks->second.signals_id_push_back(signal_index);
	
	} else {
		sys_sett_obj->sys_message(system_settings::ERROR_MSG, 
															string ("Not found all of required entries for signal"));		
		return(false);
	};

	return true;
}


bool load_block_parameters (system_settings *sys_sett_obj)
{
enum {ID=0, TYPE,  NAME, ENTRIES_COUNT};
string entry_name, block_name;
const char *entry_name_c_str;
vector<char> temp_str(512);
vector<string> entries_names(ENTRIES_COUNT);
int block_id(-1), type_id(-1);

entries_names[ID]="id";
entries_names[TYPE]="type";
entries_names[NAME]="name";

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
				block_id=temp_int;

			} else {
						string message("Wrong block_id  ");
						message+=&temp_str[0];
						sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
			};

	} else if (entry_name.compare(entries_names[TYPE])==0) 
	{
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0)
			{
						type_id=temp_int;
			} else {
						string message("Wrong type_id  ");
						message+=&temp_str[0];
						sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
			};


	
	} else if (entry_name.compare(entries_names[NAME])==0) {
		block_name=&temp_str[0];
			
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


	if ( block_id>0 &&
	! block_name.empty() ) 
	{
	iterator_devices_types tmp_iter;

	if (type_id<0)
		{
			tmp_iter=current_devices_type;
		} else {
			tmp_iter=this->find(type_id);
		};

	if ( tmp_iter==this->end())
		{
					sys_sett_obj->sys_message(system_settings::ERROR_MSG,
																string("Not found devices_type for block "));		

					return (false);
		};

	if (tmp_iter->second.blocks_find(block_id)!=
		tmp_iter->second.blocks_end()) 
		{
					string mess("Block with id ");
					vector<char> tmp_chars(8);
					itoa(block_id, &tmp_chars[0], 10);
					mess+=&tmp_chars[0];
					mess+=" already exist";
					
					sys_sett_obj->sys_message(system_settings::ERROR_MSG, mess);		
					return (false);
		};
	
	current_device_type_block=tmp_iter->second.blocks_insert (tmp_iter->second.blocks_upper_bound(block_id), 
																							metro_device_type::pair_blocks(block_id, 
																																				device_block(block_id, 
																																									block_name)
																																				)
																							);
	
	} else {
		sys_sett_obj->sys_message(system_settings::ERROR_MSG, 
															string ("Not found all of required entries for blocks"));		
		return(false);
	};

	return true;
}


bool load_type_parameters (system_settings *sys_sett_obj)
{
enum {ID=0, NAME, ENTRIES_COUNT};
string entry_name, devices_type_name;
const char *entry_name_c_str;
vector<char> temp_str(512);
vector<string> entries_names(ENTRIES_COUNT);
int devices_type_id(-1);

entries_names[ID]="id";
entries_names[NAME]="name";

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
						devices_type_id=temp_int;
					} else 	{
						string message("device type with id  ");
						message+=&temp_str[0];
						message+=" already exist";
						sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
					};
			} else {
						string message("Wrong devices_type_id  ");
						message+=&temp_str[0];
						sys_sett_obj->sys_message(system_settings::ERROR_MSG,  message);		
						return(false);
			};
	
	} else if (entry_name.compare(entries_names[NAME])==0) {
		devices_type_name=&temp_str[0];
			
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


	if ( devices_type_id>0 &&
	! devices_type_name.empty() ) 
	{

	current_devices_type=this->insert(this->upper_bound(devices_type_id), 
													devices_types_container::pair_devices_types(devices_type_id, 
																											metro_device_type(devices_type_id, devices_type_name)
																											)
													);
	
	} else {
		sys_sett_obj->sys_message(system_settings::ERROR_MSG, 
															string ("Not found all of required entries for devices_types_container"));		
		return(false);
	};

	return true;
}

bool load (system_settings *sys_sett_obj, msg_types_container *msg_types_cont, string file_name)
	{
 	enum {TYPE=0, BLOCK, SIGNAL, MESSAGE,  ENTRIES_COUNT};

	erase (begin(), end());

	string	section_name;
	const char *section_name_c_str;
	vector<string> sections_names(ENTRIES_COUNT);

	sections_names[TYPE]="type";
 	sections_names[BLOCK]="block";
 	sections_names[SIGNAL]="signal";
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

			if (section_name.compare(sections_names[TYPE])==0) 
			{
				if (! load_type_parameters (sys_sett_obj)) {return(false);};			
			} else if (section_name.compare(sections_names[BLOCK])==0) 
			{
				if (! load_block_parameters (sys_sett_obj)) {return(false);};			
			} else if (section_name.compare(sections_names[SIGNAL])==0) 
			{
				if (! load_signal_parameters (sys_sett_obj)) {return(false);};			
			} else if (section_name.compare(sections_names[MESSAGE])==0) 
			{
				if (! load_message_parameters (sys_sett_obj, msg_types_cont)) {return(false);};			
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