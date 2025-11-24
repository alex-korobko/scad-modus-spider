#include "global.h"
#include "system_settings.h"
#include "spider_exception.h"
#include "contain_msg_types.h"
#include "contain_msg_dict.h"
#include "devices_types.h"
#include "contain_metro_devices_types.h"

void metro_devices_types_container::load_message_parameters (
		msg_types_container *msg_types_cont) throw (spider_exception)  {
enum {ID=0, TYPE, MSG_TYPE,  TEXT, ENTRIES_COUNT};
ostringstream exception_description;
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
		       exception_description<<"Wrong message_id  "<<&temp_str[0];
		       throw (spider_exception(exception_description.str()));
			};

	} else if (entry_name.compare(entries_names[TYPE])==0) {
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0)
			{
						type_id=temp_int;
			} else {
		       exception_description<<"Wrong type_id  "<<&temp_str[0];
		       throw (spider_exception(exception_description.str()));
			};

	} else if (entry_name.compare(entries_names[MSG_TYPE])==0) {
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0)
			{
					message_type=temp_int;
			} else {
		       exception_description<<"Wrong message_type id  "<<&temp_str[0];
		       throw (spider_exception(exception_description.str()));
			};

	
	} else if (entry_name.compare(entries_names[TEXT])==0) {
		message_text=&temp_str[0];
			
	} else {
	       exception_description<<"Unrecognized config entry  "<<entry_name;
	       throw (spider_exception(exception_description.str()));
	};
	
	entry_name_c_str=PxConfigNextString(&temp_str[0], 
							                        temp_str.size()
							                      );
	};

	if ( message_id>0 &&
 		 message_type>0 &&	
		! message_text.empty()) {
	iterator_devices_types tmp_iter;

	if (type_id<0)
		{
			tmp_iter=current_devices_type;
		} else {
			tmp_iter=this->find(type_id);
		};

	if ( tmp_iter==this->end())
		{
	       exception_description<<"Not found devices_type for message "<<type_id;
	       throw (spider_exception(exception_description.str()));
		};

	msg_dict_container *tmp_msg_dict_cont=tmp_iter->second.get_messages();

	if (tmp_msg_dict_cont->find(message_id)!=
		tmp_msg_dict_cont->end()){
	       exception_description<<"Message with id "<<message_id<<" already exist";
	       throw (spider_exception(exception_description.str()));
		};

	if (msg_types_cont->find(message_type)==
		msg_types_cont->end()){
                 exception_description<<"Not found message type with id "<<message_type;
                 throw (spider_exception(exception_description.str()));
			};

	tmp_msg_dict_cont->insert(tmp_msg_dict_cont->upper_bound(message_id), 
	
					msg_dict_container::msg_dict_pair(message_id, 
																	 dictionary_message(message_id,
																	 								message_type, 
																	 								message_text)
																	)
												);

	} else {
       exception_description<<"Not found all of required entries for messages";
        throw (spider_exception(exception_description.str()));
	};
};


void metro_devices_types_container::load_signal_parameters ()
                           throw(spider_exception) {
enum {INDEX=0, TYPE, BLOCK,  NAME, HINT, ENTRIES_COUNT};
ostringstream exception_description;
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
		       exception_description<<"Wrong signal_index  "<<&temp_str[0];
		       throw (spider_exception(exception_description.str()));
			};

	} else if (entry_name.compare(entries_names[TYPE])==0) 
	{
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0)
			{
						type_id=temp_int;
			} else {
		       exception_description<<"Wrong type_id  "<<&temp_str[0];
		       throw (spider_exception(exception_description.str()));
			};

	} else if (entry_name.compare(entries_names[BLOCK])==0) 
	{
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0)
			{
						block_id=temp_int;
			} else {
		       exception_description<<"Wrong block_id  "<<&temp_str[0];
		       throw (spider_exception(exception_description.str()));
			};
	
	} else if (entry_name.compare(entries_names[NAME])==0) {
		signal_name=&temp_str[0];

	} else if (entry_name.compare(entries_names[HINT])==0) {
		signal_hint=&temp_str[0];
			
	} else {
        exception_description<<"Unrecognized config entry  "<<entry_name;
        throw (spider_exception(exception_description.str()));
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
            exception_description<<"Not found devices_type for signal ";
            throw (spider_exception(exception_description.str()));
		};

	if (block_id<0)
		{
			block_id=current_device_type_block->second.get_id();
		};
		
	metro_device_type::iterator_blocks tmp_iter_blocks=
									tmp_iter_device->second.blocks_find(block_id);

	if (	tmp_iter_blocks==tmp_iter_device->second.blocks_end()) 
			{
	           exception_description<<"Not found block id for signal id "<<signal_index;
 	           throw (spider_exception(exception_description.str()));
			};

		if (tmp_iter_device->second.signals_find(signal_index)!=
			tmp_iter_device->second.signals_end())
				{
                     exception_description<<"Signal with index "<<signal_index;
                     exception_description<<" already present in type "<<tmp_iter_device->second.get_id();
                     throw (spider_exception(exception_description.str()));
				};
	

		tmp_iter_device->second.signals_insert 
		      (tmp_iter_device->second.signals_upper_bound(signal_index), 
				metro_device_type::pair_signals(signal_index, 
																device_signal(signal_index, 
   																					signal_name,
                                                                                    signal_hint)
																)
				);

		tmp_iter_blocks->second.signals_id_push_back(signal_index);
	
	} else {
       exception_description<<"Not found all of required entries for signal";
       throw (spider_exception(exception_description.str()));
	};

}

void metro_devices_types_container::load_block_parameters ()
                    throw (spider_exception) {
enum {ID=0, TYPE,  NAME, ENTRIES_COUNT};
ostringstream exception_description;
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
               exception_description<<"Wrong block_id  "<<&temp_str[0];
               throw (spider_exception(exception_description.str()));
			};

	} else if (entry_name.compare(entries_names[TYPE])==0) 
	{
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0)
			{
						type_id=temp_int;
			} else {
               exception_description<<"Wrong type_id  "<<&temp_str[0];
               throw (spider_exception(exception_description.str()));
			};
	
	} else if (entry_name.compare(entries_names[NAME])==0) {
		block_name=&temp_str[0];
			
	} else {
           exception_description<<"Unrecognized config entry  "<<entry_name;
           throw (spider_exception(exception_description.str()));
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

	if ( tmp_iter==this->end()){
           exception_description<<"Not found devices_type for block, type_id "<<type_id;
           throw (spider_exception(exception_description.str()));
		};

	if (tmp_iter->second.blocks_find(block_id)!=
		tmp_iter->second.blocks_end()) 
		{
           exception_description<<"Block with id "<<block_id<<" already exist";
           throw (spider_exception(exception_description.str()));
		};
	
	current_device_type_block=tmp_iter->second.blocks_insert 
	        (tmp_iter->second.blocks_upper_bound(block_id), 
     		  metro_device_type::pair_blocks(block_id, 
  															device_block(block_id, 
																				block_name)
																)
            );

	} else {
        exception_description<<"Not found all of required entries for blocks";
         throw (spider_exception(exception_description.str()));
	};

}

void metro_devices_types_container::load_type_parameters ()
                   throw(spider_exception) {
enum {ID=0, NAME, ENTRIES_COUNT};
ostringstream exception_description;
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
                       exception_description<<"Device type with id  "<<&temp_str[0]<<" already exist";
                       throw (spider_exception(exception_description.str()));
					};
			} else {
                       exception_description<<"Wrong devices_type_id  "<<&temp_str[0];
                       throw (spider_exception(exception_description.str()));
			};
	
	} else if (entry_name.compare(entries_names[NAME])==0) {
		devices_type_name=&temp_str[0];
	} else {
           exception_description<<"Unrecognized config entry  "<<entry_name;
           throw (spider_exception(exception_description.str()));
	};
	
	entry_name_c_str=PxConfigNextString(&temp_str[0], 
							                        temp_str.size()
							                      );
	};


	if ( devices_type_id>0 &&
	! devices_type_name.empty() ) 
	{

	current_devices_type=this->insert
	            (this->upper_bound(devices_type_id), 
                  metro_devices_types_container::pair_devices_types(devices_type_id, 
                 																		metro_device_type(devices_type_id, 
                 																		                               devices_type_name)
                																			)
          		);
	
	} else {
        exception_description<<"Not found all of required entries for devices_types_container";
        throw (spider_exception(exception_description.str()));
	};
}

void metro_devices_types_container::load (
		string file_name,
		msg_types_container *msg_types_cont)  throw (spider_exception){

 	enum {TYPE=0, BLOCK, SIGNAL, MESSAGE,  ENTRIES_COUNT};
    ostringstream exception_description;
	this->erase (this->begin(), this->end());
	string	section_name;
	const char *section_name_c_str;
	vector<string> sections_names(ENTRIES_COUNT);

	sections_names[TYPE]="type";
 	sections_names[BLOCK]="block";
 	sections_names[SIGNAL]="signal";
  	sections_names[MESSAGE]="message";
 
	if (PxConfigOpen( file_name.c_str(), PXCONFIG_READ)==Pt_FALSE ){
        exception_description<<"Can`t open config file "<<file_name;
        throw spider_exception(exception_description.str());
	};

	try {
        section_name_c_str=PxConfigNextSection();
        while (section_name_c_str!=NULL) {
        
            section_name=section_name_c_str;

			if (section_name.compare(sections_names[TYPE])==0) {
				load_type_parameters ();

			} else if (section_name.compare(sections_names[BLOCK])==0) {
				load_block_parameters ();

			} else if (section_name.compare(sections_names[SIGNAL])==0) {
				load_signal_parameters ();

			} else if (section_name.compare(sections_names[MESSAGE])==0) {
				load_message_parameters (msg_types_cont);

			} else {
                exception_description<<"Unrecognized config section "<<section_name;
                throw spider_exception(exception_description.str());
			};
			
			section_name_c_str=PxConfigNextSection();
           };	 // while (section_name_c_str!=
       }catch (spider_exception spr_exc) {
       		exception_description<<spr_exc.get_description();
			if  (PxConfigClose()==Pt_FALSE){
			      exception_description<<"\nCan`t close config file "<<file_name;
				};
 	        throw spider_exception(exception_description.str());      
       };

	if  (PxConfigClose()==Pt_FALSE){
           exception_description<<"Can`t close config file "<<file_name;
           throw spider_exception(exception_description.str());
	};
}; 
