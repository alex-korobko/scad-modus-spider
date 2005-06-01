
#include <Ap.h>
#include <Ph.h>
#include <Pt.h>

#include <arpa/inet.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/neutrino.h>
#include <sys/resource.h>
#include <sys/syspage.h>
#include <inttypes.h>
#include <fcntl.h>
#include <signal.h>
#include <iostream.h>
#include <net/if.h>
#include <net/route.h>

#include <hash_map>
#include <vector>
#include <string>
#include <set>
#include <list>
#include <map>
#include <functional>
#include <algorithm>
#include <sstream>

using namespace std;

#include "test_cond.h"

#include "defines.h"
#include <sys/select.h> //#defines FD_SIZE
#include "spider_exception.h"
#include "system_settings.h"
#include "system_settings_spider.h"

#include "socket_exception.h"
#include "generic_socket.h"
#include "client_socket.h"

#include "sound.h"

#include "command.h"
#include "data_block.h"
#include "escalator_data_block.h"
#include "shavr_data_block.h"

#include "msg_types_container.h"
#include "msg_dict_container.h"

#include "devices_types.h"

#include "timer_command_container.h"
#include "metro_devices_types_container.h"

#include "log_records_container.h"

#include "metro_device.h"
#include "shavr.h"
#include "escalator.h"

#include "line.h"
#include "metro_lines_container.h"

class metro_devices_container;
#include "station.h"
#include "metro_stations_container.h"
#include "metro_devices_container.h"
#include "router.h"

msg_dict_container *messages=NULL;
log_records_container *main_log=NULL, *archive_log=NULL;

PtWidget_t *main_window=NULL;

bool sending_commands_disabled=true;
bool setting_start_time_disabled=true;

string password_for_switching_sending_commands("124");
string password_for_switching_start_time_commands("7316883");

int resize_main_window( PtWidget_t *widget,
                                          ApInfo_t *apinfo,
                                          PtCallbackInfo_t *cbinfo );

int
activate_archive_log_button_in_main_window( PtWidget_t *widget, 
                                                                                     ApInfo_t *apinfo, 
                                                                   PtCallbackInfo_t *cbinfo );

int
activate_exit_button_in_main_window( PtWidget_t *widget, 
                                                                                     ApInfo_t *apinfo, 
                                                                   PtCallbackInfo_t *cbinfo );

int
  activate_commands_pool_button_in_main_window(
                                          PtWidget_t *widget, 
                                          ApInfo_t *apinfo, 
                                          PtCallbackInfo_t *cbinfo );

int
activate_main_log_button_in_main_window( PtWidget_t *widget, 
                                                                                     ApInfo_t *apinfo, 
                                                                   PtCallbackInfo_t *cbinfo );

int
activate_timer_commands_button_in_main_window( PtWidget_t *widget, 
                                                                                     ApInfo_t *apinfo, 
                                                                   PtCallbackInfo_t *cbinfo );

int
activate_devices_parameters_button_in_main_window( PtWidget_t *widget, 
                                                                                     ApInfo_t *apinfo, 
                                                                   PtCallbackInfo_t *cbinfo );

int
close_device_panel_window( PtWidget_t *widget,
                                                   ApInfo_t *apinfo, 
                                                   PtCallbackInfo_t *cbinfo );

int
activate_close_device_panel_button( PtWidget_t *widget,
                                                        ApInfo_t *apinfo, 
                                                        PtCallbackInfo_t *cbinfo );

int activate_device_button( PtWidget_t *widget,
                                         ApInfo_t *apinfo,
                                         PtCallbackInfo_t *cbinfo );

 int activate_device_menu (PtWidget_t *widget,
                                              ApInfo_t *apinfo,
                                              PtCallbackInfo_t *cbinfo);

 int  activate_sending_commands_disabled_button_in_main_window
                                            (PtWidget_t *widget,
                                              ApInfo_t *apinfo,
                                              PtCallbackInfo_t *cbinfo);
//threads
int pulse_reciever_catcher
                    (void *data,
                     int rcvid,
                     void *message,
                    size_t mbsize );
void* metro_device_thread (void* arg);
void* routing_thread (void* arg);

void load_map(string file_name, int channel) throw (spider_exception) {
 	enum {LINE=0, STATION, ESCALATOR, SHAVR, UDKU, DOOR, ENTRIES_COUNT};
	ostringstream exception_description;
	string	section_name;
	const char *section_name_c_str;
	vector<string> sections_names(ENTRIES_COUNT);

	sections_names[LINE]="line";
	sections_names[STATION]="station";
 	sections_names[ESCALATOR]="escalator";
 	sections_names[SHAVR]="shavr";
 	sections_names[UDKU]="udku";
 	sections_names[DOOR]="door";

	if (PxConfigOpen( file_name.c_str(), PXCONFIG_READ)!=Pt_TRUE){
		exception_description<<"Can`t open config file "<<file_name<<" : "<<strerror(errno);
		throw spider_exception(exception_description.str());		
	};

   metro_devices_container* metro_devices=metro_devices_container::get_instance();
   if (metro_devices==NULL) 
       throw spider_exception("Can`t get instance of metro_devices_container");

  metro_stations_container* metro_stations=metro_stations_container::get_instance();
   if (metro_stations==NULL) 
      throw spider_exception("Can`t get instance of metro_stations_container");

  metro_lines_container* metro_lines=metro_lines_container::get_instance();
  if (metro_lines==NULL) 
      throw spider_exception("Can`t get instance of metro_lines_container");

	metro_devices->erase(metro_devices->begin(),
                                          metro_devices->end());

    metro_stations->erase(metro_stations->begin(),
                                           metro_stations->end());
	
	metro_lines->erase(metro_lines->begin(),
                                      metro_lines->end());

	try {
	    section_name_c_str=PxConfigNextSection();
          while (section_name_c_str!=NULL) {
              section_name=section_name_c_str;
	
              if (section_name.compare(sections_names[LINE])==0) 	{
                         metro_lines->load_line_parameters ();
                  } else if (section_name.compare(sections_names[STATION])==0) {
                         metro_stations->load_station_parameters ();
                  } else if (section_name.compare(sections_names[ESCALATOR])==0) {
                         metro_devices->load_escalator_parameters (channel);
                  } else if (section_name.compare(sections_names[SHAVR])==0) {
                         metro_devices->load_shavr_parameters (channel);
                  } else if (section_name.compare(sections_names[UDKU])==0) {
                         metro_devices->load_udku_parameters (channel);
                  } else if (section_name.compare(sections_names[DOOR])==0) {
                         metro_devices->load_door_parameters (channel);
                  } else {
					exception_description<<"Unrecognized config section "<<section_name;
					throw spider_exception(exception_description.str());
                 };
				section_name_c_str=PxConfigNextSection();
           };
	} catch (spider_exception (spr_exc)) {
		exception_description<<spr_exc.get_description();
		if  (PxConfigClose()!=Pt_TRUE) {
				exception_description<<"\nCan`t close config file "<<file_name;
		};
		throw spider_exception(exception_description.str());
	};
 
	metro_lines->set_current_line(metro_lines->end());
	metro_stations->set_current_station(metro_stations->end());

	if  (PxConfigClose()!=Pt_TRUE)	{
		exception_description<<"Can`t close config file "<<file_name;
		throw spider_exception(exception_description.str());
	};
}


int main (int argc , char* argv[]) {
ostringstream exception_description;
string path_to_dbase("../wgt/interface.wgtp");
PtWidget_t *current_panel=NULL, *current_button=NULL, *indicator_send_commands_disabled=NULL;
ApDBase_t *widgets_db=NULL;
PhPoint_t widget_position;
PhRect_t main_window_rectangle;
int channel=-1;
unsigned int top_panel_height=45;
metro_devices_container::iterator iter_dev;

//begin globals initialization
system_settings_spider 	*spider_sys_sett=system_settings_spider::get_instance();
if (spider_sys_sett==NULL) {
  exception_description<<"Can`t get instance of system_settings_spider";
  cout<<exception_description.str()<<endl;
  PtExit(EXIT_FAILURE);
};

msg_types_container *msg_types=msg_types_container::get_instance();
if (msg_types==NULL) {
  exception_description<<"Can`t get instance of msg_types_container";
  cout<<exception_description.str()<<endl;
  spider_sys_sett->sys_message(system_settings::ERROR_MSG,exception_description.str());
  PtExit(EXIT_FAILURE);
};

//msg_dict_container not singleton becouse own msg_dict_container hold each metro_device_type
// and exist one global msg_dict_container
messages=new msg_dict_container(); 
if (messages==NULL) {
  exception_description<<"Can`t create global variable of msg_dict_container";
  cout<<exception_description.str()<<endl;
  spider_sys_sett->sys_message(system_settings::ERROR_MSG,exception_description.str());
  PtExit(EXIT_FAILURE);
};

//log_rec_container not singleton becouse exsists main_log, log_rec_container for current events 
// and archive_log, log_rec_container for loaded from files with archivates events 
main_log=new log_records_container(); 
if (main_log==NULL) {
  exception_description<<"Can`t create global variable of log_rec_container  - main_log";
  cout<<exception_description.str()<<endl;
  spider_sys_sett->sys_message(system_settings::ERROR_MSG,exception_description.str());
  PtExit(EXIT_FAILURE);
};

archive_log=new log_records_container(); 
if (archive_log==NULL) {
  exception_description<<"Can`t create global variable of log_rec_container  - archive_log";
  cout<<exception_description.str()<<endl;
  spider_sys_sett->sys_message(system_settings::ERROR_MSG,exception_description.str());
  PtExit(EXIT_FAILURE);
};

metro_devices_types_container *metro_devices_types=metro_devices_types_container::get_instance();
if (metro_devices_types==NULL) {
  exception_description<<"Can`t get instance of metro_devices_types_container";
  cout<<exception_description.str()<<endl;
  spider_sys_sett->sys_message(system_settings::ERROR_MSG,exception_description.str());
  PtExit(EXIT_FAILURE);
 };

metro_devices_container *metro_devices=metro_devices_container::get_instance();
if (metro_devices==NULL) {
  exception_description<<"Can`t get instance of metro_devices_container";
  cout<<exception_description.str()<<endl;
  spider_sys_sett->sys_message(system_settings::ERROR_MSG,exception_description.str());
  PtExit(EXIT_FAILURE);
 };
 
metro_stations_container *metro_stations=metro_stations_container::get_instance();
if (metro_stations==NULL) {
   exception_description<<"Can`t get instance of metro_stations_container";
  cout<<exception_description.str()<<endl;
  spider_sys_sett->sys_message(system_settings::ERROR_MSG,exception_description.str());
  PtExit(EXIT_FAILURE);

 };
 
metro_lines_container *metro_lines=metro_lines_container::get_instance();
if (metro_lines==NULL) {
  exception_description<<"Can`t get instance of metro_lines_container";
  cout<<exception_description.str()<<endl;
  spider_sys_sett->sys_message(system_settings::ERROR_MSG,exception_description.str());
  PtExit(EXIT_FAILURE);
 };

router& router_instance=router::get_instance();

//end globals initialization
signal( SIGPIPE, SIG_IGN ); //ignores signal SIGPIPE

vector<PtArg_t> args(17);
vector<PtCallback_t> widget_callbacks;
PtCallback_t tmp_callback;

tmp_callback.event_f=resize_main_window;
tmp_callback.data=NULL;

widget_callbacks.clear();
widget_callbacks.push_back(tmp_callback);

PtSetArg(&args[0], Pt_ARG_WINDOW_TITLE,
         "АРМ дисперчера эскалаторов", 0);
PtSetArg(&args[1], Pt_ARG_WINDOW_MANAGED_FLAGS,
         Pt_FALSE, Ph_WM_CLOSE);
PtSetArg(&args[2], Pt_ARG_WINDOW_MANAGED_FLAGS,
         Pt_FALSE, Ph_WM_TOFRONT );
PtSetArg(&args[3], Pt_ARG_WINDOW_MANAGED_FLAGS,
         Pt_TRUE, Ph_WM_TOBACK);
PtSetArg(&args[4], Pt_ARG_WINDOW_MANAGED_FLAGS,
         Pt_FALSE, Ph_WM_MAX);
PtSetArg(&args[5], Pt_ARG_WINDOW_MANAGED_FLAGS,
         Pt_FALSE, Ph_WM_BACKDROP);
PtSetArg(&args[6], Pt_ARG_WINDOW_MANAGED_FLAGS,
         Pt_FALSE, Ph_WM_RESIZE);
PtSetArg(&args[7], Pt_ARG_WINDOW_STATE,
         Pt_TRUE, Ph_WM_STATE_ISMAX );
PtSetArg(&args[8], Pt_CB_RESIZE,
         &widget_callbacks[0],
         widget_callbacks.size());
PtSetArg(&args[9], Pt_ARG_WINDOW_RENDER_FLAGS,
         Pt_FALSE, Ph_WM_RENDER_HELP);
PtSetArg(&args[10], Pt_ARG_WINDOW_RENDER_FLAGS,
         Pt_FALSE, Ph_WM_RENDER_COLLAPSE);
PtSetArg(&args[11], Pt_ARG_WINDOW_RENDER_FLAGS,
         Pt_FALSE, Ph_WM_RENDER_MENU);
PtSetArg(&args[12], Pt_ARG_WINDOW_RENDER_FLAGS,
         Pt_FALSE, Ph_WM_RENDER_MIN);
PtSetArg(&args[13], Pt_ARG_WINDOW_RENDER_FLAGS,
         Pt_FALSE, Ph_WM_RENDER_RESIZE);
PtSetArg(&args[14], Pt_ARG_WINDOW_RENDER_FLAGS,
         Pt_FALSE, Ph_WM_RENDER_TITLE);
PtSetArg(&args[15], Pt_ARG_CURSOR_TYPE,
         Ph_CURSOR_BIG_POINTER, 0);
PtSetArg(&args[16], Pt_ARG_CURSOR_COLOR,
         0xf33ff5, 0);

main_window=PtAppInit( NULL,
                0,
                NULL ,
                args.size(),
                &args[0]);

if (main_window==NULL) {
  exception_description<<"Can`t PtAppInit main_window==NULL\n";
  cout<<exception_description.str()<<endl;
  spider_sys_sett->sys_message(system_settings::ERROR_MSG,exception_description.str());
  PtExit(EXIT_FAILURE);
};
spider_sys_sett->set_main_window(main_window);
PtExtentWidget(main_window);
PtWidgetExtent(main_window, &main_window_rectangle);

//Begin: creating in window panels and buttons
try {
args.clear();
args.resize(7);
PtSetArg(&args[0], Pt_ARG_ANCHOR_FLAGS,
         Pt_TRUE, Pt_LEFT_ANCHORED_LEFT);
PtSetArg(&args[1], Pt_ARG_ANCHOR_FLAGS,
         Pt_TRUE, Pt_RIGHT_ANCHORED_RIGHT);
PtSetArg(&args[2], Pt_ARG_ANCHOR_FLAGS,
         Pt_TRUE, Pt_TOP_ANCHORED_TOP);
PtSetArg(&args[3], Pt_ARG_ANCHOR_FLAGS,
         Pt_TRUE, Pt_BOTTOM_ANCHORED_TOP);
PtSetArg(&args[4], Pt_ARG_HEIGHT,
         top_panel_height, 0);
PtSetArg(&args[5], Pt_ARG_WIDTH,
         main_window_rectangle.lr.x-main_window_rectangle.ul.x, 0);
         
widget_position.x=0;
widget_position.y=1;
PtSetArg(&args[6], Pt_ARG_POS,
         &widget_position, 0);
current_panel=PtCreateWidget( PtPane,
                main_window,
                args.size(),
                &args[0]);
if (current_panel==NULL) {
   throw spider_exception("Can`t create buttons panel");
};


//clock widget
args.clear();
args.resize(27);
PtSetArg(&args[0], Pt_ARG_CLOCK_TYPE,
         Pt_CLOCK_LED, 0);
PtSetArg(&args[1], Pt_ARG_CLOCK_FLAGS,
         Pt_TRUE, Pt_CLOCK_SHOW_SECONDS);
PtSetArg(&args[2], Pt_ARG_CLOCK_FLAGS,
         Pt_TRUE, Pt_CLOCK_24_HOUR);
PtSetArg(&args[3], Pt_ARG_CLOCK_HOUR_COLOR,
         0x000000, 0);
PtSetArg(&args[4], Pt_ARG_CLOCK_MINUTE_COLOR,
         0x000000, 0);
PtSetArg(&args[5], Pt_ARG_CLOCK_SECOND_COLOR,
         0x000000, 0);
PtSetArg(&args[6], Pt_ARG_CLOCK_SEP1_COLOR,
         0x000000, 0);
PtSetArg(&args[7], Pt_ARG_CLOCK_SEP2_COLOR,
         0x000000, 0);
PtSetArg(&args[8], Pt_ARG_BEVEL_WIDTH,
         2, 0);
PtSetArg(&args[9], Pt_ARG_MARGIN_HEIGHT,
         2, 0);
PtSetArg(&args[10], Pt_ARG_MARGIN_WIDTH,
         2, 0);



widget_position.x=1100;
widget_position.y=0;
PtSetArg(&args[11], Pt_ARG_POS,
         &widget_position, 0);
PtSetArg(&args[12], Pt_ARG_FLAGS,
          Pt_TRUE, Pt_HIGHLIGHTED);
PtSetArg(&args[13], Pt_ARG_BORDER_CONTRAST,
         80, 0);
PtSetArg(&args[14], Pt_ARG_BASIC_FLAGS,
         Pt_TRUE, Pt_FLAT_FILL);
PtSetArg(&args[15], Pt_ARG_BASIC_FLAGS,
         Pt_TRUE, Pt_FULL_BORDERS);
         
PtSetArg(&args[16], Pt_ARG_BASIC_FLAGS,
         Pt_TRUE, Pt_TOP_OUTLINE);
PtSetArg(&args[17], Pt_ARG_BASIC_FLAGS,
         Pt_TRUE, Pt_TOP_BEVEL);

PtSetArg(&args[18], Pt_ARG_BASIC_FLAGS,
         Pt_TRUE, Pt_BOTTOM_OUTLINE);
PtSetArg(&args[19], Pt_ARG_BASIC_FLAGS,
         Pt_TRUE, Pt_BOTTOM_BEVEL);

PtSetArg(&args[20], Pt_ARG_BASIC_FLAGS,
         Pt_TRUE, Pt_LEFT_OUTLINE);
PtSetArg(&args[21], Pt_ARG_BASIC_FLAGS,
         Pt_TRUE, Pt_LEFT_BEVEL);

PtSetArg(&args[22], Pt_ARG_BASIC_FLAGS,
         Pt_TRUE, Pt_RIGHT_OUTLINE);
PtSetArg(&args[23], Pt_ARG_BASIC_FLAGS,
         Pt_TRUE, Pt_RIGHT_BEVEL);
PtSetArg(&args[24], Pt_ARG_FILL_COLOR,
         0x00ff00, 0);
PtSetArg(&args[25], Pt_ARG_WIDTH,
         150, 0);
PtSetArg(&args[26], Pt_ARG_HEIGHT,
         (system_settings_spider::TOP_PANEL_HEIGHT_IN_MAIN_WINDOW-4), 0);

  if (PtCreateWidget( PtClock,
                current_panel,
                args.size(),
                &args[0])==NULL) 
	   throw spider_exception("Can`t create clock widget");

//button command pool
args.clear();
args.resize(5);
PtSetArg(&args[0], Pt_ARG_TEXT_STRING,
         "КОМАНДЫ", 0);
PtSetArg(&args[1], Pt_ARG_HEIGHT,
         system_settings_spider::TOP_PANEL_HEIGHT_IN_MAIN_WINDOW-10,0);
PtSetArg(&args[2], Pt_ARG_WIDTH,
         120,0);
widget_position.x=2;
widget_position.y=2;
PtSetArg(&args[3], Pt_ARG_POS,
         &widget_position, 0);

tmp_callback.event_f=activate_commands_pool_button_in_main_window;
tmp_callback.data=NULL;

widget_callbacks.clear();
widget_callbacks.push_back(tmp_callback);
PtSetArg(&args[4], Pt_CB_ACTIVATE,
               &widget_callbacks[0],
               widget_callbacks.size());

current_button=PtCreateWidget( PtButton,
                current_panel,
                args.size(),
                &args[0]);

if (current_button==NULL) {
	   throw spider_exception("Can`t create button for timer_commands");
};

/*
//button devices parameters -  disabled, devices properties modifyed by click on station widget
args.clear();
args.resize(5);
PtSetArg(&args[0], Pt_ARG_TEXT_STRING,
         "УСТРОЙСТВА", 0);
PtSetArg(&args[1], Pt_ARG_HEIGHT,
         system_settings_spider::TOP_PANEL_HEIGHT_IN_MAIN_WINDOW-10,0);
PtSetArg(&args[2], Pt_ARG_WIDTH,
         120,0);
widget_position.x=140;
widget_position.y=2;
PtSetArg(&args[3], Pt_ARG_POS,
         &widget_position, 0);

tmp_callback.event_f=activate_devices_parameters_button_in_main_window;
tmp_callback.data=NULL;

widget_callbacks.clear();
widget_callbacks.push_back(tmp_callback);
PtSetArg(&args[4], Pt_CB_ACTIVATE,
               &widget_callbacks[0],
               widget_callbacks.size());

current_button=PtCreateWidget( PtButton,
                current_panel,
                args.size(),
                &args[0]);

if (current_button==NULL) {
	   throw spider_exception("Can`t create button for timer_commands");
};
*/

//button devices parameters
args.clear();
args.resize(6);
PtSetArg(&args[0], Pt_ARG_HEIGHT,
         system_settings_spider::TOP_PANEL_HEIGHT_IN_MAIN_WINDOW-10,0);
PtSetArg(&args[1], Pt_ARG_WIDTH,
         130,0);
widget_position.x=170;
widget_position.y=2;
PtSetArg(&args[2], Pt_ARG_POS,
         &widget_position, 0);

tmp_callback.event_f=activate_timer_commands_button_in_main_window;
tmp_callback.data=NULL;

widget_callbacks.clear();
widget_callbacks.push_back(tmp_callback);
PtSetArg(&args[3], Pt_CB_ACTIVATE,
               &widget_callbacks[0],
               widget_callbacks.size());
PtSetArg(&args[4], Pt_ARG_LABEL_TYPE,
                Pt_TEXT_IMAGE , 0);
 PtSetArg(&args[5], Pt_ARG_TEXT_IMAGE_SPACING,
                                4, 0);
current_button=PtCreateWidget( PtButton,
                current_panel,
                args.size(),
                &args[0]);

if (current_button==NULL) {
	   throw spider_exception("Can`t create button for timer_commands");
};

timer_command_container *timer_commands=timer_command_container::get_instance();
if (timer_commands==NULL)
        throw spider_exception ("Can`t get instance of timer_command_container ");
timer_commands->set_timer_commands_state_indicator(current_button);

//button main log
args.clear();
args.resize(5);
PtSetArg(&args[0], Pt_ARG_TEXT_STRING,
         "ЖУРНАЛ", 0);
PtSetArg(&args[1], Pt_ARG_HEIGHT,
         system_settings_spider::TOP_PANEL_HEIGHT_IN_MAIN_WINDOW-10,0);
PtSetArg(&args[2], Pt_ARG_WIDTH,
         140,0);
widget_position.x=410;
widget_position.y=2;
PtSetArg(&args[3], Pt_ARG_POS,
         &widget_position, 0);

tmp_callback.event_f=activate_main_log_button_in_main_window;
tmp_callback.data=NULL;

widget_callbacks.clear();
widget_callbacks.push_back(tmp_callback);
PtSetArg(&args[4], Pt_CB_ACTIVATE,
               &widget_callbacks[0],
               widget_callbacks.size());

current_button=PtCreateWidget( PtButton,
                current_panel,
                args.size(),
                &args[0]);

if (current_button==NULL)
	   throw spider_exception("Can`t create button for timer_commands");

//button archive log
args.clear();
args.resize(5);
PtSetArg(&args[0], Pt_ARG_TEXT_STRING,
         "АРХИВ", 0);
PtSetArg(&args[1], Pt_ARG_HEIGHT,
         system_settings_spider::TOP_PANEL_HEIGHT_IN_MAIN_WINDOW-10,0);
PtSetArg(&args[2], Pt_ARG_WIDTH,
         120,0);
widget_position.x=550;
widget_position.y=2;
PtSetArg(&args[3], Pt_ARG_POS,
         &widget_position, 0);

tmp_callback.event_f=activate_archive_log_button_in_main_window;
tmp_callback.data=NULL;

widget_callbacks.clear();
widget_callbacks.push_back(tmp_callback);
PtSetArg(&args[4], Pt_CB_ACTIVATE,
               &widget_callbacks[0],
               widget_callbacks.size());

current_button=PtCreateWidget( PtButton,
                current_panel,
                args.size(),
                &args[0]);

if (current_button==NULL)
	   throw spider_exception("Can`t create button for timer_commands");

//button sending_commands_disabled
args.clear();
args.resize(5);
PtSetArg(&args[0], Pt_ARG_LABEL_TYPE,
         Pt_TEXT_IMAGE,0);
PtSetArg(&args[1], Pt_ARG_HEIGHT,
         system_settings_spider::TOP_PANEL_HEIGHT_IN_MAIN_WINDOW-10,0);
PtSetArg(&args[2], Pt_ARG_WIDTH,
         120,0);
widget_position.x=750;
widget_position.y=2;
PtSetArg(&args[3], Pt_ARG_POS,
         &widget_position, 0);

tmp_callback.event_f=activate_sending_commands_disabled_button_in_main_window;
tmp_callback.data=NULL;

widget_callbacks.clear();
widget_callbacks.push_back(tmp_callback);
PtSetArg(&args[4], Pt_CB_ACTIVATE,
               &widget_callbacks[0],
               widget_callbacks.size());

current_button=PtCreateWidget( PtButton,
                current_panel,
                args.size(),
                &args[0]);

if (current_button==NULL)
	   throw spider_exception("Can`t create button for sending_commands_disabled");

indicator_send_commands_disabled=current_button;

//button exit
args.clear();
args.resize(5);
PtSetArg(&args[0], Pt_ARG_TEXT_STRING, "ВЫХОД", 0);
PtSetArg(&args[1], Pt_ARG_HEIGHT,
         system_settings_spider::TOP_PANEL_HEIGHT_IN_MAIN_WINDOW-10,0);
PtSetArg(&args[2], Pt_ARG_WIDTH,
         120,0);
widget_position.x=950;
widget_position.y=2;
PtSetArg(&args[3], Pt_ARG_POS,
         &widget_position, 0);

tmp_callback.event_f=activate_exit_button_in_main_window;
tmp_callback.data=NULL;

widget_callbacks.clear();
widget_callbacks.push_back(tmp_callback);
PtSetArg(&args[4], Pt_CB_ACTIVATE,
               &widget_callbacks[0],
               widget_callbacks.size());

current_button=PtCreateWidget( PtButton,
                current_panel,
                args.size(),
                &args[0]);

if (current_button==NULL)
	   throw spider_exception("Can`t create button for timer_commands");

//map container panel
args.clear();
args.resize(19);
PtSetArg(&args[0], Pt_ARG_ANCHOR_FLAGS,
         Pt_TRUE, Pt_LEFT_ANCHORED_LEFT);
PtSetArg(&args[1], Pt_ARG_ANCHOR_FLAGS,
         Pt_TRUE, Pt_RIGHT_ANCHORED_RIGHT);
PtSetArg(&args[2], Pt_ARG_ANCHOR_FLAGS,
         Pt_TRUE, Pt_TOP_ANCHORED_TOP);
PtSetArg(&args[3], Pt_ARG_ANCHOR_FLAGS,
         Pt_TRUE, Pt_BOTTOM_ANCHORED_TOP);
widget_position.x=0;
widget_position.y=top_panel_height+1;
PtSetArg(&args[4], Pt_ARG_POS,
         &widget_position, 0);
PtSetArg(&args[5], Pt_ARG_CONTRAST,
         120, 0);
PtSetArg(&args[6], Pt_ARG_BORDER_CONTRAST,
         80, 0);
PtSetArg(&args[7], Pt_ARG_BASIC_FLAGS,
         Pt_TRUE, Pt_FLAT_FILL);
PtSetArg(&args[8], Pt_ARG_BASIC_FLAGS,
         Pt_TRUE, Pt_FULL_BORDERS);
         
PtSetArg(&args[9], Pt_ARG_BASIC_FLAGS,
         Pt_TRUE, Pt_TOP_OUTLINE);
PtSetArg(&args[10], Pt_ARG_BASIC_FLAGS,
         Pt_TRUE, Pt_TOP_BEVEL);
         
PtSetArg(&args[11], Pt_ARG_BASIC_FLAGS,
         Pt_TRUE, Pt_BOTTOM_OUTLINE);
PtSetArg(&args[12], Pt_ARG_BASIC_FLAGS,
         Pt_TRUE, Pt_BOTTOM_BEVEL);

PtSetArg(&args[13], Pt_ARG_BASIC_FLAGS,
         Pt_TRUE, Pt_LEFT_OUTLINE);
PtSetArg(&args[14], Pt_ARG_BASIC_FLAGS,
         Pt_TRUE, Pt_LEFT_BEVEL);

PtSetArg(&args[15], Pt_ARG_BASIC_FLAGS,
         Pt_TRUE, Pt_RIGHT_OUTLINE);
PtSetArg(&args[16], Pt_ARG_BASIC_FLAGS,
         Pt_TRUE, Pt_RIGHT_BEVEL);

PtSetArg(&args[17], Pt_ARG_FILL_COLOR,
         0xaaaaaa, 0);
PtSetArg(&args[18], Pt_ARG_BEVEL_WIDTH, 4, 0);

  current_panel=PtCreateWidget( PtPane,
                main_window,
                args.size(),
                &args[0]);
  if (current_panel==NULL) 
	   throw spider_exception("Can`t create map panel");

   spider_sys_sett->set_map_container(current_panel);

} catch (spider_exception spr_exc) {
  exception_description<<"Initialization failure :\n"<<spr_exc.get_description();
  cout<<exception_description.str()<<endl;
  spider_sys_sett->sys_message(system_settings::ERROR_MSG,exception_description.str());
  PtExit(EXIT_FAILURE);
}
//End: creating in window panels and buttons

try { 
 channel  = PhChannelAttach(0, -1, NULL);
 if (channel==-1)
	   throw spider_exception("Can`t create channel");
} catch (spider_exception spr_exc) {
  exception_description<<"Initialization failure :\n"<<spr_exc.get_description();
  cout<<exception_description.str()<<endl;
  spider_sys_sett->sys_message(system_settings::ERROR_MSG,exception_description.str());
  PtExit(EXIT_FAILURE);
}

try {  //try1
	 widgets_db=ApOpenDBaseFile( path_to_dbase.c_str());
	 if (widgets_db==NULL) {
	 	exception_description<<"Widgets dbase file "<<path_to_dbase<<" can`t be opened.";
	 	throw spider_exception(exception_description.str());
	 };

	if (ApAddClass ("PtPane", &PtPane)!=0) {
	 	exception_description<<"Can`t add class \"PtPane\"";
	 	throw spider_exception(exception_description.str());
	};

	if (ApAddClass ("PtButton", &PtButton)!=0) {
	 	exception_description<<"Can`t add class \"PtButton\"";
	 	throw spider_exception(exception_description.str());
	};

	if (ApAddClass ("PtLabel", &PtLabel)!=0) {
	 	exception_description<<"Can`t add class \"PtPane\"";
	 	throw spider_exception(exception_description.str());
	};
	spider_sys_sett->prepare_visualization(widgets_db);		 

//Attaching callback for pulses in main thread
if (PtAppAddInput(NULL, 0, pulse_reciever_catcher, NULL)== NULL){
		spider_sys_sett->sys_message(system_settings::ERROR_MSG,
													    "Error pulse_reciever_catcher callback");
          PtExit(EXIT_FAILURE);
    	}

try {
msg_types->load(spider_sys_sett->get_messages_types_name());
messages->load(spider_sys_sett->get_global_messages_name());
metro_devices_types->load(spider_sys_sett->get_devices_types_name());
load_map(spider_sys_sett->get_devices_config_name(), channel);

//	Begin : creating devices threads
   iter_dev=metro_devices->begin();
   while (iter_dev!=metro_devices->end()) {
       if (iter_dev->second->get_enabled())
       	   if (pthread_create(NULL, NULL, &metro_device_thread, iter_dev->second) != EOK)
     		      spider_sys_sett->message_window(system_settings::ERROR_MSG,
                                                                           "Can`t create device thread" );
       iter_dev++;
    }; //while (iter_dev!=metro_devices->end())


main_log->load(spider_sys_sett->get_main_log_name());
router_instance.load(spider_sys_sett->get_routing_name());
} catch (spider_exception spr_exc) {
  string message("In loading from configuration files raised exception:\n"+spr_exc.get_description());
  cout <<message<<endl;
  spider_sys_sett->sys_message(system_settings::ERROR_MSG, message);
  PtExit(EXIT_FAILURE);
};

try {
  metro_lines_container::iterator lines_iter=metro_lines->begin();
  while (lines_iter!=metro_lines->end()) {
    lines_iter->second.create_widget(current_panel);
    lines_iter++;
   };
} catch (spider_exception spr_exc) {
  string message("In creating widgets map raised exception:\n"+spr_exc.get_description());
  cout <<message<<endl;
  spider_sys_sett->sys_message(system_settings::ERROR_MSG, message);
  PtExit(EXIT_FAILURE);
};

//Begin : Creating timer thread
	timer_t  timer_id;
	struct sigevent event;
	struct itimerspec timer_spec;

    int timer_connection_id = ConnectAttach(0, 0, channel, 0, 0);

    if (timer_connection_id==-1)
          throw spider_exception ("Can`t attach to channel for timer");

	  SIGEV_PULSE_INIT(&event, timer_connection_id, system_settings_spider::PHOTON_THREAD_PULSE,
                                    system_settings_spider::PULSE_CODE_TIMER_INTERVAL, 0);


	if (timer_create(CLOCK_REALTIME, &event, &timer_id) == -1)
             throw spider_exception("Can`t create timer in timer_create(...)");

	timer_spec.it_value.tv_sec = 1;
	timer_spec.it_value.tv_nsec = 0;
	timer_spec.it_interval.tv_sec = 5; //5 seconds
	timer_spec.it_interval.tv_nsec = 0;

	timer_settime(timer_id, 0, &timer_spec, NULL);

//End : Creating timer thread

//Begin: creating routing thread
if (router_instance.is_routing_enabled()) {
       	if (pthread_create(NULL, NULL, &routing_thread, NULL) != EOK)
     		      spider_sys_sett->message_window(system_settings::ERROR_MSG, "Can`t create routing thread" );
   };
//End:   creating thread

} catch (spider_exception spr_exc) {  //try 1
		cout << "In main raized exception :\n"<<spr_exc.get_description()<<endl;
		if (widgets_db!=NULL) ApCloseDBase( widgets_db );
		if (channel!=-1) ChannelDestroy(channel);
	 PtExit(EXIT_FAILURE);
}; //  catch (spider_exception spr_exc)

ldword tmp_autoincrement=main_log->get_records_autoincrement();
main_log->set_records_autoincrement(++tmp_autoincrement);
main_log->insert(log_record ( tmp_autoincrement,
                                          system_settings::MESSAGE_ID_SYSTEM_STARTED,
                                          4,
                                          0,
                                          0,
                                          time(NULL)));

PtRealizeWidget(main_window);
timer_command_container *timer_commands=timer_command_container::get_instance();
if (timer_commands==NULL)
        throw spider_exception ("Can`t get instance of timer_command_container after PtRealizeWidget(main_window)");
timer_commands->change_indicator_state();
args.clear();
args.resize(2);
if (sending_commands_disabled) {
       PtSetArg(&args[0], Pt_ARG_TEXT_STRING,
                       "УПРАВЛЕНИЕ ОТКЛЮЧЕНО", 0);
       PtSetArg(&args[1], Pt_ARG_LABEL_IMAGE,
                       spider_sys_sett->get_image(system_settings_spider::BLOCK_RED_LED), 0);
   } else {
       PtSetArg(&args[0], Pt_ARG_TEXT_STRING,
                       "УПРАВЛЕНИЕ ВКЛЮЧЕНО", 0);
       PtSetArg(&args[1], Pt_ARG_LABEL_IMAGE,
                       spider_sys_sett->get_image(system_settings_spider::BLOCK_GREEN_LED), 0);
   };
PtSetResources(indicator_send_commands_disabled, args.size(), &args[0]);

sound& snd=sound::get_instance();
snd.play("sounds/start.wav");

PtMainLoop();

 return (EXIT_SUCCESS); //make compiler happ
}
