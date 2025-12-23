using namespace std;

#include <Ap.h>
#include <Ph.h>
#include <Pt.h>

#include <arpa/inet.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/neutrino.h>
#include <sys/syspage.h>
#include <inttypes.h>
#include <fcntl.h>
#include <signal.h>
#include <iostream.h>

#include <hash_map>
#include <set>
#include <vector>
#include <string>
#include <list>
#include <map>
#include <functional>
#include <algorithm>
#include <sstream>

#include "test_cond.h"

#include "defines.h"
#include <sys/select.h> //#define FD_SETSIZE
#include "spider_exception.h"
#include "system_settings.h"
#include "system_settings_spider.h"

#include "socket_exception.h"
#include "generic_socket.h"
#include "client_socket.h"

#include "command.h"
#include "command_pool_container.h"
#include "data_block.h"
#include "escalator_data_block.h"
#include "shavr_data_block.h"

#include "msg_types_container.h"
#include "msg_dict_container.h"
#include "log_records_container.h"

#include "timer_command_container.h"
#include "devices_types.h"
#include "metro_devices_types_container.h"
#include "metro_device.h"
#include "metro_devices_container.h"


extern msg_dict_container *messages;
extern log_records_container *main_log;
extern log_records_container *archive_log;


//see in_code_attached_callbacks.cpp for close_window body
int
close_window( PtWidget_t *widget, 
                            void *apinfo, 
                           PtCallbackInfo_t *cbinfo );

//see draw_functinos.cpp
void commands_pool_raw_list_draw_function( PtWidget_t *widget,
            PtGenListItem_t *item,
            unsigned index,
            unsigned nitems,
            PhRect_t *where );

//local functions
int
activate_exit_button_in_commands_pool_window( PtWidget_t *widget, 
                                                                                     void *apinfo, 
                                                                   PtCallbackInfo_t *cbinfo ){
system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
if (spider_sys_sett==NULL) {
  cout<<"In activate_commands_pool_button_in_main_window: Can`t get instance of system_settings_spider"<<endl;
  return( Pt_HALT);
};

cmd_pool_container *cmd_pool_instance=cmd_pool_container::get_instance();
if (cmd_pool_instance==NULL) 
      spider_sys_sett->sys_message(system_settings::ERROR_MSG, "In activate_commands_pool_button_in_main_window: Can`t get instance of cmd_pool_container");

cmd_pool_instance->set_widget(NULL);

return( Pt_CONTINUE);
};

int
activate_send_command_button_in_commands_pool_window( PtWidget_t *widget, 
                                                                                     void *apinfo, 
                                                                   PtCallbackInfo_t *cbinfo ){

cmd_pool_container::iterator cmds_iter;
metro_devices_container::iterator devices_iter;
system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
if (spider_sys_sett==NULL) {
  cout<<"In activate_send_command_button_in_commands_pool_window: Can`t get instance of system_settings_spider"<<endl;
  return( Pt_HALT);
};

cmd_pool_container *cmd_pool_instance=cmd_pool_container::get_instance();
if (cmd_pool_instance==NULL)  {
      spider_sys_sett->sys_message(system_settings::ERROR_MSG, "activate_send_command_button_in_commands_pool_window: Can`t get instance of cmd_pool_container");
      return( Pt_HALT);
      };

if (cmd_pool_instance->empty() ||
     cmd_pool_instance->get_widget()==NULL)
                   return( Pt_HALT);

metro_devices_container *metro_devices_instance=metro_devices_container::get_instance();
if (metro_devices_instance==NULL)  {
      spider_sys_sett->sys_message(system_settings::ERROR_MSG, "activate_send_command_button_in_commands_pool_window: Can`t get instance of metro_devices_container");
      return( Pt_HALT);
      };

	vector<PtArg_t> args(2);
	const unsigned short  *internal_sel_count, *internal_top_item_pos;
	unsigned short sel_item_count, top_item_pos;

	PtSetArg( &args[0], Pt_ARG_LIST_SEL_COUNT, &internal_sel_count, 0 );
	PtSetArg( &args[1], Pt_ARG_TOP_ITEM_POS, &internal_top_item_pos, 0 );
	PtGetResources (cmd_pool_instance->get_widget(), args.size(), &args[0]);	
	sel_item_count=*internal_sel_count;
	top_item_pos=*internal_top_item_pos;

	if (sel_item_count>0) {
		vector<unsigned short> sel_indexes(sel_item_count);
		PtGenListGetSelIndexes(cmd_pool_instance->get_widget(),
							                   &sel_indexes[0]);

		if (cmd_pool_instance->size()>static_cast<unsigned short>(sel_indexes[0]-1)) {
			cmds_iter=cmd_pool_instance->begin();
			advance(cmds_iter,
							sel_indexes[0]-1);			
            devices_iter=metro_devices_instance->find(cmds_iter->get_device_id());
            if (devices_iter!=metro_devices_instance->end()) {
                    devices_iter->second->send_command(*cmds_iter);
                 };
			cmd_pool_instance->erase(cmds_iter);
			cmd_pool_instance->prepare_to_display();
		} else {
            spider_sys_sett->sys_message(system_settings::ERROR_MSG,
																"activate_send_command_button_in_commands_pool_window cmd_pool_instance->size()>static_cast<unsigned short>(sel_indexes[0]-1)");
			return( Pt_HALT );
		};
	};
return( Pt_CONTINUE);
};

int
activate_send_all_commands_button_in_commands_pool_window( PtWidget_t *widget, 
                                                                                                      void *apinfo, 
                                                                                                     PtCallbackInfo_t *cbinfo ){
cmd_pool_container::iterator cmds_iter;
metro_devices_container::iterator devices_iter;
system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
if (spider_sys_sett==NULL) {
  cout<<"In activate_commands_pool_button_in_main_window: Can`t get instance of system_settings_spider"<<endl;
  return( Pt_HALT);
};

cmd_pool_container *cmd_pool_instance=cmd_pool_container::get_instance();
if (cmd_pool_instance==NULL)  {
      spider_sys_sett->sys_message(system_settings::ERROR_MSG, "In activate_commands_pool_button_in_main_window: Can`t get instance of cmd_pool_container");
      return( Pt_HALT);
      };

if (cmd_pool_instance->empty())
                   return( Pt_HALT);

metro_devices_container *metro_devices_instance=metro_devices_container::get_instance();
if (metro_devices_instance==NULL)  {
      spider_sys_sett->sys_message(system_settings::ERROR_MSG, "In activate_commands_pool_button_in_main_window: Can`t get instance of metro_devices_container");
      return( Pt_HALT);
      };

cmds_iter=cmd_pool_instance->begin();
while(cmds_iter!=cmd_pool_instance->end()) {
      devices_iter=metro_devices_instance->find(cmds_iter->get_device_id());
      if (devices_iter!=metro_devices_instance->end()) {
                 devices_iter->second->send_command(*cmds_iter);
             };
      cmds_iter++;
    };

cmd_pool_instance->clear();
cmd_pool_instance->prepare_to_display();
return( Pt_CONTINUE);
};

int
activate_chancel_command_button_in_commands_pool_window( PtWidget_t *widget, 
                                                                                                      void *apinfo, 
                                                                                                     PtCallbackInfo_t *cbinfo ){

cmd_pool_container::iterator cmds_iter;
//metro_devices_container::iterator devices_iter;
system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
if (spider_sys_sett==NULL) {
  cout<<"In activate_chancel_command_button_in_commands_pool_window: Can`t get instance of system_settings_spider"<<endl;
  return( Pt_HALT);
};

cmd_pool_container *cmd_pool_instance=cmd_pool_container::get_instance();
if (cmd_pool_instance==NULL)  {
      spider_sys_sett->sys_message(system_settings::ERROR_MSG, "activate_chancel_command_button_in_commands_pool_window: Can`t get instance of cmd_pool_container");
      return( Pt_HALT);
      };

if (cmd_pool_instance->empty() ||
     cmd_pool_instance->get_widget()==NULL)
                   return( Pt_HALT);

	vector<PtArg_t> args(2);
	const unsigned short  *internal_sel_count, *internal_top_item_pos;
	unsigned short sel_item_count, top_item_pos;

	PtSetArg( &args[0], Pt_ARG_LIST_SEL_COUNT, &internal_sel_count, 0 );
	PtSetArg( &args[1], Pt_ARG_TOP_ITEM_POS, &internal_top_item_pos, 0 );
	PtGetResources (cmd_pool_instance->get_widget(), args.size(), &args[0]);	
	sel_item_count=*internal_sel_count;
	top_item_pos=*internal_top_item_pos;

	if (sel_item_count>0) {
		vector<unsigned short> sel_indexes(sel_item_count);
		PtGenListGetSelIndexes(cmd_pool_instance->get_widget(),
							                   &sel_indexes[0]);

		if (cmd_pool_instance->size()>static_cast<unsigned short>(sel_indexes[0]-1)) {
			cmds_iter=cmd_pool_instance->begin();
			advance(cmds_iter,
							sel_indexes[0]-1);			

			cmd_pool_instance->erase(cmds_iter);
			cmd_pool_instance->prepare_to_display();
		} else {
            spider_sys_sett->sys_message(system_settings::ERROR_MSG,
																"activate_chancel_command_button_in_commands_pool_window cmd_pool_instance->size()>static_cast<unsigned short>(sel_indexes[0]-1)");
			return( Pt_HALT );
		};
	};

return( Pt_CONTINUE);
};

int
activate_commands_pool_button_in_main_window( PtWidget_t *widget, 
                                                                                     void *apinfo, 
                                                                                     PtCallbackInfo_t *cbinfo ){
PtWidget_t *dialog_window,*buttons_panel, *raw_list, *divider_in_raw_list, *current_button;
vector<PtCallback_t> callbacks;
PtCallback_t tmp_callback;
vector<PtArg_t> args;
PhPoint_t widget_position;
string dialog_title("Команды");
unsigned int dialog_wnd_height=350, 
                     dialog_wnd_width=750, 
                     buttons_panel_height=40,
                     divider_height=35,
                     button_in_panel_width=90;
system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
if (spider_sys_sett==NULL) {
  cout<<"In activate_commands_pool_button_in_main_window: Can`t get instance of system_settings_spider"<<endl;
  return( Pt_HALT);
};

cmd_pool_container *cmd_pool_instance=cmd_pool_container::get_instance();
if (cmd_pool_instance==NULL) 
      spider_sys_sett->sys_message(system_settings::ERROR_MSG, "In activate_commands_pool_button_in_main_window: Can`t get instance of cmd_pool_container");

if (cmd_pool_instance->get_widget()!=NULL) {
    close_window(cmd_pool_instance->get_widget(), NULL, NULL);
    cmd_pool_instance->set_widget(NULL);
};

try {
		args.clear();
		args.resize(14);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, dialog_wnd_height,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, dialog_wnd_width,0);
		PtSetArg(&args[2], Pt_ARG_WINDOW_TITLE, dialog_title.c_str(),0);
		PtSetArg(&args[3], Pt_ARG_WINDOW_RENDER_FLAGS,
                      Pt_FALSE, Ph_WM_RENDER_RESIZE);
		PtSetArg(&args[4], Pt_ARG_WINDOW_RENDER_FLAGS,
                      Pt_FALSE, Ph_WM_TOBACK);
		PtSetArg(&args[5], Pt_ARG_WINDOW_RENDER_FLAGS,
                      Pt_FALSE, Ph_WM_RENDER_MAX);
		PtSetArg(&args[6], Pt_ARG_WINDOW_RENDER_FLAGS,
                      Pt_FALSE, Ph_WM_RENDER_MENU);
		PtSetArg(&args[7], Pt_ARG_WINDOW_RENDER_FLAGS,
                      Pt_FALSE, Ph_WM_RENDER_COLLAPSE );
		PtSetArg(&args[8], Pt_ARG_WINDOW_MANAGED_FLAGS,
                      Pt_FALSE, Ph_WM_MAX );
		PtSetArg(&args[9], Pt_ARG_WINDOW_MANAGED_FLAGS,
                      Pt_FALSE, Ph_WM_MENU  );
		PtSetArg(&args[10], Pt_ARG_WINDOW_MANAGED_FLAGS,
                      Pt_FALSE, Ph_WM_CLOSE  );
		PtSetArg(&args[11], Pt_ARG_WINDOW_MANAGED_FLAGS,
                      Pt_FALSE, Ph_WM_COLLAPSE );
		PtSetArg(&args[12], Pt_ARG_CURSOR_TYPE,
  				       Ph_CURSOR_BIG_POINTER, 0);
		PtSetArg(&args[13], Pt_ARG_CURSOR_COLOR,
  				       0xf33ff5, 0);


/*
		callbacks.clear();
		tmp_callback.event_f=activate_exit_button_in_commands_pool_window;
         tmp_callback.data=NULL;
		callbacks.push_back(tmp_callback);
		tmp_callback.event_f=close_window;
         tmp_callback.data=NULL;
		callbacks.push_back(tmp_callback);

        PtSetArg(&args[12],  Pt_CB_WINDOW_CLOSING,
			           &callbacks[0],
                         callbacks.size());
*/
		dialog_window=PtCreateWidget(PtWindow,
                                                     spider_sys_sett->get_main_window(),
                                                     args.size(),
                                                     &args[0]);
         if (dialog_window==NULL)
                  throw spider_exception("dialog_wnd is NULL");
        //buttons panel
		args.clear();
		args.resize(3);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, buttons_panel_height,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, dialog_wnd_width,0);
          widget_position.x=0;
          widget_position.y=dialog_wnd_height-buttons_panel_height;
         PtSetArg(&args[2], Pt_ARG_POS,
                     &widget_position, 0);
         buttons_panel=PtCreateWidget(PtPane,
                                                     dialog_window,
                                                     args.size(),
                                                     &args[0]);
         if (buttons_panel==NULL)
                  throw spider_exception("buttons_panel is NULL");

          //raw list with commands
		args.clear();
		args.resize(7);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, dialog_wnd_height-buttons_panel_height,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, dialog_wnd_width,0);
          widget_position.x=0;
          widget_position.y=0;
         PtSetArg(&args[2], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[3], Pt_ARG_RAWLIST_DRAW_F, 
                          commands_pool_raw_list_draw_function, 0);
		PtSetArg(&args[4], Pt_ARG_LIST_FLAGS,
                      Pt_TRUE, Pt_LIST_SCROLLBAR_ALWAYS);
		PtSetArg(&args[5], Pt_ARG_LIST_FLAGS,
                      Pt_TRUE, Pt_LIST_SCROLLBAR_AUTORESIZE);
		PtSetArg(&args[6], Pt_ARG_LIST_FLAGS,
                      Pt_TRUE, Pt_LIST_BALLOONS_IN_COLUMNS);

         raw_list=PtCreateWidget(PtRawList,
                                                   dialog_window,
                                                   args.size(),
                                                   &args[0]);
         if (raw_list==NULL)
                 throw spider_exception("raw_list is NULL");

           cmd_pool_instance->set_widget(raw_list);
           cmd_pool_instance->prepare_to_display();

          //divider in raw list
		args.clear();
		args.resize(6);
         PtSetArg(&args[0], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_LEFT_ANCHORED_LEFT);
         PtSetArg(&args[1], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_RIGHT_ANCHORED_RIGHT);
         PtSetArg(&args[2], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_TOP_ANCHORED_TOP);
         PtSetArg(&args[3], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_BOTTOM_ANCHORED_TOP);
         PtSetArg(&args[4], Pt_ARG_DIVIDER_FLAGS, Pt_TRUE, Pt_DIVIDER_NORESIZE);
         PtSetArg(&args[5], Pt_ARG_DIVIDER_FLAGS, Pt_FALSE, Pt_DIVIDER_RESIZE_BOTH);

         divider_in_raw_list=PtCreateWidget(PtDivider,
                                                     raw_list,
                                                     args.size(),
                                                     &args[0]);
         if (divider_in_raw_list==NULL)
                  throw spider_exception("divider_in_raw_list is NULL");

          //buttons in raw list
		args.clear();
		args.resize(3);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, divider_height-2,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, (dialog_wnd_width-2)/3,0);
        PtSetArg(&args[2], Pt_ARG_TEXT_STRING,
                       "Станция", 0);
         current_button=PtCreateWidget(PtButton,
                                                     divider_in_raw_list,
                                                     args.size(),
                                                     &args[0]);
         if (current_button==NULL)
                  throw spider_exception("button \"Station\" in raw list header is NULL");

		args.clear();
		args.resize(3);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, divider_height-2,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, (dialog_wnd_width-2)/3,0);
         PtSetArg(&args[2], Pt_ARG_TEXT_STRING,
                       "Устройство", 0);
         current_button=PtCreateWidget(PtButton,
                                                     divider_in_raw_list,
                                                     args.size(),
                                                     &args[0]);
         if (current_button==NULL)
                  throw spider_exception("button \"Station\" in raw list header is NULL");

		args.clear();
		args.resize(3);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, (dialog_wnd_width-2)/3,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, 200,0);
         PtSetArg(&args[2], Pt_ARG_TEXT_STRING,
                       "Команда", 0);
         current_button=PtCreateWidget(PtButton,
                                                     divider_in_raw_list,
                                                     args.size(),
                                                     &args[0]);
         if (current_button==NULL)
                  throw spider_exception("button \"Station\" in raw list header is NULL");

         //buttons in button panel
		args.clear();
		args.resize(5);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, buttons_panel_height-8,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, button_in_panel_width,0);
          widget_position.x=5;
          widget_position.y=2;
         PtSetArg(&args[2], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[3], Pt_ARG_TEXT_STRING,
                       "Отправить команду", 0);
		callbacks.clear();
		tmp_callback.event_f=activate_send_command_button_in_commands_pool_window;
         tmp_callback.data=raw_list;
		callbacks.push_back(tmp_callback);
         PtSetArg(&args[4], Pt_CB_ACTIVATE,
                        &callbacks[0],
                        callbacks.size());
         current_button=PtCreateWidget(PtButton,
                                                     buttons_panel,
                                                     args.size(),
                                                     &args[0]);
         if (current_button==NULL)
                  throw spider_exception("button \"Send\" in buttons_panel is NULL");

		args.clear();
		args.resize(5);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, buttons_panel_height-8,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, button_in_panel_width,0);
          widget_position.x=150;
          widget_position.y=2;
         PtSetArg(&args[2], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[3], Pt_ARG_TEXT_STRING,
                       "Отправить все команды", 0);
		callbacks.clear();
		tmp_callback.event_f=activate_send_all_commands_button_in_commands_pool_window;
         tmp_callback.data=raw_list;
		callbacks.push_back(tmp_callback);
         PtSetArg(&args[4], Pt_CB_ACTIVATE,
                        &callbacks[0],
                        callbacks.size());
         current_button=PtCreateWidget(PtButton,
                                                     buttons_panel,
                                                     args.size(),
                                                     &args[0]);
         if (current_button==NULL)
                  throw spider_exception("button \"Send all comands\" in buttons_panel is NULL");

		args.clear();
		args.resize(5);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, buttons_panel_height-8,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, button_in_panel_width,0);
          widget_position.x=350;
          widget_position.y=2;
         PtSetArg(&args[2], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[3], Pt_ARG_TEXT_STRING,
                       "Отменить команду", 0);
		callbacks.clear();
		tmp_callback.event_f=activate_chancel_command_button_in_commands_pool_window;
         tmp_callback.data=raw_list;
		callbacks.push_back(tmp_callback);
         PtSetArg(&args[4], Pt_CB_ACTIVATE,
                        &callbacks[0],
                        callbacks.size());
         current_button=PtCreateWidget(PtButton,
                                                     buttons_panel,
                                                     args.size(),
                                                     &args[0]);
         if (current_button==NULL)
                  throw spider_exception("button \"Chancel command\" in buttons_panel is NULL");

		args.clear();
		args.resize(5);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, buttons_panel_height-8,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, button_in_panel_width,0);
          widget_position.x=dialog_wnd_width-button_in_panel_width-10;
          widget_position.y=2;
         PtSetArg(&args[2], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[3], Pt_ARG_TEXT_STRING,
                       "Закрыть окно", 0);
		callbacks.clear();
		tmp_callback.event_f=activate_exit_button_in_commands_pool_window;
         tmp_callback.data=NULL;
		callbacks.push_back(tmp_callback);
		tmp_callback.event_f=close_window;
         tmp_callback.data=raw_list;
		callbacks.push_back(tmp_callback);
         PtSetArg(&args[4], Pt_CB_ACTIVATE,
                        &callbacks[0],
                        callbacks.size());
         current_button=PtCreateWidget(PtButton,
                                                     buttons_panel,
                                                     args.size(),
                                                     &args[0]);
         if (current_button==NULL)
                  throw spider_exception("button \"Close\" in buttons_panel is NULL");

       if ( PtRealizeWidget(dialog_window)!=0) 
                   throw spider_exception("Can`t realize dialog window");
   		args.clear();
		args.resize(2);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, divider_height,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, dialog_wnd_width-4,0);
         PtSetResources(divider_in_raw_list, args.size(), &args[0]);

} catch (spider_exception spr_exc) { 
    spider_sys_sett->sys_message(system_settings::ERROR_MSG, 
         "In activate_commands_pool_button_in_main_window: "+spr_exc.get_description());
     return( Pt_HALT);
}; // catch (spider_exception spr_exc)

return( Pt_CONTINUE);
};


