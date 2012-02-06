using namespace std;

#include <sys/types.h>
#include <sys/neutrino.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>

#include <string>
#include <vector>
#include <hash_map>
#include <functional>
#include <algorithm>
#include <sstream>
#include <iostream.h>

#include <Ap.h>
#include <Pt.h>


#include "defines.h"
#include "spider_exception.h"
#include "system_settings.h"
#include "system_settings_spider.h"
#include "dispatchers_persons_container.h"

dispatcher_persons_container* dispatcher_persons_container::dispatcher_persons_container_instance=NULL;

dispatcher_persons_container* dispatcher_persons_container::get_instance() {
      if (dispatcher_persons_container_instance==NULL)
            dispatcher_persons_container_instance=new dispatcher_persons_container();
      return dispatcher_persons_container_instance;
};

//Sort dispatchers by stwo_dispatchers_sorter()
void dispatcher_persons_container::sort_all(){
	 container_dispatcher_persons.sort (dispatchers_sorter);
};

dispatcher_persons_container::iterator dispatcher_persons_container::find(const int key) {
      stwo_finder by_ids_finder(key);
      return find_if(container_dispatcher_persons.begin(), container_dispatcher_persons.end(), by_ids_finder);
 };


void dispatcher_persons_container::load_dispatcher_parameters () 
        throw (spider_exception) {
enum {ID=0, FIRST_NAME, MIDDLE_NAME, LAST_NAME, ENTRIES_COUNT};
ostringstream exception_description;
string entry_name, dispatcher_first_name, dispatcher_middle_name, dispatcher_last_name;
const char *entry_name_c_str;
vector<char> temp_str(512);
vector<string> entries_names(ENTRIES_COUNT);
ldword dispatcher_person_id(0);

entries_names[ID]="id";
entries_names[FIRST_NAME]="first name";
entries_names[MIDDLE_NAME]="middle name";
entries_names[LAST_NAME]="last name";

entry_name_c_str=PxConfigNextString(&temp_str[0],temp_str.size());

while(entry_name_c_str!=NULL){
entry_name=entry_name_c_str;
if (entry_name.compare(entries_names[ID])==0) {
			std::cout<<"dispatcher id read from ini file "<<&temp_str[0]<<std::endl;
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>0){
				if (this->find(temp_int)==this->end()){
						dispatcher_person_id=temp_int;
					} else 	{
                        exception_description<<"Dispatcher with id  "<<temp_int<<" already exist";
                        throw spider_exception(exception_description.str());
					};
			} else {
                exception_description<<"Wrong dispatcher person id  "<<&temp_str[0];
                throw spider_exception(exception_description.str());
			};
	
	} else if (entry_name.compare(entries_names[FIRST_NAME])==0) {
		dispatcher_first_name=&temp_str[0];

	} else if (entry_name.compare(entries_names[MIDDLE_NAME])==0) {
		dispatcher_middle_name=&temp_str[0];

	} else if (entry_name.compare(entries_names[LAST_NAME])==0) {
		dispatcher_last_name=&temp_str[0];

	} else {
           exception_description<<"Unrecognized config entry  "<<entry_name;
            throw spider_exception(exception_description.str());
	};
	
	entry_name_c_str=PxConfigNextString(&temp_str[0], 
							                        temp_str.size()
							                      );
	};


	if ( dispatcher_person_id>0 &&
	! dispatcher_first_name.empty() &&
	! dispatcher_middle_name.empty() &&
	! dispatcher_last_name.empty() ) {
std::cout<<"dispatcher id before insert "<<dispatcher_person_id<<std::endl;
	this->insert(value_type(dispatcher_person_id, 
                                            dispatcher_first_name,
                                            dispatcher_middle_name,
                                            dispatcher_last_name
											)
						);
	if (dispatchers_count<dispatcher_person_id) dispatchers_count=dispatcher_person_id;	
	} else {
       exception_description<<"Not found all of required entries for dispatcher_persons_container";
        throw spider_exception(exception_description.str());
	};

};


void dispatcher_persons_container::load_settings_parameters () 
        throw (spider_exception) {
enum {ID=0, CURRENT_DISPATCHER, ENTRIES_COUNT};
ostringstream exception_description;
string entry_name;
const char *entry_name_c_str;
vector<char> temp_str(512);
vector<string> entries_names(ENTRIES_COUNT);

entries_names[CURRENT_DISPATCHER]="current dispatcher";
entry_name_c_str=PxConfigNextString(&temp_str[0], 
							                        temp_str.size()
							                      );

while(entry_name_c_str!=NULL){
entry_name=entry_name_c_str;
if (entry_name.compare(entries_names[CURRENT_DISPATCHER])==0) {
			int temp_int = atoi(&temp_str[0]);
			if (temp_int>=0){
                current_dispatcher=this->find(temp_int);
			} else {
                exception_description<<"Wrong dispatcher person id  "<<&temp_str[0];
                throw spider_exception(exception_description.str());
			};
	
	} else {
           exception_description<<"Unrecognized config entry  "<<entry_name;
            throw spider_exception(exception_description.str());
	};
	
	entry_name_c_str=PxConfigNextString(&temp_str[0], 
							                        temp_str.size());
	};
};


void dispatcher_persons_container::load (string file_name) throw (spider_exception) {
 	enum {DISPATCHER=0, SETTINGS,  ENTRIES_COUNT};

	ostringstream exception_description;
	string	section_name;
	const char *section_name_c_str;
	vector<string> sections_names(ENTRIES_COUNT);

	sections_names[DISPATCHER]="dispatcher";
	sections_names[SETTINGS]="settings";

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
                        if (section_name.compare(sections_names[DISPATCHER])==0) {
                           load_dispatcher_parameters (); 
                        } else if (section_name.compare(sections_names[SETTINGS])==0) {
                           load_settings_parameters (); 
                        } else {
                           exception_description<<"Unrecognized config section "<<section_name;
                            throw spider_exception(exception_description.str());
                        };
			
                      section_name_c_str=PxConfigNextSection();
                };	
	} catch (spider_exception spr_exc) {
        exception_description<<spr_exc.get_description();
		if  (PxConfigClose()!=Pt_TRUE) 
             exception_description<<"\nCan`t close file "<<file_name;		

         throw spider_exception(exception_description.str());
	};

	if  (PxConfigClose()!=Pt_TRUE) {
        exception_description<<"Can`t close file "<<file_name;		
         throw spider_exception(exception_description.str());
	}; 


    sort_all();
}; 

void dispatcher_persons_container::save_dispatcher_parameters
                          (dispatcher_persons_container::iterator dispatcher)
                                                                  throw (spider_exception) {
ostringstream exception_description;
enum {ID=0, FIRST_NAME, MIDDLE_NAME, LAST_NAME, ENTRIES_COUNT};
string section_name="dispatcher";
vector<string> entries_names(ENTRIES_COUNT);
entries_names[ID]="id";
entries_names[FIRST_NAME]="first name";
entries_names[MIDDLE_NAME]="middle name";
entries_names[LAST_NAME]="last name";

	if (PxConfigForceEmptySection(section_name.c_str())!=Pt_TRUE){
	       exception_description<<"save_dispatcher_parameters: PxConfigForceEmptySection !=Pt_TRUE dispatcher id ";
	       exception_description<<dispatcher->get_id();
           throw spider_exception(exception_description.str());
           };

	if (PxConfigWriteInt( NULL, 
			                    entries_names[ID].c_str(),
   				                PXCONFIG_FMT_INT_DECIMAL, 
			                    dispatcher->get_id())!=Pt_TRUE){
	       exception_description<<"save_dispatcher_parameters: PxConfigWriteInt ID !=Pt_TRUE dispatcher id ";
	       exception_description<<dispatcher->get_id();
           throw spider_exception(exception_description.str());
           };

	if (PxConfigWriteString( NULL, 
			                    entries_names[FIRST_NAME].c_str(), 
   				                PXCONFIG_FMT_STRING, 
			                    dispatcher->get_first_name().c_str())!=Pt_TRUE){
	       exception_description<<"save_dispatcher_parameters: PxConfigWriteString FIRST_NAME !=Pt_TRUE dispatcher id ";
	       exception_description<<dispatcher->get_id();
           throw spider_exception(exception_description.str());
          };

	if (PxConfigWriteString( NULL, 
			                    entries_names[MIDDLE_NAME].c_str(), 
   				                PXCONFIG_FMT_STRING, 
			                    dispatcher->get_middle_name().c_str())!=Pt_TRUE){
	       exception_description<<"save_dispatcher_parameters: PxConfigWriteString MIDDLE_NAME !=Pt_TRUE dispatcher id ";
	       exception_description<<dispatcher->get_id();
           throw spider_exception(exception_description.str());
          };

	if (PxConfigWriteString( NULL, 
			                    entries_names[LAST_NAME].c_str(), 
   				                PXCONFIG_FMT_STRING, 
			                    dispatcher->get_last_name().c_str())!=Pt_TRUE){
	       exception_description<<"save_dispatcher_parameters: PxConfigWriteString LAST_NAME !=Pt_TRUE dispatcher id ";
	       exception_description<<dispatcher->get_id();
           throw spider_exception(exception_description.str());
          };
};

void dispatcher_persons_container::save_settings_parameters ()
                                                             throw (spider_exception) {
ostringstream exception_description;
enum {ID=0, CURRENT_DISPATCHER, ENTRIES_COUNT};
string section_name="settings";
vector<string> entries_names(ENTRIES_COUNT);
entries_names[CURRENT_DISPATCHER]="current dispatcher";

	if (PxConfigForceEmptySection(section_name.c_str())!=Pt_TRUE){
	       exception_description<<"save_settings_parameters : PxConfigForceEmptySection !=Pt_TRUE";
           throw spider_exception(exception_description.str());
           };

   if (current_dispatcher!=container_dispatcher_persons.end()) {
        if (PxConfigWriteInt( NULL, 
			                    entries_names[CURRENT_DISPATCHER].c_str(),
   				                PXCONFIG_FMT_INT_DECIMAL, 
			                    current_dispatcher->get_id())!=Pt_TRUE){
	       exception_description<<"save_settings_parameters: PxConfigWriteInt CURRENT_DISPATCHER !=Pt_TRUE current dispatcher id ";
	       exception_description<<current_dispatcher->get_id();
           throw spider_exception(exception_description.str());
           };
       } else {
        if (PxConfigWriteInt( NULL, 
			                    entries_names[CURRENT_DISPATCHER].c_str(),
   				                PXCONFIG_FMT_INT_DECIMAL, 
			                    0)!=Pt_TRUE){
	       exception_description<<"save_settings_parameters: PxConfigWriteInt CURRENT_DISPATCHER !=Pt_TRUE current dispatcher==end() ";
           throw spider_exception(exception_description.str());
           };
       };
};

void dispatcher_persons_container::save (string file_name) throw (spider_exception) {
	ostringstream exception_description;

	if (PxConfigOpen( file_name.c_str(), PXCONFIG_WRITE)==Pt_FALSE ){
	       exception_description<<"Can`t open config file "<<file_name;
           throw spider_exception(exception_description.str());
	};

	try {
     iterator tmp_disp_iter=container_dispatcher_persons.begin();
     while(tmp_disp_iter!=container_dispatcher_persons.end()) {
         save_dispatcher_parameters(tmp_disp_iter);
         tmp_disp_iter++;
      };
      save_settings_parameters ();
	} catch (spider_exception spr_exc) {
        exception_description<<spr_exc.get_description();
		if  (PxConfigClose()!=Pt_TRUE) 
             exception_description<<"\nCan`t close file "<<file_name;		
         throw spider_exception(exception_description.str());
	};

	if  (PxConfigClose()==Pt_FALSE) {
	       exception_description<<"Can`t close config file "<<file_name;
           throw spider_exception(exception_description.str());
	};

};

