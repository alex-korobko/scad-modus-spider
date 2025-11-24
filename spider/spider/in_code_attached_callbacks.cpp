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
#include <hash_map>
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

#include "sound.h"

#include "command.h"
#include "command_pool_container.h"
#include "data_block.h"
#include "escalator_data_block.h"
#include "shavr_data_block.h"

#include "msg_types_container.h"
#include "msg_dict_container.h"
#include "log_records_container.h"

#include "devices_types.h"

#include "timer_command_container.h"
#include "metro_devices_types_container.h"
#include "metro_device.h"

#include "escalator_data_block.h"
#include "escalator.h"
#include "udku_data_block.h"
#include "udku.h"

class metro_devices_container;
#include "station.h"
#include "metro_stations_container.h"
#include "metro_devices_container.h"

#ifdef __UNIT_TESTING__

#include <cppunit/TestFixture.h> 
#include <cppunit/TestSuite.h> 
#include <cppunit/TestCaller.h>
#include <cppunit/ui/text/TestRunner.h> 

#include "command_test.h"
#include "metro_device_test.h"
#include "escalator_test.h"
#include "shavr_test.h"
#include "all_metro_devices_tests.h"

#endif

extern msg_dict_container *messages;
extern log_records_container *main_log;
extern log_records_container *archive_log;

extern string password_for_switching_sending_commands;
extern string password_for_switching_start_time_commands;
extern bool sending_commands_disabled;
extern bool setting_start_time_disabled;

//static functions
static void apply_sending_commands_to_devices(bool new_sending_commands_state)
{
   metro_devices_container* metro_devices=metro_devices_container::get_instance();
   metro_devices_container::iterator deviter = metro_devices->begin();
   while(deviter != metro_devices->end())
	{
		deviter->second->set_conduction_is_switched_off(new_sending_commands_state);
		++deviter;
	}
}

static int validate_password ( void *data,
              char const *password_entered ) {
if (password_for_switching_sending_commands.compare(password_entered)==0) {
              sending_commands_disabled=!sending_commands_disabled;
			  apply_sending_commands_to_devices(sending_commands_disabled);
              setting_start_time_disabled=true;
             return Pt_PWD_ACCEPT;
         };

if (password_for_switching_start_time_commands.compare(password_entered)==0) {
             if (sending_commands_disabled) {
                             sending_commands_disabled=false;
                              setting_start_time_disabled=false;
                } else  {
                             sending_commands_disabled=true;
                              setting_start_time_disabled=true;
                };
			 apply_sending_commands_to_devices(sending_commands_disabled);
             return Pt_PWD_ACCEPT;
         };

return Pt_PWD_REJECT;
};

//local functions
int
close_window( PtWidget_t *widget, 
                            ApInfo_t *apinfo, 
                           PtCallbackInfo_t *cbinfo ){

if (widget==NULL) return Pt_HALT;

PtWidget_t *window_to_destroy=PtGetParent(widget, PtWindow);
if (window_to_destroy!=NULL)
      PtDestroyWidget(window_to_destroy);
return( Pt_CONTINUE);
};


 int  activate_sending_commands_disabled_button_in_main_window
                                            (PtWidget_t *widget,
                                              ApInfo_t *apinfo,
                                              PtCallbackInfo_t *cbinfo){
vector<PtArg_t> args;
system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
if (spider_sys_sett==NULL) {
  cout<<"In activate_sending_commands_disabled_button_in_main_window: can`t get instance of system_settings_spider"<<endl;
  return( Pt_HALT);
};
char const *btns[] = { "&Отмена", "&ОК"};
const char* font_for_message=NULL;
 if (spider_sys_sett->large_font_prepared())
            font_for_message=spider_sys_sett->get_large_font();
string title;
string message;
if (sending_commands_disabled) {
       title="Включение управления";
       message="Введите пароль для активации управления";
    } else {
       title="Отключение управления";
       message="Введите пароль для отключения управления";
    };

//int result=
PtPassword( spider_sys_sett->get_main_window(),
                NULL,
                title.c_str(),
                spider_sys_sett->get_image(system_settings_spider::MSG_INFO),
                message.c_str(),
                font_for_message,
                btns,
                NULL,
                NULL,
                &validate_password,
                NULL,
                NULL,
                Pt_BLOCK_ALL );

////that in validate_password function
// if (result==Pt_PWD_ACCEPT) 
//       sending_commands_disabled=!sending_commands_disabled;

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

PtSetResources(widget, args.size(), &args[0]);
return Pt_CONTINUE;
};


int
activate_exit_button_in_main_window( PtWidget_t *widget, 
                                                                     ApInfo_t *apinfo, 
                                                                   PtCallbackInfo_t *cbinfo ){

ApDBase_t *widgets_db;
system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
if (spider_sys_sett==NULL) {
  cout<<"Can`t get instance of system_settings_spider"<<endl;
  return( Pt_HALT);
};
//spider_sys_sett->message_window(system_settings::INFO_MSG, "Exit");

char const *btns[] = { "&ОК", "&Отмена"};
const char* font_for_message=NULL;
 if (spider_sys_sett->large_font_prepared())
            font_for_message=spider_sys_sett->get_large_font();

if (PtAlert( spider_sys_sett->get_main_window(), NULL, "Выйти из программы", spider_sys_sett->get_image(system_settings_spider::MSG_INFO),
           "Выйти из программы ?",
             font_for_message,
              2, btns, NULL, 1, 2, Pt_BLOCK_ALL ) ==2) return( Pt_HALT);

try {
   main_log->save(spider_sys_sett->get_main_log_name());
} catch (spider_exception spr_exc) {
     spider_sys_sett->sys_message(system_settings::ERROR_MSG, "In main_log->save() :"+spr_exc.get_description());
};

  widgets_db=spider_sys_sett->get_widgets_dbase();
 if (widgets_db!=NULL) ApCloseDBase( widgets_db );
//  if (channel!=-1) ChannelDestroy(channel);

PtExit(EXIT_SUCCESS);
return( Pt_CONTINUE); //make compiler happy
};

int
activate_devices_parameters_button_in_main_window( PtWidget_t *widget, 
                                                                                     ApInfo_t *apinfo, 
                                                                   PtCallbackInfo_t *cbinfo ){
system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
if (spider_sys_sett==NULL) {
  cout<<"Can`t get instance of system_settings_spider"<<endl;
  return( Pt_HALT);
};
spider_sys_sett->message_window(system_settings::INFO_MSG, "Button disabled", true);
return( Pt_CONTINUE);
};

int resize_main_window( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo ) {
vector<PtArg_t> args(2);
unsigned int *new_window_width, *new_window_height, new_map_width, new_map_height;
PtWidget_t *map_container_widget;
system_settings_spider 	*spider_sys_sett=system_settings_spider::get_instance();
if (spider_sys_sett==NULL) {
  cout<<"Can`t get instance of system_settings_spider"<<endl;
  return( Pt_HALT);
};

PtSetArg(&args[0], Pt_ARG_WIDTH, &new_window_width, 0);
PtSetArg(&args[1], Pt_ARG_HEIGHT, &new_window_height, 0);
PtGetResources(widget, args.size(), &args[0]);

new_map_width=*new_window_width;
new_map_height=*new_window_height-system_settings_spider::TOP_PANEL_HEIGHT_IN_MAIN_WINDOW;

map_container_widget=spider_sys_sett->get_map_container();
if (map_container_widget==NULL) {
   spider_sys_sett->sys_message(system_settings::ERROR_MSG, "In resize_main_window : map_container_widget==NULL");
   return( Pt_HALT);
};

PtSetArg(&args[0], Pt_ARG_WIDTH, new_map_width, 0);
PtSetArg(&args[1], Pt_ARG_HEIGHT, new_map_height, 0);
PtSetResources(map_container_widget, args.size(), &args[0]);

return( Pt_CONTINUE);
};

int
close_device_panel_window( PtWidget_t *widget, ApInfo_t *apinfo, 
                 PtCallbackInfo_t *cbinfo ){

		metro_device::paneled_device_id=0;
         metro_device::device_panel=NULL;

		return( Pt_CONTINUE );
	};

int
activate_close_device_panel_button( PtWidget_t *widget, ApInfo_t *apinfo, 
                 PtCallbackInfo_t *cbinfo ){
		PtWidget_t *window_widget=PtGetParent(widget, PtWindow);

		if (window_widget!=NULL)
		            PtDestroyWidget(window_widget);

		return( Pt_CONTINUE );
	};


int activate_device_button( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo ){
		int *device_id;
		ostringstream exception_description;
 
		PtWidget_t *panel_window_widget;
		vector<PtCallback_t> callbacks;
		PtCallback_t tmp_callback;
        tmp_callback.event_f=close_device_panel_window;
        tmp_callback.data=NULL;
        callbacks.push_back(tmp_callback);
        tmp_callback.event_f=activate_close_device_panel_button;
        tmp_callback.data=NULL;
        callbacks.push_back(tmp_callback);

 metro_devices_container *metro_devices=metro_devices_container::get_instance();
 metro_devices_types_container *metro_devices_types=metro_devices_types_container::get_instance();
 metro_stations_container *metro_stations=metro_stations_container::get_instance();
 system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
 ApDBase_t *widgets_db=spider_sys_sett->get_widgets_dbase();

		PtGetResource(widget, Pt_ARG_USER_DATA, &device_id, 0);	
		metro_devices_container::iterator dev_iter=metro_devices->find(*device_id);
		try {
            if (dev_iter==metro_devices->end()) {
            		  exception_description<<"not found device with id : "<<*device_id;
                  throw spider_exception (exception_description.str());
            };

			if (metro_device::paneled_device_id!=*device_id &&
			    metro_device::device_panel!=NULL) {
							panel_window_widget = PtGetParent( metro_device::device_panel, PtWindow );
                            if (panel_window_widget==NULL)
                                 throw spider_exception("panel_window_widget==NULL");	
                            PtDestroyWidget(panel_window_widget);
			    };

			if (metro_device::paneled_device_id==*device_id &&
			    metro_device::device_panel!=NULL) {
                    return( Pt_CONTINUE );			    
			    };

			metro_devices_types_container::iterator type_iter=metro_devices_types->find(dev_iter->second->get_type());
			if (type_iter==metro_devices_types->end())
                      throw spider_exception("Device type not found");

             metro_stations_container::iterator station_iter=metro_stations->find(dev_iter->second->get_station_id());
			if (station_iter==metro_stations->end())
                      throw spider_exception("Station not found");

              metro_device::create_device_panel(widgets_db,
                                               station_iter->second.get_stl_name_string(),
                                               type_iter->second,
                                               dev_iter->second->get_data_block(),
                                               *device_id,
                                                dev_iter->second->get_number(),
                                               callbacks);

               dev_iter->second->update_device_panel (type_iter->second);

        } catch (spider_exception spr_exc) {
       			exception_description.str("");
       			exception_description<<"In activate_device_button(..) "<<spr_exc.get_description();
       			spider_sys_sett->sys_message(system_settings::ERROR_MSG, exception_description.str());
        };
		return( Pt_CONTINUE );
	};

void add_command_to_command_pool(const command& device_command)
{
   system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
   cmd_pool_container *cmd_pool_instance=cmd_pool_container::get_instance();
   if (cmd_pool_instance==NULL) {
      spider_sys_sett->sys_message(system_settings::ERROR_MSG, "In activate_device_menu_item (..): Can`t get instance  of cmd_pool_container");
  };

   cmd_pool_instance->push_back(device_command);
   try {
       cmd_pool_instance->prepare_to_display(); 
   } catch (spider_exception spr_exc) {
        spider_sys_sett->sys_message(system_settings::ERROR_MSG, "In activate_device_menu_item (..): In cmd_pool_instance->prepare_to_display() exception :"+spr_exc.get_description());
   };
}

void process_movement_command(const command& device_command, unsigned int previous_direction_code, unsigned int predefined_direction_code,
												 double time_diff, double predefined_delay)
{
       system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
       metro_stations_container *stations=metro_stations_container::get_instance();
       metro_stations_container::iterator statons_iter=stations->find(device_command.get_station());
       string prompt_title="Смена направления движения";
       string prompt_text="Эскалатор ";
       vector<char> tmp_chars(20);
       itoa(device_command.get_device_number(), &tmp_chars[0], 10);
      prompt_text+=&tmp_chars[0];
      if (statons_iter!=stations->end()) prompt_text+=string(" станция ")+statons_iter->second.get_stl_name_string();
      char const *buttons[] = { "Да", "Нет" };

//==========================
	           ostringstream excepiton_description;
 	          excepiton_description<<"In activate_device_menu_item (..): "
												<< " notif delay from config file "
												<< predefined_delay
												<< " diff in time "
												<< time_diff;

       			spider_sys_sett->sys_message(system_settings::INFO_MSG, excepiton_description.str());
//===========================

					string old_direction = ""	;
					string new_direction = "";
					bool prompt_is_necesssary  = false;
					if (time_diff <=predefined_delay)
						{
				             if (device_command.get_command_code() == system_settings::COMMAND_UP &&
  				                  previous_direction_code ==system_settings::DIRECTION_DOWN)
									{
									new_direction = "ПОДЪЕМ";
									old_direction = " работал  на СПУСК";
									prompt_is_necesssary  = true;
									};
       			                 if (device_command.get_command_code()==system_settings::COMMAND_DOWN &&
       			   		              previous_direction_code==system_settings::DIRECTION_UP) 
									{
										new_direction = "СПУСК";
										old_direction = " работал  на ПОДЪЕМ";
										prompt_is_necesssary  = true;
									};
						} else  if (predefined_direction_code != system_settings::DIRECTION_REVERSE){ //if (time_diff <=predefined_delay)
				             if (device_command.get_command_code() == system_settings::COMMAND_UP &&
  				                  predefined_direction_code ==system_settings::DIRECTION_DOWN)
									{
									new_direction = "ПОДЪЕМ";
									old_direction = " должен работать на СПУСК";
									prompt_is_necesssary  = true;
									};
       			                 if (device_command.get_command_code() ==system_settings::COMMAND_DOWN &&
       			   		              predefined_direction_code==system_settings::DIRECTION_UP) 
									{
										new_direction = "СПУСК";
										old_direction = " должен работать на ПОДЪЕМ";
										prompt_is_necesssary  = true;
									};
						};

						if (prompt_is_necesssary) 
							{
								prompt_text+= old_direction;
								prompt_text+="\nНо производится попытка создать команду ";
								prompt_text+= new_direction;
								prompt_text+="\nВы уверены?";
		                         if  (PtAlert( spider_sys_sett->get_main_window(),
  			                              NULL,
     			                           prompt_title.c_str(),
        			                        spider_sys_sett->get_image (system_settings_spider::MSG_WARN),
           				                     prompt_text.c_str(),
               				                 spider_sys_sett->get_small_font(),
                   				             2,
                       				          buttons,
                           			    	  NULL,
                                			 1,
                                 			2,
                                 			Pt_CENTER | Pt_BLOCK_ALL)==1) {
											add_command_to_command_pool(device_command);
                                   }; //if  (PtAlert( spider_sys_sett->get_main_window()
							}  else  {//prompt_is_necesssary
								add_command_to_command_pool(device_command);
							};	
}


int activate_device_menu_item( PtWidget_t *widget,
                                    void *client_data,
                                    PtCallbackInfo_t *cbinfo){
  vector<PtArg_t> args;
  command *callback_command
                   = static_cast<command*>(client_data);
command device_command=*callback_command;
delete(callback_command);
metro_devices_container::iterator dev_iter;
time_t curr_time = time(NULL);

   cmd_pool_container *cmd_pool_instance=cmd_pool_container::get_instance();
   if (cmd_pool_instance==NULL) {
      system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
      spider_sys_sett->sys_message(system_settings::ERROR_MSG, "In activate_device_menu_item (..): Can`t get instance  of cmd_pool_container");
  };

  metro_devices_container *metro_devices=metro_devices_container::get_instance();
  if (metro_devices==NULL){
           system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
           ostringstream excepiton_description;
           excepiton_description<<"In activate_device_menu_item can`t get instance of  metro_devices_container";
           spider_sys_sett->sys_message(system_settings::ERROR_MSG, excepiton_description.str());
          return (Pt_HALT);
     };

   dev_iter=metro_devices->find(device_command.get_device_id());
  if (dev_iter==metro_devices->end()) {
           system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
           ostringstream excepiton_description;
           excepiton_description<<"In activate_device_menu_item not found device with id "
                                                 <<device_command.get_device_id();
           spider_sys_sett->sys_message(system_settings::ERROR_MSG, excepiton_description.str());
           };

    if ( (dev_iter->second->get_type_description()==metro_device_type::DEVICE_TYPE_ESCALATOR ||
           dev_iter->second->get_type_description()==metro_device_type::DEVICE_TYPE_UDKU) &&
           (device_command.get_command_code()==system_settings::COMMAND_DOWN ||
           device_command.get_command_code()==system_settings::COMMAND_UP)) { 

			double time_diff = 0;
			unsigned int prev_direction_code, pref_direction_code;
			time_t curr_time = time(NULL);

             if (dev_iter->second->get_type_description()==metro_device_type::DEVICE_TYPE_ESCALATOR) {
                metro_escalator *esc_ptr=static_cast<metro_escalator*>(dev_iter->second);
				time_diff = difftime(curr_time, esc_ptr->get_previous_stop_time());
				prev_direction_code =  esc_ptr->get_previous_direction();
				pref_direction_code = esc_ptr->get_pref_direction();

			   process_movement_command(device_command, prev_direction_code, pref_direction_code,
																  time_diff, esc_ptr->get_conduction_notification_delay());
           }  else   if (dev_iter->second->get_type_description()==metro_device_type::DEVICE_TYPE_UDKU) { // if (dev_iter->second->get_type_description()==metro_device_type::DEVICE_TYPE_ESCALATOR) {
               metro_udku *udku_ptr=static_cast<metro_udku*>(dev_iter->second);
				time_diff = difftime(curr_time, udku_ptr->get_previous_stop_time());
				prev_direction_code =  udku_ptr->get_previous_direction();
				pref_direction_code = udku_ptr->get_pref_direction();

			   process_movement_command(device_command, prev_direction_code, pref_direction_code,
																  time_diff, udku_ptr->get_conduction_notification_delay());

			} else {  //}  else   if (dev_iter->second->get_type_description()==metro_device_type::DEVICE_TYPE_UDKU) 
             system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
		     ostringstream exception_description;
		     exception_description<<"In activate_device_menu_item impossible situation arose "
                                           <<" device id  "
                                           <<device_command.get_device_id();
		      spider_sys_sett->sys_message(system_settings::ERROR_MSG, exception_description.str());
            }; //  else   if (dev_iter->second->get_type_description()==metro_device_type::DEVICE_TYPE_UDKU) 


      } else if ((dev_iter->second->get_type_description()==metro_device_type::DEVICE_TYPE_ESCALATOR ||
           dev_iter->second->get_type_description()==metro_device_type::DEVICE_TYPE_UDKU) && 
         (device_command.get_command_code()==system_settings::COMMAND_CHANCEL ||
         device_command.get_command_code()==system_settings::COMMAND_ACCEPT )) {
          try {
             sound& snd=sound::get_instance();
			std::vector<string> *files  = new std::vector<string> ();
			files->push_back("sounds/ringdin.wav");
             snd.play(files);
             dev_iter->second->send_command(device_command);
          } catch (spider_exception spr_exc) {
             system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
		     ostringstream exception_description;
		     exception_description<<"In activate_device_menu_item raised exception "
                                           <<spr_exc.get_description()
                                           <<" device id  "
                                           <<device_command.get_device_id();
		      spider_sys_sett->sys_message(system_settings::ERROR_MSG, exception_description.str());
          };
      } else if ((dev_iter->second->get_type_description()==metro_device_type::DEVICE_TYPE_ESCALATOR ||
           dev_iter->second->get_type_description()==metro_device_type::DEVICE_TYPE_UDKU) && 
		 device_command.get_command_code()==system_settings::COMMAND_STOP) {

        cmd_pool_instance->push_back(device_command);
           try {
             cmd_pool_instance->prepare_to_display(); 
          } catch (spider_exception spr_exc) {
              system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
              spider_sys_sett->sys_message(system_settings::ERROR_MSG, "In activate_device_menu_item (..): In cmd_pool_instance->prepare_to_display() exception :"+spr_exc.get_description());
          };
    } else if (dev_iter->second->get_type_description()==metro_device_type::DEVICE_TYPE_SHAVR &&
                    device_command.get_command_code()!=system_settings::COMMAND_CHANCEL) {

        cmd_pool_instance->push_back(device_command);
           try {
             cmd_pool_instance->prepare_to_display(); 
          } catch (spider_exception spr_exc) {
              system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
              spider_sys_sett->sys_message(system_settings::ERROR_MSG, "In activate_device_menu_item (..): In cmd_pool_instance->prepare_to_display() exception :"+spr_exc.get_description());
          };

    } else if (dev_iter->second->get_type_description()==metro_device_type::DEVICE_TYPE_SHAVR &&
                    device_command.get_command_code()==system_settings::COMMAND_CHANCEL) {
          try {
             sound& snd=sound::get_instance();
			std::vector<string> *files  = new std::vector<string>();
			files->push_back("sounds/ringdin.wav");
             snd.play(files);
             dev_iter->second->send_command(device_command);
          } catch (spider_exception spr_exc) {
             system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
		     ostringstream exception_description;
		     exception_description<<"In activate_device_menu_item raised exception "
                                           <<spr_exc.get_description()
                                           <<" device id  "
                                           <<device_command.get_device_id();
		      spider_sys_sett->sys_message(system_settings::ERROR_MSG, exception_description.str());
          };
    } else if (dev_iter->second->get_type_description()==metro_device_type::DEVICE_TYPE_DOOR &&
                   device_command.get_command_code()==system_settings::COMMAND_ACCEPT) { 
          try {
             dev_iter->second->send_command(device_command);
          } catch (spider_exception spr_exc) {
             system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
		     ostringstream exception_description;
		     exception_description<<"In item raised exception "
                                           <<spr_exc.get_description()
                                           <<" device id  "
                                           <<device_command.get_device_id();
		      spider_sys_sett->sys_message(system_settings::ERROR_MSG, exception_description.str());
          };

    } else {  //  if ( dev_iter->second->get_type_description()==metro_device_type::DEVICE_TYPE_ESCALATOR &&
         system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
         ostringstream exception_description;
	     exception_description<<"In activate_device_menu_item(..) unrecognized command  with code "
                                             <<device_command.get_command_code()
                                             <<"  for device with id  "
                                             <<device_command.get_device_id();
		  spider_sys_sett->sys_message(system_settings::ERROR_MSG, exception_description.str());
     }; //    if ( dev_iter->second->get_type_description()==metro_device_type::DEVICE_TYPE_ESCALATOR &&

  return Pt_CONTINUE;
};


 int activate_device_menu (PtWidget_t *widget,
                                              ApInfo_t *apinfo,
                                              PtCallbackInfo_t *cbinfo){
  vector<PtArg_t> args;
  vector<command> appropriated_commands;
  vector<command>::iterator cmd_iter;
  string menu_title;
  int  device_id, *internal_device_id;
  PtWidget_t *menu_wgt;
  metro_devices_container *metro_devices=metro_devices_container::get_instance();
  metro_devices_container::iterator dev_iter;

  PtCallback_t tmp_callback;
  vector<PtCallback_t> callbacks;
  tmp_callback.event_f=activate_device_menu_item;

  args.clear();
  args.resize(1);

  PtSetArg(&args[0], Pt_ARG_USER_DATA, &internal_device_id, 0);
  PtGetResources(widget, args.size(), &args[0]);

  device_id=*internal_device_id;

  menu_title="Команды";

  dev_iter=metro_devices->find(device_id);
  if (dev_iter==metro_devices->end()) {
           system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
           ostringstream excepiton_description;
           excepiton_description<<"In activate_device_menu not found device with id "
                                                 <<device_id;
           spider_sys_sett->sys_message(system_settings::ERROR_MSG, excepiton_description.str());
           };


try {
    appropriated_commands=dev_iter->second->get_appropriated_commands();
  }  catch (spider_exception spr_exc) {
           system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
           spider_sys_sett->sys_message(system_settings::ERROR_MSG, string("In activate_device_menu : ")+spr_exc.get_description ());
  };

 if (appropriated_commands.empty()) {
        return Pt_CONTINUE;
    }

  args.clear();
  args.resize(1);

  PtSetArg (&args[0], Pt_ARG_MENU_TITLE, menu_title.c_str(), 0);
  menu_wgt= PtCreateWidget(PtMenu,
                                              Pt_NO_PARENT,
                                              args.size(),
                                              &args[0]);


cmd_iter=appropriated_commands.begin();
 while (cmd_iter!=appropriated_commands.end()) {

   args.clear();
   args.resize(3);

   callbacks.clear();
   tmp_callback.data=new command(*cmd_iter);
   callbacks.push_back(tmp_callback);

   string command_description;
   command_description=cmd_iter->get_command_description();
  
   PtSetArg (&args[0], Pt_ARG_LABEL_TYPE, Pt_Z_STRING , 0);
   PtSetArg (&args[1], Pt_ARG_TEXT_STRING, command_description.c_str(), 0);
   PtSetArg (&args[2],
                  Pt_CB_ACTIVATE,
                  &callbacks[0],
                  callbacks.size());

   PtCreateWidget(PtMenuButton,
                            menu_wgt,
                            args.size(),
                            &args[0]);
    
    cmd_iter++;
 }; //while (cmd_iter!=appropriated_commands.end())

 PtPositionMenu(menu_wgt, cbinfo->event);
 PtRealizeWidget(menu_wgt);

 return Pt_CONTINUE;
};


