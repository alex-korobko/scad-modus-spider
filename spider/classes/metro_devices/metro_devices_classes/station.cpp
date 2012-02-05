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

#include <vector>
#include <string>
#include <hash_map>
#include <list>
#include <functional>
#include <algorithm>
#include <sstream>

#include <iostream.h>

#include "defines.h"
#include "spider_exception.h"
#include "system_settings.h"
#include "system_settings_spider.h"

#include "command.h"
#include "data_block.h"
#include "msg_types_container.h"
#include "msg_dict_container.h"
#include "devices_types.h"
#include "timer_command_container.h"
class metro_devices_container;
#include "station.h"
#include "metro_stations_container.h"
#include "metro_devices_types_container.h"
#include "metro_device.h"
#include "metro_devices_container.h"
#include "data_block.h"
#include "shavr_data_block.h"
#include "shavr.h"

//see in_code_attached_callbacks.cpp
int activate_device_button( PtWidget_t *widget,
                                              ApInfo_t *apinfo,
                                              PtCallbackInfo_t *cbinfo );
 int activate_device_menu (PtWidget_t *widget,
                                              ApInfo_t *apinfo,
                                              PtCallbackInfo_t *cbinfo);

// see station_devices_properties_callbacks.cpp
int activate_station_window( PtWidget_t *widget, 
                                           ApInfo_t *apinfo, 
                                           PtCallbackInfo_t *cbinfo );

void metro_station::create_wnd(PtWidget_t* parent) throw (spider_exception) {
 metro_devices_container *dev_container=metro_devices_container::get_instance();
 system_settings_spider *sys_sett_obj=system_settings_spider::get_instance();

 vector<PtArg_t> args;
 PtCallback_t tmp_callback;
 vector<PtCallback_t> activate_widget_callbacks;
 vector<PtCallback_t> activate_menu_callbacks;
 PhRect_t text_extend;
 PhPoint_t position;	
 position.x=xcoord;
 position.y=ycoord;
 int station_id=get_id();

if (sys_sett_obj->small_font_prepared()) {
            PfExtentText(&text_extend,
                                 NULL,
                                 sys_sett_obj->get_small_font(),
                                 name.c_str(),
                                 name.size());
	} else {
	    ostringstream exception_description;
	 	exception_description<<"Small font not prepared! ";
	 	throw spider_exception(exception_description.str());
	};

	args.resize(4);
   	PtSetArg(&args[0], Pt_ARG_TITLE, name.c_str(), 0);
   	PtSetArg(&args[1], Pt_ARG_FLAGS, Pt_TRUE, Pt_SELECTABLE);

	tmp_callback.event_f=activate_station_window;
	tmp_callback.data=NULL;
	activate_widget_callbacks.clear();
	activate_widget_callbacks.push_back(tmp_callback);
   	PtSetArg(&args[2], Pt_CB_ACTIVATE, &activate_widget_callbacks[0], activate_widget_callbacks.size());
	PtSetArg(&args[3], Pt_ARG_USER_DATA, &station_id, sizeof(station_id));

   station_window = ApCreateDBWidget(sys_sett_obj->get_widgets_dbase(),
                                                               "station_wnd",
                                                               parent,
                                                               &position,
                                                               args.size(),
                                                              &args[0]);

	 if (station_window==NULL) {
	    ostringstream exception_description; 
	 	exception_description<<"Station window \""<<name<<"\" can`t be creatded";
	 	throw spider_exception(exception_description.str());
	 }; //if (station_window==NULL

//	metro devices widgets here
	int device_x=system_settings_spider::SPACE_BETWEEN_METRO_DEVICES_IN_STATION, 
	     device_y=system_settings_spider::TOP_BOTTOM_MARGIN_OF_STATION;
	PhRect_t rect, device_widget_rect;
	tmp_callback.event_f=activate_device_button;
	tmp_callback.data=NULL;
	activate_widget_callbacks.clear();
	activate_widget_callbacks.push_back(tmp_callback);

   	tmp_callback.event_f=activate_device_menu;
   	tmp_callback.data= NULL;
   	activate_menu_callbacks.clear();
   	activate_menu_callbacks.push_back(tmp_callback);

	int count=0;
	metro_devices_container::iterator iter_dev;
    rect.lr.y=-1;
    rect.lr.x=-1;
    rect.ul.y=-1;
    rect.ul.x=-1;

	for (iterator_devices_id dev_id_iter=devices_id.begin(); dev_id_iter!=devices_id.end(); dev_id_iter++, count++) {
		  iter_dev=dev_container->find(*dev_id_iter);
		  if (iter_dev==dev_container->end())  {
		    ostringstream exception_description;
		    exception_description<<"Not found device with id "<<*dev_id_iter<<" for station widget creating"<<endl;
		    throw spider_exception (exception_description.str());
		    }; //if (iter_dev==dev_container->end
		    
           iter_dev->second->create_device_widget(device_x,
                                                                               device_y,
                                                                               sys_sett_obj->get_widgets_dbase(),
                                                                               station_window,
                                                                               activate_widget_callbacks,
                                                                               activate_menu_callbacks);

          device_widget_rect=iter_dev->second->get_device_widget_extent();
          //max rect of all devices calculation
          if (rect.lr.y==-1 ||
              device_widget_rect.lr.y>rect.lr.y) rect.lr.y=device_widget_rect.lr.y;
          if (rect.lr.x==-1||
              device_widget_rect.lr.x<rect.lr.x) rect.lr.x=device_widget_rect.lr.x;
          if (rect.ul.y==-1||
               device_widget_rect.ul.y<rect.ul.y) rect.ul.y=device_widget_rect.ul.y;
          if (rect.ul.x==-1||
              device_widget_rect.ul.x>rect.ul.x) rect.ul.x=device_widget_rect.ul.x;

          device_x+=(device_widget_rect.lr.x-device_widget_rect.ul.x)+system_settings_spider::SPACE_BETWEEN_METRO_DEVICES_IN_STATION;
	}; // for (iterator_devices_id dev_id_iter=devices_id.begin(); dev_id_iter

   //ATTENTION - workaround to show the full station window
	//int station_width=device_x;
	int station_width=350;
	args.clear();
	args.resize(2);
	PtSetArg(&args[0], Pt_ARG_WIDTH, __max(text_extend.lr.x + 15, station_width), 0);
	int station_height;
	
	station_height=(text_extend.lr.y-text_extend.ul.y) + 
	                        2*system_settings_spider::TOP_BOTTOM_MARGIN_OF_STATION+
	                        (rect.lr.y-rect.ul.y)+15; //4 pixels to bottom border - empiric value

	PtSetArg(&args[1], Pt_ARG_HEIGHT, station_height, 0);
	PtSetResources(station_window, args.size(), &args[0]);

//	Begin : adding escalators extents
    metro_shavr* shavr;
   	metro_devices_container::iterator iter_esc;
	for (iterator_devices_id dev_id_iter=devices_id.begin(); dev_id_iter!=devices_id.end(); dev_id_iter++, count++) {
		  iter_dev=dev_container->find(*dev_id_iter);
		  if (iter_dev==dev_container->end())  {
		    ostringstream exception_description;
		    exception_description<<"Not found device with id "<<*dev_id_iter<<" for station widget creating"<<endl;
		    throw spider_exception (exception_description.str());
		    }; //if (iter_dev==dev_container->end

         if (iter_dev->second->get_type_description()!=metro_device_type::DEVICE_TYPE_SHAVR) 
                continue;

         metro_device::related_devices_ids::iterator rel_iter=iter_dev->second->devices_ids_begin();
         while(rel_iter!=iter_dev->second->devices_ids_end()) {
                   iter_esc=dev_container->find(*rel_iter);
                   if (iter_esc==dev_container->end()) {
                      ostringstream exception_description;
                      exception_description<<"adding escalators extents :  not found device  with id "<<*rel_iter;
                      throw spider_exception(exception_description.str());
                      }; //if (iter_esc==metr
                shavr=static_cast<metro_shavr*>(iter_dev->second);
                	shavr->push_back_escalator_dimension(
                                           iter_esc->second->get_device_widget_extent());		  
                rel_iter++;
                }; //while(rel_iter!=iter_dev->sec
       
   }; //for (iterator_devices_id dev_id_iter=devices_id.begin(); dev_id_iter!=devices_id
//	End : adding escalators extents

//	Begin : updating devices widgets
	for (iterator_devices_id dev_id_iter=devices_id.begin(); dev_id_iter!=devices_id.end(); dev_id_iter++, count++) {
		  iter_dev=dev_container->find(*dev_id_iter);
		  if (iter_dev==dev_container->end())  {
		    ostringstream exception_description;
		    exception_description<<"Not found device with id "<<*dev_id_iter<<" for station widget creating"<<endl;
		    throw spider_exception (exception_description.str());
		    }; //if (iter_dev==dev_container->end
		    
   		 iter_dev->second->update_device_widget();
		    
   }; //for (iterator_devices_id dev_id_iter=devices_id.begin(); dev_id_iter!=devices_id.end();
//	End : updating devices widgets
    
};

