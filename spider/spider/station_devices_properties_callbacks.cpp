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

#include "defines.h"
#include <sys/select.h> //#define FD_SETSIZE
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
#include "line.h"
#include "metro_lines_container.h"
#include "station.h"
#include "metro_stations_container.h"
#include "metro_devices_types_container.h"
#include "metro_device.h"
#include "metro_devices_container.h"
#include "data_block.h"
#include "shavr_data_block.h"
#include "shavr.h"
#include "escalator_data_block.h"
#include "escalator.h"
#include "udku_data_block.h"
#include "udku.h"

//see in_code_attached_callbacks.cpp for close_window body
int
close_window( PtWidget_t *widget, 
                            ApInfo_t *apinfo, 
                           PtCallbackInfo_t *cbinfo );

//see draw_functinos.cpp
void station_devices_properties_raw_list_draw_function( PtWidget_t *widget,
            PtGenListItem_t *item,
            unsigned index,
            unsigned nitems,
            PhRect_t *where );

//local functions
static void prepare_to_display(PtWidget_t *raw_list_widget , unsigned short size){
    system_settings_spider *sys_sett_spr=system_settings_spider::get_instance();
	PtGenListItem_t *first,
                                *last,
                                *list_item;
	if(raw_list_widget==NULL) {
           sys_sett_spr->sys_message(system_settings::ERROR_MSG,
                       "prepare_to_display(PtWidget_t raw_list_widget):raw_list_widget==NULL");
               return;
           };
	PtGenListRemoveItems(raw_list_widget, NULL,NULL);

/*
  //ATTENTION: In PtGenListAddItems free memory  of first already present??
     cout<<"prepare_to_display() first "<<(first!=NULL?"is NULL":"NULL")<<endl;

    while (first!=NULL) {
              list_item=first;
              first=first->next;
              free(list_item);
           }; //while (first!=NULL)
*/

   if (size==0) return ;

    last=NULL;
    list_item=(PtGenListItem_t*)malloc(1*sizeof(PtGenListItem_t));

	first=list_item;

	first->prev=NULL;
	first->next=NULL;
	first->size.w=0;
	first->size.h=system_settings_spider::ROW_HEIGHT;
	first->flags=0;

	for (int i=1; i<size; i++) {
			last=(PtGenListItem_t*)malloc(1*sizeof(PtGenListItem_t));
			last->prev=list_item;
			last->next=NULL;
			last->size.w=0;
			last->size.h=system_settings_spider::ROW_HEIGHT;
			last->flags=0;
			list_item->next=last;
				
			list_item=last;
		};

	PtGenListAddItems(raw_list_widget, first, NULL);

	 unsigned short *visible_count, top_item_pos;
	 PtGetResource(raw_list_widget, Pt_ARG_VISIBLE_COUNT, &visible_count, 0);

	if (*visible_count<size){
			top_item_pos=size-*visible_count+1;
			 PtSetResource(raw_list_widget, Pt_ARG_TOP_ITEM_POS,  &top_item_pos, 0);
		};
}


void device_selection(
        PtWidget_t *widget, PtGenListItem_t *item, int pos, int column,
        int nitems, int subtype, PhEvent_t *ev ) {
int *internal_station_id, station_id;

system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
if (spider_sys_sett==NULL) {
  cout<<"In device_selection: Can`t get instance of system_settings_spider"<<endl;
  return;
};

try {
        metro_stations_container *metro_stations=metro_stations_container::get_instance();
         metro_stations_container::iterator stations_iter;
        if (metro_stations==NULL)
                  throw spider_exception("Can`t get instance of metro_stations_container");

        metro_devices_container *metro_devices=metro_devices_container::get_instance();
         metro_devices_container::iterator devices_iter;
        if (metro_devices==NULL)
                  throw spider_exception("Can`t get instance of metro_devices_container");

		PtGetResource(widget, Pt_ARG_USER_DATA, &internal_station_id, 0);
        station_id=*internal_station_id;
        stations_iter=metro_stations->find(station_id);
       if (stations_iter==metro_stations->end()) {
                ostringstream message;
                 message<<"Can`t find station with id "<<station_id;
                throw spider_exception (message.str());
            };

            if (static_cast<int>(stations_iter->second.size_devices_id())<pos)
                  throw spider_exception("second.size_devices_id()>pos");
            if (metro_stations_container::get_metro_station_devices_properties_panel()==NULL)
                  throw spider_exception("metro_stations_container::get_metro_station_devices_properties_panel)==NULL");

           metro_station::iterator_devices_id  device_id = stations_iter->second.begin_devices_id();
           advance(device_id, pos-1);

          devices_iter=metro_devices->find(*device_id);
          if (devices_iter==metro_devices->end()) {
                  ostringstream exception_description;
                  exception_description<<"not found device with id "<<*device_id<<" for station with id "<<station_id;
                  throw spider_exception(exception_description.str());
               };

       devices_iter->second->create_properties_widgets
                      (metro_stations_container::get_metro_station_devices_properties_panel());
} catch (spider_exception spr_exc) { 
            spider_sys_sett->sys_message(system_settings::ERROR_MSG, 
                       "In device_selection: "+spr_exc.get_description());
            metro_stations_container::set_metro_station_devices_properties_panel(NULL);
            return ;
}; // catch (spider_exception spr_exc)

};

int activate_exit_button_in_stations_devices_properties_window ( PtWidget_t *widget, 
                                           ApInfo_t *apinfo, 
                                           PtCallbackInfo_t *cbinfo ){
    metro_stations_container::set_metro_station_devices_properties_panel(NULL);
    metro_stations_container::set_metro_station_devices_properties_raw_list(NULL);
    return Pt_CONTINUE;
};

int
activate_station_window( PtWidget_t *widget, 
                                           ApInfo_t *apinfo, 
                                           PtCallbackInfo_t *cbinfo ){
PtWidget_t *dialog_window, *common_panel, *devices_panel, *buttons_panel, *raw_list, *divider_in_raw_list, *current_button;
vector<PtCallback_t> callbacks;
PtCallback_t tmp_callback;
vector<PtArg_t> args;
PhPoint_t widget_position;
string dialog_title;
unsigned int dialog_wnd_height=350, 
                     dialog_wnd_width=850,
                     common_panel_height=100,
                     buttons_panel_height=40,
                     divider_height=35,
                     button_in_panel_width=90,
                     button_in_panel_height=buttons_panel_height-10;
int *internal_station_id, station_id;

if (metro_stations_container::get_metro_station_devices_properties_panel()!=NULL ) {
         close_window(metro_stations_container::get_metro_station_devices_properties_panel(), NULL, NULL);
          metro_stations_container::set_metro_station_devices_properties_panel(NULL);
      };

system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
if (spider_sys_sett==NULL) {
  cout<<"In activate_station_window: Can`t get instance of system_settings_spider"<<endl;
  return( Pt_HALT);
};

try {
        metro_stations_container *metro_stations=metro_stations_container::get_instance();
         metro_stations_container::iterator stations_iter;
        if (metro_stations==NULL)
                  throw spider_exception("Can`t get instance of metro_stations_container");

		PtGetResource(widget, Pt_ARG_USER_DATA, &internal_station_id, 0);
        station_id=*internal_station_id;
        stations_iter=metro_stations->find(station_id);
       if (stations_iter==metro_stations->end()) {
                ostringstream message;
                 message<<"Can`t find station with id "<<station_id;
                throw spider_exception (message.str());
            };

          dialog_title=stations_iter->second.get_stl_name_string();

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
		tmp_callback.event_f=activate_exit_button_in_stations_devices_properties_window;
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

        //buttons panel
		args.clear();
		args.resize(3);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, buttons_panel_height,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, dialog_wnd_width-4,0);
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

          metro_stations_container::set_metro_station_devices_properties_panel(devices_panel);

       //raw list with commands
		args.clear();
		args.resize(9);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, dialog_wnd_height-common_panel_height,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, dialog_wnd_width,0);
          widget_position.x=0;
          widget_position.y=0;
         PtSetArg(&args[2], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[3], Pt_ARG_RAWLIST_DRAW_F, 
                         station_devices_properties_raw_list_draw_function, 0);
         PtSetArg(&args[4], Pt_ARG_RAWLIST_SELECT_F, 
                         device_selection, 0);
	     PtSetArg(&args[5], Pt_ARG_USER_DATA, &station_id, sizeof(station_id));
		PtSetArg(&args[6], Pt_ARG_LIST_FLAGS,
                      Pt_TRUE, Pt_LIST_SCROLLBAR_ALWAYS);
		PtSetArg(&args[7], Pt_ARG_LIST_FLAGS,
                      Pt_TRUE, Pt_LIST_SCROLLBAR_AUTORESIZE);
		PtSetArg(&args[8], Pt_ARG_LIST_FLAGS,
                      Pt_TRUE, Pt_LIST_BALLOONS_IN_COLUMNS);

         raw_list=PtCreateWidget(PtRawList,
                                                   dialog_window,
                                                   args.size(),
                                                   &args[0]);
         if (raw_list==NULL)
                 throw spider_exception("raw_list is NULL");

         metro_stations_container::set_metro_station_devices_properties_raw_list(raw_list);

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
		PtSetArg(&args[1], Pt_ARG_WIDTH, (dialog_wnd_width-2)/6,0);
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
		PtSetArg(&args[1], Pt_ARG_WIDTH, (dialog_wnd_width-2)/6,0);
         PtSetArg(&args[2], Pt_ARG_TEXT_STRING,
                       "Устройство", 0);
         current_button=PtCreateWidget(PtButton,
                                                     divider_in_raw_list,
                                                     args.size(),
                                                     &args[0]);
         if (current_button==NULL)
                  throw spider_exception("button \"Device\" in raw list header is NULL");

		args.clear();
		args.resize(3);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, divider_height-2,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, (dialog_wnd_width-2)/6,0);
         PtSetArg(&args[2], Pt_ARG_TEXT_STRING,
                       "Режим", 0);
         current_button=PtCreateWidget(PtButton,
                                                     divider_in_raw_list,
                                                     args.size(),
                                                     &args[0]);
         if (current_button==NULL)
                  throw spider_exception("button \"Mode\" in raw list header is NULL");


		args.clear();
		args.resize(3);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, divider_height-2,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, (dialog_wnd_width-2)/6,0);
         PtSetArg(&args[2], Pt_ARG_TEXT_STRING,
                       "Команда", 0);
         current_button=PtCreateWidget(PtButton,
                                                     divider_in_raw_list,
                                                     args.size(),
                                                     &args[0]);
         if (current_button==NULL)
                  throw spider_exception("button \"Command\" in raw list header is NULL");


		args.clear();
		args.resize(3);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, divider_height-2, 0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, (dialog_wnd_width-2)/6,0);
         PtSetArg(&args[2], Pt_ARG_TEXT_STRING,
                       "Запуск", 0);
         current_button=PtCreateWidget(PtButton,
                                                     divider_in_raw_list,
                                                     args.size(),
                                                     &args[0]);
         if (current_button==NULL)
                  throw spider_exception("button \"Execution mode\" in raw list header is NULL");

		args.clear();
		args.resize(3);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, divider_height-2, 0);
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
          widget_position.x=dialog_wnd_width-button_in_panel_width-20;
          widget_position.y=2;
         PtSetArg(&args[2], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[3], Pt_ARG_TEXT_STRING,
                       "Закрыть окно", 0);
		callbacks.clear();
		tmp_callback.event_f=activate_exit_button_in_stations_devices_properties_window;
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

          prepare_to_display(raw_list ,stations_iter->second.size_devices_id());

       if ( PtRealizeWidget(dialog_window)!=0) 
                   throw spider_exception("Can`t realize dialog window");
   		args.clear();
		args.resize(2);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, divider_height,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, dialog_wnd_width-4,0);
         PtSetResources(divider_in_raw_list, args.size(), &args[0]);

} catch (spider_exception spr_exc) { 
    spider_sys_sett->sys_message(system_settings::ERROR_MSG, 
         "In activate_station_window: "+spr_exc.get_description());

          metro_stations_container::set_metro_station_devices_properties_panel(NULL);
          metro_stations_container::set_metro_station_devices_properties_raw_list(NULL);

     return( Pt_HALT);
}; // catch (spider_exception spr_exc)

return Pt_CONTINUE;
};

int
activate_ok_button_in_udku_properties( PtWidget_t *widget, 
                                                                     ApInfo_t *apinfo, 
                                                                   PtCallbackInfo_t *cbinfo ){
int *internal_deivce_id=NULL,
       device_id;
ushort_t *internal_sel_item_index, sel_item_index;
int *internal_numeric_value, numeric_value;
unsigned short size;
system_settings_spider* sys_sett_obj=system_settings_spider::get_instance();
if (sys_sett_obj==NULL) { 
           cout<<"Can`t get instance of system_settins_spider"<<endl;
           return Pt_HALT;
    };

PtGetResource(widget, Pt_ARG_USER_DATA, &internal_deivce_id, 0);
device_id=*internal_deivce_id;
metro_udku *udku_to_properties;
try {
      metro_devices_container *devices=metro_devices_container::get_instance();
      if (devices==NULL)
           throw spider_exception("devices==NULL");

       metro_stations_container *stations=metro_stations_container::get_instance();
       metro_stations_container::iterator stations_iter;
       if (stations==NULL)
           throw spider_exception("stations==NULL");

       metro_devices_container::iterator dev_iter=devices->find(device_id);
       if (dev_iter==devices->end())
           throw spider_exception("dev_iter==devices->end()");

       if (dev_iter->second->get_type_description()!=metro_device_type::DEVICE_TYPE_UDKU)
              throw spider_exception("get_type_description()!=metro_device_type::DEVICE_TYPE_ESCALATOR");

         if (metro_stations_container::get_metro_station_devices_properties_panel()==NULL)
              throw spider_exception("metro_stations_container::get_metro_station_devices_properties_panel()==NULL");

         try {
             udku_to_properties=dynamic_cast<metro_udku*>(dev_iter->second);
          } catch (bad_cast) {
              throw spider_exception("dynamic_cast<metro_udku*>(dev_iter->second) is a bad cast");
          };

         stations_iter=stations->find(udku_to_properties->get_station_id());
         if (stations_iter==stations->end()) {
               ostringstream message;
               message<<"Not found station with id "<<udku_to_properties->get_station_id()<<
                                  " for escalator with id "<<udku_to_properties->get_id();
               throw spider_exception(message.str());
           };

       if (udku_to_properties->get_udku_pref_direction_combobox_widget()==NULL)
               throw spider_exception("udku_to_properties->get_udku_direction_combobox_widget()==NULL");
	    PtGetResource(udku_to_properties->get_udku_pref_direction_combobox_widget(),
							Pt_ARG_CBOX_SEL_ITEM,
							&internal_sel_item_index,
							0);
 	    sel_item_index=*internal_sel_item_index;
        udku_to_properties->set_pref_direction(sel_item_index);

       if (udku_to_properties->get_udku_start_direction_combobox_widget()==NULL)
               throw spider_exception("udku_to_properties->get_udku_direction_combobox_widget()==NULL");
	    PtGetResource(udku_to_properties->get_udku_start_direction_combobox_widget(),
							Pt_ARG_CBOX_SEL_ITEM,
							&internal_sel_item_index,
							0);
 	    sel_item_index=*internal_sel_item_index;
        udku_to_properties->set_start_direction(sel_item_index);

       if (udku_to_properties->get_udku_start_mode_combobox_widget()==NULL)
               throw spider_exception("udku_to_properties->get_escalator_start_mode_combobox_widget()==NULL");
	    PtGetResource(udku_to_properties->get_udku_start_mode_combobox_widget(),
							Pt_ARG_CBOX_SEL_ITEM,
							&internal_sel_item_index,
							0);
 	    sel_item_index=*internal_sel_item_index;
        udku_to_properties->set_execution_mode(sel_item_index-1);

       if (udku_to_properties->get_udku_start_hour_widget()==NULL)
               throw spider_exception("udku_to_properties->get_udku_start_hour_widget()==NULL");
	    PtGetResource(udku_to_properties->get_udku_start_hour_widget(),
							Pt_ARG_NUMERIC_VALUE,
							&internal_numeric_value,
							0);
 	    numeric_value=*internal_numeric_value;
        udku_to_properties->set_start_hour(numeric_value);

       if (udku_to_properties->get_udku_start_hour_widget()==NULL)
               throw spider_exception("udku_to_properties->get_escalator_start_minute_widget()==NULL");
	    PtGetResource(udku_to_properties->get_udku_start_minute_widget(),
							Pt_ARG_NUMERIC_VALUE,
							&internal_numeric_value,
							0);
 	    numeric_value=*internal_numeric_value;
        udku_to_properties->set_start_minute(numeric_value);

        dev_iter->second->update_device_widget();
        size=stations_iter->second.size_devices_id();
        prepare_to_display( metro_stations_container::get_metro_station_devices_properties_raw_list(),
                                         size);
        devices->save(sys_sett_obj->get_devices_config_name());
 } catch (spider_exception spr_exc) {
   sys_sett_obj->sys_message(system_settings::ERROR_MSG, "In activate_ok_button_in_udku_properties raised exception: "+spr_exc.get_description());
   return Pt_HALT;
};

return (Pt_CONTINUE);

};


int
activate_ok_button_in_escalator_properties( PtWidget_t *widget, 
                                                                     ApInfo_t *apinfo, 
                                                                   PtCallbackInfo_t *cbinfo ){
int *internal_deivce_id=NULL,
       device_id;
ushort_t *internal_sel_item_index, sel_item_index;
int *internal_numeric_value, numeric_value;
unsigned short size;
system_settings_spider* sys_sett_obj=system_settings_spider::get_instance();
if (sys_sett_obj==NULL) { 
           cout<<"Can`t get instance of system_settins_spider"<<endl;
           return Pt_HALT;
    };

PtGetResource(widget, Pt_ARG_USER_DATA, &internal_deivce_id, 0);
device_id=*internal_deivce_id;
metro_escalator *escalator_to_properties;
try {
      metro_devices_container *devices=metro_devices_container::get_instance();
      if (devices==NULL)
           throw spider_exception("devices==NULL");

       metro_stations_container *stations=metro_stations_container::get_instance();
       metro_stations_container::iterator stations_iter;
       if (stations==NULL)
           throw spider_exception("stations==NULL");

       metro_devices_container::iterator dev_iter=devices->find(device_id);
       if (dev_iter==devices->end())
           throw spider_exception("dev_iter==devices->end()");

       if (dev_iter->second->get_type_description()!=metro_device_type::DEVICE_TYPE_ESCALATOR)
              throw spider_exception("get_type_description()!=metro_device_type::DEVICE_TYPE_ESCALATOR");

         if (metro_stations_container::get_metro_station_devices_properties_panel()==NULL)
              throw spider_exception("metro_stations_container::get_metro_station_devices_properties_panel()==NULL");

         try {
             escalator_to_properties=dynamic_cast<metro_escalator*>(dev_iter->second);
          } catch (bad_cast) {
              throw spider_exception("dynamic_cast<metro_escalator*>(dev_iter->second) is a bad cast");
          };

         stations_iter=stations->find(escalator_to_properties->get_station_id());
         if (stations_iter==stations->end()) {
               ostringstream message;
               message<<"Not found station with id "<<escalator_to_properties->get_station_id()<<
                                  " for escalator with id "<<escalator_to_properties->get_id();
               throw spider_exception(message.str());
           };

       if (escalator_to_properties->get_escalator_pref_direction_combobox_widget()==NULL)
               throw spider_exception("escalator_to_properties->get_escalator_direction_combobox_widget()==NULL");
	    PtGetResource(escalator_to_properties->get_escalator_pref_direction_combobox_widget(),
							Pt_ARG_CBOX_SEL_ITEM,
							&internal_sel_item_index,
							0);
 	    sel_item_index=*internal_sel_item_index;

        escalator_to_properties->set_pref_direction(sel_item_index);

       if (escalator_to_properties->get_escalator_start_direction_combobox_widget()==NULL)
               throw spider_exception("escalator_to_properties->get_escalator_direction_combobox_widget()==NULL");
	    PtGetResource(escalator_to_properties->get_escalator_start_direction_combobox_widget(),
							Pt_ARG_CBOX_SEL_ITEM,
							&internal_sel_item_index,
							0);
 	    sel_item_index=*internal_sel_item_index;

        escalator_to_properties->set_start_direction(sel_item_index);

       if (escalator_to_properties->get_escalator_start_mode_combobox_widget()==NULL)
               throw spider_exception("escalator_to_properties->get_escalator_start_mode_combobox_widget()==NULL");
	    PtGetResource(escalator_to_properties->get_escalator_start_mode_combobox_widget(),
							Pt_ARG_CBOX_SEL_ITEM,
							&internal_sel_item_index,
							0);
 	    sel_item_index=*internal_sel_item_index;
        escalator_to_properties->set_execution_mode(sel_item_index-1);

       if (escalator_to_properties->get_escalator_start_hour_widget()==NULL)
               throw spider_exception("escalator_to_properties->get_escalator_start_hour_widget()==NULL");
	    PtGetResource(escalator_to_properties->get_escalator_start_hour_widget(),
							Pt_ARG_NUMERIC_VALUE,
							&internal_numeric_value,
							0);
 	    numeric_value=*internal_numeric_value;
        escalator_to_properties->set_start_hour(numeric_value);

       if (escalator_to_properties->get_escalator_start_hour_widget()==NULL)
               throw spider_exception("escalator_to_properties->get_escalator_start_minute_widget()==NULL");
	    PtGetResource(escalator_to_properties->get_escalator_start_minute_widget(),
							Pt_ARG_NUMERIC_VALUE,
							&internal_numeric_value,
							0);
 	    numeric_value=*internal_numeric_value;
        escalator_to_properties->set_start_minute(numeric_value);

        dev_iter->second->update_device_widget();
        size=stations_iter->second.size_devices_id();
        prepare_to_display( metro_stations_container::get_metro_station_devices_properties_raw_list(),
                                         size);
        devices->save(sys_sett_obj->get_devices_config_name());
 } catch (spider_exception spr_exc) {
   sys_sett_obj->sys_message(system_settings::ERROR_MSG, "In activate_ok_button_in_escalator_properties raised exception: "+spr_exc.get_description());
   return Pt_HALT;
};

return (Pt_CONTINUE);
}


