#ifndef _ESCALATOR_
#define _ESCALATOR_

class metro_escalator : public metro_device {
private: 				
   	PtWidget_t *arrow;

//escalator properties widgets
    PtWidget_t *escalator_direction_combobox;
    PtWidget_t *escalator_start_mode_combobox;
    PtWidget_t *escalator_start_hour;
    PtWidget_t *escalator_start_minute;

   	pthread_t		tid;
   	int				pref_direction;

    escalator_data_block data_block;

	bool A0_x0, A0_x2, A0_x3, A0_x4, A0_x5, A0_x6;
	int  A0_x1, A0_state;

    void A0(int event) throw (spider_exception);
    void A0_z2(){A0_x1=pref_direction;};
    void A0_z3(){A0_x1=system_settings::DIRECTION_UP;};
    void A0_z4(){A0_x1=system_settings::DIRECTION_DOWN;};
    void A0_z6(){A0_x2=true;};
    void A0_z7(){A0_x2=false;};
public:

// automat A0 states

enum { A0_NOT_READY=0,
             A0_READY_NOT_ACCEPTED,
             A0_READY,
             A0_STARTING,
             A0_RUN,
             A0_STOPPING,
             A0_OFFLINE,
             A0_EXCEPTION};

/*
metro escalator`s metods
*/

/*
constructors and destructors
*/	

metro_escalator(
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
     in_addr_t	new_ip) throw (spider_exception);

virtual ~metro_escalator();

/*
get_ and set_  metods for private data members
*/
   PtWidget_t*  get_arrow_label_widget() {return (arrow);};

   	int get_pref_direction() { return pref_direction; };   	
   	void set_pref_direction(int new_dir) { pref_direction=new_dir; A0_x1=new_dir; }; 

	metro_device::buffer_data_type get_4_function_request();
	metro_device::buffer_data_type get_setting_time_request();

    bool is_escalator_run() {return A0_x0;};

void 
     decode_answer_from_device_4_function(metro_device::buffer_data_type answer) throw (spider_exception);


    PtWidget_t* get_escalator_direction_combobox_widget(){return escalator_direction_combobox;};
    PtWidget_t* get_escalator_start_mode_combobox_widget() {return escalator_start_mode_combobox;};
    PtWidget_t* get_escalator_start_hour_widget(){return escalator_start_hour;};
    PtWidget_t* get_escalator_start_minute_widget(){return escalator_start_minute;};


/*
virtual methods
*/   	

virtual int get_type_description() {return metro_device_type::DEVICE_TYPE_ESCALATOR;};
virtual void send_command(command cmd) throw (spider_exception);
virtual void create_device_widget(int x,
                                                  int y,
                                                  ApDBase_t*  widgets_dbase,
                                                  PtWidget_t *station_widget,
                                                  vector<PtCallback_t> activate_callbacks,
                                                  vector<PtCallback_t> menu_callbacks) throw (spider_exception);
virtual void update_device_widget() throw (spider_exception);
virtual PhRect_t get_device_widget_extent() throw (spider_exception);
virtual void update_device_panel(metro_device_type *dev_type) throw (spider_exception);


virtual void decode_answer_from_device_to_data_block  ();
virtual buffer_data_type get_request_for_send_to_device();

virtual ::data_block* get_data_block() { return &data_block;};
virtual command get_device_pref_command() throw (spider_exception);
virtual vector<command> get_appropriated_commands();

virtual void create_properties_widgets(PtWidget_t *parent_widget);
};

#endif
