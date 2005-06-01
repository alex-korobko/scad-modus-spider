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

using namespace std;

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

//see in_code_attached_callbacks.cpp for close_window body
int
close_window( PtWidget_t *widget, 
                            ApInfo_t *apinfo, 
                           PtCallbackInfo_t *cbinfo );

//see draw_functinos.cpp
void timer_commands_raw_list_draw_function( PtWidget_t *widget,
            PtGenListItem_t *item,
            unsigned index,
            unsigned nitems,
            PhRect_t *where );


//local functions
int
activate_accept_button_in_timer_commands_window( PtWidget_t *widget, 
                                                                                     ApInfo_t *apinfo, 
                                                                   PtCallbackInfo_t *cbinfo ){
system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
if (spider_sys_sett==NULL) {
  cout<<"In activate_exit_button_in_timer_commands_window: Can`t get instance of system_settings_spider"<<endl;
  return( Pt_HALT);
};
timer_command_container *timer_commands=timer_command_container::get_instance();
if (timer_commands==NULL) 
      spider_sys_sett->sys_message(system_settings::ERROR_MSG, "In activate_exit_button_in_timer_commands_window: Can`t get instance of timer_command_container");
timer_commands->set_timer_commands_accepted(true);
timer_commands->change_indicator_state();
return Pt_CONTINUE;
};

int
activate_generate_button_in_timer_commands_window ( PtWidget_t *widget, 
                                                                                     ApInfo_t *apinfo, 
                                                                   PtCallbackInfo_t *cbinfo ){
system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
if (spider_sys_sett==NULL) {
  cout<<"In activate_exit_button_in_timer_commands_window: Can`t get instance of system_settings_spider"<<endl;
  return( Pt_HALT);
};

try {
    metro_devices_container *devices=metro_devices_container::get_instance();
    if (devices==NULL)
        throw spider_exception("instance of metro_devices_container is NULL");

    timer_command_container *timer_commands=timer_command_container::get_instance();
    if (timer_commands==NULL)
        throw spider_exception("instance of timer_command_container is NULL");

      devices->prepare_timer_command();
      timer_commands->set_was_created_in_this_day(true);
      timer_commands->set_timer_commands_accepted(false);
      timer_commands->change_indicator_state();
      timer_commands->prepare_to_display();
} catch (spider_exception spr_exc ) {
            spider_sys_sett->sys_message(system_settings::ERROR_MSG, "In activate_generate_button_in_timer_commands_window "+spr_exc.get_description());
           return Pt_HALT;
};
return Pt_CONTINUE;
};

int
activate_exit_button_in_timer_commands_window( PtWidget_t *widget, 
                                                                                     ApInfo_t *apinfo, 
                                                                   PtCallbackInfo_t *cbinfo ){
system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
if (spider_sys_sett==NULL) {
  cout<<"In activate_exit_button_in_timer_commands_window: Can`t get instance of system_settings_spider"<<endl;
  return( Pt_HALT);
};

timer_command_container *timer_commands=timer_command_container::get_instance();
if (timer_commands==NULL) 
      spider_sys_sett->sys_message(system_settings::ERROR_MSG, "In activate_exit_button_in_timer_commands_window: Can`t get instance of timer_command_container");
timer_commands->set_timer_commands_devices_panel(NULL);
timer_commands->set_timer_commands_raw_list(NULL);

return( Pt_CONTINUE);
};


int
activate_timer_commands_button_in_main_window( PtWidget_t *widget, 
                                                                                     ApInfo_t *apinfo, 
                                                                   PtCallbackInfo_t *cbinfo ) {
PtWidget_t *dialog_window, *buttons_panel, *raw_list, *divider_in_raw_list, *current_button;
vector<PtCallback_t> callbacks;
PtCallback_t tmp_callback;
vector<PtArg_t> args;
PhPoint_t widget_position;
string dialog_title("Утренний пуск");
unsigned int dialog_wnd_height=350, 
                     dialog_wnd_width=750, 
                     buttons_panel_height=40,
                     divider_height=35,
                     button_in_panel_width=90,
                     button_in_panel_height=buttons_panel_height-10;

system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
if (spider_sys_sett==NULL) {
  cout<<"In activate_timer_commands_button_in_main_window: Can`t get instance of system_settings_spider"<<endl;
  return( Pt_HALT);
};

timer_command_container *timer_commands=timer_command_container::get_instance();
if (timer_commands==NULL) 
      spider_sys_sett->sys_message(system_settings::ERROR_MSG, "In activate_timer_commands_button_in_main_window: Can`t get instance of timer_command_container");

if ( timer_commands->get_timer_commands_raw_list()!=NULL ) {
         close_window(timer_commands->get_timer_commands_raw_list(), NULL, NULL);
          timer_commands->set_timer_commands_raw_list(NULL);
      };

try {
		args.clear();
		args.resize(13);
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

		callbacks.clear();
		tmp_callback.event_f=activate_exit_button_in_timer_commands_window;
         tmp_callback.data=NULL;
		callbacks.push_back(tmp_callback);
		tmp_callback.event_f=close_window;
         tmp_callback.data=NULL;
		callbacks.push_back(tmp_callback);

        PtSetArg(&args[12],  Pt_CB_WINDOW_CLOSING,
			           &callbacks[0],
                         callbacks.size());

		dialog_window=PtCreateWidget(PtWindow,
                                                     spider_sys_sett->get_main_window(),
                                                     args.size(),
                                                     &args[0]);
         if (dialog_window==NULL)
                  throw spider_exception("dialog_wnd is NULL");
/*
        //common
		args.clear();
		args.resize(3);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, common_panel_height,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, dialog_wnd_width,0);
          widget_position.x=0;
          widget_position.y=dialog_wnd_height-common_panel_height;
         PtSetArg(&args[2], Pt_ARG_POS,
                     &widget_position, 0);
         common_panel=PtCreateWidget(PtPane,
                                                     dialog_window,
                                                     args.size(),
                                                     &args[0]);
         if (common_panel==NULL)
                  throw spider_exception("common_panel is NULL");
*/
        //buttons panel
		args.clear();
		args.resize(3);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, buttons_panel_height,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, dialog_wnd_width-4,0);
          widget_position.x=0;
  //        widget_position.y=common_panel_height-buttons_panel_height;
         widget_position.y=dialog_wnd_height-buttons_panel_height;
         PtSetArg(&args[2], Pt_ARG_POS,
                     &widget_position, 0);
         buttons_panel=PtCreateWidget(PtPane,
//                                                    common_panel,
                                                     dialog_window,
                                                     args.size(),
                                                     &args[0]);
         if (buttons_panel==NULL)
                  throw spider_exception("buttons_panel is NULL");
/*
        //devices panel
		args.clear();
		args.resize(7);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, common_panel_height-buttons_panel_height-2,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, dialog_wnd_width-4,0);
          widget_position.x=0;
          widget_position.y=0;
         PtSetArg(&args[2], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[3], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_LEFT_ANCHORED_LEFT);
         PtSetArg(&args[4], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_RIGHT_ANCHORED_RIGHT);
         PtSetArg(&args[5], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_TOP_ANCHORED_TOP);
         PtSetArg(&args[6], Pt_ARG_ANCHOR_FLAGS,
                             Pt_TRUE, Pt_BOTTOM_ANCHORED_TOP);

         devices_panel=PtCreateWidget(PtPane,
                                                     common_panel,
                                                     args.size(),
                                                     &args[0]);
         if (devices_panel==NULL)
                  throw spider_exception("devices_panel is NULL");

          timer_commands->set_timer_commands_devices_panel(devices_panel);
*/
       //raw list with commands
		args.clear();
		args.resize(7);
//		PtSetArg(&args[0], Pt_ARG_HEIGHT, dialog_wnd_height-common_panel_height,0);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, dialog_wnd_height-buttons_panel_height,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, dialog_wnd_width,0);
          widget_position.x=0;
          widget_position.y=0;
         PtSetArg(&args[2], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[3], Pt_ARG_RAWLIST_DRAW_F, 
                          timer_commands_raw_list_draw_function, 0);
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

           timer_commands->set_timer_commands_raw_list(raw_list);
           timer_commands->prepare_to_display();

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
		PtSetArg(&args[1], Pt_ARG_WIDTH, (dialog_wnd_width-2)/4,0);
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
		PtSetArg(&args[1], Pt_ARG_WIDTH, (dialog_wnd_width-2)/4,0);
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
		PtSetArg(&args[0], Pt_ARG_HEIGHT, (dialog_wnd_width-2)/4,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, 200,0);
         PtSetArg(&args[2], Pt_ARG_TEXT_STRING,
                       "Команда", 0);
         current_button=PtCreateWidget(PtButton,
                                                     divider_in_raw_list,
                                                     args.size(),
                                                     &args[0]);
         if (current_button==NULL)
                  throw spider_exception("button \"Station\" in raw list header is NULL");

		args.clear();
		args.resize(3);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, (dialog_wnd_width-2)/4,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, 200,0);
         PtSetArg(&args[2], Pt_ARG_TEXT_STRING,
                       "Время", 0);
         current_button=PtCreateWidget(PtButton,
                                                     divider_in_raw_list,
                                                     args.size(),
                                                     &args[0]);
         if (current_button==NULL)
                  throw spider_exception("button \"Station\" in raw list header is NULL");

         //buttons in button panel
		args.clear();
		args.resize(5);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, button_in_panel_height,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, button_in_panel_width,0);
          widget_position.x=10;
          widget_position.y=2;
         PtSetArg(&args[2], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[3], Pt_ARG_TEXT_STRING,
                       "Создать", 0);
		callbacks.clear();
		tmp_callback.event_f=activate_generate_button_in_timer_commands_window;
         tmp_callback.data=NULL;
		callbacks.push_back(tmp_callback);
         PtSetArg(&args[4], Pt_CB_ACTIVATE,
                        &callbacks[0],
                        callbacks.size());
         current_button=PtCreateWidget(PtButton,
                                                     buttons_panel,
                                                     args.size(),
                                                     &args[0]);
         if (current_button==NULL)
                  throw spider_exception("button \"Generate\" in buttons_panel is NULL");

		args.clear();
		args.resize(5);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, button_in_panel_height,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, button_in_panel_width,0);
          widget_position.x=120;
          widget_position.y=2;
         PtSetArg(&args[2], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[3], Pt_ARG_TEXT_STRING,
                       "Утвердить", 0);
		callbacks.clear();
		tmp_callback.event_f=activate_accept_button_in_timer_commands_window;
         tmp_callback.data=NULL;
		callbacks.push_back(tmp_callback);
         PtSetArg(&args[4], Pt_CB_ACTIVATE,
                        &callbacks[0],
                        callbacks.size());
         current_button=PtCreateWidget(PtButton,
                                                     buttons_panel,
                                                     args.size(),
                                                     &args[0]);
         if (current_button==NULL)
                  throw spider_exception("button \"Accept\" in buttons_panel is NULL");


		args.clear();
		args.resize(5);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, button_in_panel_height,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, button_in_panel_width,0);
          widget_position.x=dialog_wnd_width-button_in_panel_width-20;
          widget_position.y=2;
         PtSetArg(&args[2], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[3], Pt_ARG_TEXT_STRING,
                       "Закрыть окно", 0);
		callbacks.clear();
		tmp_callback.event_f=activate_exit_button_in_timer_commands_window;
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
    timer_commands->set_timer_commands_devices_panel(NULL);
    timer_commands->set_timer_commands_raw_list(NULL);

     return( Pt_HALT);
}; // catch (spider_exception spr_exc)

return( Pt_CONTINUE);
}
