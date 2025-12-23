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
#include <time.h>

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

#include "defines.h"
#include "spider_exception.h"
#include "system_settings.h"
#include "system_settings_spider.h"

#include "log_records_container.h"
#include "dispatchers_persons_container.h"

extern log_records_container *main_log;

//see in_code_attached_callbacks.cpp for close_window body
int
close_window( PtWidget_t *widget, 
                            void *apinfo, 
                           PtCallbackInfo_t *cbinfo );

//local functions
static int activate_exit_button_in_smena_window( PtWidget_t *widget, 
                                           void *apinfo, 
                                           PtCallbackInfo_t *cbinfo ) {

dispatcher_persons_container *dispatchers=dispatcher_persons_container::get_instance();
if (dispatchers==NULL) {
     cout<<"In activate_exit_button_in_dispatchers_window: Can`t get instance of dispatcher_persons_container"<<endl;
     return( Pt_HALT);
    };

     dispatchers->set_dispatchers_combobox(NULL);
     dispatchers->set_dispatcher_smena_hour_numeric(NULL);
     dispatchers->set_dispatcher_smena_minute_numeric(NULL);

     return Pt_CONTINUE;
};

static int activate_ok_button_in_smena_window( PtWidget_t *widget, 
                                           void *apinfo, 
                                           PtCallbackInfo_t *cbinfo ) {

dispatcher_persons_container *dispatchers=dispatcher_persons_container::get_instance();
if (dispatchers==NULL) {
     cout<<"In activate_exit_button_in_dispatchers_window: Can`t get instance of dispatcher_persons_container"<<endl;
     return( Pt_HALT);
    };

     if ((dispatchers->get_dispatchers_combobox()!=NULL) &&
		   (dispatchers->get_dispatcher_smena_hour_numeric() !=NULL) &&
     		(dispatchers->get_dispatcher_smena_minute_numeric()!=NULL)) {
             unsigned short *selected_item;
			int *time_value;
			 int selected_dispatcher_val, selected_hour_val, selected_min_val;
			 time_t curr_time = time(NULL) ;
			 tm local_tm = *(localtime( &curr_time ));
			
             PtGetResource(dispatchers->get_dispatcher_smena_hour_numeric(),
                                       Pt_ARG_NUMERIC_VALUE, &time_value, 0);
			 selected_hour_val=*time_value;	
             PtGetResource(dispatchers->get_dispatcher_smena_minute_numeric(),
                                       Pt_ARG_NUMERIC_VALUE, &time_value, 0);
			 selected_min_val=*time_value;	
             local_tm.tm_hour = selected_hour_val;
			 local_tm.tm_min = selected_min_val;

             PtGetResource(dispatchers->get_dispatchers_combobox(),
                                       Pt_ARG_CBOX_SEL_ITEM, &selected_item, 0);
				selected_dispatcher_val= *selected_item;
             if (selected_dispatcher_val>0 &&
                  selected_dispatcher_val<=dispatchers->size()) {
                     dispatcher_persons_container::iterator tmp_iter=dispatchers->begin();
                     advance(tmp_iter, selected_dispatcher_val-1);

                     ldword tmp_autoincrement=main_log->get_records_autoincrement();
                     main_log->set_records_autoincrement(++tmp_autoincrement);
					 cout<<"dispatcher id before inserting "<< tmp_iter->get_id()<<endl;
                     main_log->insert(log_record ( tmp_autoincrement,
                                          (tmp_iter->get_id()<<8)|system_settings::DISPATCHER_BEGIN_SMENU,
                                          system_settings::MESSAGE_TYPE_DISPATCHER_DOING,
                                          0,
                                          0,
                                          mktime(&local_tm)));
                  }; //if (selected_dispatcher>0 &&
               }; //if (dispatchers->get_dispatchers_combobox()!=NULL)

     return Pt_CONTINUE;
};


int
activate_smena_window( PtWidget_t *widget, 
                                           void *apinfo, 
                                           PtCallbackInfo_t *cbinfo ) {

PtWidget_t *dialog_window, *common_panel, *buttons_panel,  *current_button;
PtWidget_t *current_widget;
vector<PtCallback_t> callbacks;
PtCallback_t tmp_callback;
vector<PtArg_t> args;
PhPoint_t widget_position;
string dialog_title;

unsigned int dialog_wnd_height=120,
                     dialog_wnd_width=350,
                     common_panel_height=dialog_wnd_height-2,
                     buttons_panel_height=40,
                     button_in_panel_width=90,
                     button_in_panel_height=buttons_panel_height-10;

system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
if (spider_sys_sett==NULL) {
  cout<<"In activate_dictionary_window: Can`t get instance of system_settings_spider"<<endl;
  return( Pt_HALT);
};

dispatcher_persons_container *dispatchers=dispatcher_persons_container::get_instance();
if (dispatchers==NULL) {
     cout<<"In activate_exit_button_in_dispatchers_window: Can`t get instance of dispatcher_persons_container"<<endl;
     return( Pt_HALT);
    };

try {
		args.clear();
		args.resize(15);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, dialog_wnd_height,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, dialog_wnd_width,0);
		PtSetArg(&args[2], Pt_ARG_WINDOW_TITLE, "Смена",0);
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
		tmp_callback.event_f=activate_exit_button_in_smena_window;
         tmp_callback.data=NULL;
		callbacks.push_back(tmp_callback);

		tmp_callback.event_f=close_window;
         tmp_callback.data=NULL;
		callbacks.push_back(tmp_callback);

        PtSetArg(&args[12],  Pt_CB_WINDOW_CLOSING,
			           &callbacks[0],
                       callbacks.size());
		PtSetArg(&args[13], Pt_ARG_CURSOR_TYPE,
  				       Ph_CURSOR_BIG_POINTER, 0);
		PtSetArg(&args[14], Pt_ARG_CURSOR_COLOR,
  				       0xf33ff5, 0);

		dialog_window=PtCreateWidget(PtWindow,
                                                     spider_sys_sett->get_main_window(),
                                                     args.size(),
                                                     &args[0]);
         if (dialog_window==NULL)
                  throw spider_exception("dialog_wnd is NULL");

        //common
		args.clear();
		args.resize(3);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, common_panel_height,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, dialog_wnd_width-4,0);
          widget_position.x=0;
          widget_position.y=0;
         PtSetArg(&args[2], Pt_ARG_POS,
                     &widget_position, 0);
         common_panel=PtCreateWidget(PtPane,
                                                     dialog_window,
                                                     args.size(),
                                                     &args[0]);
         if (common_panel==NULL)
                  throw spider_exception("common_panel is NULL");

        //dispatchers label
		 args.clear();
		 args.resize(2);
         widget_position.x=0;
         widget_position.y=8;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_TEXT_STRING,
                            "Диспетчер :", 0);

         current_widget=PtCreateWidget(PtLabel,
                                                  common_panel,
                                                  args.size(),
                                                  &args[0]);
        if (current_widget==NULL)
            throw spider_exception("label first name is NULL");

        //dispatchers combobox
		args.clear();
		args.resize(3);
         widget_position.x=90;
         widget_position.y=10;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_WIDTH,
                          (dialog_wnd_width-100),0);
         PtSetArg(&args[2], Pt_ARG_TEXT_FLAGS,
                         Pt_FALSE, Pt_EDITABLE);

         current_widget=PtCreateWidget(PtComboBox,
                                                  common_panel,
                                                  args.size(),
                                                  &args[0]);
        if (current_widget==NULL)
            throw spider_exception("dispatchers_combobox is NULL");
          dispatchers->set_dispatchers_combobox(current_widget);

         vector<const char*> item_cont(1);
         dispatcher_persons_container::iterator disp_iter=dispatchers->begin();
         string dispatcher_fio;
         while(disp_iter!=dispatchers->end()) {
              dispatcher_fio=disp_iter->get_last_name()+" "+disp_iter->get_first_name()+" "+disp_iter->get_middle_name();
              dispatcher_fio.append(dispatcher_fio.size(), ' ');
              item_cont[0]= dispatcher_fio.c_str();
                  if( PtListAddItems( dispatchers->get_dispatchers_combobox(),
                    &item_cont[0],
                    item_cont.size(), 0)!=0)
                            throw spider_exception("error in PtListAddItems(...) for dispatchers");
              disp_iter++;
           };

      if (dispatchers->get_current_dispatcher()!=dispatchers->end()) {
            int select_pos=distance(dispatchers->begin(), dispatchers->get_current_dispatcher())+1;
            PtSetResource(dispatchers->get_dispatchers_combobox(), Pt_ARG_CBOX_SEL_ITEM, select_pos, 0);
          };


         struct tm local_time;
         time_t current_time;
         current_time=time(NULL);
         localtime_r(&current_time, &local_time);

        //smena hour label
		 args.clear();
		 args.resize(2);
         widget_position.x=0;
         widget_position.y=42;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_TEXT_STRING,
                            "Час :", 0);
         current_widget=PtCreateWidget(PtLabel,
                                                  common_panel,
                                                  args.size(),
                                                  &args[0]);
        if (current_widget==NULL)
            throw spider_exception("label smena hour is NULL");

        //smena hour numeric
		args.clear();
		args.resize(10);
         widget_position.x=45;
         widget_position.y=40;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_NUMERIC_UPDOWN_WIDTH,
                          12, 0);
         PtSetArg(&args[2], Pt_ARG_NUMERIC_MIN,
                          0, 0);
         PtSetArg(&args[3], Pt_ARG_NUMERIC_MAX,
                          23, 0);
         PtSetArg(&args[4], Pt_ARG_NUMERIC_SUFFIX,
                          " ч ", 0);
         PtSetArg(&args[5], Pt_ARG_WIDTH,
                     50, 0);
         PtSetArg(&args[6], Pt_ARG_HEIGHT,
                     60, 0);
         PtSetArg(&args[7], Pt_ARG_NUMERIC_VALUE,
                     local_time.tm_hour, 0);

          current_widget=PtCreateWidget(PtNumericInteger,
                                                  common_panel,
                                                  args.size(),
                                                  &args[0]);
        if (current_widget==NULL)
            throw spider_exception("Numeric for smena hour is NULL");

         dispatchers->set_dispatcher_smena_hour_numeric(current_widget);

        //smena minute label
		 args.clear();
		 args.resize(2);
         widget_position.x=180;
         widget_position.y=42;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_TEXT_STRING,
                            "Минута :", 0);
         current_widget=PtCreateWidget(PtLabel,
                                                  common_panel,
                                                  args.size(),
                                                  &args[0]);
        if (current_widget==NULL)
            throw spider_exception("label smena minute is NULL");

        //smena hour numeric
		args.clear();
		args.resize(10);
         widget_position.x=250;
         widget_position.y=40;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_NUMERIC_UPDOWN_WIDTH,
                          12, 0);
         PtSetArg(&args[2], Pt_ARG_NUMERIC_MIN,
                          0, 0);
         PtSetArg(&args[3], Pt_ARG_NUMERIC_MAX,
                          5, 0);
         PtSetArg(&args[4], Pt_ARG_NUMERIC_SUFFIX,
                          " мин    ", 0);
         PtSetArg(&args[5], Pt_ARG_WIDTH,
                     50, 0);
         PtSetArg(&args[6], Pt_ARG_HEIGHT,
                     60, 0);
         PtSetArg(&args[7], Pt_ARG_NUMERIC_VALUE,
                     local_time.tm_min, 0);

          current_widget=PtCreateWidget(PtNumericInteger,
                                                  common_panel,
                                                  args.size(),
                                                  &args[0]);
        if (current_widget==NULL)
            throw spider_exception("Numeric for smena hour is NULL");
         dispatchers->set_dispatcher_smena_minute_numeric(current_widget);

        //buttons panel
		args.clear();
		args.resize(3);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, buttons_panel_height-4,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, dialog_wnd_width-8,0);
          widget_position.x=0;
          widget_position.y=common_panel_height-buttons_panel_height;
         PtSetArg(&args[2], Pt_ARG_POS,
                     &widget_position, 0);
         buttons_panel=PtCreateWidget(PtPane,
                                                     common_panel,
                                                     args.size(),
                                                     &args[0]);
         if (buttons_panel==NULL)
                  throw spider_exception("buttons_panel is NULL");

         //buttons in button panel
       //ok button
		args.clear();
		args.resize(5);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, button_in_panel_height,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, button_in_panel_width,0);
          widget_position.x=dialog_wnd_width-button_in_panel_width-120;
          widget_position.y=2;
         PtSetArg(&args[2], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[3], Pt_ARG_TEXT_STRING,
                       "Принять смену", 0);

		callbacks.clear();
		tmp_callback.event_f=activate_ok_button_in_smena_window;
         tmp_callback.data=NULL;
		callbacks.push_back(tmp_callback);

		tmp_callback.event_f=activate_exit_button_in_smena_window;
         tmp_callback.data=NULL;
		callbacks.push_back(tmp_callback);

		tmp_callback.event_f=close_window;
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
                  throw spider_exception("button \"OK\" in buttons_panel is NULL");



       //chancel button
		args.clear();
		args.resize(5);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, button_in_panel_height,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, button_in_panel_width,0);
          widget_position.x=dialog_wnd_width-button_in_panel_width-20;
          widget_position.y=2;
         PtSetArg(&args[2], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[3], Pt_ARG_TEXT_STRING,
                       "Отмена", 0);

		callbacks.clear();
		tmp_callback.event_f=activate_exit_button_in_smena_window;
         tmp_callback.data=NULL;
		callbacks.push_back(tmp_callback);

		tmp_callback.event_f=close_window;
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
                  throw spider_exception("button \"Chancel\" in buttons_panel is NULL");

       if ( PtRealizeWidget(dialog_window)!=0) 
                   throw spider_exception("Can`t realize dialog window");

} catch (spider_exception spr_exc) { 
    spider_sys_sett->sys_message(system_settings::ERROR_MSG, 
         "In activate_smena_window: "+spr_exc.get_description());

   dispatchers->set_dispatchers_list(NULL);
   dispatchers->set_dispatchers_first_name_text(NULL);
   dispatchers->set_dispatchers_middle_name_text(NULL);
   dispatchers->set_dispatchers_last_name_text(NULL);

     return( Pt_HALT);
}; // catch (spider_exception spr_exc)

return Pt_CONTINUE;
};




