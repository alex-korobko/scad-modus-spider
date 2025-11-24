#ifndef _DOOR_
#define _DOOR_

class metro_door : public metro_device {
private:
door_data_block data_block;

//automat
bool A0_x1, A0_x2,
        A0_x3;

int A0_state;

void A0(int event) 
          throw (spider_exception);

void A0_z1();
void A0_z2();

bool A0_x5();
bool A0_x4();

public:
   enum {
         A0_OFFLINE=0,
         A0_EXCEPTION,
         A0_DOOR_CLOSED,
         A0_DOOR_OPENED_NOT_ACCEPTED,
         A0_DOOR_OPENED_ACCEPTED
   };

/*
metro DOOR`s metods
*/

/*
constructors and destructors
*/	

metro_door(
        int id, 
		int id_station,
		int number,
		int modbus_number,
		int type,
		int channel,
		bool enabled,
		in_addr_t	new_ip,
		time_t offline_or_exception_delay,
		bool new_conduction_is_switched_off,
		bool new_log_packets ) throw (spider_exception);

virtual ~metro_door();

void decode_answer_from_device_4_function(metro_device::buffer_data_type answer) 
                                                                                  throw (spider_exception);


/*
virtual methods
*/   	
virtual int get_type_description() {return metro_device_type::DEVICE_TYPE_DOOR;};
virtual  PhRect_t get_device_widget_extent() throw (spider_exception);
virtual void send_command(command cmd)  throw (spider_exception);
virtual void create_device_widget(int x,
                                                 int y,
                                                 ApDBase_t*  widgets_dbase,
                                                 PtWidget_t *station_widget,
                                                 vector<PtCallback_t> activate_callbacks,
                                                 vector<PtCallback_t> menu_callbacks)
                                       throw (spider_exception);

virtual void update_device_widget() throw (spider_exception);
virtual void update_device_panel(metro_device_type *dev_type) throw (spider_exception);

virtual void decode_answer_from_device_to_data_block();

virtual buffer_data_type get_request_for_send_to_device();
virtual ::data_block* get_data_block() { return &data_block;};

virtual command get_device_start_command() throw (spider_exception);
virtual vector<command> get_appropriated_commands() throw (spider_exception);

virtual void create_properties_widgets(PtWidget_t *parent_widget);
};

#endif
