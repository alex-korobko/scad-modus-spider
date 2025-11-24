using namespace std;

#include <Ap.h>
#include <Ph.h>
#include <Pt.h>

#include <pthread.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/neutrino.h>
#include <sys/syspage.h>
#include <inttypes.h>
#include <fcntl.h>

#include <iostream>

#include <vector>
#include <string>
#include <hash_map>
#include <set>
#include <list>
#include <functional>
#include <algorithm>
#include <sstream>

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
#include "log_records_container.h"
class metro_devices_container;
#include "line.h"
#include "metro_lines_container.h"
#include "station.h"
#include "metro_stations_container.h"
#include "metro_devices_types_container.h"

#include "sound.h"

#include "data_block.h"
#include "shavr_data_block.h"
#include "escalator_data_block.h"

#include "metro_device.h"
#include "metro_devices_container.h"
#include "shavr.h"
#include "escalator.h"

extern msg_dict_container *messages;
extern log_records_container *main_log;

metro_shavr::metro_shavr(
		int id, 
		int id_station,
		int number,
		int modbus_number,
		int type,
        int	 start_day_mode,
        int	 start_hour,
        int	 start_minute,
		int channel,
		bool enabled,
		in_addr_t	ip,
		time_t offline_or_exception_delay,
		bool new_conduction_is_switched_off,
		bool new_log_packets,
		int new_door_id) throw (spider_exception):
	metro_device( id,
                   id_station,
                   number,
                   modbus_number,
                   type,
                   start_day_mode,
                   start_hour,
                   start_minute,                 
                   enabled,
                   ip,
                   channel,
				  offline_or_exception_delay,
				   new_conduction_is_switched_off,
					new_log_packets),
	A0_x5(false),  //offline 
	A0_state(A0_OFFLINE),
	door_id(new_door_id){};

metro_shavr::~metro_shavr(){
};

void metro_shavr::A0(int event) 
          throw (spider_exception){
 int old_automat_state=A0_state;

 switch (A0_state) {
    case A0_OFFLINE: 
         if (A0_x1 &&
             !A0_x2) {
           A0_state=A0_EXCEPTION;
           break;
         };

         if (A0_x1 &&
             !A0_x3 && !A0_x4) {
           A0_state=A0_DISABLED_1_DISABLED_2;
           break;
         };

         if (A0_x1 &&
             A0_x3 && A0_x4) {
           A0_state=A0_ENABLED_1_ENABLED_2;
           break;
         };
         if (A0_x1 &&
             !A0_x3 && A0_x4) {
           A0_state=A0_DISABLED_1_ENABLED_2;
           break;
         };
         if (A0_x1 &&
             A0_x3 && !A0_x4) {
           A0_state=A0_ENABLED_1_DISABLED_2;
           break;
         };
     break;

    case A0_EXCEPTION:
          if (!A0_x1) {
           A0_state=A0_OFFLINE;
           break;
         };
         if (A0_x2 &&
             !A0_x3 && !A0_x4) {
           A0_state=A0_DISABLED_1_DISABLED_2;
           break;
         };
         if (A0_x2 &&
             A0_x3 && A0_x4) {
           A0_state=A0_ENABLED_1_ENABLED_2;
           break;
         };
         if (A0_x2 &&
             !A0_x3 && A0_x4) {
           A0_state=A0_DISABLED_1_ENABLED_2;
           break;
         };
         if (A0_x2 &&
             A0_x3 && !A0_x4) {
           A0_state=A0_ENABLED_1_DISABLED_2;
           break;
         };     
    break;
    case A0_DISABLED_1_DISABLED_2:
          if (!A0_x1) {
           A0_state=A0_OFFLINE;
           break;
          };
          if (!A0_x2) {
           A0_state=A0_EXCEPTION;
           break;
          };
         if (event==1) {
                A0_z1();
               break;
             };
         if (A0_x3 && A0_x4) {
           A0_state=A0_ENABLED_1_ENABLED_2;
           break;
         };
         if (!A0_x3 && A0_x4) {
           A0_state=A0_DISABLED_1_ENABLED_2;
           break;
         };
         if (A0_x3 && !A0_x4) {
           A0_state=A0_ENABLED_1_DISABLED_2;
           break;
         };     
     break;
    case A0_ENABLED_1_ENABLED_2:
          if (!A0_x1) {
           A0_state=A0_OFFLINE;
           break;
          };
          if (!A0_x2) {
           A0_state=A0_EXCEPTION;
           break;
          };
         if (event==1) {
                A0_z1();
               break;
             };
         if (!A0_x3 && !A0_x4) {
           A0_state=A0_DISABLED_1_DISABLED_2;
           break;
         };
         if (!A0_x3 && A0_x4) {
           A0_state=A0_DISABLED_1_ENABLED_2;
           break;
         };
         if (A0_x3 && !A0_x4) {
           A0_state=A0_ENABLED_1_DISABLED_2;
           break;
         };     
     break;
    case A0_DISABLED_1_ENABLED_2:
          if (!A0_x1) {
           A0_state=A0_OFFLINE;
           break;
          };
          if (!A0_x2) {
           A0_state=A0_EXCEPTION;
           break;
          };
         if (event==1) {
                A0_z1();
               break;
             };
         if (!A0_x3 && !A0_x4) {
           A0_state=A0_DISABLED_1_DISABLED_2;
           break;
         };
         if (A0_x3 && A0_x4) {
           A0_state=A0_ENABLED_1_ENABLED_2;
           break;
         };
         if (A0_x3 && !A0_x4) {
           A0_state=A0_ENABLED_1_DISABLED_2;
           break;
         };     
     break;
    case A0_ENABLED_1_DISABLED_2:
          if (!A0_x1) {
           A0_state=A0_OFFLINE;
           break;
          };
          if (!A0_x2) {
           A0_state=A0_EXCEPTION;
           break;
          };
         if (event==1) {
                A0_z1();
               break;
             };
         if (!A0_x3 && !A0_x4) {
           A0_state=A0_DISABLED_1_DISABLED_2;
           break;
         };
         if (A0_x3 && A0_x4) {
           A0_state=A0_ENABLED_1_ENABLED_2;
           break;
         };
         if (!A0_x3 && A0_x4) {
           A0_state=A0_DISABLED_1_ENABLED_2;
           break;
         };     
     break;
    default :
      ostringstream exception_description;
      exception_description<<"metro_shavr::A0(...) device id "<<get_id();
      exception_description<<" wrong A0 state "<<A0_state;
      throw spider_exception(exception_description.str());        
 }; //switch (A0_state)

 if (old_automat_state==A0_state) return;

//cout<<"old A0_state="<<A0_state<<" A0_x1="<<A0_x1<<" A0_x2="<<A0_x2<<" A0_x3="<<A0_x3<<"A0_x4="<< A0_x4<<endl;
 
//initializations on enter to new state
 switch (A0_state) {
     case A0_OFFLINE:
     A0_z2();
     break;
     case A0_EXCEPTION:
     A0_z2();
     break;
     case A0_DISABLED_1_DISABLED_2:
     break;
     case A0_ENABLED_1_ENABLED_2:
     break;
     case A0_DISABLED_1_ENABLED_2:
     break;
     case A0_ENABLED_1_DISABLED_2:
     break;
     default :
      ostringstream exception_description;
      exception_description<<"metro_shavr::A0(...) device id "<<get_id();
      exception_description<<" after change A0 has undefined state "<<A0_state;
      throw spider_exception(exception_description.str());        

 }; //switch (A0_state)
};
PhRect_t metro_shavr::get_device_widget_extent() 
            throw (spider_exception){

	ostringstream exception_description;
	PhRect_t internal_widget_rectangle, 
	               widget_rectangle;

	exception_description<<"Can`t get device widget on  : device id "<<metro_device::get_id();
	
	if (metro_device::get_device_widget()==NULL)
        throw spider_exception(exception_description.str());
	
	PtExtentWidget( metro_device::get_device_widget());
	PtWidgetExtent( metro_device::get_device_widget(), &internal_widget_rectangle);
   widget_rectangle=internal_widget_rectangle;
	return widget_rectangle;
};

void metro_shavr::send_command(command cmd)  throw (spider_exception){
vector<byte> buffer(0);
vector<bool> escalators_running_state;

    if (is_conduction_is_switched_off()) return;

	if (cmd.get_device_id()!=get_id())
          throw spider_exception("metro_shavr::send_command(...) cmd.get_device_id()!=get_id() ");

//begin: ATTENTION!! LAST checking about escalator runnint before command sending
escalators_running_state.insert(escalators_running_state.end(), 4, true); //all 4 escalators is running
metro_devices_container *devices=metro_devices_container::get_instance();
metro_devices_container::iterator devices_iter;
if (devices==NULL)
          throw spider_exception("In metro_shavr::send_command() can`t get instance of metro_devices_container");

related_devices_ids::iterator related_devices_iter=devices_ids_begin();
metro_escalator *escalator_in_shavr;
byte count=0;
  while(related_devices_iter!=devices_ids_end()) {
        devices_iter=devices->find(*related_devices_iter);
        if (devices_iter!=devices->end()) {
               if (devices_iter->second->get_type_description()==metro_device_type::DEVICE_TYPE_ESCALATOR) {
                        try {
                           escalator_in_shavr=dynamic_cast<metro_escalator*>(devices_iter->second);
                           escalators_running_state[count]=escalator_in_shavr->is_escalator_run();
                           escalators_running_state[count]=false;
                        }catch (bad_cast){
                             //do nothing
                        };
                     };
            } else {
               ostringstream message;
               message<<"In metro_shavr::send_command() not found device with id "<<*related_devices_iter;
               throw spider_exception(message.str());
            };
        count++;
        related_devices_iter++;
     };

    switch (cmd.get_command_code()) {
	case system_settings::TURN_ON_ESC_1_TO_INPUT_1:
	case system_settings::TURN_OFF_ESC_1_TO_INPUT_1:
	case system_settings::TURN_ON_ESC_1_TO_INPUT_2:
	case system_settings::TURN_OFF_ESC_1_TO_INPUT_2:
                  if (escalators_running_state[0]) {
                        ostringstream message;
                         message<<"In metro_shavr::send_command try send command "<<cmd.get_command_code()<<" to RUNNING escalator 1";
                         throw spider_exception(message.str());
                     };
              break;
	case system_settings::TURN_ON_ESC_2_TO_INPUT_1:
   	case system_settings::TURN_OFF_ESC_2_TO_INPUT_1:
   	case system_settings::TURN_ON_ESC_2_TO_INPUT_2:
   	case system_settings::TURN_OFF_ESC_2_TO_INPUT_2:
                  if (escalators_running_state[1]) {
                        ostringstream message;
                         message<<"In metro_shavr::send_command try send command "<<cmd.get_command_code()<<" to RUNNING escalator 2";
                          throw spider_exception(message.str());
                     };
              break;
	case system_settings::TURN_ON_ESC_3_TO_INPUT_1:
   	case system_settings::TURN_OFF_ESC_3_TO_INPUT_1:
   	case system_settings::TURN_ON_ESC_3_TO_INPUT_2:
  	case system_settings::TURN_OFF_ESC_3_TO_INPUT_2:
                  if (escalators_running_state[2]) {
                        ostringstream message;
                         message<<"In metro_shavr::send_command try send command "<<cmd.get_command_code()<<" to RUNNING escalator 3";
                          throw spider_exception(message.str());
                     };
              break;
	case system_settings::TURN_ON_ESC_4_TO_INPUT_1:
   	case  system_settings::TURN_OFF_ESC_4_TO_INPUT_1:
   	case  system_settings::TURN_ON_ESC_4_TO_INPUT_2:
   	case  system_settings::TURN_OFF_ESC_4_TO_INPUT_2:
                  if (escalators_running_state[3]) {
                        ostringstream message;
                         message<<"In metro_shavr::send_command try send command "<<cmd.get_command_code()<<" to RUNNING escalator 4";
                          throw spider_exception(message.str());
                     };
              break;
     }; //switch (cmd.get_command_code())

//end: ATTENTION!! LAST checking about escalator runnint before command sending

if (cmd.get_command_code()!=system_settings::COMMAND_CHANCEL) {
	buffer.push_back(get_modbus_number());	//shavr number
	buffer.push_back(5);		// modbus function

    switch (cmd.get_command_code()) {
	case (system_settings::TURN_ON_ESC_1_TO_INPUT_1):
	      buffer.push_back(0x00);		// output address high byte
	      buffer.push_back(0x00);		// output address low byte 
	      buffer.push_back(0x00);		// state code high byte
	      buffer.push_back(0x00);		// state code low byte
          break;
	case (system_settings::TURN_OFF_ESC_1_TO_INPUT_1):
	      buffer.push_back(0x00);		// output address high byte
	      buffer.push_back(0x00);		// output address low byte 
	      buffer.push_back(0xFF);		// state code high byte
	      buffer.push_back(0x00);		// state code low byte
          break;
	case (system_settings::TURN_ON_ESC_1_TO_INPUT_2):
	      buffer.push_back(0x00);		// output address high byte
	      buffer.push_back(0x01);		// output address low byte 
	      buffer.push_back(0x00);		// state code high byte
	      buffer.push_back(0x00);		// state code low byte
          break;
	case (system_settings::TURN_OFF_ESC_1_TO_INPUT_2):
	      buffer.push_back(0x00);		// output address high byte
	      buffer.push_back(0x01);		// output address low byte 
	      buffer.push_back(0xFF);		// state code high byte
	      buffer.push_back(0x00);		// state code low byte
          break;

	case (system_settings::TURN_ON_ESC_2_TO_INPUT_1):
	      buffer.push_back(0x00);		// output address high byte
	      buffer.push_back(0x02);		// output address low byte 
	      buffer.push_back(0x00);		// state code high byte
	      buffer.push_back(0x00);		// state code low byte
         break;
	case (system_settings::TURN_OFF_ESC_2_TO_INPUT_1):
	      buffer.push_back(0x00);		// output address high byte
	      buffer.push_back(0x02);		// output address low byte 
	      buffer.push_back(0xFF);		// state code high byte
	      buffer.push_back(0x00);		// state code low byte
          break;
	case (system_settings::TURN_ON_ESC_2_TO_INPUT_2):
	      buffer.push_back(0x00);		// output address high byte
	      buffer.push_back(0x03);		// output address low byte 
	      buffer.push_back(0x00);		// state code high byte
	      buffer.push_back(0x00);		// state code low byte
          break;
	case (system_settings::TURN_OFF_ESC_2_TO_INPUT_2):
	      buffer.push_back(0x00);		// output address high byte
	      buffer.push_back(0x03);		// output address low byte 
	      buffer.push_back(0xFF);		// state code high byte
	      buffer.push_back(0x00);		// state code low byte
          break;

	case (system_settings::TURN_ON_ESC_3_TO_INPUT_1):
	      buffer.push_back(0x00);		// output address high byte
	      buffer.push_back(0x04);		// output address low byte 
	      buffer.push_back(0x00);		// state code high byte
	      buffer.push_back(0x00);		// state code low byte
          break;
	case (system_settings::TURN_OFF_ESC_3_TO_INPUT_1):
	      buffer.push_back(0x00);		// output address high byte
	      buffer.push_back(0x04);		// output address low byte 
	      buffer.push_back(0xFF);		// state code high byte
	      buffer.push_back(0x00);		// state code low byte
          break;
	case (system_settings::TURN_ON_ESC_3_TO_INPUT_2):
	      buffer.push_back(0x00);		// output address high byte
	      buffer.push_back(0x05);		// output address low byte 
	      buffer.push_back(0x00);		// state code high byte
	      buffer.push_back(0x00);		// state code low byte
          break;
	case (system_settings::TURN_OFF_ESC_3_TO_INPUT_2):
	      buffer.push_back(0x00);		// output address high byte
	      buffer.push_back(0x05);		// output address low byte 
	      buffer.push_back(0xFF);		// state code high byte
	      buffer.push_back(0x00);		// state code low byte
          break;

	case (system_settings::TURN_ON_ESC_4_TO_INPUT_1):
	      buffer.push_back(0x00);		// output address high byte
	      buffer.push_back(0x06);		// output address low byte 
	      buffer.push_back(0x00);		// state code high byte
	      buffer.push_back(0x00);		// state code low byte
          break;
	case (system_settings::TURN_OFF_ESC_4_TO_INPUT_1):
	      buffer.push_back(0x00);		// output address high byte
	      buffer.push_back(0x06);		// output address low byte 
	      buffer.push_back(0xFF);		// state code high byte
	      buffer.push_back(0x00);		// state code low byte
          break;
	case (system_settings::TURN_ON_ESC_4_TO_INPUT_2):
	      buffer.push_back(0x00);		// output address high byte
	      buffer.push_back(0x07);		// output address low byte 
	      buffer.push_back(0x00);		// state code high byte
	      buffer.push_back(0x00);		// state code low byte
          break;
	case (system_settings::TURN_OFF_ESC_4_TO_INPUT_2):
	      buffer.push_back(0x00);		// output address high byte
	      buffer.push_back(0x07);		// output address low byte 
	      buffer.push_back(0xFF);		// state code high byte
	      buffer.push_back(0x00);		// state code low byte
          break;

    default:
        ostringstream exception_description;
        exception_description<<"metro_shavr::send_command unrecognized command code";
        exception_description<<cmd.get_command_code()<< " device id "<<get_id();
		 throw spider_exception(exception_description.str());      
    };  //switch (cmd.get_command_code())

	system_settings::bytes tmp_bytes=system_settings::bytes_of_type<word>(system_settings::crc(buffer));
	buffer.insert(buffer.end(), tmp_bytes.begin(), tmp_bytes.end());

    metro_device::set_request_to_device_buffer(buffer);
 }  else {  // if (cmd.get_command_code()!=system_settings::COMMAND_CHANCEL)
     A0_x5=false;
 }; //if (cmd.get_command_code()!=system_settings::COMMAND_CHANCEL)


   A0(1);
   update_device_widget();
   PtFlush();
};

void metro_shavr::create_device_widget(int x,
                                                      int y,
                                                      ApDBase_t*  widgets_dbase,
                                                      PtWidget_t *station_widget,
                                                      vector<PtCallback_t> activate_callbacks,
                                                      vector<PtCallback_t> menu_callbacks)
                 throw (spider_exception){
	vector<PtArg_t> args;
	ostringstream description;
	PtWidget_t  *panel_widget;
	PhPoint_t position;
	PhRect_t internal_widget_rectangle;

	if (widgets_dbase==NULL)
    		throw spider_exception("metro_escalator::create_device_window(...) widgets_dbase pointer id NULL");		

	if (station_widget==NULL)
    		throw spider_exception("metro_escalator::create_device_window(...) station_widget pointer id NULL");		

	metro_device::set_station_widget(station_widget);

	int device_id=metro_device::get_id();
	args.clear();
	args.resize(1);
	PtSetArg(&args[0], Pt_ARG_USER_DATA,  &device_id, sizeof(device_id));    
	position.x=x;
	position.y=y;

	panel_widget=ApCreateDBWidget(widgets_dbase,
                                                               "shavr_widget",
                                                               station_widget,
                                                               &position,
                                                               args.size(),
                                                              &args[0]);

    if (panel_widget==NULL)
    		throw spider_exception("metro_escalator::create_device_window(...) can`t create  widget shavr_widget");

    metro_device::set_device_widget(panel_widget);

	args.clear();
	args.resize(5);
	PtSetArg(&args[0], Pt_ARG_USER_DATA, &device_id, sizeof(device_id));
    if (! activate_callbacks.empty()) {
 	     PtSetArg(&args[1],  Pt_CB_ACTIVATE,
			  &activate_callbacks[0],
			  activate_callbacks.size());
         } else {
 	     PtSetArg(&args[1],  Pt_CB_ACTIVATE,
			  NULL,
			  0);
        };
    if (! menu_callbacks.empty()) {
	   PtSetArg(&args[2],  Pt_CB_MENU,
			  &menu_callbacks[0],
			  menu_callbacks.size());
       }else {
	     PtSetArg(&args[2],  Pt_CB_MENU, NULL, 0 );
       };
	PtSetArg(&args[3], Pt_ARG_FLAGS, Pt_TRUE, Pt_MENUABLE);
	PtSetArg(&args[4], Pt_ARG_FLAGS, Pt_FALSE, Pt_ALL_BUTTONS);

	state_indicator_widget=ApCreateDBWidget(widgets_dbase,
                                                         "shavr_widget_panel_btn",
                                                          panel_widget,
                                                          NULL,
                                                          args.size(),
                                                          &args[0]);


    if (state_indicator_widget==NULL)
    		throw spider_exception("metro_escalator::create_device_window(...) can`t create  shavr_widget_panel_btn");

	command_indicator_widget=ApCreateDBWidget(widgets_dbase,
                                                               "shavr_widget_command_lbl",
                                                               panel_widget,
                                                               NULL,
                                                               args.size(),
                                                              &args[0]);

    if (command_indicator_widget==NULL) 
    		throw spider_exception("metro_escalator::create_device_window(...) can`t create shavr_widget_command_lbl");

	PtExtentWidget(panel_widget);
	PtWidgetExtent(panel_widget, &internal_widget_rectangle);

};

void metro_shavr::update_device_widget() throw (spider_exception){
system_settings_spider *sys_sett_obj=system_settings_spider::get_instance();
vector<PtArg_t> args;
vector<PhPoint_t> line_points(2);
PtWidget_t *created_line;
PhRect_t  internal_widget_rectangle;
PgColor_t color_input_1, color_input_2;

power_supply_lines_widgets_iterator lines_iter=power_lines.begin();
while(lines_iter!=power_lines.end()) {
	if (PtDestroyWidget(*lines_iter)!=0) {
		throw spider_exception("metro_shavr::update_device_widget() cant destroy power lines widget");
		};
	  lines_iter++;
 };
power_lines.clear();

if (!metro_device::get_enabled()) return;

if (metro_device::get_station_widget()==NULL)
	throw spider_exception("metro_shavr::update_device_widget() pointer to station widget is null");

if (state_indicator_widget==NULL)
	throw spider_exception("metro_shavr::update_device_widget() pointer to state_indicator_widget is null");

if (command_indicator_widget==NULL)
	throw spider_exception("metro_shavr::update_device_widget() pointer to command_indicator_widget is null");

//cout<<"A0_state "<<A0_state<<endl;
//updating widget state
args.clear();
args.resize(1);
switch (A0_state) {
    case A0_OFFLINE:
        PtSetArg(&args[0],
                     Pt_ARG_LABEL_IMAGE,
                     sys_sett_obj->get_image(system_settings_spider::OFFLINE),
                     0);
    break;
    case A0_EXCEPTION:
        PtSetArg(&args[0],
                     Pt_ARG_LABEL_IMAGE,
                     sys_sett_obj->get_image(system_settings_spider::EXCEPTION),
                     0);
    break;
    case A0_DISABLED_1_DISABLED_2:
        PtSetArg(&args[0],
                     Pt_ARG_LABEL_IMAGE,
                     sys_sett_obj->get_image(system_settings_spider::SHAVR_DISABLED_1_DISABLED_2),
                     0);
    break;
    case A0_ENABLED_1_ENABLED_2:
        PtSetArg(&args[0],
                     Pt_ARG_LABEL_IMAGE,
                     sys_sett_obj->get_image(system_settings_spider::SHAVR_ENABLED_1_ENABLED_2),
                     0);
    break;
    case A0_DISABLED_1_ENABLED_2:
        PtSetArg(&args[0],
                     Pt_ARG_LABEL_IMAGE,
                     sys_sett_obj->get_image(system_settings_spider::SHAVR_DISABLED_1_ENABLED_2),
                     0);
    break;
    case A0_ENABLED_1_DISABLED_2:
        PtSetArg(&args[0],
                     Pt_ARG_LABEL_IMAGE,
                     sys_sett_obj->get_image(system_settings_spider::SHAVR_ENABLED_1_DISABLED_2),
                     0);
    break;
   default :
      ostringstream exception_description;
      exception_description<<"metro_shavr::update_device_widget device id "<<get_id();
      exception_description<<" wrong A0 state "<<A0_state;
      throw spider_exception(exception_description.str());   
};

PtSetResources(state_indicator_widget,
                          args.size(),
                          &args[0]);

//command excecuted
	args.clear();
	args.resize(1);
	if (A0_x5) {
        PtSetArg(&args[0],
                     Pt_ARG_LABEL_IMAGE,
                     sys_sett_obj->get_image(system_settings_spider::SHAVR_COMMAND),
                     0);
	} else { // if (A0_x5)
        PtSetArg(&args[0],
                     Pt_ARG_LABEL_IMAGE,
                     NULL,
                     0);        
	};

  PtSetResources(command_indicator_widget,
                          args.size(),
                          &args[0]);


//power lines updating 
if (escalators_dimensions.empty())
	throw spider_exception("metro_shavr::update_device_widget() empty escalators dimensions container");

   args.clear();
   args.resize(5);
   PtSetArg(&args[0], Pt_ARG_POINTS, &line_points[0], line_points.size());
   PtSetArg(&args[2], Pt_ARG_LINE_WIDTH, system_settings_spider::POWER_LINE_WIDTH, 0);
   PtSetArg(&args[3], Pt_ARG_LINE_JOIN, Pg_ROUND_JOIN, 0);
   PtSetArg(&args[4], Pt_ARG_LINE_CAP, Pg_ROUND_CAP, 0);

 switch (A0_state) {
   case A0_OFFLINE :
   case A0_EXCEPTION : 
   case A0_DISABLED_1_DISABLED_2 :
        color_input_1=color_input_2=system_settings_spider::COLOR_GRAY;
   break;
   case A0_ENABLED_1_ENABLED_2 :
        color_input_1=color_input_2=system_settings_spider::COLOR_GREEN;
   break;
   case A0_DISABLED_1_ENABLED_2 :
        color_input_1=system_settings_spider::COLOR_GRAY;
        color_input_2=system_settings_spider::COLOR_GREEN;
   break;
   case A0_ENABLED_1_DISABLED_2 :
        color_input_1=system_settings_spider::COLOR_GREEN;
        color_input_2=system_settings_spider::COLOR_GRAY;
   break;
   default :
      ostringstream exception_description;
      exception_description<<"metro_shavr::update_device_widget device id "<<get_id();
      exception_description<<" wrong A0 state "<<A0_state;
      throw spider_exception(exception_description.str());   
 };

	PtExtentWidget( metro_device::get_device_widget());
	PtWidgetExtent( metro_device::get_device_widget(), &internal_widget_rectangle);

	//top  long  horisontal power line	(input 1)
	line_points[0].x=internal_widget_rectangle.ul.x+
							(internal_widget_rectangle.lr.x-internal_widget_rectangle.ul.x)/2;
	line_points[0].y=system_settings_spider::TOP_BOTTOM_MARGIN_OF_STATION/2-
                              system_settings_spider::POWER_LINE_WIDTH;
	line_points[1].x=escalators_dimensions[0].ul.x+
							(escalators_dimensions[0].lr.x-escalators_dimensions[0].ul.x)/2;
	line_points[1].y=line_points[0].y;
    PtSetArg(&args[1], Pt_ARG_COLOR, color_input_1, 0);	
	created_line=PtCreateWidget( 
                PtLine,
                metro_device::get_station_widget(),
                args.size(),
                &args[0] );	   
     if (created_line==NULL) {
 	     throw spider_exception("metro_shavr::update_device_widget() can`t create power line widget");     
     };
    PtRealizeWidget(created_line);
	power_lines.push_back(created_line);

	//top short vertical power line from shavr (input 1)
	line_points[1].x=line_points[0].x;
	line_points[1].y=internal_widget_rectangle.ul.y;
	created_line=PtCreateWidget( 
                PtLine,
                metro_device::get_station_widget(),
                args.size(),
                &args[0] );
     if (created_line==NULL) {
 	     throw spider_exception("metro_shavr::update_device_widget() can`t create power line widget");     
     };
    PtRealizeWidget(created_line);
	power_lines.push_back(created_line);

	//bottom short vertical power line from shavr (input 2)
    line_points[0].y=internal_widget_rectangle.lr.y;
	line_points[1].y=line_points[0].y+
							  system_settings_spider::TOP_BOTTOM_MARGIN_OF_STATION/2+
                              system_settings_spider::POWER_LINE_WIDTH;
    PtSetArg(&args[1], Pt_ARG_COLOR, color_input_2, 0);	
	created_line=PtCreateWidget( 
                PtLine,
                metro_device::get_station_widget(),
                args.size(),
                &args[0] );
     if (created_line==NULL) {
 	     throw spider_exception("metro_shavr::update_device_widget() can`t create power line widget");     
     };
    PtRealizeWidget(created_line);
	power_lines.push_back(created_line);

	//bottom long  horisontal power line (input 2)
	line_points[1].x=escalators_dimensions[0].ul.x+
							(escalators_dimensions[0].lr.x-escalators_dimensions[0].ul.x)/2;
	line_points[0].y=line_points[1].y;
	created_line=PtCreateWidget( 
                PtLine,
                metro_device::get_station_widget(),
                args.size(),
                &args[0] );	   
     if (created_line==NULL) {
 	     throw spider_exception("metro_shavr::update_device_widget() can`t create power line widget");     
     };
    PtRealizeWidget(created_line);
	power_lines.push_back(created_line);

if (!A0_x1 ||
    !A0_x2) return;

       if (escalators_dimensions.size()>=1) {
            if (A0_x10) {
                   //top short vertical power line from shavr (input 1)
                   line_points[0].x=line_points[1].x=escalators_dimensions[0].ul.x+
                                                     (escalators_dimensions[0].lr.x-escalators_dimensions[0].ul.x)/2;
                   line_points[0].y=escalators_dimensions[0].ul.y;
                   line_points[1].y=system_settings_spider::TOP_BOTTOM_MARGIN_OF_STATION/2-
                               system_settings_spider::POWER_LINE_WIDTH;
                   PtSetArg(&args[1], Pt_ARG_COLOR, color_input_1, 0);	
                   created_line=PtCreateWidget( PtLine,
                                        metro_device::get_station_widget(),
                                        args.size(),
                                        &args[0] );
                   if (created_line==NULL)
                      throw spider_exception("metro_shavr::update_device_widget() can`t create power line widget");     

                  PtRealizeWidget(created_line);
                  power_lines.push_back(created_line);
             }; //INDEX_SIGNAL_ESCALATOR_1_CONNECTED_TO_INPUT_1

            if (A0_x11) {
                   //bottom short vertical power line from shavr (input 2)
                   line_points[0].x=line_points[1].x=escalators_dimensions[0].ul.x+
                                                     (escalators_dimensions[0].lr.x-escalators_dimensions[0].ul.x)/2;
                   line_points[0].y=escalators_dimensions[0].lr.y;
                   line_points[1].y=line_points[0].y+
                               system_settings_spider::TOP_BOTTOM_MARGIN_OF_STATION/2+
                               system_settings_spider::POWER_LINE_WIDTH;
                   PtSetArg(&args[1], Pt_ARG_COLOR, color_input_2, 0);
                   created_line=PtCreateWidget( PtLine,
                                        metro_device::get_station_widget(),
                                        args.size(),
                                        &args[0] );
                   if (created_line==NULL)
                      throw spider_exception("metro_shavr::update_device_widget() can`t create power line widget");     

                  PtRealizeWidget(created_line);
                  power_lines.push_back(created_line);
             }; //INDEX_SIGNAL_ESCALATOR_1_CONNECTED_TO_INPUT_2
		}; // if (escalators_dimensions.size()>=1)

       if (escalators_dimensions.size()>=2) {
            if (A0_x12) {
                   //top short vertical power line from shavr (input 1)
                   line_points[0].x=line_points[1].x=escalators_dimensions[1].ul.x+
                                                     (escalators_dimensions[1].lr.x-escalators_dimensions[1].ul.x)/2;
                   line_points[0].y=escalators_dimensions[1].ul.y;
                   line_points[1].y=system_settings_spider::TOP_BOTTOM_MARGIN_OF_STATION/2-
                               system_settings_spider::POWER_LINE_WIDTH;
                   PtSetArg(&args[1], Pt_ARG_COLOR, color_input_1, 0);
                   created_line=PtCreateWidget( PtLine,
                                        metro_device::get_station_widget(),
                                        args.size(),
                                        &args[0] );
                   if (created_line==NULL)
                      throw spider_exception("metro_shavr::update_device_widget() can`t create power line widget");     

                  PtRealizeWidget(created_line);
                  power_lines.push_back(created_line);
             };  //INDEX_SIGNAL_ESCALATOR_2_CONNECTED_TO_INPUT_1

             if (A0_x13) {
                   //bottom short vertical power line from shavr (input 2)
                   line_points[0].x=line_points[1].x=escalators_dimensions[1].ul.x+
                                                     (escalators_dimensions[1].lr.x-escalators_dimensions[1].ul.x)/2;
                   line_points[0].y=escalators_dimensions[1].lr.y;
                   line_points[1].y=line_points[0].y+
                               system_settings_spider::TOP_BOTTOM_MARGIN_OF_STATION/2+
                               system_settings_spider::POWER_LINE_WIDTH;
                   PtSetArg(&args[1], Pt_ARG_COLOR, color_input_2, 0);	
                   created_line=PtCreateWidget( PtLine,
                                        metro_device::get_station_widget(),
                                        args.size(),
                                        &args[0] );
                   if (created_line==NULL)
                      throw spider_exception("metro_shavr::update_device_widget() can`t create power line widget");     

                  PtRealizeWidget(created_line);
                  power_lines.push_back(created_line);
             }; //INDEX_SIGNAL_ESCALATOR_2_CONNECTED_TO_INPUT_2
	  }; //if (escalators_dimensions.size()>=2)

       if (escalators_dimensions.size()>=3) {
            if (A0_x14) {
                   //top short vertical power line from shavr (input 1)
                   line_points[0].x=line_points[1].x=escalators_dimensions[2].ul.x+
                                                     (escalators_dimensions[2].lr.x-escalators_dimensions[2].ul.x)/2;
                   line_points[0].y=escalators_dimensions[2].ul.y;
                   line_points[1].y=system_settings_spider::TOP_BOTTOM_MARGIN_OF_STATION/2-
                               system_settings_spider::POWER_LINE_WIDTH;
                   PtSetArg(&args[1], Pt_ARG_COLOR, color_input_1, 0);
                   created_line=PtCreateWidget( PtLine,
                                        metro_device::get_station_widget(),
                                        args.size(),
                                        &args[0] );
                   if (created_line==NULL)
                      throw spider_exception("metro_shavr::update_device_widget() can`t create power line widget");     

                  PtRealizeWidget(created_line);
                  power_lines.push_back(created_line);
             }; //INDEX_SIGNAL_ESCALATOR_3_CONNECTED_TO_INPUT_1

            if (A0_x15) {
                   //bottom short vertical power line from shavr (input 2)
                   line_points[0].x=line_points[1].x=escalators_dimensions[2].ul.x+
                                                     (escalators_dimensions[2].lr.x-escalators_dimensions[2].ul.x)/2;
                   line_points[0].y=escalators_dimensions[2].lr.y;
                   line_points[1].y=line_points[0].y+
                               system_settings_spider::TOP_BOTTOM_MARGIN_OF_STATION/2+
                               system_settings_spider::POWER_LINE_WIDTH;
                   PtSetArg(&args[1], Pt_ARG_COLOR, color_input_2, 0);	
                   created_line=PtCreateWidget( PtLine,
                                        metro_device::get_station_widget(),
                                        args.size(),
                                        &args[0] );
                   if (created_line==NULL) 
                      throw spider_exception("metro_shavr::update_device_widget() can`t create power line widget");     

                  PtRealizeWidget(created_line);
                  power_lines.push_back(created_line);
             };//INDEX_SIGNAL_ESCALATOR_3_CONNECTED_TO_INPUT_2
		}; //if (escalators_dimensions.size()>=3)

       if (escalators_dimensions.size()>=4) {
            if (A0_x16) {
                   //top short vertical power line from shavr (input 1)
                   line_points[0].x=line_points[1].x=escalators_dimensions[3].ul.x+
                                                     (escalators_dimensions[3].lr.x-escalators_dimensions[3].ul.x)/2;
                   line_points[0].y=escalators_dimensions[3].ul.y;
                   line_points[1].y=system_settings_spider::TOP_BOTTOM_MARGIN_OF_STATION/2-
                               system_settings_spider::POWER_LINE_WIDTH;
                   PtSetArg(&args[1], Pt_ARG_COLOR, color_input_1, 0);	
                   created_line=PtCreateWidget( PtLine,
                                        metro_device::get_station_widget(),
                                        args.size(),
                                        &args[0] );
                   if (created_line==NULL)
                      throw spider_exception("metro_shavr::update_device_widget() can`t create power line widget");     

                  PtRealizeWidget(created_line);
                  power_lines.push_back(created_line);
             }; //INDEX_SIGNAL_ESCALATOR_4_CONNECTED_TO_INPUT_1

            if (A0_x17) {
                   //bottom short vertical power line from shavr (input 2)
                   line_points[0].x=line_points[1].x=escalators_dimensions[3].ul.x+
                                                     (escalators_dimensions[3].lr.x-escalators_dimensions[3].ul.x)/2;
                   line_points[0].y=escalators_dimensions[3].lr.y;
                   line_points[1].y=line_points[0].y+
                               system_settings_spider::TOP_BOTTOM_MARGIN_OF_STATION/2+
                               system_settings_spider::POWER_LINE_WIDTH;
                   PtSetArg(&args[1], Pt_ARG_COLOR, color_input_2, 0);	
                   created_line=PtCreateWidget( PtLine,
                                        metro_device::get_station_widget(),
                                        args.size(),
                                        &args[0] );
                   if (created_line==NULL)
                      throw spider_exception("metro_shavr::update_device_widget() can`t create power line widget");     

                  PtRealizeWidget(created_line);
                  power_lines.push_back(created_line);
             }; //INDEX_SIGNAL_ESCALATOR_4_CONNECTED_TO_INPUT_2
		};// if (escalators_dimensions.size()>=4)
};

void metro_shavr::update_device_panel (metro_device_type *dev_type) throw (spider_exception){
system_settings_spider *sys_sett=system_settings_spider::get_instance();
	if (metro_device::paneled_device_id!=metro_device::get_id())
      throw spider_exception ("metro_escalator::update_device_panel(...) metro_device::paneled_device_id!=metro_device::get_id()");

    if (metro_device::device_panel==NULL)
          throw spider_exception ("metro_escalator::update_device_panel(...) metro_device::device_panel==NULL");

     metro_device_type::data_unit_iterator 
                    iter_data_unit=dev_type->data_units_begin();
     while (iter_data_unit!=dev_type->data_units_end()) {
          iter_data_unit->second->update_widget(&data_block,
                                                                         sys_sett,
                                                                         A0_x1 & A0_x2);
          iter_data_unit++;
     };

};



void metro_shavr::decode_answer_from_device_to_data_block(){
    metro_device::buffer_data_type answer_from_device=metro_device::get_answer_from_device_buffer();
    int old_A0_state=A0_state;
    bool  old_A0_x10=A0_x10,
               old_A0_x11=A0_x11,
               old_A0_x12=A0_x12,
              old_A0_x13=A0_x13,
              old_A0_x14=A0_x14,
              old_A0_x15=A0_x14,
              old_A0_x16=A0_x16,
              old_A0_x17=A0_x17;
//===========================================
/*
{
  if (answer_from_device.empty()) {
         cout<<"\nanswer form shavr is empty"<<endl;
  } else { //  if (answer_from_device.empty())
       vector<char> tmp_chars(20);
       for (vector<char>::size_type i =0 ; i<answer_from_device.size() ; i++) {
            itoa(answer_from_device[i], &tmp_chars[0], 16);
            cout<<"  0x"<<&tmp_chars[0];
            if (i==3 || (i>3 && i%8==0)) cout<<endl;
       }; //for (vector<char>::size_type i =0
  }; //  if (answer_from_device.empty())

};
*/
//=============================================
   try{
		if (answer_from_device.empty()) {
             if (metro_device::get_failures_count()>system_settings::RECONNECTS_TO_DEVICE_COUNT) {
		                  A0_x1=false;
                         metro_device::set_last_message_remote_id(-1);
                   } else {
                          metro_device::set_failures_count(metro_device::get_failures_count()+1);
                   };

			 } else { //if (answer_from_device.empty()
                  if (( answer_from_device[system_settings::MODBUS_FUNCTION_CODE_INDEX] & 0x80)
                        != 0) {
                         if (metro_device::get_failures_count()>system_settings::RECONNECTS_TO_DEVICE_COUNT ||
                              !A0_x1)  {//if shavr change state from offline to internal exception, it must be changed immediatly
                                   A0_x2=false;
                                   metro_device::set_failures_count(0);
                             } else {
                                   metro_device::set_failures_count(metro_device::get_failures_count()+1);
                              };
                      } else {
                            A0_x2=true;
                            metro_device::set_failures_count(0);
                            metro_device::set_last_message_remote_id(-1);
                     };

  	             A0_x1=true;

                  switch (answer_from_device[system_settings::MODBUS_FUNCTION_CODE_INDEX]) {
                    case 4 : {
                      decode_answer_from_device_4_function(answer_from_device);
           			  break;
           			}; //case 4
           			default : {
           			  break;
           			}; //default
                   }; //switch (answer_from_device[system_settings::MODBUS_FUNCTION_CODE_INDEX])
            }; // if (answer_from_device.empty()
           A0(7); // event 7 - data refreshed
           if (old_A0_state!=A0_state ||
               old_A0_x10!=A0_x10||
               old_A0_x11!=A0_x11||
               old_A0_x12!=A0_x12||
              old_A0_x13!=A0_x13 ||
              old_A0_x14!=A0_x14 ||
              old_A0_x15!=A0_x14 ||
              old_A0_x16!=A0_x16||
              old_A0_x17!=A0_x17 ) {
               A0_x5=false;
               update_device_widget();
               PtFlush();
              };

		       if (metro_device::get_id()==metro_device::paneled_device_id &&
           			metro_device::device_panel!=NULL &&
//if all ok metro_device::get_failures_count()==0 or failures count is too much metro_device::get_failures_count()>system_settings::RECONNECTS_TO_DEVICE_COUNT
//panel must be updated, otherwise (last answer is failure but count of failures less maximum) panel is NOT updated
                             (metro_device::get_failures_count()==0|| 
                              metro_device::get_failures_count()>system_settings::RECONNECTS_TO_DEVICE_COUNT)
                             ) {
                            metro_devices_types_container *metro_devices_types=metro_devices_types_container::get_instance();
                            if (metro_devices_types==NULL) 
                                            throw spider_exception("Can`t get instance of metro_devices_types_container");
                            metro_devices_types_container::iterator dev_type_iter=
                                              metro_devices_types->find(metro_device::get_type());
                          if (dev_type_iter!=metro_devices_types->end()) {
                                        update_device_panel(dev_type_iter->second);
                                                          } else {
                                                             system_settings_spider *sys_sett=system_settings_spider::get_instance();
                                                              if (sys_sett==NULL)
                                                                       cout<<"In metro_shavr::decode_answer_from_device_to_data_block device_id: Can`t get instance of system_settings_spider "<<endl;
                                                             ostringstream error_description;
                                                             error_description<<"In metro_shavr::decode_answer_from_device_to_data_block(...) not found device type id : "<<metro_device::get_type();
                                                             error_description<<" for device id "<<metro_device::get_id();
                                                             sys_sett->sys_message(system_settings::ERROR_MSG,  
                                                                                                           error_description.str());
                                                             return;
                                                          };
                    PtFlush();
                };

   } catch (spider_exception spr_exc) {
        system_settings_spider *sys_sett=system_settings_spider::get_instance();
       ostringstream exception_description;
       exception_description<<"In metro_shavr::decode_answer_from_device_to_data_block device_id "<<get_id()<<" : ";
       exception_description<<spr_exc.get_description();
       sys_sett->sys_message(system_settings::ERROR_MSG,
                                             exception_description.str());
   };

};

metro_device::buffer_data_type 
              metro_shavr::get_request_for_send_to_device() {
buffer_data_type return_buffer(0);
     return_buffer=metro_device::get_request_for_send_to_device_buffer();

  if(return_buffer.empty() && ! metro_device::is_must_set_time()) {
         return_buffer=get_4_function_request();
         metro_device::set_request_to_device_buffer(return_buffer);
   } else if (return_buffer.empty() && metro_device::is_must_set_time()) {
         return_buffer=get_setting_time_request();
         metro_device::time_already_setted();
         metro_device::set_request_to_device_buffer(return_buffer);
   };

metro_device::set_current_request_to_device_buffer(return_buffer);

return return_buffer;
};

metro_device::buffer_data_type metro_shavr::get_4_function_request(){
	vector<byte> buffer(0);

	buffer.push_back(get_modbus_number());   // shavr number
	buffer.push_back(4);                      // function number
	buffer.push_back(0);
	buffer.push_back(7);
	buffer.push_back(0);
	buffer.push_back(76);

	system_settings::bytes tmp_bytes=system_settings::bytes_of_type<word>(system_settings::crc(buffer));
	buffer.insert(buffer.end(), tmp_bytes.begin(), tmp_bytes.end());

//	metro_device::set_request_to_device_buffer(buffer);
  	return buffer;
};

metro_device::buffer_data_type metro_shavr::get_setting_time_request(){

system_settings_spider *sys_sett=system_settings_spider::get_instance();
	vector<byte> buffer(0);
    struct tm *tm_local;
    time_t time_now=time(NULL);

	tm_local=localtime(&time_now);

    try{
       buffer.push_back(get_modbus_number());   // shavr number
       buffer.push_back(16);                      // function number

       buffer.push_back(00);                      // first register number high byte
       buffer.push_back(00);                      // first register number low byte

       buffer.push_back(00);                      // registers count high byte
       buffer.push_back(03);                      // registers count low byte

       buffer.push_back(6); //data bytes count

       buffer.push_back(system_settings::decode_to_binary_decimal_notation(static_cast<byte>(tm_local->tm_mon+1)));
       buffer.push_back(system_settings::decode_to_binary_decimal_notation(static_cast<byte>(tm_local->tm_mday)));
      if ((tm_local->tm_wday-1)<0) {
           buffer.push_back(6);
          } else {
           buffer.push_back(static_cast<byte>(tm_local->tm_wday-1));
        };

        buffer.push_back(system_settings::decode_to_binary_decimal_notation(static_cast<byte>(tm_local->tm_hour)));
        buffer.push_back(system_settings::decode_to_binary_decimal_notation(static_cast<byte>(tm_local->tm_min)));
        buffer.push_back(system_settings::decode_to_binary_decimal_notation(static_cast<byte>(tm_local->tm_sec)));

	    system_settings::bytes tmp_bytes=system_settings::bytes_of_type<word>(system_settings::crc(buffer));
        buffer.insert(buffer.end(), tmp_bytes.begin(), tmp_bytes.end());
      } catch (spider_exception spr_except) {
         sys_sett->sys_message(system_settings::ERROR_MSG,
                                                "In metro_shavr::get_setting_time_request() raised exception "+spr_except.get_description());
         buffer=get_4_function_request();
      };

//	metro_device::set_request_to_device_buffer(buffer);
   	return buffer;
};

void send_datablock_to_door(metro_devices_container::iterator& devices_iter, byte data_to_send);

void 
  metro_shavr::decode_answer_from_device_4_function
           (metro_device::buffer_data_type answer) throw (spider_exception){
   system_settings_spider *sys_sett_obj=system_settings_spider::get_instance();
   metro_devices_types_container *dev_types_cont=metro_devices_types_container::get_instance();
   msg_dict_container *local_msg_dict;
   metro_devices_types_container::iterator dev_types_iter;
   msg_dict_container::iterator msg_cont_iter;

   vector<log_record> received_messages;

   shavr_data_block local_data_block=this->data_block;
   word begin_addr, registers_count,
            register_value, register_counter, mask_value,
            answer_messages_size=0, answer_last_message_remote_id=0;

  if (answer.empty())
        throw spider_exception ("Empty answer for decoding in metro_shavr::decode_answer_from_device_4_function(...)");

   bool tmp_value;

   metro_device::buffer_data_type::iterator tmp_iter1, tmp_iter2;
   metro_device::buffer_data_type request_to_device = metro_device::get_current_request_to_device_buffer();

  if (request_to_device.empty())
        throw spider_exception ("Empty request for decoding in metro_shavr::decode_answer_from_device_4_function(...)");

   tmp_iter1=request_to_device.begin();
   tmp_iter2=tmp_iter1;
   advance(tmp_iter1,
              system_settings::MODBUS_FUNCTION_4_REQUEST_BEGIN_ADDRESS_INDEX);
   advance(tmp_iter2,
              (system_settings::MODBUS_FUNCTION_4_REQUEST_BEGIN_ADDRESS_INDEX+
                 system_settings::MODBUS_FUNCTION_4_REQUEST_BEGIN_ADDRESS_INCREM));
   begin_addr=system_settings::type_from_bytes<word>(system_settings::bytes(tmp_iter1, tmp_iter2));
   begin_addr+=shavr_data_block::MODBUS_INPUT_REGISTERS_BEGIN_ADDRESS;

  tmp_iter1=request_to_device.begin();
  tmp_iter2=tmp_iter1;
   advance(tmp_iter1,
                  system_settings::MODBUS_FUNCTION_4_REQUEST_REGISTERS_COUNT_INDEX);
   advance(tmp_iter2,
                 (system_settings::MODBUS_FUNCTION_4_REQUEST_REGISTERS_COUNT_INDEX+
                  system_settings::MODBUS_FUNCTION_4_REQUEST_REGISTERS_COUNT_INCREM));
   registers_count=
                   system_settings::type_from_bytes<word>(system_settings::bytes(tmp_iter1, tmp_iter2));

	if (registers_count*2 !=answer[system_settings::MODBUS_DATA_BYTES_COUNT_INDEX]) {
        ostringstream exception_description;
        exception_description<<"In metro_shavr::decode_answer_from_device_4_function device id "<<get_id();
        exception_description<<" recieved "<<static_cast<int>(answer[system_settings::MODBUS_DATA_BYTES_COUNT_INDEX])<<" bytes of data ";
        exception_description<<" but must "<<static_cast<int>(registers_count*2);
        if (answer.size()>0)
             exception_description<<" first three bytes 1="<<static_cast<int>(answer[0])<<"  2="<<static_cast<int>(answer[1])<<" 3="<<static_cast<int>(answer[1]);
        throw spider_exception(exception_description.str());
	};

//====================================================
/*
if (get_id()==31){
if ( answer.empty()) {
cout<<"answer to shavr is EMPTY"<<endl;
 } else { //if ( answer.empty())
cout<<"\n metro_shavr::decode_answer_from_device_4_function"<<endl;
vector<char> tmp_chars(20);
for (vector<byte>::size_type i=0; i<answer.size(); i++) {
       itoa(answer[i], &tmp_chars[0], 16);
       cout<<" 0x"<<&tmp_chars[0];
       if (i==3||(i>3 && i%8==0)) cout<<endl;
  }; //for (vector<byte>::size_type i=0
}; // if ( answer.empty())
};
*/
//====================================================

//move iteratiors to first register data
     tmp_iter1=answer.begin();
     tmp_iter2=tmp_iter1;

     advance(tmp_iter1,
                 system_settings::MODBUS_HEADER_SIZE);
     advance(tmp_iter2,
                 (system_settings::MODBUS_HEADER_SIZE+2));

//poehali!
     if (begin_addr<= 30008 &&
         begin_addr+registers_count>=30008) {
			register_value=
			    sys_sett_obj->type_from_bytes<word>(system_settings::bytes ( tmp_iter1, tmp_iter2));

           for (int bits_shift=0; bits_shift<16; bits_shift++) {
               mask_value=1<<bits_shift;
		          if ((register_value & mask_value)==0) {
		               local_data_block.set_signal_value(bits_shift+1,
		                                             shavr_data_block::SIGNAL_VALUE_BLUE);
                       tmp_value=false;
		            } else { //if ((register_value & mask_value)
		               local_data_block.set_signal_value(bits_shift+1,
		                                             shavr_data_block::SIGNAL_VALUE_GREEN);
                       tmp_value=true;
		            }; //if ((register_value & mask_value)

		            switch (bits_shift) {
               	       case shavr_data_block::INDEX_SIGNAL_ESCALATOR_1_CONNECTED_TO_INPUT_1:
                           A0_x10=tmp_value;
               	           break;
               	       case shavr_data_block::INDEX_SIGNAL_ESCALATOR_1_TELEMETR_INPUT_1:
                           A0_x18=tmp_value;
               	           break;
               	       case shavr_data_block::INDEX_SIGNAL_ESCALATOR_1_CONNECTED_TO_INPUT_2:
                           A0_x11=tmp_value;
               	           break;
               	       case shavr_data_block::INDEX_SIGNAL_ESCALATOR_1_TELEMETR_INPUT_2:
                           A0_x19=tmp_value;
               	           break;
             	       case shavr_data_block::INDEX_SIGNAL_ESCALATOR_2_CONNECTED_TO_INPUT_1:
                           A0_x12=tmp_value;
               	           break;
               	       case shavr_data_block::INDEX_SIGNAL_ESCALATOR_2_TELEMETR_INPUT_1:
                           A0_x20=tmp_value;
               	           break;
               	       case shavr_data_block::INDEX_SIGNAL_ESCALATOR_2_CONNECTED_TO_INPUT_2:
                           A0_x13=tmp_value;
               	           break;
               	       case shavr_data_block::INDEX_SIGNAL_ESCALATOR_2_TELEMETR_INPUT_2:
                           A0_x21=tmp_value;
               	           break;
               	       case shavr_data_block::INDEX_SIGNAL_ESCALATOR_3_CONNECTED_TO_INPUT_1:
                           A0_x14=tmp_value;
               	           break;
             	       case shavr_data_block::INDEX_SIGNAL_ESCALATOR_3_TELEMETR_INPUT_1:
                           A0_x22=tmp_value;
               	           break;
               	       case shavr_data_block::INDEX_SIGNAL_ESCALATOR_3_CONNECTED_TO_INPUT_2:
                           A0_x15=tmp_value;
               	           break;
               	       case shavr_data_block::INDEX_SIGNAL_ESCALATOR_3_TELEMETR_INPUT_2:
                           A0_x23=tmp_value;
               	           break;
               	       case shavr_data_block::INDEX_SIGNAL_ESCALATOR_4_CONNECTED_TO_INPUT_1:
                           A0_x16=tmp_value;
               	           break;
               	       case shavr_data_block::INDEX_SIGNAL_ESCALATOR_4_TELEMETR_INPUT_1:
                           A0_x24=tmp_value;
               	           break;
               	       case shavr_data_block::INDEX_SIGNAL_ESCALATOR_4_CONNECTED_TO_INPUT_2:
                           A0_x17=tmp_value;
               	           break;
               	       case shavr_data_block::INDEX_SIGNAL_ESCALATOR_4_TELEMETR_INPUT_2:
                           A0_x25=tmp_value;
               	           break;
		            }; // switch (bits_shift)
              }; // for (int bits_shift

            tmp_iter1=tmp_iter2;
            advance(tmp_iter2, 2);
    }; //if (begin_addr<= 30008

     register_counter=16;
     if (begin_addr<= 30009 &&
         begin_addr+registers_count>=30009) {
			register_value=
			    sys_sett_obj->type_from_bytes<word>(system_settings::bytes (tmp_iter1, tmp_iter2));

           for (int bits_shift=0; bits_shift<16; bits_shift++) {
               mask_value=1<<bits_shift;

                      if ((register_value & mask_value)==0) {
                                tmp_value=false;
                            } else { // if ((register_value & mask_value
                                tmp_value=true;
		                  }; // if ((register_value & mask_value

		            switch (bits_shift) {
                      case shavr_data_block::INDEX_SIGNAL_SMOKE_SENSOR :
//ATTENTION!! Add code for smoke sensor alarm here - sound and message window
//                         A2_x3=tmp_value;
                         if (tmp_value) {
		                        local_data_block.set_signal_value(bits_shift+register_counter+1,
		                                                   shavr_data_block::SIGNAL_VALUE_GREEN);
                            } else { // if (tmp_value)
 		                        local_data_block.set_signal_value(bits_shift+register_counter+1,
		                                            shavr_data_block::SIGNAL_VALUE_RED);
                            }; //if (tmp_value)

                         break;
                      case shavr_data_block::INDEX_SIGNAL_FIRE_ALARM :
//ATTENTION!! Add code for fire alarm here - sound and message window
//                         A2_x3=tmp_value;
                         if (tmp_value) {
		                        local_data_block.set_signal_value(bits_shift+register_counter+1,
		                                                   shavr_data_block::SIGNAL_VALUE_GREEN);
                            } else { // if (tmp_value)
 		                        local_data_block.set_signal_value(bits_shift+register_counter+1,
		                                            shavr_data_block::SIGNAL_VALUE_RED);
                            }; //if (tmp_value)
                         break;
                      case shavr_data_block::INDEX_SIGNAL_380V_ON_INPUT_1:
                         A0_x3=tmp_value;
                         if (tmp_value) {
		                        local_data_block.set_signal_value(bits_shift+register_counter+1,
		                                                   shavr_data_block::SIGNAL_VALUE_GREEN);
                            } else { // if (tmp_value)
 		                        local_data_block.set_signal_value(bits_shift+register_counter+1,
		                                            shavr_data_block::SIGNAL_VALUE_BLUE);
                            }; //if (tmp_value)
	                     break;
                      case shavr_data_block::INDEX_SIGNAL_380V_ON_INPUT_2:
                         A0_x4=tmp_value;
                         if (tmp_value) {
		                        local_data_block.set_signal_value(bits_shift+register_counter+1,
		                                                   shavr_data_block::SIGNAL_VALUE_GREEN);
                            } else { // if (tmp_value)
 		                        local_data_block.set_signal_value(bits_shift+register_counter+1,
		                                            shavr_data_block::SIGNAL_VALUE_BLUE);
                            }; //if (tmp_value)
	                     break;
                      case shavr_data_block::INDEX_SIGNAL_220V_UPS_OUTPUT:
                         if (tmp_value) {
		                        local_data_block.set_signal_value(bits_shift+register_counter+1,
		                                                   shavr_data_block::SIGNAL_VALUE_GREEN);
                            } else { // if (tmp_value)
 		                        local_data_block.set_signal_value(bits_shift+register_counter+1,
		                                            shavr_data_block::SIGNAL_VALUE_BLUE);
                            }; //if (tmp_value)
	                     break;
                      case shavr_data_block::INDEX_SIGNAL_220V_OUTPUT_1:
                         if (tmp_value) {
		                        local_data_block.set_signal_value(bits_shift+register_counter+1,
		                                                   shavr_data_block::SIGNAL_VALUE_GREEN);
                            } else { // if (tmp_value)
 		                        local_data_block.set_signal_value(bits_shift+register_counter+1,
		                                            shavr_data_block::SIGNAL_VALUE_BLUE);
                            }; //if (tmp_value)
	                     break;
                      case shavr_data_block::INDEX_SIGNAL_220V_OUTPUT_2:
                         if (tmp_value) {
		                        local_data_block.set_signal_value(bits_shift+register_counter+1,
		                                                   shavr_data_block::SIGNAL_VALUE_GREEN);
                            } else { // if (tmp_value)
 		                        local_data_block.set_signal_value(bits_shift+register_counter+1,
		                                            shavr_data_block::SIGNAL_VALUE_BLUE);
                            }; //if (tmp_value)
	                     break;
                      case shavr_data_block::INDEX_SIGNAL_DOOR_OPENED:

                         if (tmp_value) {
		                        local_data_block.set_signal_value(bits_shift+register_counter+1,
		                                                   shavr_data_block::SIGNAL_VALUE_RED);
                            } else { // if (tmp_value)
 		                        local_data_block.set_signal_value(bits_shift+register_counter+1,
		                                            shavr_data_block::SIGNAL_VALUE_BLUE);
                            }; //if (tmp_value)


//mashzal door
		if (data_block.get_signal_value(bits_shift+register_counter+1)!=
			local_data_block.get_signal_value(bits_shift+register_counter+1)) {
			metro_stations_container *stations=metro_stations_container::get_instance();
			metro_stations_container::iterator stations_iter;
			metro_devices_container *devices=metro_devices_container::get_instance();
			metro_devices_container::iterator devices_iter;
			metro_device::buffer_data_type data_to_door;
			system_settings::bytes tmp_bytes;

			 byte door_data_to_send = 1;
			 if (tmp_value)  {
				door_data_to_send = 0;
			};

			if (stations!=NULL && devices!=NULL) {
				if (door_id <0) { //this case when no info added for door
					stations_iter=stations->find(metro_device::get_station_id());
					if (stations_iter!=stations->end()) {
						metro_station::iterator_devices_id station_devices_ids=stations_iter->second.begin_devices_id();
						while(station_devices_ids!=stations_iter->second.end_devices_id()) {
							devices_iter=devices->find(*station_devices_ids);
							send_datablock_to_door(devices_iter, door_data_to_send);
							station_devices_ids++;
						}; //while(station_devices_ids!=stations_iter->second->end_devices_id()
					}; //if (stations_iter=stations->end())
				} else {//if (door_id <0) {
					devices_iter=devices->find(door_id);
					send_datablock_to_door(devices_iter, door_data_to_send);
				};//if (door_id <0)
			}; //if (stations!=NULL)
		}; //if (data_block.get_signal_value(escalator_data_block::INDEX_SIGNAL_MASHZAL_DOOR)!=

	                     break;
                      case shavr_data_block::INDEX_SIGNAL_POWER_SUPPLY_24V:
                         if (tmp_value) {
		                        local_data_block.set_signal_value(bits_shift+register_counter+1,
		                                                   shavr_data_block::SIGNAL_VALUE_GREEN);
                            } else { // if (tmp_value)
 		                        local_data_block.set_signal_value(bits_shift+register_counter+1,
		                                            shavr_data_block::SIGNAL_VALUE_BLUE);
                            }; //if (tmp_value)
	                     break;
		            }; // switch (bits_shift)
              }; // for(int bits_shift=0; bit

    }; //if (begin_addr<= 30009&&

	for(int register_counter=0;
	      register_counter<32;
	      register_counter++) {

       if (begin_addr<= 30010+register_counter*2 &&
            begin_addr+registers_count>=30010+register_counter*2) {
               tmp_iter1=tmp_iter2;
               advance(tmp_iter2, 4);
               local_data_block.set_parameter_value(register_counter+1,
                         sys_sett_obj->type_from_bytes<dword>(system_settings::bytes (tmp_iter1, tmp_iter2)));
        } else { // if (begin_addr<= 300010+register_counter*2 &&
             break;
        };
	 }; // for(int register_counter=0;

    //messages
//    cout<<"current distance "<<distance(answer.rbegin(), tmp_iter2)<<endl;

     if (begin_addr<= 30074&&
         begin_addr+registers_count>30074) {
             tmp_iter1=tmp_iter2;
             advance(tmp_iter2, 2);
            answer_last_message_remote_id=system_settings::type_from_bytes<word>( system_settings::bytes (tmp_iter1, tmp_iter2) );
           if (answer_last_message_remote_id>system_settings::MAX_UPPER_MESSAGE_ID_VALUE)
                    throw spider_exception ("In answer 3074 register answer_last_message_remote_id>system_settings::MAX_UPPER_MESSAGE_ID_VALUE");
    };

     if (begin_addr<= 30075&&
         begin_addr+registers_count>30075) {
             tmp_iter1=tmp_iter2;
             advance(tmp_iter2, 2);
            answer_messages_size=system_settings::type_from_bytes<word>( system_settings::bytes (tmp_iter1, tmp_iter2) );
           if (answer_messages_size>system_settings::MAX_MESSAGES_COUNT)
                    throw spider_exception ("In answer 3075 register answer_last_message_remote_id>system_settings::MAX_MESSAGES_COUNT");
    };

//messages
    //in initial state get_last_message_remote_id()==0, so all messages must be skiped
    int upper_last_id=metro_device::get_last_message_remote_id();
    int answer_first_message_remote_id, current_message_id=0;

     if (upper_last_id<0) { //decode first packet after program starting
               metro_device::set_last_message_remote_id(answer_last_message_remote_id);
               upper_last_id=answer_last_message_remote_id;
              };

     answer_first_message_remote_id=answer_last_message_remote_id-answer_messages_size+1;
     if (answer_first_message_remote_id<0)
           answer_first_message_remote_id+=system_settings::MAX_UPPER_MESSAGE_ID_VALUE;

     word current_register=30076;
     word register_data_word;
     ldword message_id;
     int message_type_id=0;
     string current_message_text("");
     dev_types_iter=dev_types_cont->find(get_type());
     if (dev_types_iter==dev_types_cont->end()) {
                ostringstream message;
                message<<"Not found device type with id "<<get_type()<<" for device with id "<<get_id();
                throw spider_exception(message.str() );
          };
    local_msg_dict=&(dev_types_iter->second->type_messages);

	 for (byte register_offset=0; register_offset<answer_messages_size; register_offset++){
            if (begin_addr+registers_count<current_register+register_offset) break;

           register_data_word=system_settings::type_from_bytes<word>(system_settings::bytes (tmp_iter1, tmp_iter2) );

            current_message_id=answer_first_message_remote_id+register_offset;
            if (current_message_id>system_settings::MAX_UPPER_MESSAGE_ID_VALUE)
                   current_message_id-=system_settings::MAX_UPPER_MESSAGE_ID_VALUE;

            if (answer_first_message_remote_id<=answer_last_message_remote_id &&
                answer_first_message_remote_id<=upper_last_id &&
                upper_last_id<=answer_last_message_remote_id &&
                current_message_id<=upper_last_id)  {
                     tmp_iter1=tmp_iter2;
                     advance(tmp_iter2, 2);
                     continue;
                  };

            if (answer_first_message_remote_id>answer_last_message_remote_id &&
                answer_first_message_remote_id>=upper_last_id &&
                upper_last_id<=answer_last_message_remote_id &&
                ((current_message_id>answer_last_message_remote_id  && current_message_id>=answer_first_message_remote_id)||
                current_message_id<=upper_last_id )) {
                     tmp_iter1=tmp_iter2;
                     advance(tmp_iter2, 2);
                     continue;
                  };

            if (answer_first_message_remote_id>answer_last_message_remote_id &&
                answer_first_message_remote_id<=upper_last_id &&
                upper_last_id>answer_last_message_remote_id &&
                current_message_id>=answer_last_message_remote_id &&
                current_message_id<=upper_last_id) {
                   tmp_iter1=tmp_iter2;
                   advance(tmp_iter2, 2);
                   continue;
                };

            message_type_id=0;
            message_id=register_data_word;//in second byte may be block circut id or other information
            ldword tmp_id=main_log->get_records_autoincrement();
            main_log->set_records_autoincrement(++tmp_id); 
        //resolwing message type id
        //by first by escalator type messages container
				msg_cont_iter=local_msg_dict->find(message_id&0xff);
				if(msg_cont_iter!=local_msg_dict->end()) {
  				       message_type_id=msg_cont_iter->second.get_type();
                        current_message_text=msg_cont_iter->second.get_text ();
                      };

        //if message_type_id still equal to 0 try resolw message type id in global messages container
            if (message_type_id==0) {
				msg_cont_iter=messages->find(message_id&0xff);
				if(msg_cont_iter!=messages->end()) {
  				       message_type_id=msg_cont_iter->second.get_type();
                        current_message_text=msg_cont_iter->second.get_text ();
                       } else { // if(msg_cont_iter!=msg_dict->end())  if message type id not found, raise exception
                        ostringstream message;
                        message <<"Device id "<<get_id()<<": not found message type for message with id "<<(message_id&0xff);
                        throw spider_exception(message.str());
                      }; //if(msg_cont_iter!=msg_dict->end())
               }; // if (message_type_id==0)

                    received_messages.push_back(log_record(tmp_id,
                                                            message_id,
                                                            message_type_id,
                                                            get_station_id(),
                                                            get_id(),
                                                            time(NULL)));
		           // NOTICE: time(NULL) must be replaced by time from escalator

                  metro_device::set_last_message_remote_id(current_message_id);
                  tmp_iter1=tmp_iter2;
                  advance(tmp_iter2, 2);
	 }; // for (byte register_offset=0; register_offset<

   this->data_block=local_data_block;

//inserting to main log
  if (!received_messages.empty()) {
        vector<log_record>::iterator log_mess_iter=received_messages.begin();
         while(log_mess_iter!=received_messages.end()) {
                 main_log->insert(*log_mess_iter);
                 log_mess_iter++;
               }; // while(log_mess_iter==received_messages.rend())
           main_log->prepare_to_display();
       }; // if (!received_messages.empty())

//alerts
  if (!received_messages.empty()) {
        ostringstream message;
        string message_from_messages_container;
        int find_pos;
        vector<char> tmp_chars;
        vector<log_record>::reverse_iterator log_mess_iter=received_messages.rbegin();
        dword breaking_path_value;
        string block_circut_name;
        metro_stations_container *stations=metro_stations_container::get_instance();
        metro_stations_container::iterator stat_iter;

        if (stations==NULL)
                  throw spider_exception("Can`t get instance of metro_stations_container");
         stat_iter=stations->find(metro_device::get_station_id());
         if (stat_iter==stations->end()) {
                   message<<"Not found station with id "<<metro_device::get_station_id()<<" for device "<<metro_device::get_id()<<endl;
                   throw spider_exception(message.str());
                   };

         while(log_mess_iter!=received_messages.rend()) {
                 if (log_mess_iter->get_msg_type()==escalator_data_block::ALERT_ERROR_MESSAGES_TYPE) {
                     message.str("Станция ");
                     message_from_messages_container="";

                     message<<stat_iter->second.get_stl_name_string()<<" ШАВР "<<metro_device::get_number()<<"\n";

                    system_settings::strings_container parameters_names=
                                           sys_sett_obj->get_parameters_in_messages_strings();
                 	 dev_types_iter=dev_types_cont->find(metro_device::get_type());
                      if (dev_types_iter==dev_types_cont->end()) {
                            message.str("");
                            message<<"In alerts generation not found device type with id "<<metro_device::get_type()<<" for device "<<metro_device::get_id();
                            throw spider_exception(message.str());
                      }; //if (dev_types_iter==dev_types_cont.end())

                     msg_cont_iter=dev_types_iter->second->type_messages.find(static_cast<byte>(0xff&log_mess_iter->get_msg_id()));
                     if (msg_cont_iter==dev_types_iter->second->type_messages.end()) {
                              msg_cont_iter=messages->find(static_cast<byte>(0xff&log_mess_iter->get_msg_id()));
                               if (msg_cont_iter==messages->end()) {
                                      message.str("");
                                      message<<"In alerts generation not found message with id "<<log_mess_iter->get_msg_type();
                                      throw spider_exception(message.str());
                                 }; //if (msg_cont_iter==messages->end()
                        }; //if (msg_cont_iter==type_messages->end()
                      message_from_messages_container=msg_cont_iter->second.get_text();
//---------------------------------begin message text generation------------------------------------
                    find_pos=static_cast<int>(message_from_messages_container.find(parameters_names[system_settings::PARAMETER_NAME_BREAKING_PATH_VALUE]));
                    if (find_pos!=-1) {
                      breaking_path_value=static_cast<dword>(log_mess_iter->get_msg_id()>>32);
                      tmp_chars.clear();
                      tmp_chars.resize(12);
                      itoa (breaking_path_value, &tmp_chars[0], 10);
                      message_from_messages_container.replace( find_pos,
														   parameters_names[system_settings::PARAMETER_NAME_BREAKING_PATH_VALUE].size(),
														   &tmp_chars[0]);
                    };//if (find_pos!=-1)
/*

//shavr have not block circuts
                    find_pos=static_cast<int>(message_from_messages_container.find(parameters_names[system_settings::PARAMETER_NAME_BLOCK_CIRCUT_NAME]));
                    if (find_pos!=-1) {
                      block_circut_index=static_cast<byte>((log_mess_iter->get_msg_id()&0xff00)>>8);
                      block_circut_index+=6; //shift for signals, added from registers 3004, 3005,3006 and other
                      if (block_circut_index==0) {
                                block_circut_name="ERROR!!";
                                message.str("");
                                message<<"Present PARAMETER_NAME_BLOCK_CIRCUT_NAME but block_circut_index==0 message id in log"<<log_mess_iter->get_msg_id();
                                throw spider_exception(message.str());
                           } else { // if (block_circut_index==0)
                               type_data_unit_iter=dev_types_iter->second->data_units_find(block_circut_index);
                             if (type_data_unit_iter==dev_types_iter->second->data_units_end()) {
                                          block_circut_name="ERROR!!";
                                          ostringstream mess;
                                          mess<<"Present PARAMETER_NAME_BLOCK_CIRCUT_NAME but not found block_circut_index "
                                                   <<block_circut_index<<" message id in log"<<current_message_id;
                                           throw spider_exception(mess.str());
                                           };
                               //ATENTION:index of block circut equals to id, so send to find block circut index
                             if (type_data_unit_iter->second->get_index()==block_circut_index) {
                                           block_circut_name=type_data_unit_iter->second->get_name();
                                  } else { //if (type_data_unit_iter.second.get_index()==block_circut_index)
                                       block_circut_name="ERROR!!";
                                       message.str("");
                                       message<<"Present PARAMETER_NAME_BLOCK_CIRCUT_NAME but type_data_unit_iter->second->get_index()!=block_circut_index block_circut_index "
                                                              <<block_circut_index<<" message id in log"<<log_mess_iter->get_msg_id();
                                       throw spider_exception(message.str());
                                 }; //if (type_data_unit_iter.second.get_index()==block_circut_index)
                           }; //if (block_circut_index==0)

                      message_from_messages_container.replace( find_pos,
														   parameters_names[system_settings::PARAMETER_NAME_BLOCK_CIRCUT_NAME].size(),
														   block_circut_name);
                    };//if (static_cast<int>(message_text.find(parameters_names[system_settings::PARAMETER_NAME_
*/
//-----------------------------------end message text generation-----------------------------------
                       message<<message_from_messages_container;
                        sys_sett_obj->message_window(system_settings::ERROR_MSG,
                                                                       message.str(),
                                                                        true);
					 sound& snd=sound::get_instance();
					std::vector<string> *files  = new std::vector<string> ();
					files->push_back(stat_iter->second.get_wav_file_name());
                     snd.play(files);
                 }; // if (log_mess_iter->get_msg_type()==escalator_data_block::ALERT_ERROR_MESSAGES_TYPE)
                 log_mess_iter++;
               }; // while(log_mess_iter==received_messages.rend())
       }; // if (!received_messages.empty())

};

command 
     metro_shavr::get_device_start_command() throw(spider_exception) {
  throw spider_exception("shavr not support start command");
}

vector<command> 
     metro_shavr::get_appropriated_commands()  throw (spider_exception){
 vector<command> commands_to_return;
vector<bool> escalators_running_state;
escalators_running_state.insert(escalators_running_state.end(),4, true);

system_settings_spider *sys_sett=system_settings_spider::get_instance();
if (sys_sett==NULL) 
        throw spider_exception ("In metro_shavr::get_appropriated_commands() can`t get instance of system_settings_spider");

//cout<<"in metro_shavr::get_appropriated_commands"<<endl;
//cout<<"A0_x10 "<<(A0_x10?"true":"false")<<endl;
//cout<<"A0_x11 "<<(A0_x11?"true":"false")<<endl;
//cout<<"A0_x12 "<<(A0_x12?"true":"false")<<endl;
//cout<<"A0_x13 "<<(A0_x13?"true":"false")<<endl;
//cout<<"A0_x14 "<<(A0_x14?"true":"false")<<endl;
//cout<<"A0_x15 "<<(A0_x15?"true":"false")<<endl;
//cout<<"A0_x16 "<<(A0_x16?"true":"false")<<endl;
//cout<<"A0_x17 "<<(A0_x17?"true":"false")<<endl;
//cout<<"A0_x18 "<<(A0_x18?"true":"false")<<endl;
//cout<<"A0_x19 "<<(A0_x19?"true":"false")<<endl;
//cout<<"A0_x20 "<<(A0_x20?"true":"false")<<endl;
//cout<<"A0_x21 "<<(A0_x21?"true":"false")<<endl;
//cout<<"A0_x22 "<<(A0_x22?"true":"false")<<endl;
//cout<<"A0_x23 "<<(A0_x23?"true":"false")<<endl;
//cout<<"A0_x24 "<<(A0_x24?"true":"false")<<endl;
//cout<<"A0_x25 "<<(A0_x25?"true":"false")<<endl;

metro_devices_container *devices=metro_devices_container::get_instance();
metro_devices_container::iterator devices_iter;
if (devices==NULL) 
          throw spider_exception ("In metro_shavr::get_appropriated_commands() can`t get instance of metro_devices_container");

related_devices_ids::iterator related_devices_iter=devices_ids_begin();
metro_escalator *escalator_in_shavr;
byte count=0;
  while(related_devices_iter!=devices_ids_end()) {
        devices_iter=devices->find(*related_devices_iter);
        if (devices_iter!=devices->end()) {
               if (devices_iter->second->get_type_description()==metro_device_type::DEVICE_TYPE_ESCALATOR) {
                        try {
                           escalator_in_shavr=dynamic_cast<metro_escalator*>(devices_iter->second);
//                           escalators_running_state[count]=escalator_in_shavr->is_escalator_run();
                           escalators_running_state[count]=false;
                        }catch (bad_cast){
                             //do nothing
                        };
                     };
            } else {
               ostringstream message;
               message<<"In metro_shavr::get_appropriated_commands() not found device with id "<<*related_devices_iter;
               throw spider_exception(message.str());
            };
        count++;
        related_devices_iter++;
     };

 if (!escalators_running_state[0] &&
       A0_x10 &&
       A0_x18) // escalator 1 connected to input 1 and connection of escalator 1 to input 1 can be switched by telemetry
     commands_to_return.push_back(
 	        command (metro_device::get_id(),
	                 metro_device::get_type(),
	                 system_settings::TURN_OFF_ESC_1_TO_INPUT_1,
	                 metro_device::get_station_id(),
	                 metro_device::get_modbus_number(),
                     "ОТКЛЮЧИТЬ Эск 1 от ввода 1")
              );

 if (!escalators_running_state[0] &&
       A0_x11 &&
       A0_x19) // escalator 1 connected to input 2 and connection of escalator 1 to input 2 can be switched by telemetry
     commands_to_return.push_back(
 	        command (metro_device::get_id(),
	                 metro_device::get_type(),
	                 system_settings::TURN_OFF_ESC_1_TO_INPUT_2,
	                 metro_device::get_station_id(),
	                 metro_device::get_modbus_number(),
                     "ОТКЛЮЧИТЬ Эск 1 от ввода 2")
              );

 if (!escalators_running_state[0] &&
      !A0_x10 &&
      !A0_x11 &&
       A0_x18) // escalator 1 dsconnected from input 1 and disconnected from input 2 and connection of escalator 1 to input 1 can be switched by telemetry
     commands_to_return.push_back(
 	        command (metro_device::get_id(),
	                 metro_device::get_type(),
	                 system_settings::TURN_ON_ESC_1_TO_INPUT_1,
	                 metro_device::get_station_id(),
	                 metro_device::get_modbus_number(),
                     "ПОДКЛЮЧИТЬ Эск 1 к вводу 1")
              );

 if (!escalators_running_state[0] && 
      !A0_x10 && 
      !A0_x11 && 
       A0_x19) // escalator 1 dsconnected from input 1 and disconnected from input 2 and connection of escalator 1 to input 2 can be switched by telemetry
     commands_to_return.push_back(
 	        command (metro_device::get_id(),
	                 metro_device::get_type(),
	                 system_settings::TURN_ON_ESC_1_TO_INPUT_2,
	                 metro_device::get_station_id(),
	                 metro_device::get_modbus_number(),
                     "ПОДКЛЮЧИТЬ Эск 1 к вводу 2")
              );

 if ( !escalators_running_state[1] &&
       A0_x12 &&
       A0_x20) // escalator 2 connected to input 1 and connection of escalator 2 to input 1 can be switched by telemetry
     commands_to_return.push_back(
 	        command (metro_device::get_id(),
	                 metro_device::get_type(),
	                 system_settings::TURN_OFF_ESC_2_TO_INPUT_1,
	                 metro_device::get_station_id(),
	                 metro_device::get_modbus_number(),
                     "ОТКЛЮЧИТЬ Эск 2 от ввода 1")
              );
 if (!escalators_running_state[1] &&
      A0_x13 &&
      A0_x21)  // escalator 2 connected to input 2 and connection of escalator 2 to input 2 can be switched by telemetry
     commands_to_return.push_back(
 	        command (metro_device::get_id(),
	                 metro_device::get_type(),
	                 system_settings::TURN_OFF_ESC_2_TO_INPUT_2,
	                 metro_device::get_station_id(),
	                 metro_device::get_modbus_number(),
                     "ОТКЛЮЧИТЬ Эск 2 от ввода 2")
              );

 if (!escalators_running_state[1] &&
      !A0_x12 &&
      !A0_x13 &&
       A0_x20) // escalator 2 dsconnected from input 1 and disconnectet from input 2 and connection of escalator 2 to input 1 can be switched by telemetry
     commands_to_return.push_back(
 	        command (metro_device::get_id(),
	                 metro_device::get_type(),
	                 system_settings::TURN_ON_ESC_2_TO_INPUT_1,
	                 metro_device::get_station_id(),
	                 metro_device::get_modbus_number(),
                     "ПОДКЛЮЧИТЬ Эск 2 к вводу 1")
              );

 if (!escalators_running_state[1] &&
      !A0_x12 &&
      !A0_x13 &&
       A0_x21) // escalator 2 dsconnected from input 1 and disconnectet from input 2 and connection of escalator 2 to input 2 can be switched by telemetry
     commands_to_return.push_back(
 	        command (metro_device::get_id(),
	                 metro_device::get_type(),
	                 system_settings::TURN_ON_ESC_2_TO_INPUT_2,
	                 metro_device::get_station_id(),
	                 metro_device::get_modbus_number(),
                     "ПОДКЛЮЧИТЬ Эск 2 к вводу 2")
              );


 if (!escalators_running_state[2] &&
     A0_x14 &&
     A0_x22) // escalator 3 connected to input 1 and connection of escalator 3 to input 1 can be switched by telemetry
     commands_to_return.push_back(
 	        command (metro_device::get_id(),
	                 metro_device::get_type(),
	                 system_settings::TURN_OFF_ESC_3_TO_INPUT_1,
	                 metro_device::get_station_id(),
	                 metro_device::get_modbus_number(),
                     "ОТКЛЮЧИТЬ Эск 3 от ввода 1")
              );
 if (!escalators_running_state[2] &&
       A0_x15 &&
       A0_x23)  // escalator 3 connected to input 2 and connection of escalator 3 to input 2 can be switched by telemetry
     commands_to_return.push_back(
 	        command (metro_device::get_id(),
	                 metro_device::get_type(),
	                 system_settings::TURN_OFF_ESC_3_TO_INPUT_2,
	                 metro_device::get_station_id(),
	                 metro_device::get_modbus_number(),
                     "ОТКЛЮЧИТЬ Эск 3 от ввода 2")
              );

 if (!escalators_running_state[2] &&
      !A0_x14 &&
      !A0_x15 &&
       A0_x22) // escalator 3 dsconnected from input 1 and disconnected from input 2 and connection of escalator 3 to input 1 can be switched by telemetry
     commands_to_return.push_back(
 	        command (metro_device::get_id(),
	                 metro_device::get_type(),
	                 system_settings::TURN_ON_ESC_3_TO_INPUT_1,
	                 metro_device::get_station_id(),
	                 metro_device::get_modbus_number(),
                     "ПОДКЛЮЧИТЬ Эск 3 к вводу 1")
              );

 if (!escalators_running_state[2] &&
      !A0_x14 &&
      !A0_x15 &&
       A0_x23) // escalator 3 dsconnected from input 1 and disconnected from input 2 and connection of escalator 3 to input 2 can be switched by telemetry
     commands_to_return.push_back(
 	        command (metro_device::get_id(),
	                 metro_device::get_type(),
	                 system_settings::TURN_ON_ESC_3_TO_INPUT_2,
	                 metro_device::get_station_id(),
	                 metro_device::get_modbus_number(),
                     "ПОДКЛЮЧИТЬ Эск 3 к вводу 2")
              );


 if (!escalators_running_state[3] &&
       A0_x16 &&
       A0_x24) // escalator 4 connected to input 1 and connection of escalator 4 to input 1 can be switched by telemetry
     commands_to_return.push_back(
 	        command (metro_device::get_id(),
	                 metro_device::get_type(),
	                 system_settings::TURN_OFF_ESC_4_TO_INPUT_1,
	                 metro_device::get_station_id(),
	                 metro_device::get_modbus_number(),
                     "ОТКЛЮЧИТЬ Эск 4 от ввода 1")
              );
 if (!escalators_running_state[3] &&
       A0_x17 &&
       A0_x25)  // escalator 4 connected to input 2 and connection of escalator 4 to input 2 can be switched by telemetry
     commands_to_return.push_back(
 	        command (metro_device::get_id(),
	                 metro_device::get_type(),
	                 system_settings::TURN_OFF_ESC_4_TO_INPUT_2,
	                 metro_device::get_station_id(),
	                 metro_device::get_modbus_number(),
                     "ОТКЛЮЧИТЬ Эск 4 от ввода 2")
              );

 if (!escalators_running_state[3] &&
      !A0_x16 &&
      !A0_x17 &&
       A0_x24) // escalator 4 dsconnected from input 1 and disconnected from input 2 and connection of escalator 4 to input 1 can be switched by telemetry
     commands_to_return.push_back(
 	        command (metro_device::get_id(),
	                 metro_device::get_type(),
	                 system_settings::TURN_ON_ESC_4_TO_INPUT_1,
	                 metro_device::get_station_id(),
	                 metro_device::get_modbus_number(),
                     "ПОДКЛЮЧИТЬ Эск 4 к вводу 1")
              );
 if (!escalators_running_state[3] &&
      !A0_x16 &&
      !A0_x17 &&
       A0_x25) // escalator 4 dsconnected from input 1 and disconnected from input 2 and connection of escalator 4 to input 2 can be switched by telemetry
     commands_to_return.push_back(
 	        command (metro_device::get_id(),
	                 metro_device::get_type(),
	                 system_settings::TURN_ON_ESC_4_TO_INPUT_2,
	                 metro_device::get_station_id(),
	                 metro_device::get_modbus_number(),
                     "ПОДКЛЮЧИТЬ Эск 4 к вводу 2")
              );

    if (A0_x5) 
        commands_to_return.push_back(
 	        command (metro_device::get_id(),
	                 metro_device::get_type(),
	                 system_settings::COMMAND_CHANCEL,
	                 metro_device::get_station_id(),
	                 metro_device::get_modbus_number(),
                     "СБРОСИТЬ КОМАНДУ")
              );

 return commands_to_return;
};

void 
   metro_shavr::create_properties_widgets
                                   (PtWidget_t *parent_widget){
PtWidget_t *current_widget;
vector<PtWidget_t*>  widgets;
int result;

current_widget=parent_widget;
result=Pt_TRAVERSE_START;
while( ( result=PtWidgetTree( parent_widget, &current_widget, result) ) !=
       Pt_TRAVERSE_DONE) {
           if (current_widget!=parent_widget)
               widgets.push_back(current_widget);
        };

 vector<PtWidget_t*>::iterator iter_widgets=widgets.begin();
     while (iter_widgets!=widgets.end()) {
         PtDestroyWidget(*iter_widgets);
         iter_widgets++;
      };

};

byte metro_shavr::decode_to_binary_decimal_notation(byte binary_value_less_100) throw (spider_exception){
      if (binary_value_less_100>99) throw spider_exception("In decode_to_binary_decimal_notation binary_value_less_100>99");

      byte count_10=0;
      byte count_1;
      while ((binary_value_less_100 - count_10*10)>=10)
           count_10++;

     count_1=binary_value_less_100 - count_10*10;

    return (count_10<<4)|count_1;
};

