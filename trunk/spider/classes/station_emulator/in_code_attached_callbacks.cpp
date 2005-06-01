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

#include <algorithm>
#include <map>
#include <vector>
#include <string>
#include <list>
#include <map>
#include <functional>
#include <sstream>

#include "test_cond.h"
#include "emulation.h"

#include "system_settings_classes/defines.h"
#include "system_settings_classes/spider_exception.h"
#include "system_settings_classes/system_settings.h"
#include "system_settings_classes/system_settings_spider.h"

#include "metro_devices_classes/command.h"
#include "metro_devices_classes/data_block.h"
#include "metro_devices_classes/escalator_data_block.h"
#include "metro_devices_classes/shavr_data_block.h"

#include "containers_classes/msg_types_container.h"
#include "containers_classes/msg_dict_container.h"

#include "metro_devices_classes/devices_types.h"
#include "../containers_classes/metro_devices_types_container.h"

#include "containers_classes/timer_command_container.h"

class metro_devices_container;

#include "metro_devices_classes/station.h"
#include "containers_classes/metro_stations_container.h"

#include "metro_devices_classes/metro_device.h"
#include "containers_classes/metro_devices_container.h"

#include "metro_devices_classes/escalator.h"
#include "metro_devices_classes/shavr.h"

extern ApDBase_t *widgets_db;
extern metro_devices_container metro_devices;
extern metro_devices_types_container metro_devices_types;
extern system_settings_spider *spider_sys_sett;

int activate_start_test_button( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo ){

		return( Pt_CONTINUE );
	};

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


int activate_device_menu_item( PtWidget_t *widget,
                                    void *client_data,
                                    PtCallbackInfo_t *cbinfo){
  vector<PtArg_t> args;
  metro_devices_container::iterator dev_iter;
  command *callback_command
                   = static_cast<command*>(client_data);
  command device_command=*callback_command;
  delete(callback_command);

   dev_iter=metro_devices.find(device_command.get_device_id());

  if (dev_iter==metro_devices.end()) {
           ostringstream excepiton_description;
           excepiton_description<<"In activate_device_menu_item not found device with id "
                                                 <<device_command.get_device_id();
           spider_sys_sett->sys_message(system_settings::ERROR_MSG, excepiton_description.str());
           };

  try {
      dev_iter->second->send_command(device_command,
                                                                   spider_sys_sett);
  } catch (spider_exception spr_exc) {
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

  dev_iter=metro_devices.find(device_id);
  if (dev_iter==metro_devices.end()) {
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


int activate_device_button( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo ){
		int *internal_device_id, device_id;
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

		PtGetResource(widget, Pt_ARG_USER_DATA, &internal_device_id, 0);	
		device_id=*internal_device_id;
		metro_devices_container::iterator dev_iter=metro_devices.find(device_id);

		try {
            if (dev_iter==metro_devices.end()) {
            		  exception_description<<"not found device with id : "<<device_id;
                  throw spider_exception (exception_description.str());
            };

			if (metro_device::paneled_device_id!=device_id &&
			    metro_device::device_panel!=NULL) {
							panel_window_widget = PtGetParent( metro_device::device_panel, PtWindow );
                            if (panel_window_widget==NULL)
                                 throw spider_exception("panel_window_widget==NULL");	
                            PtDestroyWidget(panel_window_widget);
			    };

			if (metro_device::paneled_device_id==device_id &&
			    metro_device::device_panel!=NULL) {
                    return( Pt_CONTINUE );			    
			    };

			metro_devices_types_container::iterator type_iter=metro_devices_types.find(dev_iter->second->get_type());
			if (type_iter==metro_devices_types.end())
                      throw spider_exception("Device type not found");	

            dev_iter->second->create_device_panel(widgets_db,
                                                                            spider_sys_sett,
                                                                            "Сырецкая",  //ATTENTION !!! ADD FINDING STATION HERE
                                                                             type_iter->second,
                                                                             dev_iter->second->get_data_block(),
                                                                             device_id,
                                                                             callbacks);

            dev_iter->second->update_device_panel
                                                 (type_iter->second,
                                                  spider_sys_sett );

        } catch (spider_exception spr_exc) {
       			exception_description.str("");
       			exception_description<<"In activate_device_button(..) "<<spr_exc.get_description();
       			spider_sys_sett->sys_message(system_settings::ERROR_MSG, exception_description.str());
        };
		return( Pt_CONTINUE );
	};


#ifdef __EMULATION__ 

int activate_signal_menu_item( PtWidget_t *widget,
                                    void *client_data,
                                    PtCallbackInfo_t *cbinfo){
  vector<PtArg_t> args;
  int signal_index, *internal_signal_index;
  data_block *data_blk;
  data_block::signal_data_type *callback_value
                   = static_cast<data_block::signal_data_type*>(client_data);
  data_block::signal_data_type value=*callback_value;
  PtWidget_t *menu_widget;
  delete(callback_value);

   menu_widget = PtGetParent( widget, PtMenu );

   if (menu_widget==NULL)
          return Pt_CONTINUE;
  
  args.clear();
  args.resize(3);
  PtSetArg (&args[0], Pt_ARG_POINTER, &data_blk, 0);
  PtSetArg (&args[1], Pt_ARG_USER_DATA, &internal_signal_index, 0);
  PtGetResources(menu_widget, args.size(), &args[0]);

  signal_index=*internal_signal_index;

  try {
    data_blk->set_signal_value
                (signal_index,
                 value);
  } catch (spider_exception spr_exc) {
		ostringstream exception_description;
		exception_description<<"In activate_signal_menu_item(..) "<<spr_exc.get_description()<<" signal index "<<signal_index;
		spider_sys_sett->sys_message(system_settings::ERROR_MSG, exception_description.str());
  };

  return Pt_CONTINUE;
};

int activate_signal_widget (PtWidget_t *widget,
                                         ApInfo_t *apinfo,
                                         PtCallbackInfo_t *cbinfo){
  vector<PtArg_t> args;
  vector<char> tmp_chars(40);
  string menu_title;
  data_block *data_blk;
  const char *internal_signal_widget_title;
  int  signal_index,*internal_signal_index;
  PtWidget_t *menu_wgt;

  PtCallback_t tmp_callback;
  vector<PtCallback_t> callbacks;
  tmp_callback.event_f=activate_signal_menu_item;

  args.clear();
  args.resize(3);
  PtSetArg (&args[0], Pt_ARG_POINTER, &data_blk, 0);
  PtSetArg (&args[1], Pt_ARG_USER_DATA, &internal_signal_index, 0);
  PtSetArg (&args[2], Pt_ARG_TEXT_STRING, &internal_signal_widget_title, 0);
  PtGetResources(widget, args.size(), &args[0]);

  signal_index=*internal_signal_index;
  menu_title=internal_signal_widget_title;

 cout<<" data_blk->get_signals_size  "<<data_blk->get_signals_size()<<endl;

  args.clear();
  args.resize(3);

  PtSetArg (&args[0], Pt_ARG_POINTER,  data_blk, 0);
  PtSetArg (&args[1], Pt_ARG_USER_DATA, &signal_index, sizeof(signal_index));
  PtSetArg (&args[2], Pt_ARG_MENU_TITLE, menu_title.c_str(), 0);
  menu_wgt= PtCreateWidget(PtMenu,
                                              Pt_NO_PARENT,
                                              args.size(),
                                              &args[0]);

if (data_blk->get_data_block_type_description()==
   data_block::DATA_BLOCK_ESCALATOR) {

   args.clear();
   args.resize(4);

   callbacks.clear();
   tmp_callback.data=new int(escalator_data_block::SIGNAL_VALUE_GREEN);
   callbacks.push_back(tmp_callback);

   itoa( escalator_data_block::SIGNAL_VALUE_GREEN,
          &tmp_chars[0],
          10);   
   PtSetArg (&args[0], Pt_ARG_LABEL_TYPE, Pt_TEXT_IMAGE , 0);
   PtSetArg (&args[1], Pt_ARG_TEXT_STRING, &tmp_chars[0], 0);
   PtSetArg (&args[2], 
                  Pt_ARG_LABEL_IMAGE,
                  spider_sys_sett->get_image(system_settings_spider::BLOCK_GREEN_LED),
                  0);
   PtSetArg (&args[3], 
                  Pt_CB_ACTIVATE,
                  &callbacks[0],
                  callbacks.size());

   PtCreateWidget(PtMenuButton,
                            menu_wgt,
                            args.size(),
                            &args[0]);

   callbacks.clear();
   tmp_callback.data=new int(escalator_data_block::SIGNAL_VALUE_BLUE);
   callbacks.push_back(tmp_callback);

   itoa( escalator_data_block::SIGNAL_VALUE_BLUE,
          &tmp_chars[0],
          10);   
   PtSetArg (&args[0], Pt_ARG_LABEL_TYPE, Pt_TEXT_IMAGE , 0);
   PtSetArg (&args[1], Pt_ARG_TEXT_STRING, &tmp_chars[0], 0);
   PtSetArg (&args[2], 
                  Pt_ARG_LABEL_IMAGE,
                  spider_sys_sett->get_image(system_settings_spider::BLOCK_BLUE_LED),
                  0);
   PtSetArg (&args[3], 
                  Pt_CB_ACTIVATE,
                  &callbacks[0],
                  callbacks.size());

   PtCreateWidget(PtMenuButton,
                            menu_wgt,
                            args.size(),
                            &args[0]);

   callbacks.clear();
   tmp_callback.data=new int(escalator_data_block::SIGNAL_VALUE_RED);
   callbacks.push_back(tmp_callback);

   itoa( escalator_data_block::SIGNAL_VALUE_RED,
          &tmp_chars[0],
          10);   
   PtSetArg (&args[0], Pt_ARG_LABEL_TYPE, Pt_TEXT_IMAGE , 0);
   PtSetArg (&args[1], Pt_ARG_TEXT_STRING, &tmp_chars[0], 0);
   PtSetArg (&args[2], 
                  Pt_ARG_LABEL_IMAGE,
                  spider_sys_sett->get_image(system_settings_spider::BLOCK_RED_LED),
                  0);
   PtSetArg (&args[3], 
                  Pt_CB_ACTIVATE,
                  &callbacks[0],
                  callbacks.size());

   PtCreateWidget(PtMenuButton,
                            menu_wgt,
                            args.size(),
                            &args[0]);

   callbacks.clear();
   tmp_callback.data=new int(escalator_data_block::SIGNAL_VALUE_GREY);
   callbacks.push_back(tmp_callback);

   itoa( escalator_data_block::SIGNAL_VALUE_GREY,
          &tmp_chars[0],
          10);   
   PtSetArg (&args[0], Pt_ARG_LABEL_TYPE, Pt_TEXT_IMAGE , 0);
   PtSetArg (&args[1], Pt_ARG_TEXT_STRING, &tmp_chars[0], 0);
   PtSetArg (&args[2], 
                  Pt_ARG_LABEL_IMAGE,
                  spider_sys_sett->get_image(system_settings_spider::BLOCK_GREY_LED),
                  0);
   PtSetArg (&args[3], 
                  Pt_CB_ACTIVATE,
                  &callbacks[0],
                  callbacks.size());

   PtCreateWidget(PtMenuButton,
                            menu_wgt,
                            args.size(),
                            &args[0]);

 } else if  (data_blk->get_data_block_type_description()==
   data_block::DATA_BLOCK_SHAVR) {
   
   
   args.clear();
   args.resize(4);

   callbacks.clear();
   tmp_callback.data=new int(shavr_data_block::SIGNAL_VALUE_GREEN);
   callbacks.push_back(tmp_callback);

   itoa( shavr_data_block::SIGNAL_VALUE_GREEN,
          &tmp_chars[0],
          10);   
   PtSetArg (&args[0], Pt_ARG_LABEL_TYPE, Pt_TEXT_IMAGE , 0);
   PtSetArg (&args[1], Pt_ARG_TEXT_STRING, &tmp_chars[0], 0);
   PtSetArg (&args[2], 
                  Pt_ARG_LABEL_IMAGE,
                  spider_sys_sett->get_image(system_settings_spider::BLOCK_GREEN_LED),
                  0);
   PtSetArg (&args[3], 
                  Pt_CB_ACTIVATE,
                  &callbacks[0],
                  callbacks.size());

   PtCreateWidget(PtMenuButton,
                            menu_wgt,
                            args.size(),
                            &args[0]);

   callbacks.clear();
   tmp_callback.data=new int(shavr_data_block::SIGNAL_VALUE_BLUE);
   callbacks.push_back(tmp_callback);

   itoa( shavr_data_block::SIGNAL_VALUE_BLUE,
          &tmp_chars[0],
          10);   
   PtSetArg (&args[0], Pt_ARG_LABEL_TYPE, Pt_TEXT_IMAGE , 0);
   PtSetArg (&args[1], Pt_ARG_TEXT_STRING, &tmp_chars[0], 0);
   PtSetArg (&args[2], 
                  Pt_ARG_LABEL_IMAGE,
                  spider_sys_sett->get_image(system_settings_spider::BLOCK_BLUE_LED),
                  0);
   PtSetArg (&args[3], 
                  Pt_CB_ACTIVATE,
                  &callbacks[0],
                  callbacks.size());

   PtCreateWidget(PtMenuButton,
                            menu_wgt,
                            args.size(),
                            &args[0]);

 };
 
 PtPositionMenu(menu_wgt, cbinfo->event);
 PtRealizeWidget(menu_wgt);

 return Pt_CONTINUE;
};

int activate_parameter_widget (PtWidget_t *widget,
                                         ApInfo_t *apinfo,
                                         PtCallbackInfo_t *cbinfo){
  vector<PtArg_t> args;
  vector<char> text_buffer(10);
  const char* btns[]={"OK", "Отмена"};
  const char *internal_parameter_widget_title;
  data_block *data_blk;

  int parameter_index, *internal_parameter_index;
  data_block::parameter_data_type value;
  string prompt_title;
  PtWidget_t *title_widget;
   
  args.clear();
  args.resize(3);
  PtSetArg (&args[0], Pt_ARG_POINTER, &data_blk, 0);
  PtSetArg (&args[1], Pt_ARG_USER_DATA, &internal_parameter_index, 0);
  PtGetResources(widget, args.size(), &args[0]);

  parameter_index=*internal_parameter_index;

  title_widget=PtWidgetBrotherBehind( widget);
  args.clear();
  args.resize(1);
  PtSetArg (&args[0], Pt_ARG_TEXT_STRING, &internal_parameter_widget_title, 0);
  PtGetResources(title_widget, args.size(), &args[0]);
  prompt_title=internal_parameter_widget_title;
  
  int ret_val=PtPrompt( widget,
              NULL,
              prompt_title.c_str(),
              NULL,
              "Значение :",
              NULL,
              2,
              btns,
              NULL,
              0,
              0,
              text_buffer.size(),
              &text_buffer[0],
              NULL,
              NULL,
              Pt_CENTER | Pt_BLOCK_ALL);
  if (ret_val==1){
                 value= atol (&text_buffer[0]);
                 try {
                    data_blk->set_parameter_value
                          (parameter_index,
                            value);
                } catch (spider_exception spr_exc) {
                    ostringstream exception_description;
                    exception_description<<
	                  "In activate_parameter_widget(..) "<<
	                  spr_exc.get_description()<<
	                  " parameter index "<<
	                  parameter_index;
                   spider_sys_sett->sys_message(system_settings::ERROR_MSG,
                                                                     exception_description.str());
                 };
              };

    return Pt_CONTINUE;
};

#endif
