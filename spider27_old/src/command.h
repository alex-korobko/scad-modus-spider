#ifndef __COMMAND_H__
#define __COMMAND_H__

class command {
private:
	byte id_escalator;
	byte command_code;
	int station_id;
	int esc_number;
	PgColor_t item_color;

public:
	command (const byte escalator_id, const byte command, const int id_station, const int escalator_number) : id_escalator(escalator_id),  command_code(command), station_id(id_station) , esc_number(escalator_number) {item_color=system_settings::COLOR_BLACK;};
	int send (void);
	int get_station(void);
	int get_escalator_number(void);
	int get_command_code(void);

	void 	set_item_color(const PgColor_t& color_of_item);
	PgColor_t& get_item_color(void);
};

#endif