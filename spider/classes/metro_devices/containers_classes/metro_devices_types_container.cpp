using namespace std;

#include <vector>
#include <string>
#include <hash_map>
#include <algorithm>
#include <functional>
#include <sstream>

#include <Ph.h>
#include <Ap.h>
#include <Pt.h>
#include <pthread.h>
#include <process.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/neutrino.h>
#include <sys/syspage.h>
#include <inttypes.h>
#include <fcntl.h>

#include "defines.h"
#include <sys/select.h> //#define FD_SETSIZE
#include "spider_exception.h"
#include "system_settings.h"
#include "system_settings_spider.h"

#include "msg_types_container.h"
#include "msg_dict_container.h"
#include "data_block.h"
#include "devices_types.h"
#include "metro_devices_types_container.h"

metro_devices_types_container* metro_devices_types_container::metro_devices_types_container_instance=NULL;
metro_devices_types_container* metro_devices_types_container::get_instance(){
    if (metro_devices_types_container_instance==NULL)
              metro_devices_types_container_instance=new metro_devices_types_container();
    return metro_devices_types_container_instance;
}; 

void metro_devices_types_container::load_message_parameters () throw (spider_exception)  {
msg_types_container* msg_types_cont=msg_types_container::get_instance();

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
	iterator tmp_iter;

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

	msg_dict_container *tmp_msg_dict_cont=tmp_iter->second->get_messages();

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
	
					msg_dict_container::value_type(message_id, 
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


void metro_devices_types_container::load_data_unit_parameters (data_unit_type unit_type)
                           throw(spider_exception) {
system_settings_spider* sys_sett_obj=system_settings_spider::get_instance();

enum {ID=0, INDEX, TYPE, BLOCK, VISIBILITY, NAME, ENTRIES_COUNT};
ostringstream exception_description;
string entry_name, data_unit_name;
const char *entry_name_c_str;
vector<char> temp_str(512);
vector<string> entries_names(ENTRIES_COUNT);
int data_unit_id(-1), data_unit_index(-1), block_id(-1), type_id(-1);
bool visibility=true;
system_settings::strings_container outer_states_strings=sys_sett_obj->get_outer_states_strings();

entries_names[ID]="id";
entries_names[INDEX]="index";
entries_names[TYPE]="type";
entries_names[BLOCK]="block";
entries_names[VISIBILITY]="visibility";
entries_names[NAME]="name";

entry_name_c_str=PxConfigNextString(&temp_str[0], 
							                        temp_str.size()
							                      );

while(entry_name_c_str!=NULL){
entry_name=entry_name_c_str;
if (entry_name.compare(entries_names[ID])==0) {
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0){
				data_unit_id=temp_int;
			} else {
		       exception_description<<"Wrong data_unit_id  "<<&temp_str[0];
		       throw (spider_exception(exception_description.str()));
			};

	} else if (entry_name.compare(entries_names[INDEX])==0) {
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0){
				data_unit_index=temp_int;
			} else {
		       exception_description<<"Wrong data_unit_index  "<<&temp_str[0];
		       throw (spider_exception(exception_description.str()));
			};

	} else if (entry_name.compare(entries_names[TYPE])==0) {
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0){
						type_id=temp_int;
			} else {
		       exception_description<<"Wrong type_id  "<<&temp_str[0];
		       throw (spider_exception(exception_description.str()));
			};

	} else if (entry_name.compare(entries_names[BLOCK])==0) {
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0)
			{
						block_id=temp_int;
			} else {
		       exception_description<<"Wrong block_id  "<<&temp_str[0];
		       throw (spider_exception(exception_description.str()));
			};

	} else if (entry_name.compare(entries_names[VISIBILITY])==0) {
			int temp_int;

			temp_int=system_settings::DISABLED;
			if (outer_states_strings[temp_int].compare(&temp_str[0])==0) 
				{
					 visibility=false;
				} else {
					temp_int=system_settings::ENABLED;
					if (outer_states_strings[temp_int].compare(&temp_str[0])==0) {
							visibility = true;
						} else {
                               exception_description<<"Wrong signal visibility state "
                                                                   <<&temp_str[0] 
                                                                   <<" ( expected enabled/disabled)"<<endl;
                           switch  (unit_type) {
                           case SIGNAL : {
                               exception_description<<" for signal";
                              break;};
                           case PARAMETER : {
                               exception_description<<" for parameter";
                              break; };
                            }; //switch (unit_type)
                       if (data_unit_id>0)
                               exception_description<<" id "<<data_unit_id;
                     if (data_unit_index>0)
                             exception_description<<" index "<<data_unit_index;

                         throw (spider_exception(exception_description.str()));
						};
				};	

	} else if (entry_name.compare(entries_names[NAME])==0) {
		data_unit_name=&temp_str[0];
	} else {
        exception_description<<"Unrecognized config entry  "<<entry_name;
        throw (spider_exception(exception_description.str()));
	};
	
	entry_name_c_str=PxConfigNextString(&temp_str[0], 
							                        temp_str.size()
							                      );
	};

	if (data_unit_id>0 && 
	    data_unit_index>0 &&
		! data_unit_name.empty() ) {
	
	iterator tmp_iter_device;
	if (type_id<0)
		{
			tmp_iter_device=current_devices_type;
		} else {
			tmp_iter_device=this->find(type_id);
		};
	if ( tmp_iter_device==this->end()){
            exception_description<<"Not found device_type for data_unit with id "<<data_unit_id;
            throw (spider_exception(exception_description.str()));
		};

	if (block_id<0)
			block_id=current_device_type_block->second.get_id();
		
	metro_device_type::iterator_blocks tmp_iter_blocks=
									tmp_iter_device->second->blocks_find(block_id);

	if (	tmp_iter_blocks==tmp_iter_device->second->blocks_end()) {
	           exception_description<<"Not found block id for data_unit with id "<<data_unit_id;
 	           throw (spider_exception(exception_description.str()));
			};

		if (tmp_iter_device->second->data_units_find(data_unit_id)!=
			tmp_iter_device->second->data_units_end()) {
                     exception_description<<"Signal with id "<<data_unit_id;
                     exception_description<<" already present in type "<<tmp_iter_device->second->get_id();
                     throw (spider_exception(exception_description.str()));
				};

	switch  (unit_type) {
       case SIGNAL : {
		tmp_iter_device->second->data_units_insert 
		      (tmp_iter_device->second->data_units_upper_bound(data_unit_id), 
				metro_device_type::data_unit_pair(data_unit_id, 
				                                                     new device_signal(data_unit_id,
                                                                                                   data_unit_index,
                                                                                                   data_unit_name,
                                                                                                    visibility)
																)
				);
		 break;
		}; 
       case PARAMETER : {
		tmp_iter_device->second->data_units_insert 
		      (tmp_iter_device->second->data_units_upper_bound(data_unit_id), 
				metro_device_type::data_unit_pair(data_unit_id, 
				                                                     new device_parameter(data_unit_id,
                                                                                                   data_unit_index,
                                                                                                   data_unit_name,
                                                                                                    visibility)
																)
				);
		 break;
		}; 

       default : {

                 exception_description<<"Wrong data_unit type in load_data_unit_parameters";
          throw (spider_exception(exception_description.str()));
		}; 

      }; // switch  (unit_type)

		tmp_iter_blocks->second.id_push_back(data_unit_id);
	
	} else {

  	   switch  (unit_type) {
           case SIGNAL : {
                 exception_description<<"Not found all of required entries for signal";
              break;
            };
           case PARAMETER : {
                 exception_description<<"Not found all of required entries parameter";
              break;
            };
        };
       if (data_unit_id>0)
              exception_description<<" id "<<data_unit_id;
       if (data_unit_index>0)
              exception_description<<" index "<<data_unit_index;

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
	iterator tmp_iter;

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

	if (tmp_iter->second->blocks_find(block_id)!=
		tmp_iter->second->blocks_end()) 
		{
           exception_description<<"Block with id "<<block_id<<" already exist";
           throw (spider_exception(exception_description.str()));
		};
	
	current_device_type_block=tmp_iter->second->blocks_insert 
	        (tmp_iter->second->blocks_upper_bound(block_id), 
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
                  metro_devices_types_container::value_type(devices_type_id, 
                 																		new metro_device_type(devices_type_id, 
                 																		                               devices_type_name)	)
          		);
	
	} else {
        exception_description<<"Not found all of required entries for devices_types_container";
        throw (spider_exception(exception_description.str()));
	};
}

void metro_devices_types_container::load (string file_name)  throw (spider_exception){
 	enum {TYPE_SECTION=0, BLOCK_SECTION, SIGNAL_SECTION, PARAMETER_SECTION, MESSAGE_SECTION,  ENTRIES_COUNT};
    ostringstream exception_description;
	this->erase (this->begin(), this->end());
	string	section_name;
	const char *section_name_c_str;
	vector<string> sections_names(ENTRIES_COUNT);

	sections_names[TYPE_SECTION]="type";
 	sections_names[BLOCK_SECTION]="block";
 	sections_names[PARAMETER_SECTION]="parameter";
 	sections_names[SIGNAL_SECTION]="signal";
  	sections_names[MESSAGE_SECTION]="message";
 
	if (PxConfigOpen( file_name.c_str(), PXCONFIG_READ)==Pt_FALSE ){
        exception_description<<"Can`t open config file "<<file_name;
        throw spider_exception(exception_description.str());
	};

	try {
        section_name_c_str=PxConfigNextSection();
        while (section_name_c_str!=NULL) {
        
            section_name=section_name_c_str;

			if (section_name.compare(sections_names[TYPE_SECTION])==0) {
				load_type_parameters ();

			} else if (section_name.compare(sections_names[BLOCK_SECTION])==0) {
				load_block_parameters ();

			} else if (section_name.compare(sections_names[SIGNAL_SECTION])==0) {
				load_data_unit_parameters (SIGNAL);

			} else if (section_name.compare(sections_names[PARAMETER_SECTION])==0) {
				load_data_unit_parameters (PARAMETER);

			} else if (section_name.compare(sections_names[MESSAGE_SECTION])==0) {
				load_message_parameters ();

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
