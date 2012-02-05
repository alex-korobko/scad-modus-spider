#ifndef _SHAVR_
#define _SHAVR_

class metro_shavr : public metro_device {
private:
typedef vector<PhRect_t> supplied_escalators_dimensions;
typedef supplied_escalators_dimensions::iterator supplied_escalators_dimensions_iterator; 
typedef supplied_escalators_dimensions::size_type supplied_escalators_dimensions_size; 

typedef vector<PtWidget_t*> power_supply_lines_widgets;
typedef power_supply_lines_widgets::iterator power_supply_lines_widgets_iterator; 
typedef power_supply_lines_widgets::size_type power_supply_lines_widgets_size; 

supplied_escalators_dimensions escalators_dimensions;
power_supply_lines_widgets power_lines;
PtWidget_t *state_indicator_widget,
                 *command_indicator_widget;
shavr_data_block data_block;

//automats
bool A0_x1, A0_x2,
        A0_x3, A0_x4,
        A0_x5,
        A0_x10, A0_x11,
        A0_x12, A0_x13,
        A0_x14, A0_x15,
        A0_x16, A0_x17,
        A0_x18, A0_x19,
        A0_x20, A0_x21,
        A0_x22, A0_x23,
        A0_x24, A0_x25;

int A0_state;

void A0(int event) 
          throw (spider_exception);

void A0_z1() {A0_x5=true;};
void A0_z2() {A0_x5=false;};

public:
   enum {
         A0_OFFLINE=0,
         A0_EXCEPTION,
         A0_DISABLED_1_DISABLED_2,
         A0_ENABLED_1_ENABLED_2,
         A0_DISABLED_1_ENABLED_2,
         A0_ENABLED_1_DISABLED_2
   };

/*
metro SHAVR`s metods
*/

/*
constructors and destructors
*/	

metro_shavr(
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
		in_addr_t	new_ip,
		time_t offline_or_exception_delay,
		bool new_conduction_is_switched_off,
		bool new_log_packets) throw (spider_exception);

virtual ~metro_shavr();

void decode_answer_from_device_4_function(metro_device::buffer_data_type answer) 
                                                                                  throw (spider_exception);



/*
shavr`s escalators widgets dimensions
*/
supplied_escalators_dimensions_iterator
   escalators_dimensions_begin(){
     return escalators_dimensions.begin();};

supplied_escalators_dimensions_iterator
   escalators_dimensions_end(){
     return escalators_dimensions.end();};

supplied_escalators_dimensions_size 
   escalators_dimensions_size(){
     return escalators_dimensions.size();}; 

bool escalators_dimensions_empty(){
     return escalators_dimensions.empty();}; 

void push_back_escalator_dimension(PhRect_t new_escalator_dimension){
       escalators_dimensions.push_back(new_escalator_dimension);};

metro_device::buffer_data_type get_4_function_request();
metro_device::buffer_data_type get_setting_time_request();

/*
shavr`s  power supply lines widgets
*/
power_supply_lines_widgets_iterator
   power_lines_begin(){
     return power_lines.begin();};

power_supply_lines_widgets_iterator
   power_lines_end(){
     return power_lines.end();};

power_supply_lines_widgets_size 
   power_lines_size(){
     return power_lines.size();}; 

bool power_lines_empty(){
     return power_lines.empty();}; 

void push_back_power_line(PtWidget_t *new_power_line){
       power_lines.push_back(new_power_line);};

/*
virtual methods
*/   	
virtual int get_type_description() {return metro_device_type::DEVICE_TYPE_SHAVR;};
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

//static 
static byte decode_to_binary_decimal_notation(byte binary_value_less_100) throw (spider_exception);
};

#endif
