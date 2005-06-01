#include "global.h"
#include "system_settings.h"
#include "spider_exception.h"
#include "contain_msg_types.h"
#include "contain_msg_dict.h"

void
msg_dict_container::load_message_parameters (
           msg_types_container *msg_types_cont) throw (spider_exception){
enum {ID=0, TYPE, TEXT, ENTRIES_COUNT};
ostringstream exception_description;
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

while(entry_name_c_str!=NULL){
entry_name=entry_name_c_str;
if (entry_name.compare(entries_names[ID])==0) {
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0){
				if (this->find(temp_int)==this->end()){
						msg_id=temp_int;
					} else 	{
						exception_description<<"Message with id  "<<temp_int<<" already exist";
						throw spider_exception(exception_description.str());
					};
			} else {
               exception_description<<"Wrong id_msg  "<<&temp_str[0];
               throw spider_exception(exception_description.str());
			};
	
	} else if (entry_name.compare(entries_names[TEXT])==0) {
		msg_text=&temp_str[0];
	}  else if (entry_name.compare(entries_names[TYPE])==0) {

			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0){
					if (msg_types_cont->end()==msg_types_cont->find(temp_int)){
                            exception_description<<"Not found type_msg id  "<<temp_int;
                            throw spider_exception(exception_description.str());
						};

						msg_type_id=temp_int;
			} else {
                       exception_description<<"Wrong type_msg  "<<&temp_str[0];
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


	if (msg_id>0 &&
		msg_type_id>0 &&
		! msg_text.empty() ) {

	this->insert(
              this->upper_bound(msg_id),
               msg_dict_pair(msg_id, 
                                     dictionary_message(msg_id,
                                                                   msg_type_id,
                                                                   msg_text
                                                                    )
									)
						);
	
	} else {
       exception_description<<"Not found all of required entries for msg_dict_container";
       throw spider_exception(exception_description.str());
	};
}


void
	msg_dict_container::load(
	  string file_name,
      msg_types_container *msg_types_cont) throw (spider_exception) {

 	enum {MESSAGE=0, ENTRIES_COUNT};
	ostringstream exception_description;

	this->erase (this->begin(), 
						this->end());

	string	section_name;
	const char *section_name_c_str;
	vector<string> sections_names(ENTRIES_COUNT);

	sections_names[MESSAGE]="message";
	
	if (PxConfigOpen( file_name.c_str(), PXCONFIG_READ)!=Pt_TRUE ){
        exception_description<<"Can`t open config file "<<file_name;
		throw spider_exception(exception_description.str());
	};

     try {
                    section_name_c_str=PxConfigNextSection();
                    while (section_name_c_str!=NULL) {
                       section_name=section_name_c_str;

                       if (section_name.compare(sections_names[MESSAGE])==0) {
                           load_message_parameters (msg_types_cont);

                        } else {
                           exception_description<<"Unrecognized config section "<<section_name;
                           throw spider_exception(exception_description.str());
                        };
                      section_name_c_str=PxConfigNextSection();
                    };
	}catch  (spider_exception spr_exc) {
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


