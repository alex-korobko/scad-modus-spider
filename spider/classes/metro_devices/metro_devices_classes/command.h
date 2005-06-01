#ifndef __COMMAND__
#define __COMMAND__

class command {
private:
	int  device_id;
	int device_type_id;
	byte command_code;
	int station_id;
	int device_number;
    string command_description;
	PgColor_t item_color;

     command ();
public:
	command (const int device_id,
	                 const int device_type_id,
	                 const byte command_code,
	                 const int station_id,
	                 const int device_number,
                      const string command_description) : 
	                 device_id(device_id), 
	                 device_type_id(device_type_id),
	                 command_code(command_code),
	                 station_id(station_id), 
	                 device_number(device_number),
                     command_description(command_description)
   {item_color=system_settings_spider::COLOR_BLACK;};

	command (const command& command_to_copy) :
           device_id(command_to_copy.device_id),
           device_type_id(command_to_copy.device_type_id),
           command_code(command_to_copy.command_code),
           station_id (command_to_copy.station_id),
           device_number(command_to_copy.device_number),
           command_description(command_to_copy.command_description) {};

    int get_device_id(){ return device_id;}
    int get_device_type_id(){ return device_type_id;}
    int get_command_code(){ return command_code;}
    int get_device_number(){return device_number; }
	int get_station(){	return station_id;}
   string get_command_description() { return command_description;};
    void set_item_color(const PgColor_t& color_of_item){item_color=color_of_item;}
    PgColor_t& get_item_color(){return  item_color;}

    bool operator == (command command_to_equal);
	
};

#endif
