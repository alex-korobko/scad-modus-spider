#include "global.h"

int command::get_command_code(void)
{
return command_code;
}

int command::get_escalator_number(void)
{
return esc_number;
}

void 	command::set_item_color(const PgColor_t& color_of_item)
{
item_color=color_of_item;
}

PgColor_t& command::get_item_color(void)
{
return  item_color;
}


int command::send()
{	
	int esc_found=0;
	metro_escalators_container::iterator_metro_escalators iter;
	iter=g_escalators.find(id_escalator);
	if (iter!=g_escalators.end())
		{
		iter->second.send_command(command_code);
		esc_found=1;
		}
	return esc_found;
}

int command::get_station()
{	
	return station_id;
}
