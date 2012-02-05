using namespace std;

#include <Ap.h>
#include <Ph.h>
#include <Pt.h>

#include <iostream>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/neutrino.h>
#include <sys/syspage.h>
#include <inttypes.h>
#include <fcntl.h>

#include <vector>
#include <string>
#include <hash_map>
#include <set>
#include <list>
#include <functional>
#include <algorithm>
#include <sstream>

#include "defines.h"
#include <sys/select.h> //#define FD_SETSIZE
#include "spider_exception.h"
#include "system_settings.h"
#include "system_settings_spider.h"

#include "sound.h"

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

#include "data_block.h"
#include "door_data_block.h"

#include "metro_device.h"
#include "metro_devices_container.h"
#include "door.h"

extern msg_dict_container *messages;
extern log_records_container *main_log;

metro_door::metro_door(
		int id, 
		int id_station,
		int number,
		int modbus_number,
		int type,
		int channel,
		bool enabled,
		in_addr_t	ip,
		time_t offline_or_exception_delay,
		bool new_conduction_is_switched_off,
		bool new_log_packets ) throw (spider_exception):
	metro_device( id,
                   id_station,
                   number,
                   modbus_number,
                   type,
                   system_settings::START_DAY_MODE_NEVER,
                   0,
                   0,
                   enabled,
                   ip,
                   channel,
				   offline_or_exception_delay,
				   new_conduction_is_switched_off,
				   new_log_packets),
    A0_x1(false),  //offline 
    A0_state(A0_OFFLINE)
    {};

metro_door::~metro_door(){};

void metro_door::A0(int event)  throw (spider_exception){
 int old_automat_state=A0_state;
 
   switch (A0_state) {
    case A0_OFFLINE:
     if (A0_x1 &
         !A0_x2) {
         A0_state=A0_EXCEPTION;
         break;
     };
     if (A0_x1 &
         !A0_x3) {
         A0_state=A0_DOOR_CLOSED;
         break;
     };
     if (A0_x1 &
         A0_x3 &
		!A0_x5()) {
         A0_state=A0_DOOR_OPENED_NOT_ACCEPTED;
         break;
     };
     if (A0_x1 &
         A0_x3 &
		 A0_x5()) {
         A0_state=A0_DOOR_OPENED_ACCEPTED;
         break;
     };
     break;
    case A0_EXCEPTION:
     if (!A0_x1) {
         A0_state=A0_OFFLINE;
         break;
     };
     if (A0_x2 &
         !A0_x3) {
         A0_state=A0_DOOR_CLOSED;
         break;
     };
     if (A0_x2 &
         A0_x3 &
		 !A0_x5()) {
         A0_state=A0_DOOR_OPENED_NOT_ACCEPTED;
         break;
     };
     if (A0_x2 &
         A0_x3 &
		 A0_x5()) {
         A0_state=A0_DOOR_OPENED_ACCEPTED;
         break;
     };
    break;
    case A0_DOOR_CLOSED:
     if (!A0_x1) {
         A0_state=A0_OFFLINE;
         break;
     };
     if (!A0_x2) {
         A0_state=A0_EXCEPTION;
         break;
     };
     if (A0_x3) {
         A0_state=A0_DOOR_OPENED_NOT_ACCEPTED;
         break;
     };
    break;
    case A0_DOOR_OPENED_NOT_ACCEPTED:
     if (!A0_x1) {
         A0_state=A0_OFFLINE;
         break;
     };
     if (!A0_x2) {
         A0_state=A0_EXCEPTION;
         break;
     };
     if (event==1) {
         A0_state=A0_DOOR_OPENED_ACCEPTED;
         break;
     };
     if (A0_x4()) {
         A0_state=A0_DOOR_OPENED_ACCEPTED;
         break;
     };
    break;
    case A0_DOOR_OPENED_ACCEPTED:
     if (!A0_x1) {
         A0_state=A0_OFFLINE;
         break;
     };
     if (!A0_x2) {
         A0_state=A0_EXCEPTION;
         break;
     };
     if (!A0_x3) {
         A0_state=A0_DOOR_CLOSED;
         break;
     };
    break;
   default :
      ostringstream exception_description;
      exception_description<<"metro_door::A0(...) device id "<<get_id();
      exception_description<<" wrong A0 state "<<A0_state;
      throw spider_exception(exception_description.str());   
    };

 if (A0_state==old_automat_state )  return;
 
 //initializations on enter to new automat state
   switch (A0_state) {
    case A0_OFFLINE:
		A0_z1();
    break;
    case A0_EXCEPTION:
		A0_z2();
    break;
    case A0_DOOR_CLOSED:
    break;
    case A0_DOOR_OPENED_NOT_ACCEPTED:
    break;
    case A0_DOOR_OPENED_ACCEPTED:
    break;
   default :
      ostringstream exception_description;
      exception_description<<"metro_door::A0(...) device id "<<get_id();
      exception_description<<" wrong A0 state "<<A0_state;
      throw spider_exception(exception_description.str());   
    };
}

void metro_door::A0_z1() { set_last_offline_or_exception_time(time(NULL));};

void metro_door::A0_z2() { set_last_offline_or_exception_time(time(NULL));};

bool metro_door::A0_x5() {
		return ((time(NULL)  - get_last_offline_or_exception_time()) <= get_offline_or_exception_delay ());
}

bool metro_door::A0_x4() {
		return is_conduction_is_switched_off();
}


PhRect_t metro_door::get_device_widget_extent() 
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

void metro_door::send_command(command cmd)  throw (spider_exception){
vector<byte> buffer(0);
int old_A0_state=A0_state;
ldword tmp_id;

	if (cmd.get_device_id()!=get_id())
          throw spider_exception("metro_door::send_command(...) cmd.get_device_id()!=get_id() ");

    if (is_conduction_is_switched_off() && 
         cmd.get_command_code() !=system_settings::COMMAND_ACCEPT)
                 return;

    switch (cmd.get_command_code()) {
	case (system_settings::COMMAND_ACCEPT):
         tmp_id=main_log->get_records_autoincrement();
         main_log->set_records_autoincrement(++tmp_id);

          main_log->insert(log_record(tmp_id,
                                     system_settings::DISPATCHER_KVIT_MASHZAL_DOOR,
                                     system_settings_spider::MESSAGE_TYPE_DISPATCHER_DOIT,
                                     cmd.get_station(),
                                     cmd.get_device_id(),
                                     time(NULL)));

             main_log->prepare_to_display();


          A0(1);
          break;
      default:
        ostringstream exception_description;
        exception_description<<"metro_escalator::send_command unrecognized command ";
        exception_description<<cmd.get_command_code()<< " device id "<<get_id();
		 throw spider_exception(exception_description.str());      
    };  //switch (cmd.get_command_code())

//Add creating modbus command here and inserting into metro_device::set_request_to_device_buffer(buffer);

    A0(7); // event 7 - data refreshed
    if (old_A0_state!=A0_state) {
              update_device_widget();
              PtFlush();
      };

};

void metro_door::create_device_widget(int x,
                                                      int y,
                                                      ApDBase_t*  widgets_dbase,
                                                      PtWidget_t *station_widget,
                                                      vector<PtCallback_t> activate_callbacks,
                                                      vector<PtCallback_t> menu_callbacks)
                 throw (spider_exception){
	vector<PtArg_t> args;
    PtWidget_t *door_sensor;
	PhPoint_t position;

	if (widgets_dbase==NULL)
    		throw spider_exception("metro_escalator::create_device_window(...) widgets_dbase pointer id NULL");		

	if (station_widget==NULL)
    		throw spider_exception("metro_escalator::create_device_window(...) station_widget pointer id NULL");		

	metro_device::set_station_widget(station_widget);

	int device_id=metro_device::get_id();

	args.clear();
	args.resize(5);
	PtSetArg(&args[0], Pt_ARG_USER_DATA, &device_id, sizeof(device_id));
	PtSetArg(&args[1], Pt_ARG_TEXT_STRING, "Д", 0);
    if (! menu_callbacks.empty()) {
	   PtSetArg(&args[2],  Pt_CB_MENU,
			  &menu_callbacks[0],
			  menu_callbacks.size());
       }else {
	     PtSetArg(&args[2],  Pt_CB_MENU, NULL, 0 );
       };
	PtSetArg(&args[3], Pt_ARG_FLAGS, Pt_TRUE, Pt_MENUABLE);
	PtSetArg(&args[4], Pt_ARG_FLAGS, Pt_FALSE, Pt_ALL_BUTTONS);

	position.x=x;
	position.y=y;

	door_sensor = ApCreateDBWidget(widgets_dbase,
                                               "sensor_wnd",
                                                station_widget,
                                                &position,
                                                args.size(),
                                                &args[0]);

    if (door_sensor==NULL)
    		throw spider_exception("metro_door::create_device_window(...) can`t get  widget sensor_wnd for door_sensor");

     metro_device::set_device_widget(door_sensor);
};

void metro_door::update_device_widget() throw (spider_exception){
vector<PtArg_t> args;

if (metro_device::get_device_widget()==NULL) return;

//door widget updating 
 args.clear();
 args.resize(3);
 if(A0_state==A0_DOOR_OPENED_NOT_ACCEPTED) {
              PtSetArg(&args[0],
                            Pt_ARG_COLOR,
                            system_settings_spider::COLOR_WHITE,
                            0);
              PtSetArg(&args[1],
                            Pt_ARG_FILL_COLOR,
                            system_settings_spider::COLOR_RED,
                            0);
              PtSetArg(&args[2],
                            Pt_ARG_INLINE_COLOR,
                            system_settings_spider::COLOR_BLACK,
                            0);
}else if (A0_state==A0_DOOR_OPENED_ACCEPTED){
              PtSetArg(&args[0],
                            Pt_ARG_COLOR,
                            system_settings_spider::COLOR_WHITE,
                            0);
              PtSetArg(&args[1],
                            Pt_ARG_FILL_COLOR,
                            system_settings_spider::COLOR_GREEN,
                            0);
              PtSetArg(&args[2],
                            Pt_ARG_INLINE_COLOR,
                            system_settings_spider::COLOR_BLACK,
                            0);
} else { // if(A0_state==A0_DOOR_OPENED_NOT_ACCEPTED
               PtSetArg(&args[0],
                              Pt_ARG_COLOR,
                              system_settings_spider::COLOR_GRAY,
                              0);
               PtSetArg(&args[1],
                             Pt_ARG_FILL_COLOR,
                             Pg_TRANSPARENT,
                             0);
               PtSetArg(&args[2],
                              Pt_ARG_INLINE_COLOR,
                              system_settings_spider::COLOR_GRAY,
                              0);
}; // if(A0_state==A0_DOOR_OPENED_NOT_ACCEPTED
PtSetResources(metro_device::get_device_widget(), args.size(), &args[0]);
};

void metro_door::update_device_panel (metro_device_type *dev_type) throw (spider_exception){};

void metro_door::decode_answer_from_device_to_data_block(){
   system_settings_spider *sys_sett=system_settings_spider::get_instance();
   metro_device::buffer_data_type answer_from_device=metro_device::get_answer_from_device_buffer();
   int old_A0_state=A0_state;

        try {
		if (answer_from_device.empty()) {
                 if (metro_device::get_failures_count()>system_settings::RECONNECTS_TO_DEVICE_COUNT) {
                           A0_x2=false;
                       } else {
                           metro_device::set_failures_count(metro_device::get_failures_count()+1);
                       };

			 } else { //if (answer_from_device.empty()
                  if (( answer_from_device[system_settings::MODBUS_FUNCTION_CODE_INDEX] & 0x80)
                         !=0) {
                         if (metro_device::get_failures_count()>system_settings::RECONNECTS_TO_DEVICE_COUNT ||
                              ! A0_x1)  {//if door change state from offline to internal exception, it must be changed immediatly
                                A0_x2=false;
                                metro_device::set_failures_count(0);
                             } else  {
                                metro_device::set_failures_count(metro_device::get_failures_count()+1);
                             };
                      } else {
                                A0_x2=true;
                                metro_device::set_failures_count(0);
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

           if (old_A0_state!=A0_state) {
               update_device_widget();
               PtFlush();

              if (A0_state==A0_DOOR_OPENED_NOT_ACCEPTED ) {
					if (!A0_x4())
					{
					   //A0_x4==true if conduction is switched off
                       string message("Открыта дверь машзала на станции\n");
					   metro_stations_container *stations=metro_stations_container::get_instance();
                       metro_stations_container::iterator stations_iter;
                       stations_iter=stations->find(metro_device::get_station_id());
                      if (stations_iter!=stations->end())
                            message+=stations_iter->second.get_stl_name_string();
                       sys_sett->message_window(system_settings::WARN_MSG,
 														              message,
																	  true);

					 sound& snd=sound::get_instance();
					std::vector<string> *files  = new std::vector<string> ();
					files->push_back(stations_iter->second.get_wav_file_name());
                     snd.play(files);
					}

                      metro_devices_types_container *dev_types=metro_devices_types_container::get_instance();
                      metro_devices_types_container::iterator types_iter;
                      types_iter=dev_types->find(metro_device::get_type());
                      if (types_iter!=dev_types->end()) {
                                  msg_dict_container::iterator msgs_iter=types_iter->second->type_messages.begin(); //in door data type only one message  - door opened

	   			      ldword tmp_id=main_log->get_records_autoincrement();
 			          main_log->set_records_autoincrement(++tmp_id); 
                       main_log->insert(log_record(tmp_id,
                                                            msgs_iter->second.get_id(),
                                                            msgs_iter->second.get_type(),
                                                            metro_device::get_station_id(),
                                                            metro_device::get_id(),
                                                            time(NULL)));
                      }; //if (types_iter!=dev_types->end())
                   }; //if (A0_state==A0_DOOR_OPENED_NOT_ACCEPTED)

              }; // if (old_A0_state=A0_state)

/*
//door now no paneled
		       if (metro_device::get_id()==metro_device::paneled_device_id &&
           			metro_device::device_panel!=NULL &&
//if all ok metro_device::get_failures_count()==0 or failures count is too much metro_device::get_failures_count()>system_settings::RECONNECTS_TO_DEVICE_COUNT
//panel must be updated, otherwise (last answer is failure but count of failures less maximum) panel is NOT updated
                             (metro_device::get_failures_count()==0|| 
                              metro_device::get_failures_count()>system_settings::RECONNECTS_TO_DEVICE_COUNT)
                    ) {
                          metro_devices_types_container::iterator dev_type_iter=
                                              metro_devices_types->find(metro_device::get_type());
                          if (dev_type_iter!=metro_devices_types->end()) {
                                        update_device_panel(dev_type_iter->second);
                                                          } else {
                                                             ostringstream error_description;
                                                             error_description<<"In metro_door::decode_answer_from_device_to_data_block not found device type id : "<<metro_device::get_type();
                                                             error_description<<" for device id "<<metro_device::get_id();
                                                             sys_sett->sys_message(system_settings::ERROR_MSG,  
                                                                                                           error_description.str());
                                                             return;
                                                          };
                    PtFlush();
                };
*/

   } catch (spider_exception spr_exc) {
       ostringstream exception_description;
       exception_description<<"In metro_door::decode_answer_from_device_to_data_block device_id "<<get_id()<<" : ";
       exception_description<<spr_exc.get_description();
       sys_sett->sys_message(system_settings::ERROR_MSG,
                                             exception_description.str());
   };

};

metro_device::buffer_data_type 
              metro_door::get_request_for_send_to_device() {
buffer_data_type return_buffer(0);
return return_buffer;
};

void 
  metro_door::decode_answer_from_device_4_function
           (metro_device::buffer_data_type answer) throw (spider_exception){
   system_settings_spider *sys_sett=system_settings_spider::get_instance();

   door_data_block local_data_block=this->data_block;
   metro_device::buffer_data_type::iterator tmp_iter1, tmp_iter2;

   word register_value;

//=======================================
/*
if (metro_device::get_id()==10) {
       vector<char> tmp_chars(20);
       if (answer.empty())
           cout<<"Answer is empty"<<endl;
       for (vector<byte>::size_type i=0; i<answer.size(); i++ ) {
            itoa(answer[i], &tmp_chars[0], 16);
            cout<<"  0x"<<&tmp_chars[0];
         };
       cout<<endl;
};
*/
//=======================================
//data bytes count
   tmp_iter1=answer.begin();
    advance(tmp_iter1,
                   system_settings::MODBUS_DATA_BYTES_COUNT_INDEX);
    register_value=*tmp_iter1;
    if (register_value!=2) {
          A0_x2=false;
          throw spider_exception("In metro_door::decode_answer_from_device_4_function(...) in data packet must be 2 bytes of data");
          };

//move iteratiors to first register data
   tmp_iter1=answer.begin();
   tmp_iter2=tmp_iter1;
   advance(tmp_iter1,
                 (system_settings::MODBUS_DATA_BYTES_COUNT_INDEX+1));
   advance(tmp_iter2,
                 (system_settings::MODBUS_DATA_BYTES_COUNT_INDEX+3));
//poehali!
			register_value=
			     sys_sett->type_from_bytes<word>(system_settings::bytes (tmp_iter1, tmp_iter2));
            if ((register_value&0x1)==0) {
                local_data_block.set_signal_value(
                                      door_data_block::INDEX_SIGNAL_DOOR_OPENED,
                                      door_data_block::SIGNAL_VALUE_GREEN);
                A0_x3=false;
             } else {
                local_data_block.set_signal_value(
                                      door_data_block::INDEX_SIGNAL_DOOR_OPENED,
                                      door_data_block::SIGNAL_VALUE_RED);
                A0_x3=true;
             };


       this->data_block=local_data_block;
};

command 
     metro_door::get_device_start_command() throw(spider_exception) {
  throw spider_exception("door not support timer_command");
}


vector<command> 
     metro_door::get_appropriated_commands() throw (spider_exception){
 vector<command> commands_to_return;

     if (A0_state==A0_DOOR_OPENED_NOT_ACCEPTED)
            commands_to_return.push_back(
 	           command (metro_device::get_id(),
	                 metro_device::get_type(),
	                 system_settings::COMMAND_ACCEPT, 
	                 metro_device::get_station_id(),
	                 metro_device::get_modbus_number(),
                     "КВИТИРОВАТЬ")
              );

 return commands_to_return;
};

void 
   metro_door::create_properties_widgets
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

