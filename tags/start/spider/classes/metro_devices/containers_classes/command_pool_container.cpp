using namespace std;

#include <Ph.h>
#include <Ap.h>
#include <Pt.h>
#include <sys/neutrino.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include <iostream.h>

#include <vector>
#include <string>
#include <hash_map>
#include <set>
#include <list>
#include <functional>
#include <algorithm>
#include <sstream>

#include "defines.h"
#include "spider_exception.h"
#include "system_settings.h"
#include "system_settings_spider.h"

#include "command.h"
#include "data_block.h"
#include "msg_types_container.h"
#include "msg_dict_container.h"
#include "log_records_container.h"
#include "devices_types.h"
#include "timer_command_container.h"
class metro_devices_container;
#include "station.h"
#include "metro_stations_container.h"
#include "metro_devices_types_container.h"
#include "metro_device.h"
#include "metro_devices_container.h"

#include "command_pool_container.h"

extern log_records_container *main_log;

cmd_pool_container* cmd_pool_container::cmd_pool_container_instance=NULL;

cmd_pool_container* cmd_pool_container::get_instance() {
     if (cmd_pool_container_instance==NULL)
                 cmd_pool_container_instance=new cmd_pool_container();
     return cmd_pool_container_instance;
};

cmd_pool_container::cmd_pool_container(){
		wnd=NULL;
        first=NULL;
};

cmd_pool_container::~cmd_pool_container(){
};


void cmd_pool_container::set_widget(PtWidget_t *new_wnd) {
	PtArg_t arg;

	if (new_wnd!=NULL) {
				PtSetArg(&arg, 
							Pt_ARG_POINTER , 
							this, 
							0);

			 PtSetResources( new_wnd,
					                    1,
                    						&arg);
 		} else {
         PtGenListItem_t *list_item;
          while (first!=NULL) {
              list_item=first;
              first=first->next;
              free(list_item);
           }; //while (last!=NULL)
       };

	wnd=new_wnd;
};


void cmd_pool_container::prepare_to_display() throw (spider_exception){
	ostringstream exception_description;
   PtGenListItem_t *last,  *list_item;

	if (wnd==NULL) {
		exception_description<<"Pointer to list widget is NULL";
		throw spider_exception(exception_description.str());
	};

	PtGenListRemoveItems(wnd, NULL,NULL);

	unsigned short size=this->size();

     while (first!=NULL) {
              list_item=first;
              first=first->next;
              free(list_item);
           }; //while (last!=NULL)

   if (size==0) return ;

	list_item=(PtGenListItem_t*) malloc(1*sizeof(PtGenListItem_t));

	first=list_item;

	first->prev=NULL;
	first->next=NULL;
	first->size.w=0;
	first->size.h=system_settings_spider::ROW_HEIGHT;
	first->flags=0;

	for (int i=1; i<size; i++)
		{
			last=(PtGenListItem_t*) malloc(1*sizeof(PtGenListItem_t));
			last->prev=list_item;
			last->next=NULL;
			last->size.w=0;
			last->size.h=system_settings_spider::ROW_HEIGHT;
			last->flags=0;
			list_item->next=last;
				
			list_item=last;
		};

	 PtGenListAddItems(wnd, first, NULL);
};


void cmd_pool_container::push_back (command cmd) {
ldword tmp_id;

system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
if (spider_sys_sett==NULL){
        cout<<"In cmd_pool_container::push_back (...) spider_sys_sett==NULL"<<endl;
        return;
};

metro_devices_container *devices=metro_devices_container::get_instance();
if (devices==NULL) {
         spider_sys_sett->sys_message(system_settings::ERROR_MSG, "In cmd_pool_container::push_back (...) devices==NULL");
         return;
};

metro_devices_container::iterator dev_iter;
dev_iter=devices->find(cmd.get_device_id());
if (dev_iter==devices->end()) {
         spider_sys_sett->sys_message(system_settings::ERROR_MSG, "In cmd_pool_container::push_back (...) dev_iter==devices->end()");
         return;
};

switch (dev_iter->second->get_type_description()) {
     case metro_device_type::DEVICE_TYPE_ESCALATOR:
     case metro_device_type::DEVICE_TYPE_UDKU:
           switch  (cmd.get_command_code()) {
                  case system_settings::COMMAND_UP:
                    tmp_id=main_log->get_records_autoincrement();
                    main_log->set_records_autoincrement(++tmp_id);

                    main_log->insert(log_record(tmp_id,
                                     system_settings::DISPATCHER_MAKE_CMD_UP,
                                     system_settings_spider::MESSAGE_TYPE_DISPATCHER_DOIT,
                                     cmd.get_station(),
                                     cmd.get_device_id(),
                                     time(NULL)));
                      main_log->prepare_to_display();
                     break;
				  case system_settings::COMMAND_DOWN:
                    tmp_id=main_log->get_records_autoincrement();
                    main_log->set_records_autoincrement(++tmp_id);

                    main_log->insert(log_record(tmp_id,
                                     system_settings::DISPATCHER_MAKE_CMD_DOWN,
                                     system_settings_spider::MESSAGE_TYPE_DISPATCHER_DOIT,
                                     cmd.get_station(),
                                     cmd.get_device_id(),
                                     time(NULL)));
                      main_log->prepare_to_display();
                      break;
               };
         break;

     case metro_device_type::DEVICE_TYPE_SHAVR:
         break;

     case metro_device_type::DEVICE_TYPE_DOOR:
         break;
     default:
         ostringstream err_descr;
         err_descr<<"In cmd_pool_container::push_back (...) device "<<cmd.get_device_id()<<" cmd code "<<cmd.get_command_code();
         spider_sys_sett->sys_message(system_settings::ERROR_MSG, err_descr.str());
         return;
 };

cmd_pool_cont.push_back(cmd);
};
