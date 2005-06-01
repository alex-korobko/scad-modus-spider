using namespace std;

#include <sys/types.h>
#include <sys/neutrino.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>

#include <string>
#include <vector>
#include <hash_map>
#include <functional>
#include <sstream>

#include <Ap.h>
#include <Pt.h>


#include "defines.h"
#include "spider_exception.h"
#include "system_settings.h"
#include "system_settings_spider.h"
#include "msg_types_container.h"

msg_types_container* msg_types_container::msg_types_container_instance=NULL;

msg_types_container* msg_types_container::get_instance() {
      if (msg_types_container_instance==NULL)
            msg_types_container_instance=new msg_types_container();
      return msg_types_container_instance;
};

void msg_types_container::load_type_parameters () 
        throw (spider_exception) {
enum {ID=0, DESCRIPTION, COLOR, ENTRIES_COUNT};
ostringstream exception_description;
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

while(entry_name_c_str!=NULL){
entry_name=entry_name_c_str;
if (entry_name.compare(entries_names[ID])==0) {
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0){
				if (this->find(temp_int)==this->end()){
						msg_type_id=temp_int;
					} else 	{
                        exception_description<<"Message type with id  "<<temp_int<<" already exist";
                        throw spider_exception(exception_description.str());
					};
			} else {
                exception_description<<"Wrong msg_type_id  "<<&temp_str[0];
                throw spider_exception(exception_description.str());
			};
	
	} else if (entry_name.compare(entries_names[DESCRIPTION])==0) {
		msg_type_description=&temp_str[0];
	}  else if (entry_name.compare(entries_names[COLOR])==0) {
			long temp_int = strtol(&temp_str[0], NULL, 16);
			if (temp_int>0){
				msg_type_color=static_cast<PgColor_t>(temp_int);
			} else {
                exception_description<<"Wrong msg_type_color  "<<temp_int;
                throw spider_exception(exception_description.str());
			};
			
	} else {
           exception_description<<"Unrecognized config entry  "<<entry_name;
            throw spider_exception(exception_description.str());
	};
	
	entry_name_c_str=PxConfigNextString(&temp_str[0], 
							                        temp_str.size()
							                      );
	};


	if ( msg_type_id>0 &&
	! msg_type_description.empty() ) {
	if (msg_type_color<=0) msg_type_color=msg_types_container::DEFAULT_COLOR;

	this->insert(this->upper_bound(msg_type_id), 
						value_type(msg_type_id, 
											msg_type(msg_type_id, msg_type_description, msg_type_color)
											)
						);
	
	} else {
       exception_description<<"Not found all of required entries for msg_types_container";
        throw spider_exception(exception_description.str());
	};

};

void msg_types_container::load (string file_name) throw (spider_exception) {
 	enum {TYPE=0, ENTRIES_COUNT};

	ostringstream exception_description;
	string	section_name;
	const char *section_name_c_str;
	vector<string> sections_names(ENTRIES_COUNT);

	sections_names[TYPE]="type";

	this->erase(this->begin(), 
						this->end());
 	
	if (PxConfigOpen( file_name.c_str(), PXCONFIG_READ)!=Pt_TRUE ){
       exception_description<<"Can`t open config file "<<file_name;
       throw spider_exception(exception_description.str());
	};

	try {
              section_name_c_str=PxConfigNextSection();
              while (section_name_c_str!=NULL) {
                        section_name=section_name_c_str;
                        if (section_name.compare(sections_names[TYPE])==0) {

                           load_type_parameters (); 
                        } else {
                            exception_description<<"Unrecognized config section "<<section_name;
                             throw spider_exception(exception_description.str());
                        };
			
                      section_name_c_str=PxConfigNextSection();
                };	
	} catch (spider_exception spr_exc) {
        exception_description<<spr_exc.get_description();
		if  (PxConfigClose()!=Pt_TRUE) {
             exception_description<<"\nCan`t close file "<<file_name;		
		}; 
         throw spider_exception(exception_description.str());
	};

	if  (PxConfigClose()!=Pt_TRUE) {
        exception_description<<"Can`t close file "<<file_name;		
         throw spider_exception(exception_description.str());
	}; 

}; 


