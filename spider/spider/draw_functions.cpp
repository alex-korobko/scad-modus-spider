using namespace std;

#include <Ph.h>
#include <Ap.h>
#include <Pt.h>
#include <sys/neutrino.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include <iostream.h>

#include <vector>
#include <string>
#include <hash_map>
#include <list>
#include <set>
#include <functional>
#include <algorithm>
#include <sstream>

#include "defines.h"
#include "spider_exception.h"
#include "system_settings.h"
#include "system_settings_spider.h"

#include "command.h"
#include "command_pool_container.h"
#include "data_block.h"
#include "msg_types_container.h"
#include "msg_dict_container.h"
#include "log_records_container.h"
#include "devices_types.h"
#include "metro_devices_types_container.h"
#include "timer_command_container.h"
#include "dispatchers_persons_container.h"

class metro_devices_container;

#include "line.h"
#include "metro_lines_container.h"
#include "station.h"
#include "metro_stations_container.h"

#include "metro_device.h"
#include "metro_devices_container.h"

#include "data_block.h"
#include "escalator_data_block.h"
#include "escalator.h"
#include "udku_data_block.h"
#include "udku.h"

extern msg_dict_container *messages;
extern log_records_container *main_log;
extern log_records_container *archive_log;

void commands_pool_raw_list_draw_function( PtWidget_t *widget,
            PtGenListItem_t *item,
            unsigned index,
            unsigned nitems,
            PhRect_t *where ) {

    vector<PtArg_t> args(1);
    vector<char> tmp_chars(20);
    PtListColumn_t *internal_column_pos=NULL;

	unsigned count;
	PhPoint_t draw_point;

	ostringstream  device_text;
	string station_text, command_description;

    cmd_pool_container::iterator cmd_pool_cont_iter;
    metro_stations_container::iterator iter_metro_stations;
    metro_devices_types_container::iterator iter_devices_types;

    system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
    if (spider_sys_sett==NULL)
               cout<<"Can`t get instance of system_settings_spider"<<endl;

    try {
       cmd_pool_container *cmd_pool_instance=cmd_pool_container::get_instance();
       if (cmd_pool_instance==NULL) 
             throw spider_exception("In activate_device_menu_item (..): Can`t get instance  of cmd_pool_container");

        metro_devices_types_container *devices_types=metro_devices_types_container::get_instance();
        if (devices_types==NULL)
                 throw spider_exception("Can`t get instance of metro_devices_types_container");

         metro_devices_container *devices=metro_devices_container::get_instance();
         if (devices==NULL)
                 throw spider_exception("Can`t get instance of metro_devices_container");
         metro_devices_container::iterator devices_iter;

          metro_stations_container *stations = metro_stations_container::get_instance();
          if (stations==NULL)
                 throw spider_exception("Can`t get instance of metro_stations_container");

	       PtGenListDrawBackground( widget, item, nitems, where, 0, 0, 0, 0 );

	        if (spider_sys_sett->small_font_prepared())	{
			      PgSetFont(  spider_sys_sett->get_small_font() );
		       } else {
			      spider_sys_sett->sys_message(system_settings::ERROR_MSG,
					"Not found system_settings::small_font for re-drawing log_items");
		        };

	        PtSetArg(&args[0], Pt_ARG_LIST_COLUMN_POS, &internal_column_pos, 0);
	        PtGetResources(widget,
							            args.size(),
							            &args[0]);

            cmd_pool_cont_iter=cmd_pool_instance->begin();
	        advance(cmd_pool_cont_iter, index-1);

	        count=0;
	        draw_point.y=where->ul.y+system_settings_spider::ROW_HEIGHT;

	         while ( count < nitems &&
				         cmd_pool_cont_iter!=cmd_pool_instance->end()){
                station_text="";
                device_text.str("");
              		PgSetTextColor(system_settings_spider::COLOR_BLUE);

                       command_description=cmd_pool_cont_iter->get_command_description();

		               iter_metro_stations=stations->find(cmd_pool_cont_iter->get_station());
		               if (iter_metro_stations!=stations->end())
				            station_text=iter_metro_stations->second.get_stl_name_string();

			iter_devices_types=devices_types->find(cmd_pool_cont_iter->get_device_type_id());
            devices_iter=devices->find(cmd_pool_cont_iter->get_device_id());
            if (devices_iter==devices->end()) {
                 ostringstream mess;
                 mess<<"Not found device type "<<cmd_pool_cont_iter->get_device_type_id()<<" device id "<<cmd_pool_cont_iter->get_device_id()<<" for redrawing command pool";
		         throw spider_exception(mess.str());
            };

            if (iter_devices_types!=devices_types->end()) {
                      device_text<<iter_devices_types->second->get_name();
                      if (devices_iter->second->get_type_description()==metro_device_type::DEVICE_TYPE_ESCALATOR ||
                           devices_iter->second->get_type_description()==metro_device_type::DEVICE_TYPE_UDKU)
                                device_text<<" "<<devices_iter->second->get_number();
							} else { // iter_devices_types=devices_types->end()
                               ostringstream mess;
                               mess<<"Not found device type "<<cmd_pool_cont_iter->get_device_type_id()<<" device id "<<cmd_pool_cont_iter->get_device_id()<<" for redrawing command pool";
                               spider_sys_sett->sys_message(system_settings::ERROR_MSG,
																			mess.str());
                            };

                       // drawing
                       draw_point.x=where->ul.x+internal_column_pos[0].from + system_settings_spider::COLUMN_LEFT_MARGIN;
                       PgDrawText(station_text.c_str(), station_text.size(), &draw_point, Pg_TEXT_BOTTOM);

                        draw_point.x = where->ul.x + internal_column_pos[1].from + system_settings_spider::COLUMN_LEFT_MARGIN;
                        PgDrawText(device_text.str().c_str(), device_text.str().size(), &draw_point, Pg_TEXT_BOTTOM);

                         draw_point.x = where->ul.x + internal_column_pos[2].from + system_settings_spider::COLUMN_LEFT_MARGIN;	
                         PgDrawText(command_description.c_str(), command_description.size(), &draw_point, Pg_TEXT_BOTTOM);

                         count++;
                         cmd_pool_cont_iter++;
                         draw_point.y+=system_settings_spider::ROW_HEIGHT;
               }; //while ( count < nitems &&

          } catch ( spider_exception spr_exc) {
              spider_sys_sett->sys_message(system_settings::ERROR_MSG, "In commands_pool_raw_list_draw_function(...): "+spr_exc.get_description());
          };

};

void log_raw_list_draw_function( PtWidget_t *widget,
            PtGenListItem_t *item,
            unsigned index,
            unsigned nitems,
            PhRect_t *where ) {
    vector<PtArg_t> args(2);
    PtListColumn_t *internal_column_pos=NULL;
    log_records_container *log_rec_contain=main_log;
	unsigned count;
	PhPoint_t draw_point;

	int tmp_msg_id, msg_type, device_id, splitting_message_find_pos;
    int breaking_path_find_pos,
          input_circut_name_find_pos,
          dispatcher_name_find_pos,
          stop_cause_find_pos,
          offset_in_list_row,
		 dispatcher_id,
		 stop_cause_id;
	ostringstream  device_text;
	string  station_text;

    log_records_container::iterator iter_log_rec;
    msg_types_container::iterator  iter_msg_types;	
	metro_devices_container::iterator iter_metro_devices;
    metro_devices_types_container::iterator iter_devices_types;
    metro_device_type::data_unit_iterator type_data_unit_iter;
    msg_dict_container::iterator tmp_msg_iter;
    metro_stations_container::iterator iter_metro_stations;
	dispatcher_persons_container::iterator iter_dispachers; 

    ldword current_message_id;
    dword breaking_path_value;
    byte input_circut_index;
    string input_circut_name, date_text, time_text;
    system_settings::strings_container message_text_lines;
    vector<char> tmp_chars;

   system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
   if (spider_sys_sett==NULL) {
            cout<<"log_raw_list_draw_function: Can`t get system_settings_spider instance"<<endl;
            return;
      };

  system_settings::strings_container parameters_names=
                      spider_sys_sett->get_parameters_in_messages_strings();

  try {
    msg_types_container *msg_types=msg_types_container::get_instance();
    if (msg_types==NULL)
                 throw spider_exception("Can`t get instance of msg_types_container");

    metro_devices_types_container *devices_types=metro_devices_types_container::get_instance();
    if (devices_types==NULL)
                 throw spider_exception("Can`t get instance of metro_devices_types_container");

    metro_devices_container *devices=metro_devices_container::get_instance();
    if (devices==NULL)
                 throw spider_exception("Can`t get instance of metro_devices_container");

    metro_stations_container *stations = metro_stations_container::get_instance();
    if (stations==NULL)
                 throw spider_exception("Can`t get instance of metro_stations_container");

	dispatcher_persons_container *dispatchers = dispatcher_persons_container::get_instance();
	if (dispatchers==NULL)
				throw spider_exception("Can`t get instance of dispatcher_persons_container");

	PtGenListDrawBackground( widget, item, nitems, where, 0, 0, 0, 0 );

	if (spider_sys_sett->small_font_prepared())	{
			PgSetFont(  spider_sys_sett->get_small_font() );
		} else {
			spider_sys_sett->sys_message(system_settings::ERROR_MSG,
					"Not found system_settings::small_font for re-drawing log_items");
		};

    args.clear();
    args.resize(2);
	PtSetArg(&args[0], Pt_ARG_LIST_COLUMN_POS, &internal_column_pos, 0);
	PtSetArg(&args[1], Pt_ARG_POINTER, &log_rec_contain, 0);	
	PtGetResources(widget,
							  args.size(),
							  &args[0]);	

	if (log_rec_contain==NULL){
			spider_sys_sett->sys_message(system_settings::ERROR_MSG,
												"Not found log_rec_contain for re-drawing log_items");
			return;
		};


   if ((index-1+nitems)>log_rec_contain->size()){
            ostringstream err_message;
            err_message<<"index-1+nitems ("<<(index-1+nitems)<<") >=log_rec_contain->size() ("<<log_rec_contain->size()<<")"<<endl;
			spider_sys_sett->sys_message(system_settings::ERROR_MSG,
												err_message.str());
			return;
		};


	iter_log_rec=log_rec_contain->begin();
	advance(iter_log_rec, index-1);

	count=0;
	draw_point.y=where->ul.y+system_settings_spider::ROW_HEIGHT/2;

	while (	count < nitems &&
				iter_log_rec!=log_rec_contain->end()){


        station_text="";
        device_text.str("");
        message_text_lines.resize(1);
        message_text_lines[0] ="";

        current_message_id=iter_log_rec->get_msg_id();

		tmp_msg_id=static_cast<int>(current_message_id & 0x00FF);
		msg_type=0, device_id=0;

		msg_type=iter_log_rec->get_msg_type();
		iter_msg_types=msg_types->find(msg_type);
		if (iter_msg_types==msg_types->end()){
				ostringstream mess;
				mess<<"Not found msg_type "<<msg_type<<" for re-drawing log_items";
				spider_sys_sett->sys_message(system_settings::ERROR_MSG,
																	mess.str());
				return;
			};

		PgSetTextColor(iter_msg_types->second.get_type_color());
		if (tmp_msg_id!=0) {	// it`s real id of message
			device_id=iter_log_rec->get_device_id();
			if (device_id>0){
                iter_devices_types=devices_types->end(); //for block circut name definition - see below in that function
				iter_metro_devices=devices->find(device_id);
				if (iter_metro_devices!=devices->end()) {
						  iter_devices_types=devices_types->find(iter_metro_devices->second->get_type());
                          if (iter_devices_types!=devices_types->end()) {
                                 tmp_msg_iter=iter_devices_types->second->type_messages.find(tmp_msg_id);
                              if (tmp_msg_iter!=iter_devices_types->second->type_messages.end())
                                          message_text_lines[0]=tmp_msg_iter->second.get_text();
                                device_text<<iter_devices_types->second->get_name();
                              if (iter_metro_devices->second->get_type_description()==metro_device_type::DEVICE_TYPE_ESCALATOR ||
                                  iter_metro_devices->second->get_type_description()==metro_device_type::DEVICE_TYPE_UDKU)
                                  device_text<<" "<<iter_metro_devices->second->get_number();
							} else { // iter_devices_types=devices_types->end()
                               ostringstream mess;
                               mess<<"Not found device type "<<iter_metro_devices->second->get_type()<<" device id "<<device_id<<" for re-drawing log_items";
                               spider_sys_sett->sys_message(system_settings::ERROR_MSG,
																			mess.str());
                               return;	
                            };
					} else { //insert here code to finding in other metro devices containers  -  using GLOBAL id`s identificators
						ostringstream mess;
						mess<<"Not found device id "<<device_id<<" for re-drawing log_items";
						spider_sys_sett->sys_message(system_settings::ERROR_MSG,
																			mess.str());
						return;	
					}; // if (iter_metro_devices!=devices->end())
			}; // if (device_id>0)

//         cout<<"device_text "<<device_text.str()<<endl;

			if (message_text_lines[0].empty()){
				tmp_msg_iter=messages->find(tmp_msg_id);
					if (tmp_msg_iter!=
						messages->end()) {
					 		message_text_lines[0]=tmp_msg_iter->second.get_text();
					 	} else { // if (tmp_msg_iter! =
							ostringstream mess;
							mess<<"Not found message id "<<tmp_msg_id<<" for re-drawing log_items";
							spider_sys_sett->sys_message(system_settings::ERROR_MSG,
																				mess.str());
							return;	
					 	}; // if (tmp_msg_iter!=
				}; // if (message_text_lines[0].empty())

                    breaking_path_find_pos=static_cast<int>(message_text_lines[0].find(parameters_names[system_settings::PARAMETER_NAME_BREAKING_PATH_VALUE]));
                    if (breaking_path_find_pos!=-1) {
                      breaking_path_value=static_cast<dword>(current_message_id>>32);
                      tmp_chars.clear();
                      tmp_chars.resize(12);
                      itoa (breaking_path_value, &tmp_chars[0], 10);
                      message_text_lines[0].replace(
                            breaking_path_find_pos,
                            parameters_names[system_settings::PARAMETER_NAME_BREAKING_PATH_VALUE].size(),
                            &tmp_chars[0]);
                    };//if (breaking_path_find_pos!=-1)

                    input_circut_name_find_pos=static_cast<int>(message_text_lines[0].find(parameters_names[system_settings::PARAMETER_NAME_BLOCK_CIRCUT_NAME]));
                    if (input_circut_name_find_pos!=-1) {
                        input_circut_index=static_cast<byte>((current_message_id&0x00ff00)>>8);

                      if (device_id<=0) {
                           input_circut_name="ERROR!!";
                           ostringstream mess;
                           mess<<"Present PARAMETER_NAME_BLOCK_CIRCUT_NAME but device_id==0 message id in log"<<current_message_id;
                           spider_sys_sett->sys_message(system_settings::ERROR_MSG, mess.str());
                      } else { //if (device_id<=0)
                          if (iter_devices_types!=devices_types->end() &&
                               iter_devices_types->second->circuts_size()>input_circut_index) {
                                input_circut_name=(iter_devices_types->second->at_circut(input_circut_index)).get_hint();
                              } else  { // if (iter_devices_types!=devices_types->end()) &&
                                 input_circut_name="ERROR!!";
                                 ostringstream mess;
                                 mess<<"Present PARAMETER_NAME_BLOCK_CIRCUT_NAME but iter_devices_types!=devices_types->end() or circuts_size()>input_circut_index message id in log"<<current_message_id;
                                 spider_sys_sett->sys_message(system_settings::ERROR_MSG,
                                                                                  mess.str());
                             }; // if (iter_devices_types!=devices_types->end())
                     }; // if (device_id<=0)

                      message_text_lines[0].replace( input_circut_name_find_pos,
							   parameters_names[system_settings::PARAMETER_NAME_BLOCK_CIRCUT_NAME].size(),
							   input_circut_name);
                      };//if (input_circut_name_find_pos!=-1)

                    dispatcher_name_find_pos=static_cast<int>(message_text_lines[0].find(parameters_names[system_settings::PARAMETER_NAME_DISPATCHER]));
                    if (dispatcher_name_find_pos!=-1) {
                      dispatcher_id=static_cast<dword>(current_message_id>>8);
					iter_dispachers= dispatchers->find(dispatcher_id);
					if (iter_dispachers != dispatchers->end())
						{
						string  dispatcher_fio=iter_dispachers->get_last_name()+" "+iter_dispachers->get_first_name()+" "+iter_dispachers->get_middle_name();
	    	             message_text_lines[0].replace(
                            dispatcher_name_find_pos,
                            parameters_names[system_settings::PARAMETER_NAME_DISPATCHER].size(),
                            dispatcher_fio.c_str());
						};
                    };//if (dispatcher_name_find_pos!=-1)
	               //ATTENTION add here  stop_cause_find_pos;

                    stop_cause_find_pos=static_cast<int>(message_text_lines[0].find(parameters_names[system_settings::PARAMETER_NAME_STOP_CAUSE]));
                    if (stop_cause_find_pos!=-1) {
                      breaking_path_value=static_cast<dword>(current_message_id>>32);
                      tmp_chars.clear();
                      tmp_chars.resize(12);
                      itoa (breaking_path_value, &tmp_chars[0], 10);
                      message_text_lines[0].replace(
                            stop_cause_find_pos,
                            parameters_names[system_settings::PARAMETER_NAME_STOP_CAUSE].size(),
                            &tmp_chars[0]);
                    };//if (stop_cause_find_pos!=-1)

                     //splitting text
                    if (message_text_lines.size()>0) {
                    while (message_text_lines[message_text_lines.size()-1].size()>system_settings_spider::MAX_CHARS_COUNT_IN_ROW) {
                        splitting_message_find_pos=
                           static_cast<int>(message_text_lines[message_text_lines.size()-1].find_last_of(' ', system_settings_spider::MAX_CHARS_COUNT_IN_ROW));	
			
                        if (splitting_message_find_pos<0) splitting_message_find_pos=system_settings_spider::MAX_CHARS_COUNT_IN_ROW;

                        message_text_lines.push_back(message_text_lines[message_text_lines.size()-1].substr(splitting_message_find_pos, message_text_lines[message_text_lines.size()-1].size()-splitting_message_find_pos));
                        message_text_lines[message_text_lines.size()-2].replace(
                                       splitting_message_find_pos, 
                                       message_text_lines[message_text_lines.size()-2].size()-splitting_message_find_pos,
                                       " ",
                                       0);
                      }; //while (message_text_lines[
				 }; //if (message_text_lines.size()>0)
		}; // if (tmp_msg_id!=0)

		iter_metro_stations=stations->find(iter_log_rec->get_station_id());
		if (iter_metro_stations!=stations->end()){
				station_text=iter_metro_stations->second.get_stl_name_string();
			};

		 time_t tmp_time=iter_log_rec->get_record_time();		

         tmp_chars.clear();
         tmp_chars.resize(20);
	 	strftime(&tmp_chars[0], tmp_chars.size(), "%d-%m-%Y", localtime( &tmp_time));	
        date_text=&tmp_chars[0];
	 	strftime(&tmp_chars[0], tmp_chars.size(), "%H:%M:%S", localtime( &tmp_time ));		
        time_text=&tmp_chars[0];

		// drawing
		draw_point.x=where->ul.x+internal_column_pos[0].from + system_settings_spider::COLUMN_LEFT_MARGIN;
		PgDrawText(date_text.c_str(), date_text.size(), &draw_point, Pg_TEXT_BOTTOM);

		draw_point.x = where->ul.x + internal_column_pos[1].from + system_settings_spider::COLUMN_LEFT_MARGIN;
		PgDrawText(time_text.c_str(), time_text.size(), &draw_point, Pg_TEXT_BOTTOM);

		draw_point.x = where->ul.x + internal_column_pos[2].from + system_settings_spider::COLUMN_LEFT_MARGIN;	
		PgDrawText(station_text.c_str(), station_text.size(), &draw_point, Pg_TEXT_BOTTOM);
		
		draw_point.x = where->ul.x + internal_column_pos[3].from + system_settings_spider::COLUMN_LEFT_MARGIN;	
		PgDrawText(device_text.str().c_str(), device_text.str().size(), &draw_point, Pg_TEXT_BOTTOM);

       //return to up position in row
		draw_point.y-=system_settings_spider::ROW_HEIGHT/2;

		if (message_text_lines.size()<=1) {
         offset_in_list_row=system_settings_spider::ROW_HEIGHT/2;
        } else {
         offset_in_list_row=system_settings_spider::ROW_HEIGHT/message_text_lines.size();
        };

        for (system_settings::strings_container::size_type i=0; i<message_text_lines.size(); i++) {
		draw_point.x = where->ul.x + internal_column_pos[4].from + system_settings_spider::COLUMN_LEFT_MARGIN;	
		draw_point.y+=offset_in_list_row;
		PgDrawText(message_text_lines[i].c_str(), message_text_lines[i].size(), &draw_point, Pg_TEXT_BOTTOM);

        };
        //last part offset of previos line 
		if (message_text_lines.size()<=1) {
		draw_point.y+=offset_in_list_row;
        };
        //new line offset
		draw_point.y+=system_settings_spider::ROW_HEIGHT/2;
		count++;
		iter_log_rec++;
     }; // while (	count< nitems &&


   } catch (spider_exception spr_exc) {
           ostringstream mess;
           mess<<"In log_raw_list_draw_function : "<<spr_exc.get_description();
                     spider_sys_sett->sys_message(system_settings::ERROR_MSG,
                                                                         mess.str());
    }; //   catch (spider_exception spr_exc) 

};

void timer_commands_raw_list_draw_function( PtWidget_t *widget,
            PtGenListItem_t *item,
            unsigned index,
            unsigned nitems,
            PhRect_t *where ){
PtListColumn_t *internal_column_pos=NULL;
int start_hour, start_minute;
string station_text, command_text;
ostringstream device_text, time_text;
unsigned count;
PhPoint_t draw_point;

system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
if (spider_sys_sett==NULL) {
  cout<<"In station_devices_properties_raw_list_draw_function: Can`t get instance of system_settings_spider"<<endl;
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

        metro_devices_types_container *metro_devices_types=metro_devices_types_container::get_instance();
         metro_devices_types_container::iterator devices_types_iter;
        if (metro_devices_types==NULL)
                  throw spider_exception("Can`t get instance of metro_devices_types_container");

        timer_command_container *timers_commands=timer_command_container::get_instance();
        timer_command_container::iterator_timer_command timers_commands_iter;
        if (timers_commands==NULL)
                  throw spider_exception("Can`t get instance of timer_command_container");

        PtGenListDrawBackground( widget, item, nitems, where, 0, 0, 0, 0 );
	        if (spider_sys_sett->small_font_prepared())	{
			      PgSetFont(  spider_sys_sett->get_small_font() );
		       } else {
			      spider_sys_sett->sys_message(system_settings::ERROR_MSG,
					"Not found system_settings::small_font for re-drawing log_items");
		        };

	        PtGetResource(widget, Pt_ARG_LIST_COLUMN_POS, &internal_column_pos, 0);

        count=0;
        draw_point.y=where->ul.y+system_settings_spider::ROW_HEIGHT;
        timers_commands_iter=timers_commands->begin();
         advance(timers_commands_iter, index-1);
   		PgSetTextColor(system_settings_spider::COLOR_BLUE);
	         while ( count < nitems &&
				         timers_commands_iter!=timers_commands->end()){
              device_text.str("");
              time_text.str("");
              station_text="";
              command cmd=timers_commands_iter->get_command();

               stations_iter=metro_stations->find(cmd.get_station());
               if (stations_iter==metro_stations->end()) {
                   ostringstream message;
                   message<<"Can`t find station with id "<<cmd.get_station();
                   throw spider_exception (message.str());
                   };
               station_text=stations_iter->second.get_stl_name_string();

              devices_iter=metro_devices->find(cmd.get_device_id());
               if (devices_iter==metro_devices->end()) {
                    ostringstream message;
                    message<<"Not found device with id "<<cmd.get_station()<<" in station "<<cmd.get_device_id();
                     throw spider_exception(message.str());
               };

               devices_types_iter=metro_devices_types->find(cmd.get_device_type_id());
               if (devices_types_iter==metro_devices_types->end()) {
                    ostringstream message;
                    message<<"Not found device type with id "<<cmd.get_device_type_id()<<" for device with id "<<cmd.get_device_id();
                     throw spider_exception(message.str());
                   };

                device_text<<devices_types_iter->second->get_name();
                if (devices_iter->second->get_type_description()==metro_device_type::DEVICE_TYPE_ESCALATOR ||
                    devices_iter->second->get_type_description()==metro_device_type::DEVICE_TYPE_UDKU)
                    device_text<<" "<<devices_iter->second->get_number();
                command_text=cmd.get_command_description();
                start_hour=timers_commands_iter->get_timer_hour();
                start_minute=timers_commands_iter->get_timer_minute();
                time_text<<(start_hour<10?"0":"")<<start_hour<<" часов : "<<(start_minute<10?"0":"")<<start_minute<<" минут";

             // drawing
                draw_point.x=where->ul.x+internal_column_pos[0].from + system_settings_spider::COLUMN_LEFT_MARGIN;
                PgDrawText(station_text.c_str(), station_text.size(), &draw_point, Pg_TEXT_BOTTOM);

                draw_point.x = where->ul.x + internal_column_pos[1].from + system_settings_spider::COLUMN_LEFT_MARGIN;
				PgDrawText(device_text.str().c_str(), device_text.str().size(), &draw_point, Pg_TEXT_BOTTOM);

				draw_point.x = where->ul.x + internal_column_pos[2].from + system_settings_spider::COLUMN_LEFT_MARGIN;	
				PgDrawText(command_text.c_str(), command_text.size(), &draw_point, Pg_TEXT_BOTTOM);

				draw_point.x = where->ul.x + internal_column_pos[3].from + system_settings_spider::COLUMN_LEFT_MARGIN;	
				PgDrawText(time_text.str().c_str(), time_text.str().size(), &draw_point, Pg_TEXT_BOTTOM);

				count++;
				draw_point.y+=system_settings_spider::ROW_HEIGHT;
				timers_commands_iter++;
         }; //while ( count < nitems &&

 } catch (spider_exception spr_exc) { 
    spider_sys_sett->sys_message(system_settings::ERROR_MSG, 
         "In station_devices_properties_raw_list_draw_function: "+spr_exc.get_description());
     return ;
}; // catch (spider_exception spr_exc)

};

void station_devices_properties_raw_list_draw_function( PtWidget_t *widget,
            PtGenListItem_t *item,
            unsigned index,
            unsigned nitems,
            PhRect_t *where ){
vector<PtArg_t> args(1);
vector<char> tmp_chars(20);
PtListColumn_t *internal_column_pos=NULL;
int *internal_station_id;
string station_text, pref_command_text, start_command_text, execution_mode_text;
ostringstream device_text, time_text;
unsigned count;
PhPoint_t draw_point;
int start_hour, start_minute;

system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
if (spider_sys_sett==NULL) {
  cout<<"In station_devices_properties_raw_list_draw_function: Can`t get instance of system_settings_spider"<<endl;
  return;
};

try {
        system_settings::strings_container execution_modes_texts=spider_sys_sett->get_start_days_modes_russ_strings();
        system_settings::strings_container directions_strings=spider_sys_sett->get_directions_russ_strings();
        metro_stations_container *metro_stations=metro_stations_container::get_instance();
         metro_stations_container::iterator stations_iter;
        if (metro_stations==NULL)
                  throw spider_exception("Can`t get instance of metro_stations_container");

        metro_devices_container *metro_devices=metro_devices_container::get_instance();
         metro_devices_container::iterator devices_iter;
        if (metro_devices==NULL)
                  throw spider_exception("Can`t get instance of metro_devices_container");

        metro_devices_types_container *metro_devices_types=metro_devices_types_container::get_instance();
         metro_devices_types_container::iterator devices_types_iter;
        if (metro_devices_types==NULL)
                  throw spider_exception("Can`t get instance of metro_devices_types_container");


		PtGetResource(widget, Pt_ARG_USER_DATA, &internal_station_id, 0);
        stations_iter=metro_stations->find(*internal_station_id);
       if (stations_iter==metro_stations->end()) {
                ostringstream message;
                 message<<"Can`t find station with id "<<*internal_station_id;
                throw spider_exception (message.str());
            };

          station_text=stations_iter->second.get_stl_name_string();

	       PtGenListDrawBackground( widget, item, nitems, where, 0, 0, 0, 0 );

	        if (spider_sys_sett->small_font_prepared())	{
			      PgSetFont(  spider_sys_sett->get_small_font() );
		       } else {
			      spider_sys_sett->sys_message(system_settings::ERROR_MSG,
					"Not found system_settings::small_font for re-drawing log_items");
		        };

	        PtSetArg(&args[0], Pt_ARG_LIST_COLUMN_POS, &internal_column_pos, 0);
	        PtGetResources(widget,
							            args.size(),
							            &args[0]);

        count=0;
        draw_point.y=where->ul.y+system_settings_spider::ROW_HEIGHT;
        metro_station::iterator_devices_id device_in_station_id=stations_iter->second.begin_devices_id();
         advance(device_in_station_id, index-1);
   		PgSetTextColor(system_settings_spider::COLOR_BLUE);
	         while ( count < nitems &&
				         device_in_station_id!=stations_iter->second.end_devices_id()){
              device_text.str("");
              time_text.str("");
              execution_mode_text="";

              devices_iter=metro_devices->find(*device_in_station_id);
               if (devices_iter==metro_devices->end()) {
                    ostringstream message;
                    message<<"Not found device with id "<<*device_in_station_id<<" in station "<<*internal_station_id;
                     throw spider_exception(message.str());
               };

               devices_types_iter=metro_devices_types->find(devices_iter->second->get_type());
               if (devices_types_iter==metro_devices_types->end()) {
                    ostringstream message;
                    message<<"Not found device type with id "<<devices_iter->second->get_type()<<" for device with id "<<*device_in_station_id<<" in station "<<*internal_station_id;
                     throw spider_exception(message.str());
                   };

               device_text<<devices_types_iter->second->get_name();
               if (devices_iter->second->get_type_description()==metro_device_type::DEVICE_TYPE_ESCALATOR ||
                    devices_iter->second->get_type_description()==metro_device_type::DEVICE_TYPE_UDKU)
                    device_text<<" "<<devices_iter->second->get_number();

                 try { //for shavr and door throw exception
                    pref_command_text=""; 
                    start_command_text="";//in get_device_start_command(); for shavr and door throw exception, old value of command_text must be cleaned
                    command start_command=devices_iter->second->get_device_start_command();
                    start_command_text=start_command.get_command_description();

                     start_hour=devices_iter->second->get_start_hour();
                     start_minute=devices_iter->second->get_start_minute();
                     time_text<<(start_hour<10?"0":"")<<start_hour<<" часов : "<<(start_minute<10?"0":"")<<start_minute<<" минут";
                     execution_mode_text=execution_modes_texts[devices_iter->second->get_execution_mode()];


                    system_settings::strings_container::size_type pref_direction=100;
                    if (devices_iter->second->get_type_description()==metro_device_type::DEVICE_TYPE_ESCALATOR) {
                           metro_escalator *esc;
                            try {
                                 esc=dynamic_cast<metro_escalator*>(devices_iter->second);
                            } catch (bad_cast) {
                                 throw spider_exception("dynamic_cast<metro_escalator*>(devices_iter->second) is a bad cast");
                            };

                            pref_direction=esc->get_pref_direction();
                      }; //if (devices_iter->second->get_type_description()==metro_device_type::DEVICE_TYPE_ESCALATOR)

                    if (devices_iter->second->get_type_description()==metro_device_type::DEVICE_TYPE_UDKU) {
                           metro_udku *udku;
                            try {
                                 udku=dynamic_cast<metro_udku*>(devices_iter->second);
                            } catch (bad_cast) {
                                 throw spider_exception("dynamic_cast<metro_udku*>(devices_iter->second) is a bad cast");
                            };

                           pref_direction=udku->get_pref_direction();
                      }; //if (devices_iter->second->get_type_description()==metro_device_type::DEVICE_TYPE_UDKU)

                           if (pref_direction<directions_strings.size()) {
                              pref_command_text=directions_strings[pref_direction];
                            } else {
                               pref_command_text="NONE";
                            };


                  } catch (spider_exception) {
                  };
             // drawing
                draw_point.x=where->ul.x+internal_column_pos[0].from + system_settings_spider::COLUMN_LEFT_MARGIN;
                PgDrawText(station_text.c_str(), station_text.size(), &draw_point, Pg_TEXT_BOTTOM);

                draw_point.x = where->ul.x + internal_column_pos[1].from + system_settings_spider::COLUMN_LEFT_MARGIN;
				PgDrawText(device_text.str().c_str(), device_text.str().size(), &draw_point, Pg_TEXT_BOTTOM);

				draw_point.x = where->ul.x + internal_column_pos[2].from + system_settings_spider::COLUMN_LEFT_MARGIN;	
				PgDrawText(pref_command_text.c_str(), pref_command_text.size(), &draw_point, Pg_TEXT_BOTTOM);

				draw_point.x = where->ul.x + internal_column_pos[3].from + system_settings_spider::COLUMN_LEFT_MARGIN;	
				PgDrawText(start_command_text.c_str(), start_command_text.size(), &draw_point, Pg_TEXT_BOTTOM);

				draw_point.x = where->ul.x + internal_column_pos[4].from + system_settings_spider::COLUMN_LEFT_MARGIN;	
				PgDrawText(time_text.str().c_str(), time_text.str().size(), &draw_point, Pg_TEXT_BOTTOM);

				draw_point.x = where->ul.x + internal_column_pos[5].from + system_settings_spider::COLUMN_LEFT_MARGIN;	
				PgDrawText(execution_mode_text.c_str(), execution_mode_text.size(), &draw_point, Pg_TEXT_BOTTOM);

				count++;
				draw_point.y+=system_settings_spider::ROW_HEIGHT;
				device_in_station_id++;
         }; //while ( count < nitems &&

 } catch (spider_exception spr_exc) { 
    spider_sys_sett->sys_message(system_settings::ERROR_MSG, 
         "In station_devices_properties_raw_list_draw_function: "+spr_exc.get_description());
     return ;
}; // catch (spider_exception spr_exc)


};

