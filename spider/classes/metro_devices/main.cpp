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

using namespace std;

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
#include "shavr.h"
#include "escalator.h"


class metro_devices_container;
#include "station.h"
#include "metro_stations_container.h"
#include "metro_devices_container.h"

#include "router.h"

msg_dict_container *messages=NULL;
log_records_container *main_log=NULL, *archive_log=NULL;

int close_device_panel_window( PtWidget_t *widget,
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

int pulse_reciever_catcher
                    (void *data,
                     int rcvid,
                     void *message,
                    size_t mbsize );

void* metro_device_thread (void* arg);

void* routing_thread (void* arg);

int main (int argc , char* argv[]) {
ostringstream exception_description;
string path_to_dbase("../wgt/interface.wgtp");
PtWidget_t *main_window=NULL;
ApDBase_t *widgets_db;
PtCallback_t tmp_callback;
PhRect_t text_extend;
int channel;
metro_devices_container::iterator iter_dev;

//begin globals initialization
system_settings_spider 	*spider_sys_sett=system_settings_spider::get_instance();
if (spider_sys_sett==NULL) {
   cout<<"Can`t get instance of system_settings_spider"<<endl;
   return 1;
};
msg_types_container *msg_types=msg_types_container::get_instance();
if (msg_types==NULL) {
     string message ("Can`t get instance of msg_types_container");
     spider_sys_sett->sys_message(system_settings::ERROR_MSG, message);
     cout<<message<<endl;
     return 1;
};
//msg_dict_container not singleton becouse own msg_dict_container hold each metro_device_type
// and exist one global msg_dict_container
messages=new msg_dict_container(); 
if (messages==NULL) {
     string message ("Can`t create global variable of msg_dict_container");
     spider_sys_sett->sys_message(system_settings::ERROR_MSG, message);
     cout<<message<<endl;
     return 1;
};

//log_rec_container not singleton becouse exsists main_log, log_rec_container for current events 
// and archive_log, log_rec_container for loaded from files with archivates events 
main_log=new log_records_container(); 
if (main_log==NULL) {
     string message ("Can`t create global variable of log_records_container  - main_log");
     spider_sys_sett->sys_message(system_settings::ERROR_MSG, message);
     cout<<message<<endl;
     return 1;
};

archive_log=new log_records_container(); 
if (archive_log==NULL) {
     string message ("Can`t create global variable of log_records_container  - archive_log");
     spider_sys_sett->sys_message(system_settings::ERROR_MSG, message);
     cout<<message<<endl;
     return 1;
};

metro_devices_types_container *metro_devices_types=metro_devices_types_container::get_instance();
if (metro_devices_types==NULL) {
     string message ("Can`t get instance of metro_devices_types_container");
     spider_sys_sett->sys_message(system_settings::ERROR_MSG, message);
     cout<<message<<endl;
     return 1;
 };
metro_devices_container *metro_devices=metro_devices_container::get_instance();
if (metro_devices==NULL) {
     string message ("Can`t get instance of metro_devices_container");
     spider_sys_sett->sys_message(system_settings::ERROR_MSG, message);
     cout<<message<<endl;
     return 1;
 };
metro_stations_container *metro_stations=metro_stations_container::get_instance();
if (metro_stations==NULL) {
     string message ("Can`t get instance of metro_stations_container");
     spider_sys_sett->sys_message(system_settings::ERROR_MSG, message);
     cout<<message<<endl;
     return 1;
 };

router router_instance=router::get_instance();

//end globals initialization

signal( SIGPIPE, SIG_IGN );

vector<PtArg_t> args(6);
vector<PtCallback_t> activate_device_widget_callbacks;

tmp_callback.event_f=activate_start_test_button;
tmp_callback.data=NULL;

activate_device_widget_callbacks.clear();
activate_device_widget_callbacks.push_back(tmp_callback);

vector<PtCallback_t> activate_device_menu_callbacks;
tmp_callback.event_f=activate_device_menu;
tmp_callback.data=NULL;

activate_device_menu_callbacks.clear();
activate_device_menu_callbacks.push_back(tmp_callback);

PtSetArg(&args[0], Pt_ARG_WINDOW_TITLE,
         "Test of system_settings_spider object", 0);
PtSetArg(&args[1], Pt_ARG_WINDOW_MANAGED_FLAGS,
         Pt_TRUE, Ph_WM_CLOSE);
PtSetArg(&args[2], Pt_ARG_WINDOW_MANAGED_FLAGS,
         Pt_FALSE, Ph_WM_TOFRONT );
PtSetArg(&args[3], Pt_ARG_WINDOW_MANAGED_FLAGS,
         Pt_TRUE, Ph_WM_TOBACK);
PtSetArg(&args[4], Pt_ARG_HEIGHT,
         300, 0);
PtSetArg(&args[5], Pt_ARG_WIDTH,
         500, 0);

main_window=PtAppInit( NULL,
                0,
                NULL ,
                args.size(),
                &args[0]);

if (main_window==NULL) {
  string message("Can`t PtAppInit main_window==NULL\n");
  cout<<message<<endl;
  spider_sys_sett->sys_message(system_settings::ERROR_MSG, message);
  return 1;
};
spider_sys_sett->set_main_window(main_window);

try { 
 channel  = PhChannelAttach(0, -1, NULL);
 if (channel==-1)
	   throw spider_exception("Can`t create channel");
} catch (spider_exception spr_exc) {
	cout << "Initialization failure :\n"<<spr_exc.get_description()<<endl;
}

try {
msg_types->load(spider_sys_sett->messages_types_name());
messages->load(spider_sys_sett->global_messages_name());
metro_devices_types->load(spider_sys_sett->devices_types_name());
metro_devices->load(spider_sys_sett->devices_config_name(),
                                channel);
router_instance.load(spider_sys_sett->get_routing_name());
} catch (spider_exception spr_exc) {
  string message("In loading from configuration files raised exception:\n"+spr_exc.get_description());
  cout <<message<<endl;
  spider_sys_sett->sys_message(system_settings::ERROR_MSG, message);
  return 1;
};


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

    if (spider_sys_sett->small_font_prepared()) {
            PfExtentText(&text_extend,
                                 NULL,
                                 (const char*)spider_sys_sett->get_small_font(),
                                 station_name.c_str(),
                                 station_name.size());
	} else {
	 	exception_description<<"Small font not prepared! ";
	 	throw spider_exception(exception_description.str());
	};
	
metro_stations_container::iterator iter_stations;

int x=100, y=100, counter=50;

iter_stations=metro_stations->begin();
while (iter_stations!=metro_stations->end()) {

   iter_stations->second.create_wnd(main_window,
                     x+counter, y+counter);  // throw(spider_exception)

    counter+=100;
    iter_stations++;
};

//Attaching callback for pulses in main thread
if (PtAppAddInput(NULL, 0, pulse_reciever_catcher, NULL)== NULL){
		spider_sys_sett->sys_message(system_settings::ERROR_MSG,
													    "Error pulse_reciever_catcher callback");

          PtExit(EXIT_FAILURE);
    	}

//	Begin : creating devices threads
    pthread_attr_t detached_thread_attributes;

    if (pthread_attr_init(&detached_thread_attributes)!=EOK) {
		spider_sys_sett->sys_message(system_settings::ERROR_MSG,
													    "Can`t pthread_attr_init(&detached_thread_attributes)");
          PtExit(EXIT_FAILURE);
        };

        if (pthread_attr_setdetachstate( &detached_thread_attributes,
                 PTHREAD_CREATE_DETACHED)!=EOK) {
           spider_sys_sett->sys_message(system_settings::ERROR_MSG,
													    "Can`t pthread_attr_setdetachstate( ..., PTHREAD_CREATE_DETACHED)");
          PtExit(EXIT_FAILURE);
        };

   iter_dev=metro_devices->begin();
   while (iter_dev!=metro_devices->end()) {       
       	if (pthread_create(NULL, &detached_thread_attributes, &metro_device_thread, iter_dev->second) != EOK)
     		      spider_sys_sett->message_window(system_settings::ERROR_MSG,
                                                                           "Can`t create device thread" );
            iter_dev++;
          };
} catch (spider_exception spr_exc) {  //try 1
		cout << "In main raized exception :\n"<<spr_exc.get_description()<<endl;
		if (widgets_db!=NULL) ApCloseDBase( widgets_db );
		if (channel!=-1) ChannelDestroy(channel);
	 return (EXIT_FAILURE);
}; //  catch (spider_exception spr_exc)

PtRealizeWidget(main_window);
PtMainLoop();

 if (widgets_db!=NULL) ApCloseDBase( widgets_db );
 if (channel!=-1) ChannelDestroy(channel);
 return (EXIT_SUCCESS);
}
