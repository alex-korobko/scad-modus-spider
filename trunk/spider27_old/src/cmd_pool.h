#ifndef __CMD_POOL_H__
#define __CMD_POOL_H__
	
#include "system.h"

class Command {
public:
	Command (const byte escalator_id, const byte command, const int id_station, const int escalator_number) : id_escalator(escalator_id),  command_code(command), station_id(id_station) , esc_number(escalator_number){};
	int Send ();
	int GetStation();
private:
	byte id_escalator;
	byte command_code;
	int station_id;
	int esc_number;
};

#endif