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
#include <map>
#include <list>
#include <functional>
#include <algorithm>
#include <sstream>

#include "test_cond.h"

#include "system_settings_classes/defines.h"
#include "system_settings_classes/spider_exception.h"
#include "system_settings_classes/system_settings.h"
#include "system_settings_classes/system_settings_spider.h"

#include "com_port_class/comport_exception.h"
#include "com_port_class/comport.h"

#include "metro_devices_classes/command.h"
#include "metro_devices_classes/data_block.h"
#include "metro_devices_classes/escalator_data_block.h"
#include "metro_devices_classes/shavr_data_block.h"

#include "containers_classes/timer_command_container.h"

#include "containers_classes/msg_types_container.h"
#include "containers_classes/msg_dict_container.h"
#include "metro_devices_classes/devices_types.h"

#include "containers_classes/metro_devices_types_container.h"

#include "metro_devices_classes/metro_device.h"
#include "metro_devices_classes/escalator.h"
#include "metro_devices_classes/shavr.h"

class metro_devices_container;
#include "metro_devices_classes/station.h"
#include "containers_classes/metro_stations_container.h"
#include "containers_classes/metro_devices_container.h"

metro_stations_container metro_stations;
msg_types_container msg_types;
msg_dict_container messages;
metro_devices_types_container metro_devices_types;
metro_devices_container metro_devices;


ApDBase_t *widgets_db=NULL;
PtWidget_t *main_window=NULL, *station_window=NULL;
system_settings_spider *spider_sys_sett=NULL;

int  escalator_type_id=1, shavr_type_id=2;

int activate_start_test_button( PtWidget_t *widget,
                                              ApInfo_t *apinfo,
                                              PtCallbackInfo_t *cbinfo );

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

int pulse_reciever_catcher (void *data,
                                          int rcvid,
                                          void *message,
                                          size_t mbsize);

int activate_device_menu_item( PtWidget_t *widget,
                                    void *client_data,
                                    PtCallbackInfo_t *cbinfo);

 int activate_device_menu (PtWidget_t *widget,
                                              ApInfo_t *apinfo,
                                              PtCallbackInfo_t *cbinfo);

void* comport_thread_function(void* comport_ptr);

int main (int argc , char* argv[]) {
ostringstream exception_description;
string path_to_dbase("../wgt/interface.wgtp"), station_name("Сырецкая");

PtCallback_t tmp_callback;
PhPoint_t position;
PhRect_t text_extend;
int station_width=0, station_height=0 ,channel, pthread_creating_result;

metro_shavr *shavr;
metro_devices_container::iterator iter_dev;

comport *comport_device;
pthread_attr_t      attr;

pthread_attr_init(&attr);
pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	
signal( SIGPIPE, SIG_IGN );

vector<PtArg_t> args(6);
vector<PtCallback_t> callbacks(1);
tmp_callback.event_f=activate_start_test_button;
tmp_callback.data=NULL;
callbacks.push_back(tmp_callback);

vector<PtCallback_t> menu_callbacks(1);
tmp_callback.event_f=activate_device_menu;
tmp_callback.data=NULL;
menu_callbacks.push_back(tmp_callback);


PtSetArg(&args[0], Pt_ARG_WINDOW_TITLE,
         "Emulation metro station devices", 0);
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

/*
Begin globals initialization
*/
spider_sys_sett=new system_settings_spider();
try {

channel  = PhChannelAttach(0, -1, NULL);;

if (channel==-1)
	throw spider_exception("Can`t create channel");

} catch (spider_exception spr_exc) { 
	cout << "Initialization failure :\n"<<spr_exc.get_description()<<endl;
}; // catch (spider_exception spr_exc) 

//intialize_metro_devices_types();
//intialize_metro_devices(channel);

metro_stations.insert (metro_stations.end(), 
                                  metro_stations_container::value_type (1,
                                     metro_station (
                                           1,
                                           1,
                                           200,
                                           100,
                                           "Test station",
                                           "nothing.wav")
                                            ) 
                                   );

try {
msg_types.load("messages_types.ini");
messages.load("global_messages.ini",
                         &msg_types);
metro_devices_types.load("devices_types.ini",
                                          &msg_types,
                                          spider_sys_sett);
metro_devices.load("devices.ini",
                                channel,
                                &metro_stations,
                                &metro_devices_types,
                                spider_sys_sett);
} catch (spider_exception spr_exc) {
  cout <<"In loading from configuration files raised exception:\n"<<spr_exc.get_description()<<endl;
  return 1;
}; //  catch (spider_exception spr_exc) 

if (PtAppAddInput(NULL, 0, pulse_reciever_catcher, NULL)== NULL){
		spider_sys_sett->sys_message(system_settings::ERROR_MSG,
													    "Error pulse_reciever_catcher callback");
          PtExit(EXIT_FAILURE);
    	} ; // if (PtAppAddInput(NULL, 0, 


 args.resize(8);
 
 position.x=20;
 position.y=30;
 string button_text("Start tests");
 
PtSetArg(&args[0], Pt_ARG_LABEL_TYPE,
         Pt_Z_STRING, 0);
PtSetArg(&args[1], Pt_ARG_TEXT_STRING,
         button_text.c_str(), 0);
PtSetArg(&args[3], Pt_ARG_HEIGHT,
         40, 0);
PtSetArg(&args[4], Pt_ARG_WIDTH,
         90, 0);
PtSetArg(&args[5], Pt_ARG_POS,
         &position, 0);      
PtSetArg(&args[6],  Pt_CB_ACTIVATE,
			  &callbacks[0],
			  callbacks.size());

PtCreateWidget(PtButton,
						main_window,
 		                args.size(),
  			            &args[0]);

try {  // try 1
	 widgets_db=ApOpenDBaseFile( path_to_dbase.c_str());

	 if (widgets_db==NULL) {
	 	exception_description<<"Widgets dbase file "<<path_to_dbase<<" can`t be opened.";
	 	throw spider_exception(exception_description.str());
	 }; //  if (widgets_db==NULL)

	if (ApAddClass ("PtPane", &PtPane)!=0) {
	 	exception_description<<"Can`t add class \"PtPane\"";
	 	throw spider_exception(exception_description.str());
	};  //if (ApAddClass ("PtPane", &PtPane)!=0)

	if (ApAddClass ("PtButton", &PtButton)!=0) {
	 	exception_description<<"Can`t add class \"PtButton\"";
	 	throw spider_exception(exception_description.str());
	}; //if (ApAddClass ("PtButton", &PtButton)!=0)

	if (ApAddClass ("PtLabel", &PtLabel)!=0) {
	 	exception_description<<"Can`t add class \"PtPane\"";
	 	throw spider_exception(exception_description.str());
	}; //if (ApAddClass ("PtLabel", &PtLabel)!=0)

	 
	spider_sys_sett->prepare_visualization(widgets_db);		 
	
    if (spider_sys_sett->small_font_prepared()) {
            PfExtentText(&text_extend,
                                 NULL,
                                 (const char*)spider_sys_sett->get_small_font(),
                                 station_name.c_str(),
                                 station_name.size());
	} else {  //if (spider_sys_sett->small_font_prepared())
	 	exception_description<<"Small font not prepared! ";
	 	throw spider_exception(exception_description.str());
	}; // if (spider_sys_sett->small_font_prepared())
	
	position.x=150;
 	position.y=150;
 
	args.resize(1);

    	PtSetArg(&args[0], Pt_ARG_TITLE, station_name.c_str(), 0);

    station_window = ApCreateDBWidget(widgets_db,
                                                               "station_wnd",
                                                               main_window,
                                                               &position,
                                                               args.size(),
                                                              &args[0]);
	 if (station_window==NULL) {
	 	exception_description<<"Station window \""<<station_name<<"\" can`t be creatded";
	 	throw spider_exception(exception_description.str());
	 }; // if (station_window==NULL)

	/*
	metro devices widgets here
	*/
	int x=system_settings_spider::SPACE_BETWEEN_METRO_DEVICES_IN_STATION, 
	     y=system_settings_spider::TOP_BOTTOM_MARGIN_OF_STATION;
	PhRect_t rect;
	tmp_callback.event_f=activate_device_button;
	tmp_callback.data=NULL;
	callbacks.clear();
	callbacks.push_back(tmp_callback);
	
	iter_dev=metro_devices.begin();
	while (iter_dev!=metro_devices.end()) {
	iter_dev->second->create_device_widget(x,
                                                                     y,
                                                                     widgets_db,
                                                                     station_window,
                                                                     callbacks,
                                                                     menu_callbacks);

	rect=iter_dev->second->get_device_widget_extent();

	x+=(rect.lr.x-rect.ul.x)+system_settings_spider::SPACE_BETWEEN_METRO_DEVICES_IN_STATION;
	iter_dev++;
	}; //  while (iter_dev!=metro_devices.end(

	station_width=x;
	args.clear();
	args.resize(2);
	PtSetArg(&args[0], Pt_ARG_WIDTH, __max(text_extend.lr.x + 15, station_width), 0);
	station_height=100;
	
	station_height=(text_extend.lr.y-text_extend.ul.y) + 
	                        2*system_settings_spider::TOP_BOTTOM_MARGIN_OF_STATION+
	                        (rect.lr.y-rect.ul.y);
	PtSetArg(&args[1], Pt_ARG_HEIGHT, station_height, 0);
	PtSetResources(station_window, args.size(), &args[0]);

/*
	Begin : adding escalators extents
*/
   metro_devices_container::iterator iter_esc;
   iter_dev=metro_devices.begin();
   while (iter_dev!=metro_devices.end()) {
         if (iter_dev->second->get_type_description()!=metro_device_type::DEVICE_TYPE_SHAVR) {
                iter_dev++;
                continue;
            }; // if (iter_dev->second->get_type_description()!=metro_device_type::
         metro_device::related_devices_ids::iterator rel_iter=iter_dev->second->devices_ids_begin();
         while(rel_iter!=iter_dev->second->devices_ids_end()) {
                   iter_esc=metro_devices.find(*rel_iter);
                   if (iter_esc==metro_devices.end()) {
                      exception_description<<"adding escalators extents :  not found device  with id "<<*rel_iter;
                      throw spider_exception(exception_description.str());
                      }; //if (iter_esc==metro_devices.end())
                shavr=static_cast<metro_shavr*>(iter_dev->second);
                	shavr->push_back_escalator_dimension(
                                           iter_esc->second->get_device_widget_extent());		  
                rel_iter++;
                };  //while(rel_iter!=iter_dev->second->devices_ids_end(
         iter_dev++;
         }; // while (iter_dev!=metro_devices.end())

/*
	End : adding escalators extents
*/

/*
	Begin : updating devices widgets
*/
   iter_dev=metro_devices.begin();
   while (iter_dev!=metro_devices.end()) {
   		 iter_dev->second->update_device_widget(spider_sys_sett);
         iter_dev++;
         }; //  while (iter_dev!=metro_devices.end())
/*
	End : updating devices widgets
*/


	try {
	comport_device=new comport(1, //only for  emulation used comport 1 connected to comport 2 
                                       comport::BAUD_19200,
                                       comport::PARITY_DISABLE,
                                       comport::DATA_BITS_8,
                                       comport::STOP_BITS_1);

	} catch (comport_exception cmp_exc) {
	     throw spider_exception (cmp_exc.get_description());
    }; // catch (comport_e

	try {
		pthread_creating_result=pthread_create(NULL, &attr, &comport_thread_function, comport_device);
		if ( pthread_creating_result!= EOK){
			   string message("fail to create device thread : ");	
			   message+=strerror(pthread_creating_result);
  			   throw comport_exception(message);
		}; //  if ( pthread_creating_result!
	} catch (comport_exception cmp_exc) {
	     delete(comport_device);
         throw spider_exception (cmp_exc.get_description());
   }; // } catch (comport_exception cmp_exc)


} catch (spider_exception spr_exc) {  //try 1
		cout << "In main raized exception :\n"<<spr_exc.get_description()<<endl;
/*
Begin globals destruction
*/
  if (spider_sys_sett!=NULL) 	 delete(spider_sys_sett);
/*
End globals destruction
*/
		if (widgets_db!=NULL) ApCloseDBase( widgets_db );
		if (channel!=-1) ChannelDestroy(channel);
		 return (EXIT_FAILURE);
};  //} catch (spider_exception spr_exc) {

PtRealizeWidget(main_window);
PtMainLoop();

/*
Begin globals destruction
*/
if (spider_sys_sett!=NULL) delete(spider_sys_sett);		
/*
End globals destruction
*/
 if (widgets_db!=NULL) ApCloseDBase( widgets_db );
 if (channel!=-1) ChannelDestroy(channel);
 return (EXIT_SUCCESS);
}


