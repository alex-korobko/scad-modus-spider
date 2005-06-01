using namespace std;

#include <Ap.h>
#include <Ph.h>
#include <Pt.h>

#include <arpa/inet.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/neutrino.h>
#include <sys/syspage.h>
#include <inttypes.h>
#include <fcntl.h>
#include <signal.h>
#include <iostream.h>

#include <map>
#include <vector>
#include <string>
#include <list>
#include <map>
#include <functional>
#include <algorithm>
#include <sstream>

#include "test_cond.h"

#include "defines.h"
#include "spider_exception.h"
#include "system_settings.h"
#include "system_settings_spider.h"

#include "socket_exception.h"
#include "generic_socket.h"
#include "client_socket.h"

#include "command.h"
#include "data_block.h"
#include "escalator_data_block.h"
#include "shavr_data_block.h"

#include "msg_types_container.h"
#include "msg_dict_container.h"

#include "devices_types.h"

#include "timer_command_container.h"
#include "metro_devices_types_container.h"
#include "metro_device.h"

class metro_devices_container;
#include "station.h"
#include "metro_stations_container.h"
#include "metro_devices_container.h"


int
close_device_panel_window( PtWidget_t *widget, ApInfo_t *apinfo, 
                 PtCallbackInfo_t *cbinfo ){

		metro_device::paneled_device_id=0;
         metro_device::device_panel=NULL;

		return( Pt_CONTINUE );
	};

int
activate_close_device_panel_button( PtWidget_t *widget, ApInfo_t *apinfo, 
                 PtCallbackInfo_t *cbinfo ){
		PtWidget_t *window_widget=PtGetParent(widget, PtWindow);

		if (window_widget!=NULL)
		            PtDestroyWidget(window_widget);

		return( Pt_CONTINUE );
	};


int activate_device_button( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo ){
		int *device_id;
		ostringstream exception_description;
 
		PtWidget_t *panel_window_widget;
		vector<PtCallback_t> callbacks;
		PtCallback_t tmp_callback;
        tmp_callback.event_f=close_device_panel_window;
        tmp_callback.data=NULL;
        callbacks.push_back(tmp_callback);
        tmp_callback.event_f=activate_close_device_panel_button;
        tmp_callback.data=NULL;
        callbacks.push_back(tmp_callback);

 metro_devices_container *metro_devices=metro_devices_container::get_instance();
 metro_devices_types_container *metro_devices_types=metro_devices_types_container::get_instance();
 metro_stations_container *metro_stations=metro_stations_container::get_instance();
 system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
 ApDBase_t *widgets_db=spider_sys_sett->get_widgets_dbase();

		PtGetResource(widget, Pt_ARG_USER_DATA, &device_id, 0);	
		metro_devices_container::iterator dev_iter=metro_devices->find(*device_id);
		try {
            if (dev_iter==metro_devices->end()) {
            		  exception_description<<"not found device with id : "<<*device_id;
                  throw spider_exception (exception_description.str());
            };

			if (metro_device::paneled_device_id!=*device_id &&
			    metro_device::device_panel!=NULL) {
							panel_window_widget = PtGetParent( metro_device::device_panel, PtWindow );
                            if (panel_window_widget==NULL)
                                 throw spider_exception("panel_window_widget==NULL");	
                            PtDestroyWidget(panel_window_widget);
			    };

			if (metro_device::paneled_device_id==*device_id &&
			    metro_device::device_panel!=NULL) {
                    return( Pt_CONTINUE );			    
			    };

			metro_devices_types_container::iterator type_iter=metro_devices_types->find(dev_iter->second->get_type());
			if (type_iter==metro_devices_types->end())
                      throw spider_exception("Device type not found");

             metro_stations_container::iterator station_iter=metro_stations->find(dev_iter->second->get_station_id());
			if (station_iter==metro_stations->end())
                      throw spider_exception("Station not found");

              metro_device::create_device_panel(widgets_db,
                                               station_iter->second.get_stl_name_string(),
                                               type_iter->second,
                                               dev_iter->second->get_data_block(),
                                               *device_id,
                                               callbacks);

               dev_iter->second->update_device_panel (type_iter->second);

        } catch (spider_exception spr_exc) {
       			exception_description.str("");
       			exception_description<<"In activate_device_button(..) "<<spr_exc.get_description();
       			spider_sys_sett->sys_message(system_settings::ERROR_MSG, exception_description.str());
        };
		return( Pt_CONTINUE );
	};

int activate_device_menu_item( PtWidget_t *widget,
                                    void *client_data,
                                    PtCallbackInfo_t *cbinfo){
  vector<PtArg_t> args;
  metro_devices_container::iterator dev_iter;
  command *callback_command
                   = static_cast<command*>(client_data);
  command device_command=*callback_command;
  delete(callback_command);

  metro_devices_container *metro_devices=metro_devices_container::get_instance();
   dev_iter=metro_devices->find(device_command.get_device_id());

  if (dev_iter==metro_devices->end()) {
           system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
           ostringstream excepiton_description;
           excepiton_description<<"In activate_device_menu_item not found device with id "
                                                 <<device_command.get_device_id();
           spider_sys_sett->sys_message(system_settings::ERROR_MSG, excepiton_description.str());
           };

  try {
      dev_iter->second->send_command(device_command);
  } catch (spider_exception spr_exc) {
 system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
		ostringstream exception_description;
		exception_description<<"In activate_device_menu_item raised exception "
                                           <<spr_exc.get_description()
                                           <<" device id  "
                                           <<device_command.get_device_id();
		spider_sys_sett->sys_message(system_settings::ERROR_MSG, exception_description.str());
  };

  return Pt_CONTINUE;
};


 int activate_device_menu (PtWidget_t *widget,
                                              ApInfo_t *apinfo,
                                              PtCallbackInfo_t *cbinfo){
  vector<PtArg_t> args;
  vector<command> appropriated_commands;
  vector<command>::iterator cmd_iter;
  string menu_title;
  int  device_id, *internal_device_id;
  PtWidget_t *menu_wgt;
  metro_devices_container *metro_devices=metro_devices_container::get_instance();
  metro_devices_container::iterator dev_iter;

  PtCallback_t tmp_callback;
  vector<PtCallback_t> callbacks;
  tmp_callback.event_f=activate_device_menu_item;

  args.clear();
  args.resize(1);

  PtSetArg(&args[0], Pt_ARG_USER_DATA, &internal_device_id, 0);
  PtGetResources(widget, args.size(), &args[0]);

  device_id=*internal_device_id;

 
  menu_title="Команды";

  dev_iter=metro_devices->find(device_id);
  if (dev_iter==metro_devices->end()) {
           system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
           ostringstream excepiton_description;
           excepiton_description<<"In activate_device_menu not found device with id "
                                                 <<device_id;
           spider_sys_sett->sys_message(system_settings::ERROR_MSG, excepiton_description.str());
           };

 appropriated_commands=dev_iter->second->get_appropriated_commands();

 if (appropriated_commands.empty()) 
        return Pt_CONTINUE;

  args.clear();
  args.resize(1);

  PtSetArg (&args[0], Pt_ARG_MENU_TITLE, menu_title.c_str(), 0);
  menu_wgt= PtCreateWidget(PtMenu,
                                              Pt_NO_PARENT,
                                              args.size(),
                                              &args[0]);


cmd_iter=appropriated_commands.begin();
 while (cmd_iter!=appropriated_commands.end()) {

   args.clear();
   args.resize(3);

   callbacks.clear();
   tmp_callback.data=new command(*cmd_iter);
   callbacks.push_back(tmp_callback);

   string command_description;
   command_description=cmd_iter->get_command_description();
  
   PtSetArg (&args[0], Pt_ARG_LABEL_TYPE, Pt_Z_STRING , 0);
   PtSetArg (&args[1], Pt_ARG_TEXT_STRING, command_description.c_str(), 0);
//   PtSetArg (&args[1], Pt_ARG_TEXT_STRING, "Команда 1", 0);
   PtSetArg (&args[2],
                  Pt_CB_ACTIVATE,
                  &callbacks[0],
                  callbacks.size());

   PtCreateWidget(PtMenuButton,
                            menu_wgt,
                            args.size(),
                            &args[0]);
    
    cmd_iter++;
 }; //while (cmd_iter!=appropriated_commands.end())

 PtPositionMenu(menu_wgt, cbinfo->event);
 PtRealizeWidget(menu_wgt);

 return Pt_CONTINUE;
};


