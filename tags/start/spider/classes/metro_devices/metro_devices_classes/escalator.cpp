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
#include <iostream.h>

#include <vector>
#include <string>
#include <hash_map>
#include <set>
#include <list>
#include <functional>
#include <algorithm>
#include <sstream>
#include <iterator>

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
#include "log_records_container.h"
class metro_devices_container;
#include "line.h"
#include "metro_lines_container.h"
#include "station.h"
#include "metro_stations_container.h"
#include "metro_devices_types_container.h"

#include "sound.h"

#include "data_block.h"
#include "escalator_data_block.h"

#include "metro_device.h"
#include "metro_devices_container.h"
#include "escalator.h"

extern msg_dict_container *messages;
extern log_records_container *main_log;

//see main.cpp
extern bool sending_commands_disabled;
extern bool setting_start_time_disabled;

//see staton_devices_properties_callbacks.cpp
int
activate_ok_button_in_escalator_properties( PtWidget_t *widget, 
                                                                     ApInfo_t *apinfo, 
                                                                   PtCallbackInfo_t *cbinfo );


metro_escalator::metro_escalator(
		int id, 
		int id_station,
		int number,
		int type,
		int pref_direction,
	   	int	 start_day_mode,
	   	int	 start_hour,
	   	int	 start_minute,
        int channel,
		bool enabled,
		in_addr_t	ip) throw (spider_exception):

	metro_device( id,
                   id_station,
                   number,
                   type,
                   start_day_mode,
                   start_hour,
                   start_minute,
                   enabled,
                   ip,
                   channel),
   arrow(NULL),
   escalator_direction_combobox(NULL),
   escalator_start_hour(NULL),
   escalator_start_minute(NULL),
	tid(0),	
	pref_direction(pref_direction),
    A0_x2(true), //state accepted
	A0_x5(false),  //offline
    A0_x1(pref_direction), //predefined direction
    A0_state(A0_OFFLINE){};


	metro_escalator::~metro_escalator() {
       if (tid!=0) {
           pthread_cancel(tid);
		};
};

/*
Metro escalator  metods
*/
void metro_escalator::A0(int event)  throw (spider_exception){
 int old_automat_state=A0_state;

 switch (A0_state) {

    case A0_NOT_READY:
          if (!A0_x5) {
            A0_state=A0_OFFLINE;
            break;
            };
          if (!A0_x6) {
            A0_state=A0_EXCEPTION;
            break;
           };
          if (A0_x4 && !A0_x0) {
            A0_state=A0_READY_NOT_ACCEPTED;
            break;
            };
          if (A0_x0) {
           A0_state=A0_RUN;
            break;
            };
     break;

    case A0_READY_NOT_ACCEPTED:
          if (!A0_x5) {
            A0_state=A0_OFFLINE;
            break;
            };
          if (!A0_x6) {
            A0_state=A0_EXCEPTION;
            break;
           };
         if (!A0_x4)  {
            A0_state=A0_NOT_READY;
            break;
            };
         if (A0_x0)  {
            A0_state=A0_RUN;
            break;
            };
          switch (event) {
           case 3:
           if (A0_x3) {
               A0_state=A0_STARTING;
               A0_z3();
                break;
               };
            break;
           case 4:
             if (A0_x3) {
               A0_state=A0_STARTING;
               A0_z4();
               break;
               };
            break;
           case 6:
               A0_state=A0_READY;
            break;
          }; //switch (event)

    break;

    case A0_READY:
          if (!A0_x5) {
            A0_state=A0_OFFLINE;
            break;
            };
          if (!A0_x6) {
            A0_state=A0_EXCEPTION;
            break;
           };
         if (!A0_x4)  {
            A0_state=A0_NOT_READY;
            break;
            };
         if (A0_x0)  {
            A0_state=A0_RUN;
            break;
            };

          switch (event) {
           case 3:
             if (A0_x3) {
               A0_state=A0_STARTING;
               A0_z3();
               break;
               };
            break;
           case 4:
             if (A0_x3) {
               A0_state=A0_STARTING;
               A0_z4();
               break;
               };
            break;
          }; //switch (event)
    break;

    case A0_STARTING:
          if (!A0_x5) {
            A0_state=A0_OFFLINE;
            break;
            };
          if (!A0_x6) {
            A0_state=A0_EXCEPTION;
            break;
           };
         if (!A0_x4)  {
            A0_state=A0_NOT_READY;
            break;
            };
        if (A0_x0) {
            A0_state=A0_RUN;
            break;
            };
          switch (event) {
           case 6:
           if (A0_x3 && !A0_x0) {
               A0_state=A0_READY;
                break;
                };
             };
    break;

    case A0_RUN:
          if (!A0_x5) {
            A0_state=A0_OFFLINE;
            break;
            };
          if (!A0_x6) {
            A0_state=A0_EXCEPTION;
            break;
           };
         if (!A0_x4)  {
            A0_state=A0_NOT_READY;
            break;
            };
       if (!A0_x0) {
            A0_state=A0_READY;
            break;
            };

          switch (event) {
           case 5:
              if (A0_x3) {
                A0_state=A0_STOPPING;
                break;
                };
            break;
          }; //switch (event)
    break;

    case A0_STOPPING:
         if (!A0_x5) {
           A0_state=A0_OFFLINE;
           break;
         };
         if (!A0_x6) {
           A0_state=A0_EXCEPTION;
           break;
         };
        if (!A0_x4)  {
           A0_state=A0_NOT_READY;
           break;
         };
       if (!A0_x0) {
           A0_state=A0_READY;
           break;
         };

          switch (event) {
           case 6:
           if (A0_x3 && A0_x0) {
               A0_state=A0_RUN;
                break;
                };
             };

     break;

    case A0_OFFLINE:
         if (A0_x5 &&
            !A0_x6) {
           A0_state=A0_EXCEPTION;
           break;
         };
         if (A0_x5 &&
             !A0_x4) {
           A0_state=A0_NOT_READY;
           break;
         };
         if (A0_x3 && A0_x5 && !A0_x2 &&
             A0_x4 && !A0_x0) {
           A0_state=A0_READY_NOT_ACCEPTED;
           break;
         };
         if ((!A0_x3 || (A0_x3 && A0_x2)) && 
             A0_x5 && A0_x4 && !A0_x0) {
           A0_state=A0_READY;
           break;
         };
         if ( A0_x5 && A0_x0) {
           A0_state=A0_RUN;
           break;
         };

     break;

    case A0_EXCEPTION:
          if (!A0_x5) {
           A0_state=A0_OFFLINE;
           break;
         };
         if (A0_x6 &&
              !A0_x4) {
           A0_state=A0_NOT_READY;
           break;
         };
         if (!A0_x2 &&
              A0_x6 &&
              !A0_x0) {
           A0_state=A0_READY_NOT_ACCEPTED;
           break;
         };

         if (A0_x2 &&
              A0_x6 &&
              !A0_x0) {
           A0_state=A0_READY;
           break;
         };
         if (A0_x6 &&
             A0_x0) {
           A0_state=A0_RUN;
           break;
         };
       
    break;

    default :
      ostringstream exception_description;
      exception_description<<"metro_escalator::A0(...) device id "<<get_id();
      exception_description<<" A0 state "<<A0_state;
      throw spider_exception(exception_description.str());        
 }; //switch (A0_state)

 if (old_automat_state==A0_state) return;

//  cout<<"Escalator automate A0 state changed from "<<old_automat_state
//         <<" to "<<A0_state<<endl;

//  cout<<"A0_x0 "<<(A0_x0? "true": "false")<<endl;
//  cout<<"A0_x2 "<<(A0_x2? "true": "false")<<endl;
//  cout<<"A0_x3 "<<(A0_x3? "true": "false")<<endl;
//  cout<<"A0_x4 "<<(A0_x4? "true": "false")<<endl;
//  cout<<"A0_x5 "<<(A0_x5? "true": "false")<<endl;
//  cout<<"A0_x6 "<<(A0_x6? "true": "false")<<endl;

//initializations on enter to new state
 switch (A0_state) {
    case A0_NOT_READY:
    break;
    case A0_READY_NOT_ACCEPTED:
           A0_z6();
     break;
    case A0_READY:
           A0_z7();
          break;
    case A0_STARTING:
          break;
    case A0_RUN:
          break;
    case A0_STOPPING:
          break;
    case A0_OFFLINE:
          break;
    case A0_EXCEPTION:
          break;
    default :
      ostringstream exception_description;
      exception_description<<"metro_escalator::A0(...) device id "<<get_id();
      exception_description<<" after change A0 has undefined state "<<A0_state;
      throw spider_exception(exception_description.str());        
   }; //switch (A0_state)
};


PhRect_t metro_escalator::get_device_widget_extent() 
            throw (spider_exception){
	ostringstream exception_description;
	PhRect_t widget_rectangle, internal_widget_rectangle;

	exception_description<<"Can`t get device widget : device id "<<metro_device::get_id();
	if (metro_device::get_device_widget()==NULL) {
        throw spider_exception(exception_description.str());
	};
	
	PtExtentWidget(metro_device::get_device_widget());
	PtWidgetExtent( metro_device::get_device_widget(), &internal_widget_rectangle);
    widget_rectangle=internal_widget_rectangle;
	return widget_rectangle;
};

void metro_escalator::create_device_widget(int x,
                                                      int y,
                                                      ApDBase_t*  widgets_dbase,
                                                      PtWidget_t *station_widget,
                                                      vector<PtCallback_t> activate_callbacks,
                                                      vector<PtCallback_t> menu_callbacks) throw (spider_exception){
	vector<PtArg_t> args;
	vector<char> description(10);
	PtWidget_t *escalator_widget, *num_label;
	PhPoint_t position;

	if (widgets_dbase==NULL) {
    		throw spider_exception("metro_escalator::create_device_window(...) widgets_dbase pointer id NULL");		
	};

	if (station_widget==NULL) {
    		throw spider_exception("metro_escalator::create_device_window(...) station_widget pointer id NULL");		
	};

	metro_device::set_station_widget(station_widget);
	int device_id=metro_device::get_id();
	args.clear();
	args.resize(1);
	PtSetArg(&args[0], Pt_ARG_USER_DATA, &device_id, sizeof(device_id));
	position.x=x;
	position.y=y;

	escalator_widget=ApCreateDBWidget(widgets_dbase,
                                                               "escalator_widget",
                                                               station_widget,
                                                               &position,
                                                               args.size(),
                                                              &args[0]);

    if (escalator_widget==NULL)  {
   		throw spider_exception("metro_escalator::create_device_window(...) can`t create  widget escalator_widget");
    };

	args.clear();
	args.resize(5);
	PtSetArg(&args[0], Pt_ARG_USER_DATA, &device_id, sizeof(device_id));
	PtSetArg(&args[1],  Pt_CB_ACTIVATE,
			  &activate_callbacks[0],
			  activate_callbacks.size());
	PtSetArg(&args[2], Pt_CB_MENU,
                   &menu_callbacks[0], 
                   menu_callbacks.size());
	PtSetArg(&args[3], Pt_ARG_FLAGS, Pt_TRUE, Pt_MENUABLE);
	PtSetArg(&args[4], Pt_ARG_FLAGS, Pt_FALSE, Pt_ALL_BUTTONS);

    metro_device::set_device_widget(	escalator_widget);
	arrow = ApCreateDBWidget(widgets_dbase,
                                                "escalator_widget_panel_btn",
                                                escalator_widget,
                                                 NULL,
                                                 args.size(),
                                                 &args[0]);

    if (arrow==NULL)  {
    		throw spider_exception("metro_escalator::create_device_window(...) can`t get  widget escalator_widget_panel_btn");
    };

	itoa (	metro_device::get_number(),
	        &description[0],
			10);
	args.clear();
	args.resize(1);
	PtSetArg(&args[0], Pt_ARG_TEXT_STRING, &description[0], description.size());
	num_label =ApCreateDBWidget(widgets_dbase,
                                                "escalator_number_lbl",
                                                escalator_widget,
                                                 NULL,
                                                 args.size(),
                                                 &args[0]);
	
    if (num_label==NULL)  {
    		throw spider_exception("metro_escalator::create_device_window(...) can`t get  widget num_label");
    };
};

void metro_escalator::update_device_widget() throw (spider_exception) {
    system_settings_spider *sys_sett_obj=system_settings_spider::get_instance();
	vector<PtArg_t> args(3);
	ostringstream exception_description;

	if (! metro_device::get_enabled()) return;
	
	if (arrow==NULL) 
		throw spider_exception("metro_escalator::update_device_widget() arrow widget is null");

    switch (A0_state) {
       case A0_NOT_READY:
                 PtSetArg(&args[0],
                                Pt_ARG_LABEL_IMAGE,
                                sys_sett_obj->get_image(system_settings_spider::BLOCK),
                                 0);
       break;
       case A0_READY_NOT_ACCEPTED:
          if (A0_x3) {
                  switch (A0_x1) {
                       case system_settings::DIRECTION_UP:
                             PtSetArg(&args[0],
                                            Pt_ARG_LABEL_IMAGE,
                                            sys_sett_obj->get_image(system_settings_spider::RED_UP),
                                            0);
                        break;
                        case system_settings::DIRECTION_DOWN:
                              PtSetArg(&args[0],
                                             Pt_ARG_LABEL_IMAGE,
                                             sys_sett_obj->get_image(system_settings_spider::RED_DOWN),
                                             0);
                        break;
                        case system_settings::DIRECTION_REVERSE:
                                PtSetArg(&args[0],
                                              Pt_ARG_LABEL_IMAGE,
                                              sys_sett_obj->get_image(system_settings_spider::RED_REVERSE),
                                              0);
                         break;
                        default:
                                ostringstream exception_description;
                                exception_description<<"metro_escalator::update_device_widget() device id "<<get_id();
                                exception_description<<" A0_READY_NOT_ACCEPTED unknown escalator direction "<<A0_x1;
                                throw spider_exception(exception_description.str());
                            }; //switch (A0_x1)
              }else { // if (A0_x3)
                  switch (A0_x1) {
                       case system_settings::DIRECTION_UP:
                             PtSetArg(&args[0],
                                            Pt_ARG_LABEL_IMAGE,
                                            sys_sett_obj->get_image(system_settings_spider::RED_S_UP),
                                            0);
                        break;
                        case system_settings::DIRECTION_DOWN:
                              PtSetArg(&args[0],
                                             Pt_ARG_LABEL_IMAGE,
                                             sys_sett_obj->get_image(system_settings_spider::RED_S_DOWN),
                                             0);
                        break;
                        case system_settings::DIRECTION_REVERSE:
                                PtSetArg(&args[0],
                                              Pt_ARG_LABEL_IMAGE,
                                              sys_sett_obj->get_image(system_settings_spider::RED_REVERSE), //ATTENTION! : that widget equal to widget in DU mode !
                                              0);                //reverse direction now unused, so, be red 
                         break;
                        default:
                                ostringstream exception_description;
                                exception_description<<"metro_escalator::update_device_widget() device id "<<get_id();
                                exception_description<<" A0_READY_NOT_ACCEPTED unknown escalator direction "<<A0_x1;
                                throw spider_exception(exception_description.str());
                            }; //switch (A0_x1)
                    }; //if (A0_x3)
      break;
      case A0_READY:
                     if (A0_x3) {
                        switch (A0_x1) {
                              case system_settings::DIRECTION_UP:
                                                 PtSetArg(&args[0],
                                                   Pt_ARG_LABEL_IMAGE,
                                                  sys_sett_obj->get_image(system_settings_spider::YELLOW_UP),
                                                  0);
                                     break;
                              case system_settings::DIRECTION_DOWN:
                                                 PtSetArg(&args[0],
                                                 Pt_ARG_LABEL_IMAGE,
                                                 sys_sett_obj->get_image(system_settings_spider::YELLOW_DOWN),
                                                 0);
                                     break;
                             case system_settings::DIRECTION_REVERSE:
                                                 PtSetArg(&args[0],
                                                   Pt_ARG_LABEL_IMAGE,
                                                  sys_sett_obj->get_image(system_settings_spider::RED_REVERSE), //reverse direction now unused, so, be red 
                                                  0);
                                     break;
                            default:
                                   ostringstream exception_description;
                                   exception_description<<"metro_escalator::update_device_widget() device id "<<get_id();
                                   exception_description<<" A0_READY unknown escalator direction "<<A0_x1;
                                   throw spider_exception(exception_description.str());
                          }; //switch (A0_x1)
                    } else { // if (A0_x3) {
                          switch (A0_x1) {
                              case system_settings::DIRECTION_UP:
                                    PtSetArg(&args[0],
                                                  Pt_ARG_LABEL_IMAGE,
                                                  sys_sett_obj->get_image(system_settings_spider::YELLOW_S_UP),
                                                  0);
                                    break;
                               case system_settings::DIRECTION_DOWN:
                                     PtSetArg(&args[0],
                                                   Pt_ARG_LABEL_IMAGE,
                                                   sys_sett_obj->get_image(system_settings_spider::YELLOW_S_DOWN),
                                                   0);
                                     break;
                               case system_settings::DIRECTION_REVERSE:
                                      PtSetArg(&args[0],
                                                     Pt_ARG_LABEL_IMAGE,
                                                     sys_sett_obj->get_image(system_settings_spider::RED_REVERSE), //reverse direction now unused, so, be red  
                                                     0);
                                      break;
                               default:
                                  ostringstream exception_description;
                                  exception_description<<"metro_escalator::update_device_widget() device id "<<get_id();
                                   exception_description<<" A0_READY unknown escalator direction "<<A0_x1;
                                   throw spider_exception(exception_description.str());
                             }; //switch (A0_x1)
                       }; //if (A0_x3)
      break;
      case A0_STARTING:
                        if (A0_x2) {
                           switch (A0_x1) {
                              case system_settings::DIRECTION_UP:
                                   PtSetArg(&args[0],
                                                 Pt_ARG_LABEL_IMAGE,
                                                 sys_sett_obj->get_image(system_settings_spider::RED_UP_COMMAND),
                                                 0);
                                    break;
                             case system_settings::DIRECTION_DOWN:
                                    PtSetArg(&args[0],
                                                  Pt_ARG_LABEL_IMAGE,
                                                  sys_sett_obj->get_image(system_settings_spider::RED_DOWN_COMMAND),
                                                  0);
                                    break;
                            default:
                                   ostringstream exception_description;
                                   exception_description<<"metro_escalator::update_device_widget() device id "<<get_id();
                                   exception_description<<" A0_STARTING unknown escalator direction "<<A0_x1;
                                   throw spider_exception(exception_description.str());
                          }; //switch (A0_x1)
                  } else { //if (A0_x2)
                        switch (A0_x1) {
                          case system_settings::DIRECTION_UP:
                                    PtSetArg(&args[0],
                                               Pt_ARG_LABEL_IMAGE,
                                               sys_sett_obj->get_image(system_settings_spider::YELLOW_UP_COMMAND),
                                               0);
                                   break;
                         case system_settings::DIRECTION_DOWN:
                                   PtSetArg(&args[0],
                                                 Pt_ARG_LABEL_IMAGE,
                                                 sys_sett_obj->get_image(system_settings_spider::YELLOW_DOWN_COMMAND),
                                                 0);
                                  break;
                        default:
                                ostringstream exception_description;
                                exception_description<<"metro_escalator::update_device_widget() device id "<<get_id();
                                exception_description<<" A0_STARTING unknown escalator direction "<<A0_x1;
                                throw spider_exception(exception_description.str());
                         }; //switch (A0_x1)
                }; //if (A0_x2)
       break;
       case A0_RUN:
              if (A0_x3) {
                          switch (A0_x1) {
                             case system_settings::DIRECTION_UP:
                                    PtSetArg(&args[0],
                                                  Pt_ARG_LABEL_IMAGE,
                                                  sys_sett_obj->get_image(system_settings_spider::GREEN_UP),
                                                  0);
                                    break;
                             case system_settings::DIRECTION_DOWN:
                                     PtSetArg(&args[0],
                                                 Pt_ARG_LABEL_IMAGE,
                                                 sys_sett_obj->get_image(system_settings_spider::GREEN_DOWN),
                                                 0);
                                     break;
                           default:
                                ostringstream exception_description;
                                exception_description<<"metro_escalator::update_device_widget() device id "<<get_id();
                                exception_description<<" A0_RUN unknown escalator direction "<<A0_x1;
                                throw spider_exception(exception_description.str());
                           }; //switch (A0_x1)
                     } else { //if (A0_x3) {
                          switch (A0_x1) {
                              case system_settings::DIRECTION_UP:
                                    PtSetArg(&args[0],
                                                Pt_ARG_LABEL_IMAGE,
                                                sys_sett_obj->get_image(system_settings_spider::GREEN_S_UP),
                                                0);
                                   break;
                              case system_settings::DIRECTION_DOWN:
                                   PtSetArg(&args[0],
                                                Pt_ARG_LABEL_IMAGE,
                                                sys_sett_obj->get_image(system_settings_spider::GREEN_S_DOWN),
                                                0);
                                   break;
                              default:
                                   ostringstream exception_description;
                                   exception_description<<"metro_escalator::update_device_widget() device id "<<get_id();
                                   exception_description<<" A0_RUN unknown escalator direction "<<A0_x1;
                                   throw spider_exception(exception_description.str());
                             }; //switch (A0_x1)
                        }; //if (A0_x3) {
      break;
      case A0_STOPPING:
                          switch (A0_x1) {
                              case system_settings::DIRECTION_UP:
                                    PtSetArg(&args[0],
                                                  Pt_ARG_LABEL_IMAGE,
                                                  sys_sett_obj->get_image(system_settings_spider::GREEN_UP_COMMAND),
                                                  0);
                                    break;
                              case system_settings::DIRECTION_DOWN:
                                    PtSetArg(&args[0],
                                                Pt_ARG_LABEL_IMAGE,
                                               sys_sett_obj->get_image(system_settings_spider::GREEN_DOWN_COMMAND),
                                               0);
                                    break;
                            default:
                                  ostringstream exception_description;
                                  exception_description<<"metro_escalator::update_device_widget() device id "<<get_id();
                                  exception_description<<" A0_RUN unknown escalator direction "<<A0_x1;
                                  throw spider_exception(exception_description.str());
                           }; //switch (A0_x1)
       break;
       case A0_EXCEPTION: 
    	      PtSetArg(&args[0],
    	                    Pt_ARG_LABEL_IMAGE,
    	                    sys_sett_obj->get_image(system_settings_spider::EXCEPTION),
    	                    0);
      break;
      case A0_OFFLINE:
	      PtSetArg(&args[0],
	                    Pt_ARG_LABEL_IMAGE,
	                    sys_sett_obj->get_image(system_settings_spider::OFFLINE),
	                    0);
      break;

       default :
           ostringstream exception_description;
           exception_description<<"metro_escalator::update_device_widget() device id "<<get_id();
           exception_description<<" unknown A0_state "<<A0_state;
           throw spider_exception(exception_description.str());
      }; //switch (A0_state)

	PtSetResources(this->get_arrow_label_widget(), args.size(), &args[0]);
};

void metro_escalator::update_device_panel(metro_device_type *dev_type) throw (spider_exception){
 system_settings_spider *sys_sett=system_settings_spider::get_instance();
	if (metro_device::paneled_device_id!=metro_device::get_id())
      throw spider_exception ("metro_escalator::update_device_panel(...) metro_device::paneled_device_id!=metro_device::get_id()");

    if (metro_device::device_panel==NULL)
          throw spider_exception ("metro_escalator::update_device_panel(...) metro_device::device_panel==NULL");

	escalator_data_block local_data_block=data_block;
          
     metro_device_type::data_unit_iterator 
                    iter_data_unit=dev_type->data_units_begin();
     while (iter_data_unit!=dev_type->data_units_end()) {
          iter_data_unit->second->update_widget(&local_data_block,
                                                                         sys_sett,
                                                                         A0_x5 && A0_x6);
          iter_data_unit++;
     };
     
};

void metro_escalator::decode_answer_from_device_to_data_block(){
      metro_devices_types_container *metro_devices_types=metro_devices_types_container::get_instance();
      system_settings_spider *sys_sett=system_settings_spider::get_instance();
      metro_device::buffer_data_type answer_from_device=metro_device::get_answer_from_device_buffer();
      int old_A0_state=A0_state, old_A0_x1_state=A0_x1;
      bool old_A0_x3=A0_x3;

// previos_request_to_device
//============================================
/*
metro_device::buffer_data_type current_request_to_device=
                        metro_device::get_current_request_to_device_buffer();
if (metro_device::get_id()==2) {
 if (current_request_to_device.empty()) {
      cout<<"current_request_to_device is EMPTY"<<endl;
//   } else  if (current_request_to_device[1]!=4){ //if (previos_request_to_device.empty())
 }else {
     cout<<"current_request_to_device :"<<endl;
     vector<char> tmp_buffer(32);
     for (metro_device::buffer_data_type::size_type i=0;
     i<current_request_to_device.size();
//       i<4; //only header
       i++) {
      itoa(current_request_to_device[i],
              &tmp_buffer[0], 16);
      if (i%8==0) cout<<endl;
      cout<<"\t0x"<<&tmp_buffer[0];
    }; //for (metro_device::buffer_data_type::size_type i=0;
    cout<<endl;
    cout<<endl;
  };//if (current_request_to_device.empty())
};
*/
//============================================


// answer_from_device
//============================================
/*
if (metro_device::get_id()==12) {
if (answer_from_device.empty()) {
      cout<<"answer_from_device is EMPTY"<<endl;
//  } else if (answer_from_device [1]!=4) { //if (answer_from_device.empty())
} else {
 cout<<"answer_from_device id  "<<metro_device::get_id()<<" : "<<endl;
vector<char> tmp_buffer(32);
for (metro_device::buffer_data_type::size_type i=0;
       i<answer_from_device.size();
//      i<4; //only header
       i++) {
      itoa(answer_from_device[i],
              &tmp_buffer[0], 16);
      if (i==3 ||
          (i-3)%8==0) cout<<endl;
      cout<<"\t0x"<<&tmp_buffer[0];
    }; //for (metro_device::buffer_data_type::size_type i=0;
    cout<<endl;
    cout<<endl;
};//if (answer_from_device.empty())

}; //if (metro_device::get_id()==2)
*/
//============================================

//  if (metro_device::get_id()==1 &&metro_device::get_failures_count()>0)
//      cout<<"In escalator with id "<<metro_device::get_id()<<" failures count "<<metro_device::get_failures_count()<<" A0_state "<<A0_state<<endl;

        try {
		if (answer_from_device.empty()) {
                 if (metro_device::get_failures_count()>system_settings::RECONNECTS_TO_DEVICE_COUNT) {
                           A0_x5=false;
                           metro_device::set_last_message_remote_id(-1);
                       } else {
                           metro_device::set_failures_count(metro_device::get_failures_count()+1);
//                    if (metro_device::get_id()==1) 
//                           cout<<"Increment failures in answer_from_device.empty() "<<endl;
                       };
//            if (metro_device::get_id()==1) 
//                cout<<"In escalator with id "<<metro_device::get_id()<<" answer from device is empty "<<endl;

			 } else { //if (answer_from_device.empty()
                  if (( answer_from_device[system_settings::MODBUS_FUNCTION_CODE_INDEX] & 0x80)!=0) {
                         if (metro_device::get_failures_count()>system_settings::RECONNECTS_TO_DEVICE_COUNT ||
                              ! A0_x5)  {//if ecalator change state from offline to internal exception, it must be changed immediatly
//                           if (metro_device::get_id()==1) 
//                                cout<<"In escalator with id "<<metro_device::get_id()<<" returned error "<<static_cast<int>(answer_from_device[system_settings::MODBUS_FUNCTION_CODE_INDEX])<<endl;
                                A0_x6=false;
                                metro_device::set_failures_count(0);
                             } else  {
                                metro_device::set_failures_count(metro_device::get_failures_count()+1);
//                              if (metro_device::get_id()==1) 
//                                   cout<<"Increment failures in answer_from_device[system_settings::MODBUS_FUNCTION_CODE_INDEX] "<<endl;
                             };
                      } else {
                                A0_x6=true;
                                metro_device::set_failures_count(0);
                     };

		          A0_x5=true;

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
                old_A0_x3!=A0_x3 ||
                old_A0_x1_state!=A0_x1) {
               update_device_widget();
               PtFlush();
              }; // if (old_A0_state=A0_state)

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
                                                             error_description<<"In metro_escalator::decode_answer_from_device_to_data_block(...) not found device type id : "<<metro_device::get_type();
                                                             error_description<<" for device id "<<metro_device::get_id();
                                                             sys_sett->sys_message(system_settings::ERROR_MSG,  
                                                                                                           error_description.str());
                                                             return;
                                                          };
                    PtFlush();
                };


   } catch (spider_exception spr_exc) {
       ostringstream exception_description;
       exception_description<<"In metro_escalator::decode_answer_from_device_to_data_block device_id "<<get_id()<<" : ";
       exception_description<<spr_exc.get_description();
       sys_sett->sys_message(system_settings::ERROR_MSG,
                                             exception_description.str());
   };
};

metro_device::buffer_data_type 
         metro_escalator::get_request_for_send_to_device() {
 buffer_data_type return_buffer;
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

metro_device::buffer_data_type metro_escalator::get_4_function_request(){
system_settings_spider *sys_sett=system_settings_spider::get_instance();
	vector<byte> buffer(0);
	word		crc_value;

	buffer.push_back(get_number());   // escalator number
	buffer.push_back(4);                      // function number
	buffer.push_back(0);
	buffer.push_back(0);
	buffer.push_back(0);
	buffer.push_back(38);

	crc_value = sys_sett->crc(buffer);

	system_settings::bytes tmp_bytes=sys_sett->bytes_of_type<word>(crc_value);
	buffer.push_back(tmp_bytes[1]);
	buffer.push_back(tmp_bytes[0]);

//	metro_device::set_request_to_device_buffer(buffer);
   	return buffer;
};

metro_device::buffer_data_type metro_escalator::get_setting_time_request(){
    system_settings *sys_sett=system_settings_spider::get_instance();
	vector<byte> buffer(0);
	word		crc_value;
    struct tm *tm_local;
    time_t time_now=time(NULL);

	tm_local=localtime(&time_now);

  try{
      buffer.push_back(get_number());   // escalator number
      buffer.push_back(16);                      // function number

      buffer.push_back(00);                      // first register number high byte
      buffer.push_back(03);                      // first register number low byte

      buffer.push_back(00);                      // registers count high byte
      buffer.push_back(03);                      // registers count low byte

      buffer.push_back(6); //data bytes count

      buffer.push_back(static_cast<byte>(tm_local->tm_min));
      buffer.push_back(static_cast<byte>(tm_local->tm_sec));

      buffer.push_back(static_cast<byte>(tm_local->tm_mday));
       buffer.push_back(static_cast<byte>(tm_local->tm_hour));

      buffer.push_back(static_cast<byte>(tm_local->tm_year-100));
      buffer.push_back(static_cast<byte>(tm_local->tm_mon+1));

       crc_value = sys_sett->crc(buffer);

       system_settings::bytes tmp_bytes=system_settings::bytes_of_type<word>(crc_value);
       buffer.push_back(tmp_bytes[1]);
       buffer.push_back(tmp_bytes[0]);

      } catch (spider_exception spr_except) {
         sys_sett->sys_message(system_settings::ERROR_MSG,
                                                "In metro_escalator::get_setting_time_request() raised exception "+spr_except.get_description());
         buffer=get_4_function_request();
      };


//	metro_device::set_request_to_device_buffer(buffer);
   	return buffer;

};


void metro_escalator::send_command(command cmd)throw (spider_exception){
system_settings_spider* sys_sett=system_settings_spider::get_instance();
vector<byte> buffer(0);
word		crc_value;
int old_A0_state=A0_state;
ldword tmp_id;

	if (cmd.get_device_id()!=get_id())
          throw spider_exception("metro_escalator::send_command(...) cmd.get_device_id()!=get_id() ");

    if (sending_commands_disabled && 
         cmd.get_command_code() !=system_settings::COMMAND_ACCEPT)
                 return;

    switch (cmd.get_command_code()) {
	case (system_settings::COMMAND_UP):
         tmp_id=main_log->get_records_autoincrement();
         main_log->set_records_autoincrement(++tmp_id);

          main_log->insert(log_record(tmp_id,
                                     system_settings::DISPATCHER_SEND_CMD_UP,
                                     system_settings_spider::MESSAGE_TYPE_DISPATCHER_DOIT,
                                     cmd.get_station(),
                                     cmd.get_device_id(),
                                     time(NULL)));

             main_log->prepare_to_display();

          A0(3);
          break;
	case (system_settings::COMMAND_DOWN):
         tmp_id=main_log->get_records_autoincrement();
         main_log->set_records_autoincrement(++tmp_id);

          main_log->insert(log_record(tmp_id,
                                     system_settings::DISPATCHER_SEND_CMD_DOWN,
                                     system_settings_spider::MESSAGE_TYPE_DISPATCHER_DOIT,
                                     cmd.get_station(),
                                     cmd.get_device_id(),
                                     time(NULL)));

             main_log->prepare_to_display();

          A0(4);
          break;
	case (system_settings::COMMAND_STOP):
         tmp_id=main_log->get_records_autoincrement();
         main_log->set_records_autoincrement(++tmp_id);

          main_log->insert(log_record(tmp_id,
                                     system_settings::DISPATCHER_SEND_CMD_STOP,
                                     system_settings_spider::MESSAGE_TYPE_DISPATCHER_DOIT,
                                     cmd.get_station(),
                                     cmd.get_device_id(),
                                     time(NULL)));

             main_log->prepare_to_display();

          A0(5);
          break;
	case (system_settings::COMMAND_ACCEPT):
          A0(6);
          break;
      default:
        ostringstream exception_description;
        exception_description<<"metro_escalator::send_command unrecognized command ";
        exception_description<<cmd.get_command_code()<< " device id "<<get_id();
		 throw spider_exception(exception_description.str());      
    };  //switch (cmd.get_command_code())

  if (cmd.get_command_code()!=system_settings::COMMAND_ACCEPT) {
	  buffer.push_back(get_number());	//escalator number
	   buffer.push_back(6);		// modbus function
	   buffer.push_back(0);
	   buffer.push_back(0);
	   buffer.push_back(0);
	   buffer.push_back(cmd.get_command_code());
       crc_value = sys_sett->crc(buffer);
	   system_settings::bytes tmp_bytes=sys_sett->bytes_of_type<word>(crc_value);
	   buffer.push_back(tmp_bytes[1]);
	   buffer.push_back(tmp_bytes[0]);

	     metro_device::set_request_to_device_buffer(buffer);
    }; // if (cmd.get_command_code()!=system_settings::COMMAND_ACCEPT

    A0(7); // event 7 - data refreshed
    if (old_A0_state!=A0_state) {
              update_device_widget();
              PtFlush();
      };

};

void metro_escalator::decode_answer_from_device_4_function
                         (metro_device::buffer_data_type answer) throw (spider_exception){
   system_settings_spider *sys_sett=system_settings_spider::get_instance();
   metro_devices_types_container *dev_types_cont=metro_devices_types_container::get_instance();
   msg_dict_container *local_msg_dict;
   msg_dict_container::iterator msg_cont_iter;

   metro_devices_types_container::iterator dev_types_iter;
   metro_device_type::data_unit_iterator type_data_unit_iter;

   vector<log_record> received_messages;

   system_settings::strings_container parameters_names=sys_sett->get_parameters_in_messages_strings();

   escalator_data_block local_data_block=this->data_block;
   word begin_addr, registers_count=0, register_data_word=0, answer_messages_size=0, answer_last_message_remote_id=0;
   word mode_value, ready_value, direction_value, block_circut_status_value;
   metro_device::buffer_data_type::reverse_iterator tmp_iter1, tmp_iter2;
   ldword data_block_breaking_path_value;
   metro_device::buffer_data_type request_to_device = metro_device::get_current_request_to_device_buffer();

   if (request_to_device.empty())
              throw spider_exception("void metro_escalator::decode_answer_from_device_4_function :  prvios_request_to_device is empty ");
   
   tmp_iter1=request_to_device.rend();
   tmp_iter2=tmp_iter1;

   advance(tmp_iter1,
              -(system_settings::MODBUS_FUNCTION_4_REQUEST_BEGIN_ADDRESS_INDEX));
   advance(tmp_iter2,
                -(system_settings::MODBUS_FUNCTION_4_REQUEST_BEGIN_ADDRESS_INDEX+
                   system_settings::MODBUS_FUNCTION_4_REQUEST_BEGIN_ADDRESS_INCREM));

   begin_addr=sys_sett->type_from_bytes<word>(metro_device::buffer_data_type(tmp_iter2, tmp_iter1));
   begin_addr+=escalator_data_block::MODBUS_INPUT_REGISTERS_BEGIN_ADDRESS;

   tmp_iter1=request_to_device.rend();
   tmp_iter2=tmp_iter1;

   advance(tmp_iter1,
                  -(system_settings::MODBUS_FUNCTION_4_REQUEST_REGISTERS_COUNT_INDEX));
   advance(tmp_iter2,
                 -(system_settings::MODBUS_FUNCTION_4_REQUEST_REGISTERS_COUNT_INDEX+
                  system_settings::MODBUS_FUNCTION_4_REQUEST_REGISTERS_COUNT_INCREM));

    registers_count=
                   sys_sett->type_from_bytes<word>(metro_device::buffer_data_type(tmp_iter2, tmp_iter1));

	if (registers_count*2 !=answer[system_settings::MODBUS_DATA_BYTES_COUNT_INDEX]) {
        ostringstream exception_description;
        exception_description<<"In metro_escalator::decode_answer_from_device_4_function device id "<<get_id();
        exception_description<<" recieved "<<static_cast<int>(answer[system_settings::MODBUS_DATA_BYTES_COUNT_INDEX])<<" bytes of data ";
        exception_description<<" but must "<<registers_count*2;
        if (answer.size()>4) 
              exception_description<<" first three bytes: 0="<<static_cast<int>(answer[0])<<" 1="<<static_cast<int>(answer[1])<<" 2="<<static_cast<int>(answer[2]);
        throw spider_exception(exception_description.str());
	};

//move iteratiors to first register data
   tmp_iter1=answer.rend();
   tmp_iter2=tmp_iter1;
     advance(tmp_iter1,
                   -(system_settings::MODBUS_DATA_BYTES_COUNT_INDEX+2));
     advance(tmp_iter2,
                    -(system_settings::MODBUS_DATA_BYTES_COUNT_INDEX+3));

//poehali!
  if (begin_addr<= 30001&&
         begin_addr+registers_count>30001) {
			direction_value=
			     sys_sett->type_from_bytes<word>(system_settings::bytes (tmp_iter2, tmp_iter1));
            local_data_block.set_parameter_value(
                                      escalator_data_block::INDEX_PARAM_STATE_VALUE,
                                       direction_value
                                       );

			if (direction_value==2||
			    direction_value==4) {
			      A0_x0=true;
			      A0_x1=system_settings::DIRECTION_UP;
			      } else if (direction_value==3||
 			                    direction_value==5) {
			      A0_x0=true;
			      A0_x1=system_settings::DIRECTION_DOWN;
			    } else {
			      A0_x0=false;
			    };

            tmp_iter1=tmp_iter2;
            advance(tmp_iter2, -2);
    };

    if (begin_addr<= 30002&&
         begin_addr+registers_count>30002) {
           mode_value=
                sys_sett->type_from_bytes<word>( system_settings::bytes (tmp_iter2, tmp_iter1));
            local_data_block.set_parameter_value(
                              escalator_data_block::INDEX_PARAM_MODE_VALUE,
                               mode_value
                              );
            if (mode_value==3) { // main drive at remoute 
                     A0_x3=true;
               } else {
                     A0_x3=false;
               };

            tmp_iter1=tmp_iter2;
            advance(tmp_iter2, -2);
    };

     if (begin_addr<= 30003&&
         begin_addr+registers_count>30003) {
            ready_value=
                 sys_sett->type_from_bytes<word>( system_settings::bytes (tmp_iter2, tmp_iter1) );

            local_data_block.set_parameter_value(
                                     escalator_data_block::INDEX_PARAM_READY_VALUE,
                                      ready_value
                                      );

            tmp_iter1=tmp_iter2;
            advance(tmp_iter2, -2);
    };

     if (begin_addr<= 30004&&
         begin_addr+registers_count>30004) {
            block_circut_status_value= sys_sett->type_from_bytes<word>(system_settings::bytes (tmp_iter2, tmp_iter1) );
            local_data_block.set_signal_value(
                             escalator_data_block::INDEX_SIGNAL_BLOCK_CIRCUT_STATUS,
                             block_circut_status_value
                             );

           if (block_circut_status_value!=0) {
                 A0_x4=true;
           } else {
                A0_x4=false;
           };


            tmp_iter1=tmp_iter2;
            advance(tmp_iter2, -2);
    };

     if (begin_addr<= 30005&&
         begin_addr+registers_count>30005) {

            local_data_block.set_signal_value(
                             escalator_data_block::INDEX_SIGNAL_BREAKING_PATH_STATUS,
                             sys_sett->type_from_bytes<word>(system_settings::bytes (tmp_iter2, tmp_iter1))
                             );
            tmp_iter1=tmp_iter2;
            advance(tmp_iter2, -2);
    };

     if (begin_addr<= 30006&&
         begin_addr+registers_count>30006) {
            register_data_word=
                   sys_sett->type_from_bytes<word>( system_settings::bytes (tmp_iter2, tmp_iter1));

              for(int i=0;i<3;i++)
                       local_data_block.set_signal_value(
                                                escalator_data_block::INDEX_CHANNEL_A_STATUS+i,
                                               (register_data_word & (0xf<<i*4)) >>(i*4));

            tmp_iter1=tmp_iter2;
            advance(tmp_iter2, -2);
    };

     if (begin_addr<= 30007&&
         begin_addr+registers_count>30007) {
            answer_last_message_remote_id=sys_sett->type_from_bytes<word>( system_settings::bytes (tmp_iter2, tmp_iter1) );
//			cout<<"answer_last_message_remote_id "<<answer_last_message_remote_id<<endl;
           if (answer_last_message_remote_id>system_settings::MAX_UPPER_MESSAGE_ID_VALUE)
                    throw spider_exception ("In answer 3008 register answer_last_message_remote_id>system_settings::MAX_UPPER_MESSAGE_ID_VALUE");
            tmp_iter1=tmp_iter2;
            advance(tmp_iter2, -2);
    };

     if (begin_addr<= 30008&&
         begin_addr+registers_count>30008) {
            answer_messages_size=sys_sett->type_from_bytes<word>( system_settings::bytes (tmp_iter2, tmp_iter1) );
//			cout<<"answer_messages_size "<<answer_messages_size<<endl;
           if (answer_messages_size>system_settings::MAX_MESSAGES_COUNT)
                    throw spider_exception ("In answer 3007 register answer_messages_size>system_settings::MAX_MESSAGES_COUNT");
            tmp_iter1=tmp_iter2;
            advance(tmp_iter2, -2);
    };

     if (begin_addr<= 30009&&
         begin_addr+registers_count>30009) {
             //reserved  - add code here 
            tmp_iter1=tmp_iter2;
            advance(tmp_iter2, -2);
    };

     if (begin_addr<= 30010&&
         begin_addr+registers_count>=30011) {
           if (begin_addr<=30010 && begin_addr+registers_count==30011)
                         throw spider_exception ("In answer 30010 register present but not 30011 - it must be answered both");
            advance(tmp_iter2, -2); // 4 bytes must be readed

            local_data_block.set_parameter_value(
                        escalator_data_block::INDEX_PARAM_BREAKING_PATH_VALUE,
                        sys_sett->type_from_bytes<data_block::parameter_data_type>( system_settings::bytes (tmp_iter2, tmp_iter1) )
                        );
            tmp_iter1=tmp_iter2;
            advance(tmp_iter2, -2);
    };

     if (begin_addr<= 30012&&
         begin_addr+registers_count>=30013) {
           if (begin_addr<=30012 && begin_addr+registers_count==30013)
                         throw spider_exception ("In answer 30012 register present but not 30013 - it must be answered both");

            advance(tmp_iter2, -2); // 4 bytes must be readed
            local_data_block.set_parameter_value(
                        escalator_data_block::INDEX_PARAM_RUNNING_PATH_VALUE,
                        sys_sett->type_from_bytes<data_block::parameter_data_type>( system_settings::bytes (tmp_iter2, tmp_iter1) )
                        );
            tmp_iter1=tmp_iter2;
            advance(tmp_iter2, -2);
    };

     if (begin_addr<= 30014&&
         begin_addr+registers_count>30014) {
            //reserved : add code here
            tmp_iter1=tmp_iter2;
            advance(tmp_iter2, -2);
    };

     if (begin_addr<= 30015&&
         begin_addr+registers_count>30015) {
            //reserved : add code here
            tmp_iter1=tmp_iter2;
            advance(tmp_iter2, -2);
    };

//block circuts
     word current_register=30016;
     word current_signal_offset=escalator_data_block::INDEX_OF_BLOCK_CIRCUTS;
	 for (byte register_offset=0; register_offset<8; register_offset++){
	            if (begin_addr+registers_count<current_register+register_offset) break;

	            if (begin_addr<=current_register+register_offset&&
                begin_addr+registers_count>current_register+register_offset) {

                 register_data_word=sys_sett->type_from_bytes<word>(
                                                           system_settings::bytes (tmp_iter2, tmp_iter1)
                                                           );

                  for(int i=0;i<8;i++) {
                       local_data_block.set_signal_value(
                                                current_signal_offset+register_offset*8+i,
                                               (register_data_word & (0xC000>>i*2)) >>(14-i*2));
                    };
                  tmp_iter1=tmp_iter2;
                  advance(tmp_iter2, -2);
			    };

           if (data_block.get_signal_value(escalator_data_block::INDEX_OF_MASHZAL_DOOR)!=
                local_data_block.get_signal_value(escalator_data_block::INDEX_OF_MASHZAL_DOOR)) {
                 metro_stations_container *stations=metro_stations_container::get_instance();
                 metro_stations_container::iterator stations_iter;
                 metro_devices_container *devices=metro_devices_container::get_instance();
                 metro_devices_container::iterator devices_iter;
                 metro_device::buffer_data_type data_to_door;
                 system_settings::bytes tmp_bytes;
                  word crc_value;

                 if (stations!=NULL && devices!=NULL) {
                          stations_iter=stations->find(metro_device::get_station_id());
                          if (stations_iter!=stations->end()) {
                                metro_station::iterator_devices_id station_devices_ids=stations_iter->second.begin_devices_id();
                                while(station_devices_ids!=stations_iter->second.end_devices_id()) {
                                     devices_iter=devices->find(*station_devices_ids);
                                     if (devices_iter!=devices->end()) {
                                          if (devices_iter->second->get_type_description()==metro_device_type::DEVICE_TYPE_DOOR &&
                                               devices_iter->second->get_enabled()) {
                                             data_to_door.clear();
                                             data_to_door.push_back(devices_iter->second->get_number()); //modbus device number
                                             data_to_door.push_back(4); //function code
                                             data_to_door.push_back(2); //data bytes count
                                             data_to_door.push_back(0); //high byte
                                             //low byte
                                             if (local_data_block.get_signal_value(escalator_data_block::INDEX_OF_MASHZAL_DOOR)==escalator_data_block::SIGNAL_VALUE_RED)  {
                                                     data_to_door.push_back(1);
                                                   } else  {
                                                     data_to_door.push_back(0);
                                                   }
	                                          crc_value = sys_sett->crc(data_to_door);

	                                        tmp_bytes=sys_sett->bytes_of_type<word>(crc_value);
                                            data_to_door.push_back(tmp_bytes[1]);
                                            data_to_door.push_back(tmp_bytes[0]);
                                            devices_iter->second->set_answer_from_device_buffer(data_to_door);

                                           MsgSendPulse(devices_iter->second->get_connection_id(),
                                                                    system_settings_spider::PHOTON_THREAD_PULSE,
                                                                    system_settings_spider::PULSE_CODE_UPDATE_DEVICE,
                                                                     devices_iter->second->get_id());

                                          }; //if (devices_iter->second->get_type_description()
                                      }; //if (devices_iter=devices->end())
                                     station_devices_ids++;
                                }; //while(station_devices_ids!=stations_iter->second->end_devices_id()
                          }; //if (stations_iter=stations->end())
                      }; //if (stations!=NULL)
              }; //if (data_block.get_signal_value(escalator_data_block::INDEX_SIGNAL_MASHZAL_DOOR)!=

	 }

     if (begin_addr<= 30025&&
         begin_addr+registers_count>30025) {
            //reserved : add code here
            tmp_iter1=tmp_iter2;
            advance(tmp_iter2, -2);
    };


//contactors
           if (begin_addr<= 30026&&
                begin_addr+registers_count>30026) {

                 register_data_word=sys_sett->type_from_bytes<word>(
                                                           system_settings::bytes (tmp_iter2, tmp_iter1)
                                                           );

                  for(int i=0;i<8;i++) 
                       local_data_block.set_signal_value(
                                  escalator_data_block::INDEX_OF_CONTACTORS+i,
                                    (register_data_word & (0xC000>>i*2)) >>(14-i*2));

                  tmp_iter1=tmp_iter2;
                  advance(tmp_iter2, -2);

            };

//automats
           if (begin_addr<= 30027&&
                begin_addr+registers_count>30027) {
                 register_data_word=sys_sett->type_from_bytes<word>(
                                                           system_settings::bytes (tmp_iter2, tmp_iter1)
                                                           );


                  for(int i=0;i<8;i++) 
                       local_data_block.set_signal_value(escalator_data_block::INDEX_OF_AUTOMATS+i,
                                               (register_data_word & (0xC000>>i*2)) >>(14-i*2));

                  tmp_iter1=tmp_iter2;
                  advance(tmp_iter2, -2);
            };

//other
           if (begin_addr<= 30028&&
                begin_addr+registers_count>30028) {
                 register_data_word=sys_sett->type_from_bytes<word>(
                                                           system_settings::bytes (tmp_iter2, tmp_iter1)
                                                           );

                  for(int i=0; i<8; i++) 
                       local_data_block.set_signal_value(escalator_data_block:: INDEX_OF_OTHER+i,
                                               (register_data_word & (0xC000>>i*2)) >>(14-i*2));

                  tmp_iter1=tmp_iter2;
                  advance(tmp_iter2, -2);
            };

     if (begin_addr<= 30029&&
         begin_addr+registers_count>30029) {
            //reserved : add code here
            tmp_iter1=tmp_iter2;
            advance(tmp_iter2, -2);
    };

     if (begin_addr<= 30030&&
         begin_addr+registers_count>30030) {
            //reserved : add code here
            tmp_iter1=tmp_iter2;
            advance(tmp_iter2, -2);
    };

     if (begin_addr<= 30031&&
         begin_addr+registers_count>30031) {
            //reserved : add code here
            tmp_iter1=tmp_iter2;
            advance(tmp_iter2, -2);
    };

//messages
    //in initial state get_last_message_remote_id()==-1, so all messages must be skiped
    int upper_last_id=metro_device::get_last_message_remote_id();
    int answer_first_message_remote_id, current_message_id=0;

     if (upper_last_id<0) { //decode first packet after program starting
               metro_device::set_last_message_remote_id(answer_last_message_remote_id);
               upper_last_id=answer_last_message_remote_id;
              };

     answer_first_message_remote_id=answer_last_message_remote_id-answer_messages_size+1;
     if (answer_first_message_remote_id<0)
           answer_first_message_remote_id+=system_settings::MAX_UPPER_MESSAGE_ID_VALUE;

     current_register=30032;
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

          register_data_word=sys_sett->type_from_bytes<word>(system_settings::bytes (tmp_iter2, tmp_iter1) );
          current_message_id=answer_first_message_remote_id+register_offset;

            if (current_message_id>system_settings::MAX_UPPER_MESSAGE_ID_VALUE)
                   current_message_id-=system_settings::MAX_UPPER_MESSAGE_ID_VALUE;

            if (answer_first_message_remote_id<=answer_last_message_remote_id &&
                answer_first_message_remote_id<=upper_last_id &&
                upper_last_id<=answer_last_message_remote_id &&
                current_message_id<=upper_last_id)  {
                     tmp_iter1=tmp_iter2;
                     advance(tmp_iter2, -2);
                     continue;
                  };

            if (answer_first_message_remote_id>answer_last_message_remote_id &&
                answer_first_message_remote_id>=upper_last_id &&
                upper_last_id<=answer_last_message_remote_id &&
                ((current_message_id>answer_last_message_remote_id  && current_message_id>=answer_first_message_remote_id)||
                current_message_id<=upper_last_id )) {
                     tmp_iter1=tmp_iter2;
                     advance(tmp_iter2, -2);
                     continue;
                  };

            if (answer_first_message_remote_id>answer_last_message_remote_id &&
                answer_first_message_remote_id<=upper_last_id &&
                upper_last_id>answer_last_message_remote_id &&
                current_message_id>=answer_last_message_remote_id &&
                current_message_id<=upper_last_id) {
                   tmp_iter1=tmp_iter2;
                   advance(tmp_iter2, -2);
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


              if (!current_message_text.empty() &&
                   (static_cast<int>(current_message_text.find(parameters_names[system_settings::PARAMETER_NAME_BREAKING_PATH_VALUE]))!=-1)) {
                     data_block_breaking_path_value=local_data_block.get_parameter_value(escalator_data_block::INDEX_PARAM_BREAKING_PATH_VALUE);
                     message_id|=(data_block_breaking_path_value<<32);
                      };

                    received_messages.push_back(log_record(tmp_id,
                                                            message_id,
                                                            message_type_id,
                                                            get_station_id(),
                                                            get_id(),
                                                            time(NULL)));
		           // NOTICE: time(NULL) must be replaced by time from escalator

                  metro_device::set_last_message_remote_id(current_message_id);
                  tmp_iter1=tmp_iter2;
                  advance(tmp_iter2, -2);
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
        byte block_circut_index;
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
                     message<<stat_iter->second.get_stl_name_string()<<" Эскалатор "<<metro_device::get_number()<<"\n";
                    system_settings::strings_container parameters_names=
                                           sys_sett->get_parameters_in_messages_strings();
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
                                       msg_cont_iter=messages->find(static_cast<byte>(0xff&log_mess_iter->get_msg_id()));
                                       if(msg_cont_iter==messages->end()) {// if(msg_cont_iter!=messages->end())  if message type id not found, raise exception
                                                 message.str("");
                                                 message<<"In alerts generation not found message with id "<<log_mess_iter->get_msg_type();
                                                  throw spider_exception(message.str());
                                      }; //if(msg_cont_iter!=messages->end())
                                 }; //if (msg_cont_iter==messages->end()
                        }; //if (msg_cont_iter==type_messages->end()
                      message_from_messages_container=msg_cont_iter->second.get_text();
//---------------------------------begin message text generation------------------------------------

               if (!message_from_messages_container.empty()) {
                    find_pos=static_cast<int>(message_from_messages_container.find(parameters_names[system_settings::PARAMETER_NAME_BREAKING_PATH_VALUE]));
                    if (find_pos!=-1) {
                      breaking_path_value=static_cast<dword>(log_mess_iter->get_msg_id() >>32);
                      tmp_chars.clear();
                      tmp_chars.resize(12);
                      itoa (breaking_path_value, &tmp_chars[0], 10);
                      message_from_messages_container.replace( find_pos,
													parameters_names[system_settings::PARAMETER_NAME_BREAKING_PATH_VALUE].size(),
													&tmp_chars[0]);
                    };//if (find_pos!=-1)

                    find_pos=static_cast<int>(message_from_messages_container.find(parameters_names[system_settings::PARAMETER_NAME_BLOCK_CIRCUT_NAME]));

                    if (find_pos!=-1) {
                      block_circut_index=static_cast<byte>((log_mess_iter->get_msg_id()&0xff00)>>8);
//                      if (metro_device::get_id()==2)
//                                cout<<" current_message_id "<<current_message_id<<"  block_circut_index "<<block_circut_index<<endl;
                      block_circut_index+=6; //shift for signals, added from registers 3004, 3005,3006 and other

//==============================================
 {
          vector<char> tmp_chars(20);
          itoa(register_data_word, &tmp_chars[0],  16);
             cout<<"message_id 0x"<<&tmp_chars[0]<<endl;
             cout<<"block_circut_index "<<static_cast<int>(block_circut_index)<<endl;
}
//==============================================


                      metro_devices_types_container *devices_types=metro_devices_types_container::get_instance();
                      metro_devices_types_container::iterator iter_devices_types=devices_types->find(metro_device::get_type());
    				  if (iter_devices_types!=devices_types->end()) {
                             type_data_unit_iter=iter_devices_types->second->data_units_find(block_circut_index);
                             //ATENTION:index of block circut equals to id, so send to find block circut index
                             if (type_data_unit_iter->second->get_index()==block_circut_index) {
                                        block_circut_name=type_data_unit_iter->second->get_name();
                                    } else { //if (type_data_unit_iter.second.get_index()==block_circut_index)
                                          block_circut_name="ERROR!!";
                                          ostringstream mess;
                                          mess<<"Present PARAMETER_NAME_BLOCK_CIRCUT_NAME but type_data_unit_iter->second->get_index()!=block_circut_index block_circut_index "
                                                   <<block_circut_index<<" message id in log"<<current_message_id;
                                           throw spider_exception(mess.str());
                                   }; //if (type_data_unit_iter.second.get_index()==block_circut_index)
                       } else  { // if (iter_devices_types!=devices_types->end())
                                             block_circut_name="ERROR!!";
                                             ostringstream mess;
                                             mess<<"Present PARAMETER_NAME_BLOCK_CIRCUT_NAME but iter_devices_types!=devices_types->end() message id in log"<<current_message_id;
                                             throw spider_exception(mess.str());
                         }; // if (iter_devices_types!=devices_types->end())

                      message_from_messages_container.replace( find_pos,
												   parameters_names[system_settings::PARAMETER_NAME_BLOCK_CIRCUT_NAME].size(),
												   block_circut_name);
                    };//if (find_pos!=-1)
                };

//-----------------------------------end message text generation-----------------------------------
                       message<<message_from_messages_container;
                        sys_sett->message_window(system_settings::ERROR_MSG,
                                                                       message.str(),
                                                                        true);
					 sound& snd=sound::get_instance();
                     snd.play(stat_iter->second.get_wav_file_name());

                 }; // if (log_mess_iter->get_msg_type()==escalator_data_block::ALERT_ERROR_MESSAGES_TYPE)
                 log_mess_iter++;
               }; // while(log_mess_iter==received_messages.rend())
       }; // if (!received_messages.empty())

};

command metro_escalator::get_device_pref_command() throw(spider_exception) {
 if (pref_direction==system_settings::DIRECTION_DOWN )
    return 	command (metro_device::get_id(),
	                 metro_device::get_type(),
	                 system_settings::COMMAND_DOWN,
	                 metro_device::get_station_id(),
	                 metro_device::get_number(),
                     "СПУСК" );

 if (pref_direction==system_settings::DIRECTION_UP)
    return 	command (metro_device::get_id(),
	                 metro_device::get_type(),
	                 system_settings::COMMAND_UP,
	                 metro_device::get_station_id(),
	                 metro_device::get_number(),
                     "ПОДЪЕМ"); 

  throw spider_exception("Cant get prf command - reverse escalator");
}


vector<command>
     metro_escalator::get_appropriated_commands(){
     vector<command> commands_to_return; 

          if (A0_x3) {
            if (pref_direction==system_settings::DIRECTION_DOWN ) {
                  commands_to_return.push_back(
 	                command (metro_device::get_id(),
	                   metro_device::get_type(),
	                   system_settings::COMMAND_DOWN,
	                   metro_device::get_station_id(),
	                   metro_device::get_number(),
                       "СПУСК")
                    );
              } else  if (pref_direction==system_settings::DIRECTION_UP ) {
                commands_to_return.push_back(
 	              command (metro_device::get_id(),
	                 metro_device::get_type(),
	                 system_settings::COMMAND_UP,
	                 metro_device::get_station_id(),
	                 metro_device::get_number(),
                     "ПОДЪЕМ")
                  );
             };

            commands_to_return.push_back(
 	           command (metro_device::get_id(),
	                 metro_device::get_type(),
	                 system_settings::COMMAND_STOP,
	                 metro_device::get_station_id(),
	                 metro_device::get_number(),
                     "СТОП")
              );
        }; //if (A0_x3)

     if (A0_state==A0_READY_NOT_ACCEPTED)
            commands_to_return.push_back(
 	           command (metro_device::get_id(),
	                 metro_device::get_type(),
	                 system_settings::COMMAND_ACCEPT, 
	                 metro_device::get_station_id(),
	                 metro_device::get_number(),
                     "КВИТИРОВАТЬ")
              );

    return commands_to_return;
};

void 
   metro_escalator::create_properties_widgets
                                   (PtWidget_t *parent_widget){
//clear parent
PtWidget_t *current_widget;
vector<PtCallback_t> callbacks;
vector<PtWidget_t*>  widgets;
PtCallback_t tmp_callback;
vector<PtArg_t> args;
PhPoint_t widget_position;
ostringstream device_text;

int result ,device_id, 
      parent_widget_width, widgets_y_position=2,
      *internal_parent_widget_width;

current_widget=parent_widget;
result=Pt_TRAVERSE_START;
while( ( result=PtWidgetTree( parent_widget, &current_widget, result) ) !=
       Pt_TRAVERSE_DONE)
           if (current_widget!=parent_widget)
               widgets.push_back(current_widget);

 vector<PtWidget_t*>::iterator iter_widgets=widgets.begin();
     while (iter_widgets!=widgets.end()) {
         PtDestroyWidget(*iter_widgets);
         iter_widgets++;
      };

system_settings_spider* sys_sett_obj=system_settings_spider::get_instance();
if (sys_sett_obj==NULL) { 
           cout<<"Can`t get instance of system_settins_spider"<<endl;
           return;
    };

//create new widgets
try {
      if (parent_widget==NULL)
             throw spider_exception("parent_widget==NULL");

      PtGetResource(parent_widget, Pt_ARG_WIDTH, &internal_parent_widget_width, 0);
      parent_widget_width=*internal_parent_widget_width;
      device_text<<"Эскалатор "<<metro_device::get_number()<<" команда эскалатору";

      //label with device description
		args.clear();
		args.resize(3);
          widget_position.x=5;
          widget_position.y=widgets_y_position;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_TEXT_STRING,
                       device_text.str().c_str(), device_text.str().size());
         PtSetArg(&args[2], Pt_ARG_LABEL_TYPE,
                       Pt_Z_STRING,0);

         current_widget=PtCreateWidget(PtLabel,
                                                     parent_widget,
                                                     args.size(),
                                                     &args[0]);
         if (current_widget==NULL)
                  throw spider_exception("label with device text is NULL");
         PtRealizeWidget(current_widget);

       //combobox with commands
		args.clear();
		args.resize(5);
		PtSetArg(&args[0], Pt_ARG_FLAGS, Pt_FALSE, Pt_GHOST);
		PtSetArg(&args[1], Pt_ARG_FLAGS, Pt_FALSE, Pt_BLOCKED);
		PtSetArg(&args[2], Pt_ARG_WIDTH, 120, 0);
        widget_position.x=220;
        widget_position.y=widgets_y_position;
        PtSetArg(&args[3], Pt_ARG_POS,
                       &widget_position, 0);
        PtSetArg(&args[4], Pt_ARG_TEXT_FLAGS,
                       Pt_FALSE, Pt_EDITABLE);

        current_widget=PtCreateWidget(PtComboBox,
                                                     parent_widget,
                                                     args.size(),
                                                     &args[0]);
        if (current_widget==NULL)
                  throw spider_exception("combobox with appropriated commands is NULL");


            system_settings::strings_container directions_strings=sys_sett_obj->get_directions_russ_strings();
            escalator_direction_combobox=current_widget;

           directions_strings[system_settings::DIRECTION_UP]+="   ";
           directions_strings[system_settings::DIRECTION_DOWN]+="   ";

			const char *items[2];
			items[0]=directions_strings[system_settings::DIRECTION_UP].c_str();
			items[1]=directions_strings[system_settings::DIRECTION_DOWN].c_str();

            PtListAddItems(current_widget,
									items,
									2,
									0);

            if (pref_direction==system_settings::DIRECTION_UP||
                 pref_direction==system_settings::DIRECTION_DOWN)
                     PtSetResource(current_widget,
									Pt_ARG_CBOX_SEL_ITEM,
									pref_direction,
									0);

        PtRealizeWidget(current_widget);

        //label  of start mode
		args.clear();
		args.resize(3);
          widget_position.x=350;
          widget_position.y=widgets_y_position;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_TEXT_STRING,
                       "тип запуска", 0);
         PtSetArg(&args[2], Pt_ARG_LABEL_TYPE,
                       Pt_Z_STRING,0);
         current_widget=PtCreateWidget(PtLabel,
                                                     parent_widget,
                                                     args.size(),
                                                     &args[0]);
         if (current_widget==NULL)
                  throw spider_exception("label with device text is NULL");
         PtRealizeWidget(current_widget);

       //combobox with starting mode
		args.clear();
		args.resize(5);
		PtSetArg(&args[0], Pt_ARG_FLAGS, Pt_FALSE, Pt_GHOST);
		PtSetArg(&args[1], Pt_ARG_FLAGS, Pt_FALSE, Pt_BLOCKED);
		PtSetArg(&args[2], Pt_ARG_WIDTH, 150, 0);
        widget_position.x=430;
        widget_position.y=widgets_y_position;
        PtSetArg(&args[3], Pt_ARG_POS,
                       &widget_position, 0);
        PtSetArg(&args[4], Pt_ARG_TEXT_FLAGS,
                       Pt_FALSE, Pt_EDITABLE);

        current_widget=PtCreateWidget(PtComboBox,
                                                     parent_widget,
                                                     args.size(),
                                                     &args[0]);
        if (current_widget==NULL)
                  throw spider_exception("combobox with appropriated commands is NULL");

            system_settings::strings_container start_day_modes=sys_sett_obj->get_start_days_modes_russ_strings();
            escalator_start_mode_combobox=current_widget;


            start_day_modes[system_settings::START_DAY_MODE_EVERYDAY]+="        ";
			start_day_modes[system_settings::START_DAY_MODE_NEVER]+="         ";
			start_day_modes[system_settings::START_DAY_MODE_ODD]+="          ";
			start_day_modes[system_settings::START_DAY_MODE_EVEN]+="         ";

			const char *start_day_items[system_settings::START_DAYS_MODES_COUNT];
			start_day_items[system_settings::START_DAY_MODE_EVERYDAY]=start_day_modes[system_settings::START_DAY_MODE_EVERYDAY].c_str();
			start_day_items[system_settings::START_DAY_MODE_NEVER]=start_day_modes[system_settings::START_DAY_MODE_NEVER].c_str();
			start_day_items[system_settings::START_DAY_MODE_ODD]=start_day_modes[system_settings::START_DAY_MODE_ODD].c_str();
			start_day_items[system_settings::START_DAY_MODE_EVEN]=start_day_modes[system_settings::START_DAY_MODE_EVEN].c_str();

            PtListAddItems(current_widget,
									start_day_items,
									system_settings::START_DAYS_MODES_COUNT,
									0);

             PtSetResource(current_widget,
									Pt_ARG_CBOX_SEL_ITEM,
									metro_device::get_execution_mode()+1,
									0);

        PtRealizeWidget(current_widget);

        //label  of start hour
		args.clear();
		args.resize(3);
          widget_position.x=5;
          widget_position.y=widgets_y_position+25;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_TEXT_STRING,
                       "Запуск в", 0);
         PtSetArg(&args[2], Pt_ARG_LABEL_TYPE,
                       Pt_Z_STRING,0);
         current_widget=PtCreateWidget(PtLabel,
                                                     parent_widget,
                                                     args.size(),
                                                     &args[0]);
         if (current_widget==NULL)
                  throw spider_exception("label with start hour is NULL");
         PtRealizeWidget(current_widget);

        //numeric input for start hour
		args.clear();

     if (setting_start_time_disabled) {
		  args.resize(8);
          widget_position.x=70;
          widget_position.y=widgets_y_position+25;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_NUMERIC_VALUE,
                       metro_device::get_start_hour(), 0);
         PtSetArg(&args[2], Pt_ARG_NUMERIC_MIN,
                       0,0);
         PtSetArg(&args[3], Pt_ARG_NUMERIC_MAX,
                       23,0);
         PtSetArg(&args[4], Pt_ARG_NUMERIC_SUFFIX,
                       "  ч ",0);
         PtSetArg(&args[6], Pt_ARG_FLAGS,
                       Pt_TRUE, Pt_BLOCKED);
         PtSetArg(&args[7], Pt_ARG_FLAGS,
                       Pt_TRUE, Pt_GHOST);
          } else {
		  args.resize(5);
          widget_position.x=70;
          widget_position.y=widgets_y_position+25;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_NUMERIC_VALUE,
                       metro_device::get_start_hour(), 0);
         PtSetArg(&args[2], Pt_ARG_NUMERIC_MIN,
                       0,0);
         PtSetArg(&args[3], Pt_ARG_NUMERIC_MAX,
                       23,0);
         PtSetArg(&args[4], Pt_ARG_NUMERIC_SUFFIX,
                       "  ч ",0);
         };
         current_widget=PtCreateWidget(PtNumericInteger,
                                                     parent_widget,
                                                     args.size(),
                                                     &args[0]);
         if (current_widget==NULL)
                  throw spider_exception("label with start hour is NULL");
         escalator_start_hour=current_widget;
         PtRealizeWidget(current_widget);

        //label  of start minute
		args.clear();
		args.resize(3);
          widget_position.x=130;
          widget_position.y=widgets_y_position+25;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_TEXT_STRING,
                       "  : ", 0);
         PtSetArg(&args[2], Pt_ARG_LABEL_TYPE,
                       Pt_Z_STRING,0);
         current_widget=PtCreateWidget(PtLabel,
                                                     parent_widget,
                                                     args.size(),
                                                     &args[0]);
         if (current_widget==NULL)
                  throw spider_exception("label with start minute is NULL");
         PtRealizeWidget(current_widget);

        //numeric input for start minute
		args.clear();
     if (setting_start_time_disabled) {
		   args.resize(8);
          widget_position.x=150;
          widget_position.y=widgets_y_position+25;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_NUMERIC_VALUE,
                       metro_device::get_start_minute(), 0);
         PtSetArg(&args[2], Pt_ARG_NUMERIC_MIN,
                       0,0);
         PtSetArg(&args[3], Pt_ARG_NUMERIC_MAX,
                       60,0);
         PtSetArg(&args[4], Pt_ARG_NUMERIC_SUFFIX,
                       " мин    ",0);
         PtSetArg(&args[5], Pt_ARG_WIDTH,
                       50,0);
         PtSetArg(&args[6], Pt_ARG_FLAGS,
                       Pt_TRUE, Pt_BLOCKED);
         PtSetArg(&args[7], Pt_ARG_FLAGS,
                       Pt_TRUE, Pt_GHOST);
           } else { //if (setting_start_time_disabled)

		 args.resize(6);
          widget_position.x=150;
          widget_position.y=widgets_y_position+25;
         PtSetArg(&args[0], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[1], Pt_ARG_NUMERIC_VALUE,
                       metro_device::get_start_minute(), 0);
         PtSetArg(&args[2], Pt_ARG_NUMERIC_MIN,
                       0,0);
         PtSetArg(&args[3], Pt_ARG_NUMERIC_MAX,
                       59,0);
         PtSetArg(&args[4], Pt_ARG_NUMERIC_SUFFIX,
                       " мин    ",0);
         PtSetArg(&args[5], Pt_ARG_WIDTH,
                       50,0);
           }; //if (setting_start_time_disabled)

         current_widget=PtCreateWidget(PtNumericInteger,
                                                     parent_widget,
                                                     args.size(),
                                                     &args[0]);
         if (current_widget==NULL)
                  throw spider_exception("label with start hour is NULL");
         escalator_start_minute=current_widget;

         PtRealizeWidget(current_widget);

       //button ok
		args.clear();
		args.resize(6);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, 30,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, 60,0);
          widget_position.x=parent_widget_width-60-30;
          widget_position.y=widgets_y_position;
         PtSetArg(&args[2], Pt_ARG_POS,
                     &widget_position, 0);
         PtSetArg(&args[3], Pt_ARG_TEXT_STRING,
                       "Сохранить", 0);
		callbacks.clear();
		tmp_callback.event_f=activate_ok_button_in_escalator_properties;
        tmp_callback.data=NULL;
		callbacks.push_back(tmp_callback);
         PtSetArg(&args[4], Pt_CB_ACTIVATE,
                        &callbacks[0],
                        callbacks.size());
         device_id=get_id();
         PtSetArg(&args[5], Pt_ARG_USER_DATA,
                       &device_id, sizeof(device_id));

         current_widget=PtCreateWidget(PtButton,
                                                     parent_widget,
                                                     args.size(),
                                                     &args[0]);
         if (current_widget==NULL)
                  throw spider_exception("button \"OK\" in buttons_panel is NULL");

         PtRealizeWidget(current_widget);
 } catch (spider_exception spr_exc) {
      sys_sett_obj->sys_message(system_settings::ERROR_MSG, "In metro_escalator::create_properties_widgets raised exception "+spr_exc.get_description());
 };

};

