/* Y o u r   D e s c r i p t i o n                       */
/*                            AppBuilder Photon Code Lib */
/*                                         Version 2.01  */

#include "global.h"

void draw_log_item(
        PtWidget_t *widget,
        PtGenListItem_t *items,
        unsigned index,
        unsigned nitems, PhRect_t *where
        ) {

     vector<PtArg_t> args(2);
     PtListColumn_t *internal_column_pos=NULL;
    log_records_container *log_rec_contain=NULL;
	log_records_container::iterator_log_rec iter_log_rec;
	msg_types_container::msg_types_iterator  iter_msg_types;	
	
	unsigned count;
	PhPoint_t draw_point;

	PtGenListDrawBackground( widget, items, nitems, where, 0, 0, 0, 0 );

	if (g_system_settings.small_font_prepared())
		{
			PgSetFont( (char*) g_system_settings.get_small_font() );
		} else {
			g_system_settings.sys_message(system_settings::ERROR_MSG,
														"Not found system_settings::small_font for re-drawing log_items");
		};

	PtSetArg(&args[0], Pt_ARG_LIST_COLUMN_POS, &internal_column_pos, 0);
	PtSetArg(&args[1], Pt_ARG_POINTER, &log_rec_contain, 0);	

	PtGetResources(widget,
							  args.size(),
							  &args[0]);		
	
	if (log_rec_contain==NULL){
			g_system_settings.sys_message(system_settings::ERROR_MSG,
														"Not found log_rec_contain for re-drawing log_items");
			return;
		};
	iter_log_rec=log_rec_contain->begin();
	advance(iter_log_rec, index-1);

	count=0;
	draw_point.y=where->ul.y+system_settings::ROW_HEIGHT;

	while (	count < nitems &&
				iter_log_rec!=log_rec_contain->end())
	{
		int tmp_msg_id=iter_log_rec->get_msg_id() & 0x7FFF;
		int msg_value=(tmp_msg_id >> 16) & 0xFFFF;
		int msg_type=0, device_id=0;
		string message_text, device_text;

		msg_type=iter_log_rec->get_msg_type();
		iter_msg_types=g_msg_types.find(msg_type);
		if (iter_msg_types==g_msg_types.end())
			{
				vector<char> tmp_chars(8);
				itoa (msg_type, &tmp_chars[0], 10);
				string mess="Not found msg_type ";
				mess+=&tmp_chars[0];
				mess+=" for re-drawing log_items";					
				g_system_settings.sys_message(system_settings::ERROR_MSG,
																	mess);
				return;
			};

		PgSetTextColor(iter_msg_types->second.get_type_color());

		if (tmp_msg_id!=0)	// it`s real id of message
		{
			metro_escalators_container::iterator_metro_escalators iter_metro_devices;			
			devices_types_container::iterator_devices_types iter_devices_types;
			
			device_id=iter_log_rec->get_device_id();
			if (device_id>0)
			{
				iter_metro_devices=g_escalators.find(device_id);
				if (iter_metro_devices!=g_escalators.end())
					{
						iter_devices_types=g_metro_devices_types.find(iter_metro_devices->second.get_type());
						if (iter_devices_types!=g_metro_devices_types.end()) 
							{
								msg_dict_container::msg_dict_iterator tmp_msg_iter=
								iter_devices_types->second.type_messages.find(tmp_msg_id);
								if (tmp_msg_iter!=
									iter_devices_types->second.type_messages.end()) 
									{
										message_text=tmp_msg_iter->second.get_text();
										device_text.append(iter_devices_types->second.get_name(),
																		0,
																		5);
										device_text+=" ";
										vector<char> tmp_chars(8);
										itoa (iter_metro_devices->second.get_number(), 
												&tmp_chars[0], 
												10);
										device_text+=&tmp_chars[0];
									};	//if (tmp_msg_iter!=
							} // if (iter_devices_types!=g_metro_devices_types.end())
					} else { //insert here code to finding in other metro devices containers  -  using GLOBAL id`s identificators
						vector<char> tmp_chars(8);
						itoa (device_id, &tmp_chars[0], 10);
						string mess="Not found device id ";
						mess+=&tmp_chars[0];
						mess+=" for re-drawing log_items";					
						g_system_settings.sys_message(system_settings::ERROR_MSG,
																			mess);
						return;	
					}; // if (iter_metro_devices!=g_escalators.end())
			}; // if (device_id>0)

			if (message_text.empty())
				{
					msg_dict_container::msg_dict_iterator tmp_msg_iter=
																			g_msg_dictionary.find(tmp_msg_id);
					if (tmp_msg_iter!=
								g_msg_dictionary.end()) 
					 	{
					 		message_text=tmp_msg_iter->second.get_text();
					 	} else { // if (tmp_msg_iter! =
							vector<char> tmp_chars(8);
							itoa (tmp_msg_id, &tmp_chars[0], 10);
							string mess="Not found message id ";
							mess+=&tmp_chars[0];
							mess+=" for re-drawing log_items";					
							g_system_settings.sys_message(system_settings::ERROR_MSG,
																				mess);
							return;	
					 	}; // if (tmp_msg_iter!=
				}; // if (message_text.empty())
				

			int find_pos=message_text.find("%d");
			
			if (find_pos>=0)
				{
					vector<char> tmp_chars(8);
					itoa (msg_value, &tmp_chars[0], 10);
					message_text.replace( find_pos,
														2,
														&tmp_chars[0]);
				};

		}; // if (tmp_msg_id!=0)

		string station_text;
		metro_stations_container::iterator_metro_stations iter_metro_stat=
									g_stations.find(iter_log_rec->get_station_id());
		if (iter_metro_stat!=g_stations.end())
			{
				station_text=iter_metro_stat->second.get_stl_name_string();
			};

		 time_t tmp_time=iter_log_rec->get_record_time();		
  
		vector<char> date_text(20);
	 	strftime(&date_text[0], date_text.size(), "%d-%m-%Y", localtime( &tmp_time));		
	 	
		vector<char> time_text(20);
	 	strftime(&time_text[0], time_text.size(), "%H:%M:%S", localtime( &tmp_time ));		

		// drawing
		draw_point.x=where->ul.x+internal_column_pos[0].from + system_settings::COLUMN_LEFT_MARGIN;
		PgDrawText(&date_text[0], date_text.size(), &draw_point, Pg_TEXT_BOTTOM);

		draw_point.x = where->ul.x + internal_column_pos[1].from + system_settings::COLUMN_LEFT_MARGIN;				
		PgDrawText(&time_text[0], time_text.size(), &draw_point, Pg_TEXT_BOTTOM);

		draw_point.x = where->ul.x + internal_column_pos[2].from + system_settings::COLUMN_LEFT_MARGIN;				
		PgDrawText(station_text.c_str(), station_text.size(), &draw_point, Pg_TEXT_BOTTOM);

		draw_point.x = where->ul.x + internal_column_pos[3].from + system_settings::COLUMN_LEFT_MARGIN;				
		PgDrawText(device_text.c_str(), device_text.size(), &draw_point, Pg_TEXT_BOTTOM);

		draw_point.x = where->ul.x + internal_column_pos[4].from + system_settings::COLUMN_LEFT_MARGIN;				
		PgDrawText(message_text.c_str(), message_text.size(), &draw_point, Pg_TEXT_BOTTOM);

		count++;
		iter_log_rec++;
		draw_point.y+=system_settings::ROW_HEIGHT;
	}; // while (	count< nitems &&

	}


void draw_command_pool_item(
        PtWidget_t *widget, PtGenListItem_t *items, unsigned index,
        unsigned nitems, PhRect_t *where
        ) {
	PtGenListDrawBackground( widget, items, nitems, where, 0, 0, 0, 0 );
	
	}


void draw_escalator_start_item(
        PtWidget_t *widget, PtGenListItem_t *items, unsigned index,
        unsigned nitems, PhRect_t *where
        ) {
    	unsigned count, escalator_number;
	PhPoint_t draw_point;
    vector<PtArg_t> args(2);
    vector<char> tmp_chars(10);
    PtListColumn_t *internal_column_pos=NULL;
    PtWidget_t *wnd_widget;
	contain_morning_start *ptr_morn_start_cont=NULL;	
    	contain_morning_start::iterator_morining_start iter_morn_start;	
    	metro_stations_container::iterator_metro_stations iter_stations;
    	metro_escalators_container::iterator_metro_escalators iter_escalators;
	string tmp_string;
	system_settings::strings_container directions_strings_ru=g_system_settings.get_directions_russ_strings();    	
	system_settings::strings_container start_modes_strings_ru=g_system_settings.get_start_days_modes_russ_strings();    	

	PtGenListDrawBackground( widget, items, nitems, where, 0, 0, 0, 0 );
	if (g_system_settings.small_font_prepared())
	{
		PgSetFont( (char*) g_system_settings.get_small_font() );
	} else {
		g_system_settings.sys_message(system_settings::ERROR_MSG,
													"Not found system_settings::small_font for re-drawing log_items");
	};

	PtGetResource(widget, Pt_ARG_LIST_COLUMN_POS, &internal_column_pos, 0);
	wnd_widget=ApGetInstance(widget);	
	if (wnd_widget==NULL){
			g_system_settings.sys_message(system_settings::ERROR_MSG,
														"Not found window widget for re-drawing escalator_start items");
			return;
		};
	
	PtGetResource(wnd_widget, Pt_ARG_POINTER, &ptr_morn_start_cont, 0);	
	if (ptr_morn_start_cont==NULL){
			g_system_settings.sys_message(system_settings::ERROR_MSG,
														"Not found ptr_morn_start_cont for re-drawing escalator_start items");
			return;
		};

	iter_morn_start=ptr_morn_start_cont->begin();
	advance(iter_morn_start, index-1);
	
	count=0;
	draw_point.y=where->ul.y+system_settings::ROW_HEIGHT;

	while (	count < nitems &&
				iter_morn_start!=ptr_morn_start_cont->end())
	{
		iter_escalators=g_escalators.find(iter_morn_start->get_escalator_id());
		if (iter_escalators!=g_escalators.end())
			{
				escalator_number=iter_escalators->second.get_number();
				iter_stations=g_stations.find(iter_escalators->second.get_station_id());
				if (iter_stations!=g_stations.end())
					{
						tmp_string=iter_stations->second.get_stl_name_string();
					} else {
						tmp_string="СТАНЦИЯ НЕ НАЙДЕНА";
						iter_morn_start->set_start_enabled(false);
						string mess="Not found station for escalator id ";
						itoa(iter_escalators->second.get_id(), 
							   &tmp_chars[0], 
							   10);
						mess+=&tmp_chars[0];	   
						mess+=" for re-drawing escalator_start items";
						g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
					};
			} else { // if (iter_escalators!=g_
				escalator_number=0;
				tmp_string="СТАНЦИЯ НЕ НАЙДЕНА";
				iter_morn_start->set_start_enabled(false);
				string mess="Not found escalator with id ";
				itoa(iter_morn_start->get_escalator_id(), 
					   &tmp_chars[0], 
					   10);
				mess+=&tmp_chars[0];	   
				mess+=" for re-drawing escalator_start items";
				g_system_settings.sys_message(system_settings::ERROR_MSG, mess);

			};
			
		iter_morn_start->get_start_enabled()?
						PgSetTextColor(system_settings::COLOR_RED):
						PgSetTextColor(system_settings::COLOR_BLACK);	

		//drawing
		draw_point.x=where->ul.x+internal_column_pos[0].from + system_settings::COLUMN_LEFT_MARGIN;
		PgDrawText(tmp_string.c_str(), tmp_string.size(), &draw_point, Pg_TEXT_BOTTOM);

		itoa(escalator_number, &tmp_chars[0], 10);
		draw_point.x=where->ul.x+internal_column_pos[1].from + system_settings::COLUMN_LEFT_MARGIN;
		PgDrawText(&tmp_chars[0], tmp_chars.size(), &draw_point, Pg_TEXT_BOTTOM);
		
		tmp_string=directions_strings_ru[iter_morn_start->get_escalator_direction()];
		draw_point.x=where->ul.x+internal_column_pos[2].from + system_settings::COLUMN_LEFT_MARGIN;
		PgDrawText(tmp_string.c_str(), tmp_string.size(), &draw_point, Pg_TEXT_BOTTOM);

		itoa(iter_morn_start->get_start_hour(), &tmp_chars[0], 10);
		tmp_string=&tmp_chars[0];
		tmp_string+="ч  ";
		itoa(iter_morn_start->get_start_minute(), &tmp_chars[0], 10);
		tmp_string+=&tmp_chars[0];
		tmp_string+="мин";
		draw_point.x=where->ul.x+internal_column_pos[3].from + system_settings::COLUMN_LEFT_MARGIN;
		PgDrawText(tmp_string.c_str(), tmp_string.size(), &draw_point, Pg_TEXT_BOTTOM);

		tmp_string=start_modes_strings_ru[iter_morn_start->get_start_mode()];
		draw_point.x=where->ul.x+internal_column_pos[4].from + system_settings::COLUMN_LEFT_MARGIN;
		PgDrawText(tmp_string.c_str(), tmp_string.size(), &draw_point, Pg_TEXT_BOTTOM);

		count++;
		iter_morn_start++;
		draw_point.y+=system_settings::ROW_HEIGHT;
	};// 	while (	count < nitems &&

	}


