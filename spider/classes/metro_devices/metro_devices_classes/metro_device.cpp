using namespace std;

#include <Ap.h>
#include <Ph.h>
#include <Pt.h>

#include <pthread.h>
#include <process.h>
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
#include <functional>
#include <sstream>

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
#include "metro_devices_types_container.h"

#include "metro_device.h"

int metro_device::paneled_device_id=0;
PtWidget_t* metro_device::device_panel=NULL;

metro_device::metro_device(int  id,
                   int	  id_station,
                   int number,
                   int modbus_number,
                   int	 type,
                   int	 start_day_mode,
                   int	 start_hour,
                   int	 start_minute,
                   bool enabled,
                   in_addr_t	ip,
                   int channel,
				   double offline_or_exception_delay,
					bool new_conduction_is_switched_off,
					bool new_log_packets
                   )  throw (spider_exception):
                   id(id),
                   id_station(id_station),
                   number(number),
                   modbus_number(modbus_number),
                   type(type),
                   execution_mode(start_day_mode),
                   start_hour(start_hour),
                   start_minute(start_minute),
                   sleepticks(0),
                   enabled(enabled),
				   conduction_is_switched_off(new_conduction_is_switched_off),
                   last_failures_count(0),
                   last_message_remote_id(-1),
                   last_message_remote_time(0),
                   ip(ip),
                   answer_from_device(0),
                   request_for_send_to_device(0),
                   current_request_to_device(0),
				   connection_id(0),
				   channel_id(0),
				  last_offline_or_exception_time (0),
				  offline_or_exception_delay(offline_or_exception_delay),
				 log_packets(new_log_packets) {

		connection_id = ConnectAttach(0, 0, channel, 0, 0 );
		if (connection_id<0){
			throw spider_exception("Fail connect to pulse channel");	
		};
		channel_id = channel;	

		int init_res=pthread_mutex_init(&answer_from_device_mutex, NULL);
		if (init_res!=EOK){
			string mess("Fail initialize answer_from_device_mutex : ");
			mess+=strerror(init_res);
			throw spider_exception(mess);	
		};

		init_res=pthread_mutex_init(&request_to_device_mutex, NULL);
		if (init_res!=EOK){
			string mess("Fail initialize request_to_device_mutex : ");
			mess+=strerror(init_res);
			throw spider_exception(mess);	
		};

		init_res=pthread_mutex_init(&time_setting_flag_mutex, NULL);
		if (init_res!=EOK){
			string mess("Fail initialize time_setting_flag_mutex : ");
			mess+=strerror(init_res);
			throw spider_exception(mess);	
		};

		device_widget=station_widget=NULL;
   };


metro_device::~metro_device(){
		if (ConnectDetach(connection_id)<0)
					cout<<"connect detach "<<strerror(errno)<<endl;

		int ret_value=pthread_mutex_destroy(&request_to_device_mutex);
		if (ret_value!=EOK) 
		       cout<<"can`t destroy request_to_device_mutex error : "<< strerror(ret_value)<<endl;

		ret_value=pthread_mutex_destroy(&answer_from_device_mutex);
		if (ret_value!=EOK) 
		       cout<<"can`t destroy answer_from_device_mutex error : "<< strerror(ret_value)<<endl;

		ret_value=pthread_mutex_destroy(&time_setting_flag_mutex);
		if (ret_value!=EOK) 
		       cout<<"can`t destroy time_setting_flag_mutex error : "<< strerror(ret_value)<<endl;
};


    void 
     metro_device::set_answer_from_device_buffer 
               (const metro_device::buffer_data_type& answer){
            lock_mutex mutex_lock(&answer_from_device_mutex);
            answer_from_device=answer;
          };

    metro_device::buffer_data_type 
         metro_device::get_answer_from_device_buffer(){
            buffer_data_type return_buffer(0);
            {
               lock_mutex mutex_lock(&answer_from_device_mutex);
               return_buffer = answer_from_device;
             }

            return return_buffer;
          };

    void 
      metro_device::set_request_to_device_buffer
           (const metro_device::buffer_data_type& request){
                 {
                    lock_mutex mutex_lock(&request_to_device_mutex);
                    request_for_send_to_device=request;
                 };
          };

    void 
      metro_device::set_current_request_to_device_buffer
           (const metro_device::buffer_data_type& request){
                 {
                    lock_mutex mutex_lock(&request_to_device_mutex);
                    current_request_to_device=request;
                 };
          };


    metro_device::buffer_data_type 
         metro_device::get_request_for_send_to_device_buffer(){
            buffer_data_type return_buffer(0);
            {
               lock_mutex mutex_lock(&request_to_device_mutex);
               request_for_send_to_device.swap(return_buffer); 
            }
            return return_buffer;
          };

   metro_device::buffer_data_type 
      metro_device::get_current_request_to_device_buffer(){
            buffer_data_type return_buffer;
            {
               lock_mutex mutex_lock(&request_to_device_mutex);
               return_buffer=current_request_to_device; 
             }

            return return_buffer;
          };


void metro_device::create_device_panel(ApDBase_t *widgets_db,
                                           string panel_title,
                                           metro_device_type *dev_type,
                                           data_block *data_blk,
                                           int device_id,
                                           int device_number,
                                           vector<PtCallback_t> callbacks) throw (spider_exception){
 system_settings_spider *sys_sett_obj=system_settings_spider::get_instance();
		PtWidget_t *dialog_wnd=NULL, *dialog_wnd_widget=NULL, *panel_group_for_indicators=NULL;
		vector<PtArg_t> args;
		PhPoint_t pos_of_dialog_wnd_widget;
		vector<PtCallback_t> widget_callbacks;
        vector<char> tmp_chars(10);

         itoa(device_number, &tmp_chars[0], 10);

	     panel_title+="/";
         panel_title+=dev_type->get_name();
         panel_title+="  ";
         panel_title+=&tmp_chars[0];

	  if (sys_sett_obj==NULL)
         throw spider_exception("metro_device::create_device_panel(..) sys_sett_obj==NULL");
	  if (callbacks.size()!=2)
         throw spider_exception("metro_device::create_device_panel(..) callbacks.size()!=2");


		int panel_width=500,
             panel_height=400,
             dialog_wnd_widget_height=0,
             dialog_wnd_widget_width=0;

		args.clear();
		args.resize(13);
		PtSetArg(&args[0], Pt_ARG_HEIGHT, panel_height,0);
		PtSetArg(&args[1], Pt_ARG_WIDTH, panel_width,0);
		PtSetArg(&args[2], Pt_ARG_WINDOW_TITLE, panel_title.c_str(),0);
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
                      Pt_TRUE, Ph_WM_CLOSE  );
		PtSetArg(&args[11], Pt_ARG_WINDOW_MANAGED_FLAGS,
                      Pt_FALSE, Ph_WM_COLLAPSE );
 

		widget_callbacks.clear();
		widget_callbacks.resize(1);
		widget_callbacks[0]=callbacks[0];
        PtSetArg(&args[12],  Pt_CB_WINDOW_CLOSING,
			           &widget_callbacks[0],
                       widget_callbacks.size());

		dialog_wnd=PtCreateWidget(PtWindow,
                                                     sys_sett_obj->get_main_window(),
                                                     args.size(),
                                                     &args[0]);
     if (dialog_wnd==NULL)
     	throw spider_exception("metro_device::create_device_panel(..) dialog_wnd==NULL");

		args.clear();
		args.resize(9);
		PtSetArg(&args[0], Pt_ARG_ANCHOR_FLAGS,
                      Pt_FALSE, Pt_LEFT_ANCHORED_LEFT);
		PtSetArg(&args[1], Pt_ARG_ANCHOR_FLAGS,
                      Pt_FALSE, Pt_RIGHT_ANCHORED_RIGHT);
		PtSetArg(&args[2], Pt_ARG_ANCHOR_FLAGS,
                      Pt_FALSE, Pt_TOP_ANCHORED_BOTTOM);
		PtSetArg(&args[3], Pt_ARG_ANCHOR_FLAGS,
                      Pt_FALSE, Pt_BOTTOM_ANCHORED_BOTTOM);

		dialog_wnd_widget_height=50;
		PtSetArg(&args[4], Pt_ARG_HEIGHT,
                      dialog_wnd_widget_height, 0);
		PtSetArg(&args[5], Pt_ARG_WIDTH,
                      panel_width, 0);

        pos_of_dialog_wnd_widget.x=0;
        pos_of_dialog_wnd_widget.y=panel_height-dialog_wnd_widget_height;

		PtSetArg(&args[6], Pt_ARG_POS,
                      &pos_of_dialog_wnd_widget, 0);
		PtSetArg(&args[7], Pt_ARG_FLAGS,
                      Pt_TRUE, Pt_HIGHLIGHTED);
		PtSetArg(&args[8], Pt_ARG_FLAGS,
                      Pt_TRUE, Pt_SET);

		dialog_wnd_widget=PtCreateWidget(PtPane,
                                                                 dialog_wnd,
                                                                 args.size(),
                                                                 &args[0]);

     if (dialog_wnd_widget==NULL)
     	throw spider_exception("metro_device::create_device_panel(..) dialog_wnd_widget==NULL");

		args.clear();
		args.resize(6);
		PtSetArg(&args[0], Pt_ARG_LABEL_TYPE,
                      Pt_Z_STRING, 0);
		PtSetArg(&args[1], Pt_ARG_TEXT_STRING,
                      "OK", 0);
		PtSetArg(&args[2], Pt_ARG_HEIGHT,
                      30, 0);
        dialog_wnd_widget_width=80;
		PtSetArg(&args[3], Pt_ARG_WIDTH,
                      80, 0);
        pos_of_dialog_wnd_widget.x=panel_width-
                                   dialog_wnd_widget_width-
                           system_settings_spider::SPACE_BETWEEN_METRO_DEVICES_IN_STATION;
        pos_of_dialog_wnd_widget.y=5;
		PtSetArg(&args[4], Pt_ARG_POS,
                      &pos_of_dialog_wnd_widget, 0);
    		widget_callbacks[0]=callbacks[1];
         PtSetArg(&args[5],  Pt_CB_ACTIVATE,
			           &widget_callbacks[0],
                       widget_callbacks.size());

	    dialog_wnd_widget=PtCreateWidget(PtButton,
                                                             dialog_wnd_widget,
                                                              args.size(),
                                                              &args[0]);

     if (dialog_wnd_widget==NULL)
     	throw spider_exception("metro_shavr::create_device_panel(..) dialog_wnd_widget==NULL");

		args.clear();
		args.resize(9);
		PtSetArg(&args[0], Pt_ARG_ANCHOR_FLAGS,
                      Pt_FALSE, Pt_LEFT_ANCHORED_LEFT);
		PtSetArg(&args[1], Pt_ARG_ANCHOR_FLAGS,
                      Pt_FALSE, Pt_RIGHT_ANCHORED_RIGHT);
		PtSetArg(&args[2], Pt_ARG_ANCHOR_FLAGS,
                      Pt_FALSE, Pt_TOP_ANCHORED_TOP);
		PtSetArg(&args[3], Pt_ARG_ANCHOR_FLAGS,
                      Pt_FALSE, Pt_BOTTOM_ANCHORED_BOTTOM);
        pos_of_dialog_wnd_widget.x=0;
        pos_of_dialog_wnd_widget.y=0;

		PtSetArg(&args[4], Pt_ARG_POS,
                      &pos_of_dialog_wnd_widget, 0);

		dialog_wnd_widget_height=panel_height-dialog_wnd_widget_height;
		PtSetArg(&args[5], Pt_ARG_HEIGHT,
                      dialog_wnd_widget_height, 0);
		PtSetArg(&args[6], Pt_ARG_WIDTH,
                      panel_width, 0);

		panel_group_for_indicators=PtCreateWidget(PtPanelGroup,
                                                                 dialog_wnd,
                                                                 args.size(),
                                                                 &args[0]);

     if (panel_group_for_indicators==NULL)
     	throw spider_exception("metro_device::create_device_panel(..) panel_group_for_indicators==NULL");

    metro_device::paneled_device_id=device_id;
    metro_device::device_panel=panel_group_for_indicators;

	dev_type->create_panels(panel_group_for_indicators,
									       sys_sett_obj->get_widgets_dbase(),
									       data_blk);

	PtRealizeWidget(dialog_wnd);
};


metro_device::buffer_data_type
      metro_device::generate_error_answer_for_function
                             ( int device_number,
                               int function_code,
                               int exception_code){
 system_settings_spider* sys_sett=system_settings_spider::get_instance();
 	buffer_data_type buffer(0);
	word		crc_value;

	buffer.push_back(device_number);
	buffer.push_back(function_code | 0x80);                      // function number with error detected flag
	buffer.push_back(exception_code);

	crc_value = sys_sett->crc(buffer);

	system_settings::bytes tmp_bytes=sys_sett->bytes_of_type<word>(crc_value);
	buffer.push_back(tmp_bytes[1]);
	buffer.push_back(tmp_bytes[0]);

   	return buffer;
};
