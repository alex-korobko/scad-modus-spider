#ifndef __CMD_POOL_H__
#define __CMD_POOL_H__
	
#include "system.h"

class Command {
public:
	Command (const byte escalator_id, const byte command, const int id_station, const int escalator_number) : id_escalator(escalator_id),  command_code(command), station_id(id_station) , esc_number(escalator_number) {item_color=Pg_BLACK;};
	int Send (void);
	int GetStation(void);
	int GetEscNum(void);
	int GetCmdCode(void);

	void 	SetItemColor(const PgColor_t& color_of_item);
	PgColor_t& GetItemColor(void);

private:
	byte id_escalator;
	byte command_code;
	int station_id;
	int esc_number;
	PgColor_t item_color;
};

#endif