/* Y o u r   D e s c r i p t i o n                       */
/*                            AppBuilder Photon Code Lib */
/*                                         Version 2.01  */

#include "global.h"

log_filter temp_log_filter;

/*
Local functions
*/

static void redraw_devices_list_in_filter_dlg(
													PtWidget_t *link_instance,
													log_filter filter,
													metro_escalators_container *ptr_to_esc_contain,
													metro_stations_container *ptr_to_stations_contain,
													system_settings *ptr_to_system_sett
															)
{
	PtWidget_t *tmp_widget=ApGetWidgetPtr(link_instance, ABN_DevicesList);
	if (tmp_widget!=NULL)
		{
		 PtListDeleteAllItems(tmp_widget);
		if (	 ! ptr_to_esc_contain->empty() &&
		     ! ptr_to_stations_contain->empty())
			{
				vector<unsigned short> indexes_of_selected_items;
				const char* list_items[1];
				vector<char> tmp_chars(8);

				string tmp_string;
				tmp_string.reserve(30);

				indexes_of_selected_items.reserve(filter.devices_size());
				unsigned short current_index=1;
		
				metro_stations_container::iterator_metro_stations tmp_iter_stat;
				
				log_filter::ids_container_iterator ids_of_selected_stations_iter=filter.stations_begin();				
				while (ids_of_selected_stations_iter!=filter.stations_end())
				{
						tmp_iter_stat=ptr_to_stations_contain->find(*ids_of_selected_stations_iter);
				
						if (tmp_iter_stat==ptr_to_stations_contain->end())
							{
								ids_of_selected_stations_iter++;
								continue;
							};

						metro_station::iterator_escalators_id tmp_iter_esc_id=tmp_iter_stat->second.begin_escalators_id();
						while (tmp_iter_esc_id!=tmp_iter_stat->second.end_escalators_id())
							{
		 						metro_escalators_container::iterator_metro_escalators tmp_iter_esc=ptr_to_esc_contain->find(*tmp_iter_esc_id);	
							 
							 	if (tmp_iter_esc==ptr_to_esc_contain->end())
							 		{
 										tmp_iter_esc_id++;
 										continue;
							 		};
							 
								itoa(tmp_iter_esc->second.get_number() , &tmp_chars[0], 10);
								tmp_string=tmp_iter_stat->second.get_stl_name_string();
								tmp_string+=" эскалатор ";
								tmp_string+=&tmp_chars[0];
								list_items[0]=tmp_string.c_str();

								PtListAddItems( tmp_widget,
											             list_items,
											             1,
												         0);
					
								if (filter.devices_end()!=
									filter.find_device (tmp_iter_esc->second.get_id()))
									{
										indexes_of_selected_items.push_back(current_index);
									};
			
								current_index++;
								tmp_iter_esc_id++;
								}; // while (tmp_iter_esc_id!=tmp_iter_stat->
						  ids_of_selected_stations_iter++;
					}; // while (ids_of_selected_stations_iter!=

				if (! indexes_of_selected_items.empty())
					{									       
						PtSetResource(tmp_widget, Pt_ARG_SELECTION_INDEXES, &indexes_of_selected_items[0], indexes_of_selected_items.size()); 
					} else {
						PtSetResource(tmp_widget, Pt_ARG_SELECTION_INDEXES, NULL, 0); 
					};
				}; //	if (	 ! selected_stations_ids.empty() &&
		} else {
			ptr_to_system_sett->sys_message(system_settings::ERROR_MSG, string("Not found ponter to instanse of ABN_DevicesList"));
		}

};

void re_calculate_command_items_count(PtWidget_t *cmd_pool_list)
	{
	PtGenListItem_t *first=NULL, *list_item=new(PtGenListItem_t), *last;

	cmd_pool_container* cmd_pool_cont;
	PtGetResource(cmd_pool_list, Pt_ARG_POINTER, &cmd_pool_cont, 0);				
	
	PtGenListRemoveItems(cmd_pool_list, NULL,NULL);
	
	if(cmd_pool_cont!=NULL && !cmd_pool_cont->empty())
	{
		first=list_item;
		
		first->prev=NULL;
		first->next=NULL;
		first->size.w=0;
		first->size.h=system_settings::ROW_HEIGHT;
		first->flags=0;
		
		int size=cmd_pool_cont->size();
		for (int i=1; i<size; i++)
			{
				last=	new(PtGenListItem_t);
				
				last->prev=list_item;
				last->next=NULL;
				last->size.w=0;
				last->size.h=system_settings::ROW_HEIGHT;
				last->flags=0;
				
				list_item->next=last;
				
				list_item=last;
			};

		 PtGenListAddItems(cmd_pool_list, first, NULL);
		 
	}

}


static int CreateScheme()
{
	PhDim_t		*size;
	vector<PtArg_t>		args(3);

    PtGetResource(ABW_Scheme, Pt_ARG_DIM, &size, 0);
 
	metro_lines_container::iterator_metro_lines   temp_lines_iterator;   
	
	temp_lines_iterator=g_lines.begin();
	
	while (temp_lines_iterator!=g_lines.end())
	{
		vector<PhPoint_t>  points;
		
		metro_line::iterator_stations_id temp_stations_id_iterator=temp_lines_iterator->second.begin_stations_id();
		while(temp_stations_id_iterator!=temp_lines_iterator->second.end_stations_id())
		{
			metro_stations_container::iterator_metro_stations temp_metro_stations_iterator=g_stations.find(*temp_stations_id_iterator);
			if(temp_metro_stations_iterator!=g_stations.end())
				{
					PhPoint_t temp_point;
					temp_point.x=temp_metro_stations_iterator->second.get_x_coord()*size->w/100;
					temp_point.y= temp_metro_stations_iterator->second.get_y_coord()*size->h/100;
					points.push_back(temp_point);
				}; 
		
			temp_stations_id_iterator++;
		};

	 	PtSetArg(&args[0], Pt_ARG_COLOR, temp_lines_iterator->second.get_color(), 0);
	 	PtSetArg(&args[1], Pt_ARG_POINTS, &points[0], points.size());	 	
	 	PtSetArg(&args[2], Pt_ARG_LINE_WIDTH, 6, 0);
	    temp_lines_iterator->second.set_line_widget( PtCreateWidget(PtPolygon,
	    																									ABW_Scheme,
	    																									args.size(),
	    																									&args[0])
	    																		);
        	if (temp_lines_iterator->second.get_line_widget()!=NULL
        		&& PtRealizeWidget(temp_lines_iterator->second.get_line_widget())!=0 ) 
        		{
			vector<char> tmp_chars(10);
			string message("Can`t create widget for line ");
			itoa (temp_lines_iterator->second.get_id(), &tmp_chars[0], 10);
			g_system_settings.sys_message(system_settings::ERROR_MSG,  message);

        			return 0;
        		};

		temp_stations_id_iterator=temp_lines_iterator->second.begin_stations_id();
		while(temp_stations_id_iterator!=temp_lines_iterator->second.end_stations_id())
		{
			metro_stations_container::iterator_metro_stations temp_metro_stations_iterator=g_stations.find(*temp_stations_id_iterator);
			if(temp_metro_stations_iterator!=g_stations.end())
				{
					temp_metro_stations_iterator->second.create_wnd(ABW_Scheme, size ->w, size ->h);
				}; 
		
			temp_stations_id_iterator++;
		};

    	
		temp_lines_iterator++;
	};
	return 1;
}


/*
Callbacks command pool window
*/
int
activate_SendCommandButton( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
	{
	PtWidget_t *cmd_pool_list=NULL, *cmd_pool_wnd=NULL;
	PtGenListItem_t *curr_item;

	int count ;
	cmd_pool_container::cmd_pool_iterator p_cmd_pool;	

   cmd_pool_wnd = ApGetInstance(widget );
   cmd_pool_list = ApGetWidgetPtr(cmd_pool_wnd, ABN_CommandPoolRwLst);

	if (cmd_pool_list!=NULL)
	{
	curr_item = PtGenListFirstItem(cmd_pool_list);
	count=g_command_pool.size();
	p_cmd_pool=g_command_pool.begin();
	
	while(p_cmd_pool!=g_command_pool.end() && curr_item)
		{
			if(curr_item->flags&Pt_LIST_ITEM_SELECTED>0)
						{
						(*p_cmd_pool).send();
						g_command_pool.erase(p_cmd_pool);
						re_calculate_command_items_count(cmd_pool_list);			
						}
		p_cmd_pool++;
		curr_item=curr_item->next;
		};
	};

	return( Pt_CONTINUE );
	}


int
activate_DeleteCommandBtn( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
	{
	PtGenListItem_t *curr_item;

	int count ;
	cmd_pool_container::cmd_pool_iterator p_cmd_pool;	

   PtWidget_t *cmd_pool_wnd = ApGetInstance(widget );

	if (cmd_pool_wnd!=NULL) 
	{
	    PtWidget_t *cmd_pool_list = ApGetWidgetPtr(cmd_pool_wnd, ABN_CommandPoolRwLst);

		if (cmd_pool_list!=NULL)
		{
		    cmd_pool_list = ApGetWidgetPtr(cmd_pool_wnd, ABN_CommandPoolRwLst);
			curr_item = PtGenListFirstItem(cmd_pool_list);
			count=g_command_pool.size();
			p_cmd_pool=g_command_pool.begin();
	
			while(p_cmd_pool!=g_command_pool.end() && curr_item)
				{
					if(curr_item->flags&Pt_LIST_ITEM_SELECTED>0)
						{
							g_command_pool.erase(p_cmd_pool);
							re_calculate_command_items_count(cmd_pool_list);					
						}
					p_cmd_pool++;
					curr_item=curr_item->next;
				};
		};
	};

	return( Pt_CONTINUE );
	}


int
realized_CommandPoolRwLst( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
	{
	re_calculate_command_items_count(widget);
	return( Pt_CONTINUE );
	}


int
unrealized_CommandPoolRwLst( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
	{
	cmd_pool_container* cmd_pool_cont;
	PtGetResource(widget, Pt_ARG_POINTER, &cmd_pool_cont, 0);				
	if(cmd_pool_cont!=NULL)
	{
		cmd_pool_cont->set_widget(NULL);
	}

	return( Pt_CONTINUE );

	}


int
activate_CmdPoolBtn( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
	{
	
	PtWidget_t *cmd_pool_wnd= ApCreateModule( ABM_PrevCmdPoolDlg, NULL, NULL);
	
	if (cmd_pool_wnd!=NULL)
		{
			PtWidget_t *cmd_pool_list=ApGetWidgetPtr(cmd_pool_wnd, ABN_CommandPoolRwLst);
			if( cmd_pool_list!=NULL) 
				{
						cmd_pool_container* cmd_pool_cont;
						PtGetResource(cmd_pool_list, Pt_ARG_POINTER, &cmd_pool_cont, 0);				
							if(cmd_pool_cont!=&g_command_pool)
							{
								g_command_pool.set_widget(cmd_pool_list);
							}; 
						PtRealizeWidget(cmd_pool_wnd);
				}else {
						PtDestroyWidget(cmd_pool_wnd);
						string mess ="Can`t get RawList for command pool";
						g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
				};
		}
	
	return( Pt_CONTINUE );

	}

/*
Callbacks log filter window
*/


int
activate_FilterBtn( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
	{

	return( Pt_CONTINUE );
	}


int
activate_LogFilterOK( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
	{
	log_records_container* ptr_log_rec_cont=NULL;
	PtWidget_t  *dialog_widget=NULL, *tmp_widget=NULL;

// getting instanse of LogFilterDlg
	dialog_widget=ApGetInstance(widget);
	if (dialog_widget==NULL)
		{
		g_system_settings.sys_message(system_settings::ERROR_MSG, string("Not found ponter to instanse of ABM_LogFilterDlg"));
		return( Pt_END );		
		}

//getting pointer of log_records_container
	PtGetResource(dialog_widget, Pt_ARG_POINTER, &ptr_log_rec_cont, 0);

	if (ptr_log_rec_cont==NULL) return Pt_END;
	
	log_filter old_log_filter(ptr_log_rec_cont->filter);

//getting instanse of FilteringBtn and set it`s state by set_filtering 
	tmp_widget=ApGetWidgetPtr(dialog_widget, ABN_FilteringBtn);
	if (tmp_widget!=NULL)
		{
				long flags;
				long const *internal_flags;
				PtGetResource(tmp_widget, Pt_ARG_FLAGS, &internal_flags, 0);				
				flags=*internal_flags;
				ptr_log_rec_cont->filter.set_filter_state((flags & Pt_SET)!=0);
		} else {
			g_system_settings.sys_message(system_settings::ERROR_MSG, string("Not found ponter to instanse of ABN_FilteringBtn"));
		}

//getting instanse of AllTimeRangeBtn and set it`s state by set_all_times 
	tmp_widget=ApGetWidgetPtr(dialog_widget, ABN_AllTimeRangeBtn);
	if (tmp_widget!=NULL)
		{
				long flags;
				long const *internal_flags;
				PtGetResource(tmp_widget, Pt_ARG_FLAGS, &internal_flags, 0);				
				flags=*internal_flags;
				ptr_log_rec_cont->filter.set_all_times((flags & Pt_SET)!=0);
		} else {
			g_system_settings.sys_message(system_settings::ERROR_MSG, string("Not found ponter to instanse of ABN_AllTimeRangeBtn"));
		}

//getting instanse of AllMessagesTypesBtn and set it`s state by set_all_msg_types 
	tmp_widget=ApGetWidgetPtr(dialog_widget, ABN_AllMessagesTypesBtn);
	if (tmp_widget!=NULL)
		{
				long flags;
				long const *internal_flags;
				PtGetResource(tmp_widget, Pt_ARG_FLAGS, &internal_flags, 0);				
				flags=*internal_flags;
				ptr_log_rec_cont->filter.set_all_msg_types((flags & Pt_SET)!=0);
		} else {
			g_system_settings.sys_message(system_settings::ERROR_MSG, string("Not found ponter to instanse of ABN_AllMessagesTypesBtn"));
		}

//getting instanse of AllStationsBtn and set it`s state by set_all_stations 
	tmp_widget=ApGetWidgetPtr(dialog_widget, ABN_AllStationsBtn);
	if (tmp_widget!=NULL)
		{
				long flags;
				long const *internal_flags;
				PtGetResource(tmp_widget, Pt_ARG_FLAGS, &internal_flags, 0);				
				flags=*internal_flags;
				ptr_log_rec_cont->filter.set_all_stations((flags & Pt_SET)!=0);
		} else {
			g_system_settings.sys_message(system_settings::ERROR_MSG, string("Not found ponter to instanse of ABN_AllStationsBtn"));
		}

//getting instanse of AllDevicesBtn and set it`s state by set_all_devices 
	tmp_widget=ApGetWidgetPtr(dialog_widget, ABN_AllDevicesBtn);
	if (tmp_widget!=NULL)
		{
				long flags;
				long const *internal_flags;
				PtGetResource(tmp_widget, Pt_ARG_FLAGS, &internal_flags, 0);				
				flags=*internal_flags;
				ptr_log_rec_cont->filter.set_all_devices((flags & Pt_SET)!=0);
		} else {
			g_system_settings.sys_message(system_settings::ERROR_MSG, string("Not found ponter to instanse of ABN_AllDevicesBtn"));
		}

// preparing data of START filtering time
	tm  time_to_filtered;
	time_to_filtered.tm_isdst =daylight;
	tmp_widget=ApGetWidgetPtr(dialog_widget, ABN_CalendarStart);
	if (tmp_widget!=NULL)
		{
			const PtCalendarDate_t *internal_pt_time;
			PtCalendarDate_t pt_time;

			PtGetResource(tmp_widget, Pt_ARG_CALENDAR_DATE, &internal_pt_time, sizeof(PtCalendarDate_t));		
			pt_time=*internal_pt_time;

			time_to_filtered.	tm_sec=0;
			time_to_filtered.	tm_mday=pt_time.day+1;
			time_to_filtered.	tm_mon=pt_time.month;
			time_to_filtered.	tm_year=pt_time.year-1900;
	
			tmp_widget=ApGetWidgetPtr(dialog_widget, ABN_StartHourNumeric);
			if (tmp_widget!=NULL)
			{
				const int *hour_value;
				PtGetResource(tmp_widget, Pt_ARG_NUMERIC_VALUE, &hour_value, 0);		
				time_to_filtered.	tm_hour=*hour_value;

				tmp_widget=ApGetWidgetPtr(dialog_widget, ABN_StartMinNumeric);
				if (tmp_widget!=NULL)
				{
					const int *min_value;
					PtGetResource(tmp_widget, Pt_ARG_NUMERIC_VALUE, &min_value, 0);		
					time_to_filtered.	tm_min=*min_value;
					
					ptr_log_rec_cont->filter.set_start_time(mktime(&time_to_filtered));
					
				} else {
					g_system_settings.sys_message(system_settings::ERROR_MSG, string("Not found ponter to instanse of ABN_StartMinNumeric"));			
				};

			} else {
				g_system_settings.sys_message(system_settings::ERROR_MSG, string("Not found ponter to instanse of ABN_StartHourNumeric"));			
			};
		} else {
			g_system_settings.sys_message(system_settings::ERROR_MSG, string("Not found ponter to instanse of ABN_CalendarStart"));
		};


	
// preparing data of END filtering time
	memset (&time_to_filtered, 0, sizeof(time_to_filtered));
	time_to_filtered.tm_isdst =daylight ;
	tmp_widget=ApGetWidgetPtr(dialog_widget, ABN_CalendarEnd);
	if (tmp_widget!=NULL)
		{
			const PtCalendarDate_t *internal_pt_time;
			PtCalendarDate_t pt_time;

			PtGetResource(tmp_widget, Pt_ARG_CALENDAR_DATE, &internal_pt_time, sizeof(PtCalendarDate_t));		
			pt_time=*internal_pt_time;

			time_to_filtered.	tm_sec=0;
			time_to_filtered.	tm_mday=pt_time.day+1;
			time_to_filtered.	tm_mon=pt_time.month;
			time_to_filtered.	tm_year=pt_time.year-1900;
	
			tmp_widget=ApGetWidgetPtr(dialog_widget, ABN_EndHourNumeric);
			if (tmp_widget!=NULL)
			{
				const int *hour_value;
				PtGetResource(tmp_widget, Pt_ARG_NUMERIC_VALUE, &hour_value, 0);		
				time_to_filtered.	tm_hour=*hour_value;

				tmp_widget=ApGetWidgetPtr(dialog_widget, ABN_EndMinNumeric);
				if (tmp_widget!=NULL)
				{
					const int *min_value;
					PtGetResource(tmp_widget, Pt_ARG_NUMERIC_VALUE, &min_value, 0);		
					time_to_filtered.	tm_min=*min_value;
					
					ptr_log_rec_cont->filter.set_stop_time(mktime(&time_to_filtered));
					
				} else {
					g_system_settings.sys_message(system_settings::ERROR_MSG, string("Not found ponter to instanse of ABN_EndMinNumeric"));			
				};

			} else {
				g_system_settings.sys_message(system_settings::ERROR_MSG, string("Not found ponter to instanse of ABN_EndHourNumeric"));			
			};
		} else {
			g_system_settings.sys_message(system_settings::ERROR_MSG, string("Not found ponter to instanse of ABN_CalendarEnd"));
		};

//list of devices for filtering
	ptr_log_rec_cont->filter.erase_devices_range(ptr_log_rec_cont->filter.devices_begin(),
																	ptr_log_rec_cont->filter.devices_end());

	ptr_log_rec_cont->filter.insert_devices_range(temp_log_filter.devices_begin(),
																	temp_log_filter.devices_end());

//list of stations for filtering
	ptr_log_rec_cont->filter.erase_stations_range(ptr_log_rec_cont->filter.stations_begin(),
																	ptr_log_rec_cont->filter.stations_end());

	ptr_log_rec_cont->filter.insert_stations_range(temp_log_filter.stations_begin(),
																	temp_log_filter.stations_end());

//message types list
	tmp_widget=ApGetWidgetPtr(dialog_widget, ABN_MessagesTypesList);
	if (tmp_widget!=NULL)
		{
		vector<PtArg_t> args(2);
		const unsigned short *internal_sel_indexes, *internal_sel_count;
		unsigned short sel_item_count;

		ptr_log_rec_cont->filter.erase_msg_types_range(	ptr_log_rec_cont->filter.msg_types_begin(),
													ptr_log_rec_cont->filter.msg_types_end());


		PtSetArg( &args[0], Pt_ARG_SELECTION_INDEXES, &internal_sel_indexes, 0 );
		PtSetArg( &args[1], Pt_ARG_LIST_SEL_COUNT, &internal_sel_count, 0 );
		PtGetResources (tmp_widget, args.size(), &args[0]);	
		sel_item_count=*internal_sel_count;


			if (sel_item_count>0) 
			{
			vector<unsigned short> selected_indexes(&internal_sel_indexes[0],
																			 	&internal_sel_indexes[sel_item_count]);

			vector<unsigned short>::iterator iter_sel_indexes=selected_indexes.begin();		
			while (iter_sel_indexes!=selected_indexes.end())
				{
					msg_types_container::msg_types_iterator 
								tmp_msg_types_iter=g_msg_types.begin();

					advance(tmp_msg_types_iter, static_cast<int>(*iter_sel_indexes)-1);

					if (tmp_msg_types_iter!=g_msg_types.end())
							ptr_log_rec_cont->filter.insert_msg_type (tmp_msg_types_iter->second.get_type_id());
		
					iter_sel_indexes++;
				}; // while (iter_sel_indexes!=selected_
			}; //if (sel_item_count>0) 
		} else {
			g_system_settings.sys_message(system_settings::ERROR_MSG, string("Not found ponter to instanse of ABN_MessagesTypesList"));
		}

	if (	ptr_log_rec_cont->filter.get_all_times() &&
		ptr_log_rec_cont->filter.get_stop_time()<=
		ptr_log_rec_cont->filter.get_start_time()) 	
				g_system_settings.message_window(system_settings::WARN_MSG, 
																		"Коая дата фильтрации должна быть позже, чем начальная");

	if (! ptr_log_rec_cont->filter.get_all_times() &&
		ptr_log_rec_cont->filter.get_stop_time()<=
		ptr_log_rec_cont->filter.get_start_time()) 
			{ 
				g_system_settings.message_window(system_settings::ERROR_MSG, 
																		"Конечная дата фильтрации должна быть позже, чем начальная");
				return( Pt_CONTINUE );
			};


	if (	! ptr_log_rec_cont->filter.get_all_msg_types() &&
		ptr_log_rec_cont->filter.msg_types_empty()) 
			{
				g_system_settings.message_window(system_settings::ERROR_MSG,
																		"Не выбран ни один тип сообщения");
				return( Pt_CONTINUE );
			}

	if (	! ptr_log_rec_cont->filter.get_all_stations() &&
		ptr_log_rec_cont->filter.stations_empty()) 
			{
				g_system_settings.message_window(system_settings::ERROR_MSG, 
																		"Не выбрана ни одна станция");
				return( Pt_CONTINUE );
			};

	if (	! ptr_log_rec_cont->filter.get_all_devices() &&
		ptr_log_rec_cont->filter.devices_empty()) 
			{
				g_system_settings.message_window(system_settings::ERROR_MSG, 
																		"Не выбрано ни одно из устройств на станциях");
				return( Pt_CONTINUE );
			};

	if (old_log_filter==ptr_log_rec_cont->filter) 	
			{
				PtUnrealizeWidget( dialog_widget);
				return  Pt_CONTINUE;
			};

	tmp_widget=ptr_log_rec_cont->get_filtration_state_indicator();
	if (tmp_widget!=NULL)
		{
		vector<PtArg_t> args(3);
		string indicator_text;
		if (ptr_log_rec_cont->filter.get_filter_state())
			{ 
				PtSetArg( &args[0], Pt_ARG_LABEL_IMAGE, g_system_settings.get_image(system_settings::BLOCK_RED_LED), 0);
				PtSetArg( &args[1], Pt_ARG_COLOR, system_settings::COLOR_RED, 0);				
				indicator_text="Фильтр ВКЛ";
			} else {
				PtSetArg( &args[0], Pt_ARG_LABEL_IMAGE, g_system_settings.get_image(system_settings::BLOCK_GREY_LED), 0);
				PtSetArg( &args[1], Pt_ARG_COLOR, system_settings::COLOR_BLACK, 0);				
				indicator_text="Фильтр ВЫКЛ";
			};

		PtSetArg( &args[2], Pt_ARG_TEXT_STRING, indicator_text.c_str(),  indicator_text.size());
		PtSetResources (tmp_widget, args.size(), &args[0]);	
			
		} else {
			g_system_settings.sys_message(system_settings::ERROR_MSG, string("Not found ponter to filtration state indicator"));
		}

		
	ptr_log_rec_cont->set_filtering();
	ptr_log_rec_cont->prepare_to_display();

	PtUnrealizeWidget( dialog_widget);

	return( Pt_CONTINUE );
	}

int
activate_AllTimeRangeBtn( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
	{

	PtWidget_t   *filter_dlg_wnd, *tmp_widget_pointer, *widget_tree_cursor;
	int tree_return_value;	
	vector<PtArg_t> args(2);
	
	filter_dlg_wnd=ApGetInstance(	widget);
	if (filter_dlg_wnd==NULL) {
		g_system_settings.sys_message(system_settings::ERROR_MSG, string ("Can`t get pointer to LogFilterDlg"));
		return( Pt_END );
	};
	
	PtBasicCallback_t *tmp_cbdata=static_cast<PtBasicCallback_t*>(cbinfo->cbdata);
		
	if (static_cast<long>(tmp_cbdata->value) !=0)	 //previos state
	 {
		PtSetArg(&args[0], Pt_ARG_FLAGS, Pt_TRUE, Pt_GHOST);
		PtSetArg(&args[1], Pt_ARG_FLAGS, Pt_TRUE, Pt_BLOCKED);
	} else {
		PtSetArg(&args[0], Pt_ARG_FLAGS, Pt_FALSE, Pt_GHOST);
		PtSetArg(&args[1], Pt_ARG_FLAGS, Pt_FALSE, Pt_BLOCKED);
	};
	
	tmp_widget_pointer=ApGetWidgetPtr(filter_dlg_wnd, ABN_StartTimePanel);
	// now tmp_widget_pointer point to  ABN_StartTimePanel instance in window
	if (tmp_widget_pointer==NULL) 
		{
			g_system_settings.sys_message(system_settings::ERROR_MSG, string("Can`t get StartTimePanel for LogFilterDlg window"));
			return( Pt_END );
		} else {
			widget_tree_cursor=tmp_widget_pointer;
			tree_return_value=Pt_TRAVERSE_START;
			
			PtSetResources(widget_tree_cursor, args.size(), &args[0]);
			while( ( tree_return_value=PtWidgetTree( tmp_widget_pointer,
																			&widget_tree_cursor,
																			tree_return_value ) 
						) !=Pt_TRAVERSE_DONE)
			{
				PtSetResources(widget_tree_cursor, args.size(), &args[0]);
     		};

		};

	tmp_widget_pointer=ApGetWidgetPtr(filter_dlg_wnd, ABN_EndTimePanel);
	// now tmp_widget_pointer point to  ABN_StartTimePanel instance in window
	if (tmp_widget_pointer==NULL) 
		{
			g_system_settings.sys_message(system_settings::ERROR_MSG, string("Can`t get EndTimePanel for LogFilterDlg window"));
			return( Pt_END );
		} else {
			widget_tree_cursor=tmp_widget_pointer;
			tree_return_value=Pt_TRAVERSE_START;
			
			PtSetResources(widget_tree_cursor, args.size(), &args[0]);
			while( ( tree_return_value=PtWidgetTree( tmp_widget_pointer,
																			&widget_tree_cursor,
																			tree_return_value ) 
						) !=Pt_TRAVERSE_DONE)
			{
				PtSetResources(widget_tree_cursor, args.size(), &args[0]);
     		};

		};


	return( Pt_CONTINUE );

	}

int
NextAlert( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{
	PtArg_t		arg;

	if (!g_alerts.empty())
	{
		g_alerts.pop_back();
	} else { //it`s impossible - pop_back of last alert must turn off button
		PtSetArg(&arg, Pt_ARG_FLAGS, Pt_TRUE, Pt_BLOCKED | Pt_GHOST);
		PtSetResources(ABW_AlertOK, 1, &arg); 	
	};
	return( Pt_CONTINUE );

	}


int
ActivateAlertBtn( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	PtArg_t		arg;
	if (g_alerts.empty())
	{
		PtSetArg(&arg, Pt_ARG_FLAGS, Pt_TRUE, Pt_BLOCKED | Pt_GHOST);
		PtSetResources(ABW_AlertOK, 1, &arg); 
	}

	return( Pt_CONTINUE );

	}

/*
Callbacks from main_wnd
*/

int
Uninitialize( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{
	if (pthread_cancel(g_pingTID) != EOK)
		{
		string  message ("In closing ping thread error - ");
		message+=strerror(errno);
		g_system_settings.sys_message(system_settings::ERROR_MSG, 
															message);

		};

	if (pthread_cancel(g_timerTID) != EOK)
		{
		string  message ("In closing system timer thread error - ");
		message+=strerror(errno);
		g_system_settings.sys_message(system_settings::ERROR_MSG, 
															message);

		};
	
 	metro_escalators_container::iterator_metro_escalators iter_esc = g_escalators.begin();
	while (iter_esc != g_escalators.end())
	{
		iter_esc->second.disconnect_from_channel();
		iter_esc++;
	}

	// зыкрваем канал
	ChannelDestroy(g_chanID);
	// сохраняем лог
 	g_main_log.save(g_system_settings.get_main_log_name());

	// закрываем блоиотеку виджетов
	if (g_system_settings.get_widgets_dbase()!=NULL)
		{
			ApCloseDBase(g_system_settings.get_widgets_dbase());
		};
		

	return( Pt_CONTINUE );

	}

int
RealizeMainWnd( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
	{

	g_system_settings.set_main_window(widget);
	
	g_msg_types.load(&g_system_settings, 
								g_system_settings.get_messages_types_name() );

	g_msg_dictionary.load(&g_system_settings, 
										&g_msg_types,
										g_system_settings.get_global_messages_name() );

	g_metro_devices_types.load( &g_system_settings,
			  										&g_msg_types,	
													g_system_settings.get_devices_types_name() );

	g_main_log.set_widget(ABW_main_log_wnd);
	g_main_log.load(g_system_settings.get_main_log_name());

	// добавляем сообщение
	int msg_id=msg_dict_container::MSG_PROGRAM_STARTED;	
	msg_dict_container::msg_dict_iterator tmp_msg_iter=g_msg_dictionary.find(msg_id);
	if(tmp_msg_iter==g_msg_dictionary.end())
			{
				vector<char> tmp_chars(10);
				string mess("Not found message id ");
				itoa(msg_id, &tmp_chars[0], 10);
				mess +=&tmp_chars[0];

				g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
				return false;
		};
	int msg_type=tmp_msg_iter->second.get_type();
	
 	g_main_log.set_records_autoincrement(g_main_log.get_records_autoincrement()+1); 
	g_main_log.insert(log_record (
												 g_main_log.get_records_autoincrement(),
												 msg_id, //	message_id  Program started
												 msg_type, //	message_type
												 0, // station_id -  broadcast
												 0, // device_id -  broadcast
												 time(NULL)
												)
							);
	
	g_main_log.filter.set_filter_state(false);
	g_main_log.set_filtering();
	g_main_log.prepare_to_display();

	{
	vector<PtArg_t> args(3);
	string indicator_text;
	g_main_log.set_filtration_state_indicator(ABW_on_off_filter_main_log);
	if (g_main_log.filter.get_filter_state())
		{ 
			PtSetArg( &args[0], Pt_ARG_LABEL_IMAGE, g_system_settings.get_image(system_settings::BLOCK_RED_LED), 0);
			PtSetArg( &args[1], Pt_ARG_COLOR, system_settings::COLOR_RED, 0);				
			indicator_text="Фильтр ВКЛ";
		} else {
			PtSetArg( &args[0], Pt_ARG_LABEL_IMAGE, g_system_settings.get_image(system_settings::BLOCK_GREY_LED), 0);
			PtSetArg( &args[1], Pt_ARG_COLOR, system_settings::COLOR_BLACK, 0);				
			indicator_text="Фильтр ВЫКЛ";
		};

	PtSetArg( &args[2], Pt_ARG_TEXT_STRING, indicator_text.c_str(),  indicator_text.size());
	
	PtSetResources (ABW_on_off_filter_main_log, args.size(), &args[0]);	
	}

	
	// создаем схему
	CreateScheme();
	
	return( Pt_CONTINUE );

	}


int
CloseMainWnd( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
	{
	g_system_settings.set_main_window(NULL);

	// дбвляем сообщение
	int msg_id=msg_dict_container::MSG_PROGRAM_CLOSED;	
	msg_dict_container::msg_dict_iterator tmp_msg_iter=g_msg_dictionary.find(msg_id);
	if(tmp_msg_iter==g_msg_dictionary.end())
			{
				vector<char> tmp_chars(10);
				string mess("Not found message id ");
				itoa(msg_id, &tmp_chars[0], 10);
				mess +=&tmp_chars[0];

				g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
				return false;
		};
	int msg_type=tmp_msg_iter->second.get_type();
	
 	g_main_log.set_records_autoincrement(g_main_log.get_records_autoincrement()+1); 
	g_main_log.insert(log_record (
												 g_main_log.get_records_autoincrement(),
												 msg_id, //	message_id  Program started
												 msg_type, //	message_type
												 0, // station_id -  broadcast
												 0, // device_id -  broadcast
												 time(NULL)
												)
							);

	g_main_log.save(g_system_settings.get_main_log_name());

	return( Pt_CONTINUE );

	}


int
ResizeScheme( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{
	PtArg_t		arg;
	PhDim_t		*wnd_size;
	PtContainerCallback_t	*data = (PtContainerCallback_t*)cbinfo->cbdata;
	vector<PhPoint_t>  points;

	metro_lines_container::iterator_metro_lines   temp_lines_iterator;   
	
	temp_lines_iterator=g_lines.begin();
	while (temp_lines_iterator!=g_lines.end())
	{
		vector<PhPoint_t>  points;

		metro_line::iterator_stations_id temp_stations_id_iterator=temp_lines_iterator->second.begin_stations_id();
		while(temp_stations_id_iterator!=temp_lines_iterator->second.end_stations_id())
		{
			metro_stations_container::iterator_metro_stations temp_metro_stations_iterator=g_stations.find(*temp_stations_id_iterator);
			if(temp_metro_stations_iterator!=g_stations.end())
				{
					PhPoint_t temp_point;
					
					temp_point.x=temp_metro_stations_iterator->second.get_x_coord()*data->new_dim.w/100;
					temp_point.y= temp_metro_stations_iterator->second.get_y_coord()*data->new_dim.h/100;
					
					temp_metro_stations_iterator->second.set_x_coord(temp_point.x);
					temp_metro_stations_iterator->second.set_y_coord(temp_point.y);
					
					points.push_back(temp_point);
			
					if (temp_metro_stations_iterator->second.get_wnd()!=NULL)
						{		
							PtSetArg(&arg, Pt_ARG_DIM, &wnd_size, 0);
							PtGetResources(temp_metro_stations_iterator->second.get_wnd(), 1, &arg);

							temp_point.x = temp_metro_stations_iterator->second.get_x_coord() - wnd_size->w/2;
							temp_point.y = temp_metro_stations_iterator->second.get_y_coord() - wnd_size->h/2;
	
					 		PtSetArg(&arg, Pt_ARG_POS, &temp_point, 0);
					 		PtSetResources(temp_metro_stations_iterator->second.get_wnd(), 1, &arg);		
					 	};

					
				}; 
			temp_stations_id_iterator++;
		};
		if (temp_lines_iterator->second.get_line_widget()!=NULL) 
			{
		 	PtSetArg(&arg, Pt_ARG_POINTS, &points[0], points.size());	 	
		 	PtSetResources(temp_lines_iterator->second.get_line_widget(), 1, &arg);
		 	};
    	
		temp_lines_iterator++;
	};

	return( Pt_CONTINUE );

	}


int
OpenArchive( PtWidget_t *link_instance, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

 	PtWidget_t*	wnd= g_archive_log.get_widget();

	if (wnd!=NULL)
	{
		PtWindowToFront(wnd);
		return (Pt_IGNORE);
	}


	return( Pt_CONTINUE );

	}
/*
escalator callbacks
*/

int
SelectEscalatorDirection( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{
	int		direction, index;
	vector<PtArg_t> args(3);

	metro_escalators_container::iterator_metro_escalators iter_esc;
	
	index=((PtListCallback_t*)cbinfo->cbdata)->item_pos-1;
	
	if ( (!g_escalators.empty())
		&& (cbinfo->reason == Pt_CB_SELECTION) 
		&& (cbinfo->reason_subtype == Pt_LIST_SELECTION_BROWSE)
		&& (index<g_escalators.size())	
	)
	{

		iter_esc=g_escalators.begin();
		advance(iter_esc, index);
		g_escalators.set_current_escalator_in_directions(iter_esc);
		direction = iter_esc->second.get_pref_direction();


		switch(direction)
		{
			case system_settings::DIRECTION_STOP:
				PtSetArg(&args[0], Pt_ARG_FLAGS, Pt_FALSE, Pt_SET);
				PtSetArg(&args[1], Pt_ARG_FLAGS, Pt_FALSE, Pt_SET);
				PtSetArg(&args[2], Pt_ARG_FLAGS, Pt_FALSE, Pt_SET);
				break;
			case system_settings::DIRECTION_UP:
				PtSetArg(&args[0], Pt_ARG_FLAGS, Pt_TRUE, Pt_SET);
				PtSetArg(&args[1], Pt_ARG_FLAGS, Pt_FALSE, Pt_SET);
				PtSetArg(&args[2], Pt_ARG_FLAGS, Pt_FALSE, Pt_SET);
				break;
			case system_settings::DIRECTION_DOWN:
				PtSetArg(&args[0], Pt_ARG_FLAGS, Pt_FALSE, Pt_SET);
				PtSetArg(&args[1], Pt_ARG_FLAGS, Pt_TRUE, Pt_SET);
				PtSetArg(&args[2], Pt_ARG_FLAGS, Pt_FALSE, Pt_SET);
				break;
			case system_settings::DIRECTION_REVERSE:
				PtSetArg(&args[0], Pt_ARG_FLAGS, Pt_FALSE, Pt_SET);
				PtSetArg(&args[1], Pt_ARG_FLAGS, Pt_FALSE, Pt_SET);
				PtSetArg(&args[2], Pt_ARG_FLAGS, Pt_TRUE, Pt_SET);
				break;
			default:
				PtSetArg(&args[0], Pt_ARG_FLAGS, Pt_TRUE, Pt_SET);
				PtSetArg(&args[1], Pt_ARG_FLAGS, Pt_TRUE, Pt_SET);
				PtSetArg(&args[2], Pt_ARG_FLAGS, Pt_TRUE, Pt_SET);
				break;
		}		

	PtSetResources(ABW_SetEscUpBtn, 1, &args[0]);
	PtSetResources(ABW_SetEscDownBtn, 1, &args[1]);
	PtSetResources(ABW_SetEscReverseBtn, 1, &args[2]);
	}


	return( Pt_CONTINUE );

	}


int
SetUpDirection( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{
	PtArg_t		arg_other;
	string	str_to_list;
	metro_stations_container::iterator_metro_stations iter_stat;
	metro_escalators_container::iterator_metro_escalators  selected_escalator_in_directions=g_escalators.get_current_escalator_in_directions();
	
	if (selected_escalator_in_directions!=g_escalators.end())
	{
		str_to_list.reserve(128);
		vector<char> tmp_char(10);
		itoa(selected_escalator_in_directions->second.get_number(),&tmp_char[0],10);
		
		PtSetArg(&arg_other, Pt_ARG_FLAGS, Pt_FALSE, Pt_SET);
		PtSetResources(ABW_SetEscDownBtn, 1, &arg_other);
		PtSetResources(ABW_SetEscReverseBtn, 1, &arg_other);
		
		selected_escalator_in_directions->second.set_pref_direction(system_settings::DIRECTION_UP);
		iter_stat=g_stations.find(selected_escalator_in_directions->second.get_station_id());
		if(iter_stat!=g_stations.end())
			{
			str_to_list=iter_stat->second.get_stl_name_string();			
			};
			str_to_list+="\t";
			str_to_list+=&tmp_char[0];
			str_to_list+="\tподьем";
			
			PtListReplaceItemPos(ABW_EscDirectionList, 
											(const char**)(str_to_list.c_str()), 
											1, 
											distance(g_escalators.begin(), selected_escalator_in_directions) + 1);	
	}

	return( Pt_CONTINUE );

	}


int
SetDownDirection( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	PtArg_t		arg_other;
	string	str_to_list;
	metro_stations_container::iterator_metro_stations iter_stat;
	metro_escalators_container::iterator_metro_escalators  selected_escalator_in_directions=g_escalators.get_current_escalator_in_directions();
	
	if (selected_escalator_in_directions!=g_escalators.end())
	{
		str_to_list.reserve(128);
		vector<char> tmp_char(10);
		itoa(selected_escalator_in_directions->second.get_number(),&tmp_char[0],10);
		
		PtSetArg(&arg_other, Pt_ARG_FLAGS, Pt_FALSE, Pt_SET);
		PtSetResources(ABW_SetEscUpBtn, 1, &arg_other);
		PtSetResources(ABW_SetEscReverseBtn, 1, &arg_other);
		
		selected_escalator_in_directions->second.set_pref_direction(system_settings::DIRECTION_DOWN);
		iter_stat=g_stations.find(selected_escalator_in_directions->second.get_station_id());
		if(iter_stat!=g_stations.end())
			{
			str_to_list=iter_stat->second.get_stl_name_string();			
			};
			str_to_list+="\t";
			str_to_list+=&tmp_char[0];
			str_to_list+="\tспуск";

			PtListReplaceItemPos(ABW_EscDirectionList,
											(const char**)(str_to_list.c_str()),
											1,
											distance(g_escalators.begin(), selected_escalator_in_directions) + 1);	
	}

	return( Pt_CONTINUE );

	}


int
OnSaveDirections( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{
	if (!g_escalators.save_directions("directions.dat") )
		{
		g_system_settings.message_window (system_settings::ERROR_MSG, string("Не удсь сохранить направления движеня эскалаторов в айле !"));
		};

	return( Pt_CONTINUE );

	}


int
MoveUp( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
	{

metro_escalators_container::iterator_metro_escalators esc_iter=g_escalators.get_current_escalator();
if (esc_iter==g_escalators.end())  return( Pt_CONTINUE ); // no escalators selected

//(const byte escalator_id, const byte command, const int id_station, const int escalator_number) 
command cmd(esc_iter->second.get_id(), system_settings::COMMAND_UP, esc_iter->second.get_station_id(), esc_iter->second.get_number());

//begin: Remove it!!!
cmd.set_item_color(Pg_PURPLE);
g_command_pool.push_back(cmd);
//end: Remove it!!!

	if ( esc_iter->second.get_enabled()==system_settings::ENABLED
		&& esc_iter->second.get_online() == system_settings::ONLINE
		&& esc_iter->second.get_mode() ==  system_settings::MODE_MAIN_DRIVE_AT_REMOTE_CONTROL)
	{
		if (esc_iter->second.get_pref_direction() == system_settings::DIRECTION_REVERSE)
		{		
			if (esc_iter->second.confirm_direction(system_settings::DIRECTION_UP) == 1) 
				{
				cmd.set_item_color(Pg_RED);
				g_command_pool.push_back(cmd);
				};
		}
		else if ((esc_iter->second.get_pref_direction() == system_settings::DIRECTION_DOWN)
					||(esc_iter->second.get_direction() == system_settings::DIRECTION_DOWN))
		{
			if (esc_iter->second.confirm_direction(system_settings::DIRECTION_UP) == 1)
				{
				cmd.set_item_color(Pg_RED);
				g_command_pool.push_back(cmd);	
				};
		}
		else	
			{
				cmd.set_item_color(Pg_PURPLE);
				g_command_pool.push_back(cmd);
			};
 	}

	return( Pt_CONTINUE );
	}


int
MoveStop( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{
metro_escalators_container::iterator_metro_escalators esc_iter=g_escalators.get_current_escalator();
if (esc_iter==g_escalators.end())  return( Pt_CONTINUE ); // no escalators selected

	if ( esc_iter->second.get_enabled()==system_settings::ENABLED
		&& esc_iter->second.get_online() == system_settings::ONLINE
		&& esc_iter->second.get_mode() == system_settings::MODE_MAIN_DRIVE_AT_REMOTE_CONTROL)
	{
			esc_iter->second.send_command(system_settings::COMMAND_STOP);
	}

	return( Pt_CONTINUE );

	}


int
MoveDown( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

metro_escalators_container::iterator_metro_escalators esc_iter=g_escalators.get_current_escalator();
if (esc_iter==g_escalators.end())  return( Pt_CONTINUE ); // no escalators selected


//(const byte escalator_id, const byte command, const int id_station, const int escalator_number) 
command cmd(esc_iter->second.get_id(), system_settings::COMMAND_DOWN, esc_iter->second.get_id(), esc_iter->second.get_number());

//begin: Remove it!!!
cmd.set_item_color(Pg_DGREEN);
g_command_pool.push_back(cmd);
//end: Remove it!!!


	if ( esc_iter->second.get_enabled()==system_settings::ENABLED
		&& esc_iter->second.get_online() == system_settings::ONLINE
		&& esc_iter->second.get_mode() == system_settings::MODE_MAIN_DRIVE_AT_REMOTE_CONTROL)
	{
		if (esc_iter->second.get_pref_direction() == system_settings::DIRECTION_REVERSE)
		{		
			if (esc_iter->second.confirm_direction(system_settings::DIRECTION_DOWN) == 1)
				{
				cmd.set_item_color(Pg_RED);
				g_command_pool.push_back(cmd);
				};
		}	else if ((esc_iter->second.get_pref_direction() == system_settings::DIRECTION_UP)
					||(esc_iter->second.get_direction() == system_settings::DIRECTION_UP))
		{
			if (esc_iter->second.confirm_direction(system_settings::DIRECTION_DOWN)==1)
				{
				cmd.set_item_color(Pg_RED);
				g_command_pool.push_back(cmd);
				};
		}else	{
			cmd.set_item_color(Pg_DGREEN);
			g_command_pool.push_back(cmd);
		};
	};
	return( Pt_CONTINUE );

	}


int
SetupPanel( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{
	dword escalator_id;
	PtGetResource(widget, Pt_ARG_USER_DATA, escalator_id, 0);
	
	metro_escalators_container::iterator_metro_escalators escalator_iterator=g_escalators.find(escalator_id);	
	
	if (g_system_settings.get_escalators_panel()!=NULL
		&& g_system_settings.get_paneled_escalator_id()!=escalator_id)
		{
		PtDestroyWidget(g_system_settings.get_escalators_panel());
		};

	g_system_settings.set_escalators_panel(NULL);
	
	if (escalator_iterator!=g_escalators.end())
		{
		string	panel_title;
		g_system_settings.set_paneled_escalator_id(escalator_id);

		metro_stations_container::iterator_metro_stations iter_station=g_stations.find(escalator_iterator->second.get_station_id());
		if (iter_station!=g_stations.end()) 
			{
				panel_title=iter_station->second.get_stl_name_string();
			} else {
				panel_title="--------";
			};

		panel_title+=" /Эскалатор ";
		vector<char> tmp_char(10);
		itoa(escalator_iterator->second.get_number(), &tmp_char[0], 10);		
		panel_title+=&tmp_char[0];
		
//		PtSetResource(ApGetWidgetPtr(escalator_iterator->second., ABN_PanelTitle), Pt_ARG_TEXT_STRING, panel_title.c_str(), 0);
		PtSetResource(ApGetWidgetPtr(widget, ABN_PanelTitle), Pt_ARG_TEXT_STRING, panel_title.c_str(), 0);
		escalator_iterator->second.update_panel();
		escalator_iterator->second.update_leds();

		} else {
			string mess="Not found escalator id ";
			vector<char> tmp_chars(10);
			itoa(escalator_id, &tmp_chars[0], 10);
			mess+=&tmp_chars[0];
			g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
			return( Pt_END );

		};

	return( Pt_CONTINUE );

	}


int
SetReverseDirection( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{
	PtArg_t		arg_other;
	string	str_to_list;
	metro_stations_container::iterator_metro_stations iter_stat;
	metro_escalators_container::iterator_metro_escalators  selected_escalator_in_directions=g_escalators.get_current_escalator_in_directions();
	
	if (selected_escalator_in_directions!=g_escalators.end())
	{
		str_to_list.reserve(128);
		vector<char> tmp_char(10);
		itoa(selected_escalator_in_directions->second.get_number(), &tmp_char[0],10);
	
		PtSetArg(&arg_other, Pt_ARG_FLAGS, Pt_FALSE, Pt_SET);
		PtSetResources(ABW_SetEscDownBtn, 1, &arg_other);
		PtSetResources(ABW_SetEscUpBtn, 1, &arg_other);
		
		selected_escalator_in_directions->second.set_pref_direction(system_settings::DIRECTION_REVERSE);
		iter_stat=g_stations.find(selected_escalator_in_directions->second.get_station_id());
		if(iter_stat!=g_stations.end())
			{
			str_to_list=iter_stat->second.get_stl_name_string();			
			};
			str_to_list+="\t";
			str_to_list+=&tmp_char[0];
			str_to_list+="\tреверсивный";
					
			PtListReplaceItemPos(ABW_EscDirectionList, 
											(const char**)(str_to_list.c_str()),
											1,
											distance(g_escalators.begin(), selected_escalator_in_directions) + 1);	
	}

	return( Pt_CONTINUE );

	}


int
OnChancelDirections( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{
	if (!g_escalators.load_directions(g_system_settings.get_devices_config_name()) )
		{
		g_system_settings.message_window (system_settings::ERROR_MSG, string("Не удалось загрузить направления движени эскалаторов из файла !"));
		};

	return( Pt_CONTINUE );

	}


int
FillEscConfList( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
	{
		vector<const char*> list_strings;
		enum{UNDEFINED=0, UP, DOWN, REVERSE, ITEMS_COUNT};
		vector<string> directions_strings(ITEMS_COUNT);
				
		directions_strings[UNDEFINED]="не определено";
		directions_strings[UP]="вверх";
		directions_strings[DOWN]="вниз";
		directions_strings[REVERSE]="реерсивный";
	
		string item_string;
		vector<char> tmp_chars(8);
		int tmp_int;
		
		metro_stations_container::iterator_metro_stations iter_stat;
		metro_escalators_container::iterator_metro_escalators iter_esc=g_escalators.begin();
		
		while(iter_esc!=g_escalators.end())		
		{
			iter_stat=g_stations.find(iter_esc->second.get_station_id());		
			if (iter_stat==g_stations.end()) 
				{
					item_string="нет станции !\t";
				} else {
					item_string=iter_stat->second.get_stl_name_string();
					item_string="\t";
				};
			itoa(iter_esc->second.get_number(), &tmp_chars[0],10);
			item_string+=&tmp_chars[0];
			item_string+="\t";
			tmp_int=iter_esc->second.get_pref_direction();
			if (tmp_int<=UNDEFINED ||
				tmp_int>=ITEMS_COUNT) tmp_int=UNDEFINED;
				
			item_string+=directions_strings[tmp_int];
			list_strings.push_back(item_string.c_str());
				
			iter_esc++;
		};
	
		PtListAddItems(ABW_EscDirectionList, &list_strings[0], list_strings.size(), 0);	

	return( Pt_CONTINUE );

	}

int
UnrealizeEscalatorPanel( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	g_system_settings.set_paneled_escalator_id(0);
	g_system_settings.set_escalators_panel(NULL);

	return( Pt_CONTINUE );

	}


int
RealizeEscalatorPanel( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{
	string	escalator_description;
	metro_escalators_container::iterator_metro_escalators paneled_escalator;
	metro_stations_container::iterator_metro_stations station_iterator;
	
		if ((paneled_escalator=g_escalators.find(g_system_settings.get_paneled_escalator_id())) != g_escalators.end())
		{	
			g_system_settings.set_escalators_panel(widget);
			station_iterator=g_stations.find(paneled_escalator->second.get_station_id());

			if(station_iterator!=g_stations.end()) 
				{
				escalator_description=station_iterator->second.get_stl_name_string();
				} else {
				escalator_description="-----";
				};

			escalator_description+=" / Эскалатор ";
			escalator_description+=itoa(paneled_escalator->second.get_number(), NULL, 10);

			PtSetResource(ApGetWidgetPtr(widget, ABN_PanelTitle), Pt_ARG_TEXT_STRING, escalator_description.c_str(), 0);
			
			devices_types_container::iterator_devices_types iter_types;
			
			iter_types=g_metro_devices_types.find(paneled_escalator->second.get_type());
			if (iter_types!=g_metro_devices_types.end()) 
				{
				PtWidget_t* panelTabs = ApGetWidgetPtr(widget, ABN_SignalPanel);
				iter_types->second.create_panels(panelTabs,
									&g_system_settings);
						
				}; //if (iter_types!...
			
			paneled_escalator->second.update_leds();
			paneled_escalator->second.update_panel();
		};

	return( Pt_CONTINUE );

	}


int
PopupControlMenu( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{
	int escalator_id;
	metro_escalators_container::iterator_metro_escalators esc_iter;

	PtGetResource(widget, Pt_ARG_USER_DATA, escalator_id, 0);
	esc_iter=g_escalators.find(escalator_id);	
	if (esc_iter==g_escalators.end())  return( Pt_CONTINUE ); // that escalator not found
	
//	begin: only for command pool testing -  remove it!
	g_escalators.set_current_escalator(esc_iter);
	ApCreateModule(ABM_EscalatorMenu, widget, cbinfo);
//	end: only for command pool testing -  remove it!

	if ((esc_iter->second.get_enabled())
		&& (esc_iter->second.get_online() == system_settings::ONLINE)
		&& (esc_iter->second.get_mode() == system_settings::MODE_MAIN_DRIVE_AT_REMOTE_CONTROL))
	{
		if (esc_iter->second.get_ready()!=system_settings::BLOCK_NORMA)
			{
			g_system_settings.message_window (system_settings::WARN_MSG, string("Нет готовности!"));
			} else
			{
			g_escalators.set_current_escalator(esc_iter);
			ApCreateModule(ABM_EscalatorMenu, widget, cbinfo);
			}
	}

	return( Pt_CONTINUE );

	}


int
activate_on_off_filter_main_log( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
	{
	PtCallbackInfo_t clbc_info;
	clbc_info.cbdata=&g_main_log;
	g_main_log.set_filtration_state_indicator(widget);

	ApCreateModule( ABM_LogFilterDlg, NULL, &clbc_info);

	return( Pt_CONTINUE );
	}

int
link_setup_LogFilterDlg( PtWidget_t *link_instance, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
	{
	log_records_container* ptr_log_rec_cont=NULL;
	ptr_log_rec_cont=static_cast<log_records_container*>(cbinfo->cbdata);
	PtWidget_t *tmp_widget=NULL;
	vector<char>tmp_chars(8);
	string tmp_string;
	tmp_string.reserve(20);

	if (ptr_log_rec_cont==NULL)
	 {
		g_system_settings.sys_message(system_settings::ERROR_MSG, string("Not found ponter to log_records_container"));
		return( Pt_END );
	}
	PtSetResource(link_instance, Pt_ARG_POINTER, ptr_log_rec_cont, 0);
	
	tmp_widget=ApGetWidgetPtr(link_instance, ABN_FilteringBtn);
	if (tmp_widget!=NULL)
		{

			if (ptr_log_rec_cont->filter.get_filter_state())
				{
					PtSetResource(tmp_widget, Pt_ARG_FLAGS, Pt_TRUE, Pt_SET); 
				} else {
					PtSetResource(tmp_widget, Pt_ARG_FLAGS, Pt_FALSE, Pt_SET); 
				};
				
		} else {
			g_system_settings.sys_message(system_settings::ERROR_MSG, string("Not found ponter to instanse of ABN_FilteringBtn"));
		}

	tmp_widget=ApGetWidgetPtr(link_instance, ABN_AllTimeRangeBtn);
	if (tmp_widget!=NULL)
		{
			PtCallbackInfo_t cbinfo_actvate;
			PtBasicCallback_t cbdata_actvate;
			int toggle_state;

			if (ptr_log_rec_cont->filter.get_all_times())
				{
		 			toggle_state=1;
					PtSetResource(tmp_widget, Pt_ARG_FLAGS, Pt_TRUE, Pt_SET); 
				} else {
		 			toggle_state=0;
					PtSetResource(tmp_widget, Pt_ARG_FLAGS, Pt_FALSE, Pt_SET); 
				};
			
			cbdata_actvate.value=toggle_state;
			cbinfo_actvate.cbdata=&cbdata_actvate;
			activate_AllTimeRangeBtn( tmp_widget, NULL, &cbinfo_actvate);

		} else {
			g_system_settings.sys_message(system_settings::ERROR_MSG, string("Not found ponter to instanse of ABN_AllTimeRangeBtn"));
		}

	tmp_widget=ApGetWidgetPtr(link_instance, ABN_AllMessagesTypesBtn);
	if (tmp_widget!=NULL)
		{
			PtCallbackInfo_t cbinfo_actvate;
			PtBasicCallback_t cbdata_actvate;
			int toggle_state;
			if (ptr_log_rec_cont->filter.get_all_msg_types())
				{
		 			toggle_state=1;
					PtSetResource(tmp_widget, Pt_ARG_FLAGS, Pt_TRUE, Pt_SET); 
				} else {
		 			toggle_state=0;
					PtSetResource(tmp_widget, Pt_ARG_FLAGS, Pt_FALSE, Pt_SET); 
				};
				
			cbdata_actvate.value=toggle_state;
			cbinfo_actvate.cbdata=&cbdata_actvate;
			activate_AllMessagesTypesBtn( tmp_widget, NULL, &cbinfo_actvate);

		} else {
			g_system_settings.sys_message(system_settings::ERROR_MSG, string("Not found ponter to instanse of ABN_AllMessagesTypesBtn"));
		}

	tmp_widget=ApGetWidgetPtr(link_instance, ABN_AllStationsBtn);
	if (tmp_widget!=NULL)
		{
			PtCallbackInfo_t cbinfo_actvate;
			PtBasicCallback_t cbdata_actvate;
			int toggle_state;

			if (ptr_log_rec_cont->filter.get_all_stations())
				{
					toggle_state=1;
					PtSetResource(tmp_widget, Pt_ARG_FLAGS, Pt_TRUE, Pt_SET); 
				} else {
					toggle_state=0;
					PtSetResource(tmp_widget, Pt_ARG_FLAGS, Pt_FALSE, Pt_SET); 
				};

			cbdata_actvate.value=toggle_state;
			cbinfo_actvate.cbdata=&cbdata_actvate;
			activate_AllStationsBtn( tmp_widget, NULL, &cbinfo_actvate);

		} else {
			g_system_settings.sys_message(system_settings::ERROR_MSG, string("Not found ponter to instanse of ABN_AllStationsBtn"));
		}

	tmp_widget=ApGetWidgetPtr(link_instance, ABN_AllDevicesBtn);
	if (tmp_widget!=NULL)
		{
			PtCallbackInfo_t cbinfo_actvate;
			PtBasicCallback_t cbdata_actvate;
			int toggle_state;
		
			if (ptr_log_rec_cont->filter.get_all_devices())
				{
					toggle_state=1;
					PtSetResource(tmp_widget, Pt_ARG_FLAGS, Pt_TRUE, Pt_SET); 
				} else {
					toggle_state=0;
					PtSetResource(tmp_widget, Pt_ARG_FLAGS, Pt_FALSE, Pt_SET); 
				};
	
			cbdata_actvate.value=toggle_state;
			cbinfo_actvate.cbdata=&cbdata_actvate;
			activate_AllDevicesBtn( tmp_widget, NULL, &cbinfo_actvate);


		} else {
			g_system_settings.sys_message(system_settings::ERROR_MSG, string("Not found ponter to instanse of ABN_AllDevicesBtn"));
		}

	tmp_widget=ApGetWidgetPtr(link_instance, ABN_MessagesTypesList);
	if (tmp_widget!=NULL)
		{
		if (!g_msg_types.empty())
			{
					vector<unsigned short> indexes_of_selected_items;
					const char* list_items[1];
					indexes_of_selected_items.reserve(ptr_log_rec_cont->filter.msg_types_size());
					unsigned short current_index=1;
					msg_types_container::msg_types_iterator tmp_iter=g_msg_types.begin();
	
					while (tmp_iter!=g_msg_types.end())
					{
					list_items[0]=tmp_iter->second.get_type_description().c_str();

					PtListAddItems( tmp_widget,
								             list_items,
								             1,
									         0);
					
					if (ptr_log_rec_cont->filter.msg_types_end()!=
						ptr_log_rec_cont->filter.find_msg_type(tmp_iter->second.get_type_id()))
						{
							indexes_of_selected_items.push_back(current_index);
						};
			
					current_index++;
					tmp_iter++;
					}

					if (! indexes_of_selected_items.empty())
						{									       
							PtSetResource(tmp_widget, Pt_ARG_SELECTION_INDEXES, &indexes_of_selected_items[0], indexes_of_selected_items.size()); 
						} else {
							PtSetResource(tmp_widget, Pt_ARG_SELECTION_INDEXES, NULL, 0); 
						};

			};


		} else {
			g_system_settings.sys_message(system_settings::ERROR_MSG, string("Not found ponter to instanse of ABN_MessagesTypesList"));
		}

	tmp_widget=ApGetWidgetPtr(link_instance, ABN_StationsList);
	if (tmp_widget!=NULL)
		{
		if (!g_stations.empty())
			{
					vector<unsigned short> indexes_of_selected_items;
					const char* list_items[1];
					indexes_of_selected_items.reserve(ptr_log_rec_cont->filter.stations_size());
					unsigned short current_index=1;
					metro_stations_container::iterator_metro_stations tmp_iter=g_stations.begin();
	
					while (tmp_iter!=g_stations.end())
					{
					list_items[0]=tmp_iter->second.get_c_name_string();

					PtListAddItems( tmp_widget,
								             list_items,
								             1,
									         0);
					
					if (ptr_log_rec_cont->filter.stations_end()!=
						ptr_log_rec_cont->filter.find_station(tmp_iter->second.get_id()))
						{
							indexes_of_selected_items.push_back(current_index);
						};
			
					current_index++;
					tmp_iter++;
					}
					if (! indexes_of_selected_items.empty())
						{									       
							PtSetResource(tmp_widget, Pt_ARG_SELECTION_INDEXES, &indexes_of_selected_items[0], indexes_of_selected_items.size()); 
						} else {
							PtSetResource(tmp_widget, Pt_ARG_SELECTION_INDEXES, NULL, 0); 
						};

			};


		} else {
			g_system_settings.sys_message(system_settings::ERROR_MSG, string("Not found ponter to instanse of ABN_StationsList"));
		};

    temp_log_filter=ptr_log_rec_cont->filter;
 
	redraw_devices_list_in_filter_dlg(
													link_instance,
													temp_log_filter,
													&g_escalators,
													&g_stations,
													&g_system_settings
															);

		struct tm curTimeTM;
		time_t tmp_time=ptr_log_rec_cont->filter.get_start_time();
		localtime_r(&tmp_time, &curTimeTM);

		tmp_widget=ApGetWidgetPtr(link_instance, ABN_StartHourNumeric);
		if (tmp_widget!=NULL)
		{
			PtSetResource(tmp_widget,
									Pt_ARG_NUMERIC_VALUE,
									curTimeTM.tm_hour,
									0);
		} else {
			g_system_settings.sys_message(system_settings::ERROR_MSG, string("Not found ponter to instanse of ABN_StartHourNumeric"));
		};

		tmp_widget=ApGetWidgetPtr(link_instance, ABN_StartMinNumeric);
		if (tmp_widget!=NULL)
		{
			PtSetResource(tmp_widget,
									Pt_ARG_NUMERIC_VALUE,
									curTimeTM.tm_min,
									0);
		} else {
			g_system_settings.sys_message(system_settings::ERROR_MSG, string("Not found ponter to instanse of ABN_StartMinNumeric"));
		};

		tmp_widget=ApGetWidgetPtr(link_instance, ABN_CalendarStart);
		if (tmp_widget!=NULL)
		{
			PtSetResource(tmp_widget,
									Pt_ARG_CALENDAR_TIME_T,
									ptr_log_rec_cont->filter.get_start_time(),
									sizeof (ptr_log_rec_cont->filter.get_start_time()));
		} else {
			g_system_settings.sys_message(system_settings::ERROR_MSG, string("Not found ponter to instanse of ABN_CalendarStart"));
		};

		tmp_time=ptr_log_rec_cont->filter.get_stop_time();
		localtime_r(&tmp_time, &curTimeTM);

		tmp_widget=ApGetWidgetPtr(link_instance, ABN_EndHourNumeric);
		if (tmp_widget!=NULL)
		{
			PtSetResource(tmp_widget,
									Pt_ARG_NUMERIC_VALUE,
									curTimeTM.tm_hour,
									0);
		} else {
			g_system_settings.sys_message(system_settings::ERROR_MSG, string("Not found ponter to instanse of ABN_EndHourNumeric"));
		};

		tmp_widget=ApGetWidgetPtr(link_instance, ABN_EndMinNumeric);
		if (tmp_widget!=NULL)
		{
			PtSetResource(tmp_widget,
									Pt_ARG_NUMERIC_VALUE,
									curTimeTM.tm_min,
									0);
		} else {
			g_system_settings.sys_message(system_settings::ERROR_MSG, string("Not found ponter to instanse of ABN_EndMinNumeric"));
		};

		tmp_widget=ApGetWidgetPtr(link_instance, ABN_CalendarEnd);
		if (tmp_widget!=NULL)
		{
			PtSetResource(tmp_widget,
									Pt_ARG_CALENDAR_TIME_T,
									ptr_log_rec_cont->filter.get_stop_time(),
									sizeof (ptr_log_rec_cont->filter.get_stop_time()));
		} else {
			g_system_settings.sys_message(system_settings::ERROR_MSG, string("Not found ponter to instanse of ABN_CalendarEnd"));
		};


	return( Pt_CONTINUE );
	}


int
unrealized_LogFilterDlg( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
	{

	return( Pt_CONTINUE );
	}


int
activate_AllStationsBtn( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	PtWidget_t   *filter_dlg_wnd, *tmp_widget_pointer;
	vector<PtArg_t> args(2);
	
	filter_dlg_wnd=ApGetInstance(	widget);
	if (filter_dlg_wnd==NULL) {
		g_system_settings.sys_message(system_settings::ERROR_MSG, string ("Can`t get pointer to LogFilterDlg"));
		return( Pt_END );
	};
	
	PtBasicCallback_t *tmp_cbdata=static_cast<PtBasicCallback_t*>(cbinfo->cbdata);
		
	if (static_cast<long>(tmp_cbdata->value) !=0)	 //previos state
	 {
		PtSetArg(&args[0], Pt_ARG_FLAGS, Pt_TRUE, Pt_GHOST);
		PtSetArg(&args[1], Pt_ARG_FLAGS, Pt_TRUE, Pt_BLOCKED);
	} else {
		PtSetArg(&args[0], Pt_ARG_FLAGS, Pt_FALSE, Pt_GHOST);
		PtSetArg(&args[1], Pt_ARG_FLAGS, Pt_FALSE, Pt_BLOCKED);
	};
	
	tmp_widget_pointer=ApGetWidgetPtr(filter_dlg_wnd, ABN_StationsList);
	if (tmp_widget_pointer==NULL) 
		{
			g_system_settings.sys_message(system_settings::ERROR_MSG, string("Can`t get StationsList for LogFilterDlg window"));
			return( Pt_END );
		} else {
				PtSetResources(tmp_widget_pointer, args.size(), &args[0]);
		};

	tmp_widget_pointer=ApGetWidgetPtr(filter_dlg_wnd, ABN_AllDevicesBtn);
	if (tmp_widget_pointer==NULL) 
		{
			g_system_settings.sys_message(system_settings::ERROR_MSG, string("Can`t get AllDevicesBtn for LogFilterDlg window"));
			return( Pt_END );
		} else {
			// now tmp_widget_pointer point to  ABN_AllDevicesBtn instance in window	
			const unsigned long  *internal_flags_all_devices_state;	
			unsigned long  flags_all_devices_state;	
			PtGetResource( tmp_widget_pointer,  Pt_ARG_FLAGS, &internal_flags_all_devices_state, 0);
			PtSetResources(tmp_widget_pointer, args.size(), &args[0]);			
			flags_all_devices_state=*internal_flags_all_devices_state;
				
			tmp_widget_pointer=ApGetWidgetPtr(filter_dlg_wnd, ABN_DevicesList);
			if (tmp_widget_pointer==NULL) 
			{
				g_system_settings.sys_message(system_settings::ERROR_MSG, string("Can`t get DevicesList for LogFilterDlg window"));
				return( Pt_END );
			} else {

				if ( static_cast<long>(tmp_cbdata->value) ==0 &&
				(flags_all_devices_state & Pt_SET) ==0)	
				 {
					PtSetArg(&args[0], Pt_ARG_FLAGS, Pt_FALSE, Pt_GHOST);
					PtSetArg(&args[1], Pt_ARG_FLAGS, Pt_FALSE, Pt_BLOCKED);

				 }  else {
					PtSetArg(&args[0], Pt_ARG_FLAGS, Pt_TRUE, Pt_GHOST);
					PtSetArg(&args[1], Pt_ARG_FLAGS, Pt_TRUE, Pt_BLOCKED);

				};
			
				PtSetResources(tmp_widget_pointer, args.size(), &args[0]);
			};

		};
	
	return( Pt_CONTINUE );

	}


int
activate_AllDevicesBtn( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
	{
	PtWidget_t   *filter_dlg_wnd, *tmp_widget_pointer;
	vector<PtArg_t> args(2);
	
	filter_dlg_wnd=ApGetInstance(	widget);
	if (filter_dlg_wnd==NULL) {
		g_system_settings.sys_message(system_settings::ERROR_MSG, string ("Can`t get pointer to LogFilterDlg"));
		return( Pt_END );
	};
	
	PtBasicCallback_t *tmp_cbdata=static_cast<PtBasicCallback_t*>(cbinfo->cbdata);

	if (static_cast<long>(tmp_cbdata->value) !=0)	 //previos state
	 {
		PtSetArg(&args[0], Pt_ARG_FLAGS, Pt_TRUE, Pt_GHOST);
		PtSetArg(&args[1], Pt_ARG_FLAGS, Pt_TRUE, Pt_BLOCKED);
	} else {
		PtSetArg(&args[0], Pt_ARG_FLAGS, Pt_FALSE, Pt_GHOST);
		PtSetArg(&args[1], Pt_ARG_FLAGS, Pt_FALSE, Pt_BLOCKED);
	};
	
	tmp_widget_pointer=ApGetWidgetPtr(filter_dlg_wnd, ABN_DevicesList);
	if (tmp_widget_pointer==NULL) 
		{
			g_system_settings.sys_message(system_settings::ERROR_MSG, string("Can`t get DevicesList for LogFilterDlg window"));
			return( Pt_END );
		} else {
			PtSetResources(tmp_widget_pointer, args.size(), &args[0]);
		};

	return( Pt_CONTINUE );
	}


int
activate_AllMessagesTypesBtn( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{
	PtWidget_t   *filter_dlg_wnd, *tmp_widget_pointer;
	vector<PtArg_t> args(2);
	
	filter_dlg_wnd=ApGetInstance(	widget);
	if (filter_dlg_wnd==NULL) {
		g_system_settings.sys_message(system_settings::ERROR_MSG, string ("Can`t get pointer to LogFilterDlg"));
		return( Pt_END );
	};
	
	PtBasicCallback_t *tmp_cbdata=static_cast<PtBasicCallback_t*>(cbinfo->cbdata);
		
	if (static_cast<long>(tmp_cbdata->value) !=0)	 //previos state
	 {
		PtSetArg(&args[0], Pt_ARG_FLAGS, Pt_TRUE, Pt_GHOST);
		PtSetArg(&args[1], Pt_ARG_FLAGS, Pt_TRUE, Pt_BLOCKED);
	} else {
		PtSetArg(&args[0], Pt_ARG_FLAGS, Pt_FALSE, Pt_GHOST);
		PtSetArg(&args[1], Pt_ARG_FLAGS, Pt_FALSE, Pt_BLOCKED);
	};
	
	tmp_widget_pointer=ApGetWidgetPtr(filter_dlg_wnd, ABN_MessagesTypesList);
	if (tmp_widget_pointer==NULL) 
		{
			g_system_settings.sys_message(system_settings::ERROR_MSG, string("Can`t get MessagesTypesList for LogFilterDlg window"));
			return( Pt_END );
		} else {
			PtSetResources(tmp_widget_pointer, args.size(), &args[0]);
		};

	return( Pt_CONTINUE );

	}


int
itemselection_StationList( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
	{
	PtListCallback_t *list_callback_data=static_cast<PtListCallback_t*>(cbinfo->cbdata);
	ushort_t *sel_array=list_callback_data->sel_array;
	int sel_item_count=list_callback_data->sel_item_count;
	log_filter::ids_container new_stations_selection;
	metro_stations_container::iterator_metro_stations tmp_stat_iter;

	for(int i=0;i<sel_item_count; i++ )
		{
		tmp_stat_iter=g_stations.begin();
		advance(tmp_stat_iter, static_cast<int>(sel_array[i])-1);

		if (tmp_stat_iter!=g_stations.end())
			new_stations_selection.insert(tmp_stat_iter->second.get_id());
		};

//removing selected escalators of unselected stations
	log_filter::ids_container_iterator tmp_stat_id_iter=temp_log_filter.stations_begin();
	while(tmp_stat_id_iter!=temp_log_filter.stations_end())
	{
		if (new_stations_selection.find(*tmp_stat_id_iter)==
			new_stations_selection.end())
		{
		// all of escalators id from that station was deleted
			tmp_stat_iter=g_stations.find(*tmp_stat_id_iter);
			if (tmp_stat_iter!=g_stations.end())
				{

					metro_station::iterator_escalators_id tmp_esc_id=tmp_stat_iter->second.begin_escalators_id();
					while(tmp_esc_id!=tmp_stat_iter->second.end_escalators_id())
						{
						temp_log_filter.erase_device (*tmp_esc_id);
						tmp_esc_id++;
						}
				};			
		};
   	tmp_stat_id_iter++;
	}
	
	temp_log_filter.erase_stations_range (temp_log_filter.stations_begin(),
 												   temp_log_filter.stations_end());

	temp_log_filter.insert_stations_range (new_stations_selection.begin(),
 												   new_stations_selection.end());

	redraw_devices_list_in_filter_dlg(
													ApGetInstance( widget ),
													temp_log_filter,
													&g_escalators,
													&g_stations,
													&g_system_settings
															);


	return( Pt_CONTINUE );

	}


int
itemselection_DevicesList( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{
	PtListCallback_t *list_callback_data=static_cast<PtListCallback_t*>(cbinfo->cbdata);
	ushort_t *sel_array=list_callback_data->sel_array;
	int sel_item_count=list_callback_data->sel_item_count;
	log_filter::ids_container all_viewed_escalators;
	metro_stations_container::iterator_metro_stations tmp_stat_iter;

	log_filter::ids_container_iterator tmp_ids_iterator=temp_log_filter.stations_begin();
	while (tmp_ids_iterator!=temp_log_filter.stations_end())
	{
		tmp_stat_iter=g_stations.find(*tmp_ids_iterator);	
		if(tmp_stat_iter!=g_stations.end())
			{
				all_viewed_escalators.insert(tmp_stat_iter->second.begin_escalators_id(),
															tmp_stat_iter->second.end_escalators_id());
			};
		
		tmp_ids_iterator++;
	}

	temp_log_filter.erase_devices_range( temp_log_filter.devices_begin(),
																temp_log_filter.devices_end());

	for(int i=0;i<sel_item_count; i++ )
		{
		tmp_ids_iterator=all_viewed_escalators.begin();
		advance(tmp_ids_iterator, static_cast<int>(sel_array[i])-1);

		if (tmp_ids_iterator!=all_viewed_escalators.end())
			{
			temp_log_filter.insert_device(*tmp_ids_iterator);
			};
		};


	return( Pt_CONTINUE );

	}

/*
Callbacks for ArchiveLog  and ArchiveDlg window
*/


int
realized_ArchiveLog( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{
	PtWidget_t  *filtration_state_indicator, *archive_log_window;

	archive_log_window = ApGetWidgetPtr(widget, 
																ABN_ArchiveLogWnd);

	if (archive_log_window!=NULL)
		g_archive_log.set_widget(archive_log_window);

	g_archive_log.filter.set_filter_state(false);

	filtration_state_indicator = ApGetWidgetPtr(widget, 
																	ABN_on_off_filter_archive_log);

	if (filtration_state_indicator!=NULL) 
	{
		vector<PtArg_t> args(3);
		string indicator_text;
		g_archive_log.set_filtration_state_indicator(filtration_state_indicator);
		if (g_archive_log.filter.get_filter_state())
		{ 
			PtSetArg( &args[0], Pt_ARG_LABEL_IMAGE, g_system_settings.get_image(system_settings::BLOCK_RED_LED), 0);
			PtSetArg( &args[1], Pt_ARG_COLOR, system_settings::COLOR_RED, 0);				
			indicator_text="Фильтр ВКЛ";
		} else {
			PtSetArg( &args[0], Pt_ARG_LABEL_IMAGE, g_system_settings.get_image(system_settings::BLOCK_GREY_LED), 0);
			PtSetArg( &args[1], Pt_ARG_COLOR, system_settings::COLOR_BLACK, 0);				
			indicator_text="Фильтр ВЫКЛ";
		};

		PtSetArg( &args[2], Pt_ARG_TEXT_STRING, indicator_text.c_str(),  indicator_text.size());

		PtSetResources (filtration_state_indicator, args.size(), &args[0]);	
	} else { //if (filtration_state_indicator..
		g_system_settings.sys_message(	system_settings::ERROR_MSG,
															"Not found filtration_state_indicator in realized_ArchiveLog");
	}; //if (filtration_state_indicator..

	return( Pt_CONTINUE );
	}


int
unrealized_ArchiveLog( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{
	
	g_archive_log.set_widget(NULL);
	g_archive_log.set_filtration_state_indicator(NULL);
	g_archive_log.erase_all();

	return( Pt_CONTINUE );

	}


int
activate_LoadArchiveBtn( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{
	PtWidget_t  *window_wgt, *file_selection_wgt;
	PtFileSelItem_t *item;	
	
	window_wgt=ApGetInstance(widget);

	if (window_wgt==NULL) 
		{
			g_system_settings.sys_message(	system_settings::ERROR_MSG,
															"Not found window_wgt in activate_LoadArchiveBtn");
			
			return Pt_HALT;
		};

	file_selection_wgt = ApGetWidgetPtr( window_wgt,
															  ABN_ArchiveFiles);

	if (file_selection_wgt==NULL) 
		{
			g_system_settings.sys_message(	system_settings::ERROR_MSG,
															"Not found file_selection_wgt in activate_LoadArchiveBtn");
			
			return Pt_HALT;
		};

	item = PtFSGetCurrent(file_selection_wgt);
	 
    if (item == NULL)
    		{
			g_system_settings.sys_message(	system_settings::ERROR_MSG,
															"Not found item in activate_LoadArchiveBtn");
    			
	        return(Pt_CONTINUE);
        };

	g_archive_log.load (item->fullpath);
	g_archive_log.filter.set_filter_state(false);
	g_archive_log.set_filtering();
	
	if (g_archive_log.get_filtration_state_indicator()!=NULL) 
	{
		vector<PtArg_t> args(3);
		string indicator_text;
		if (g_archive_log.filter.get_filter_state())
		{ 
			PtSetArg( &args[0], Pt_ARG_LABEL_IMAGE, g_system_settings.get_image(system_settings::BLOCK_RED_LED), 0);
			PtSetArg( &args[1], Pt_ARG_COLOR, system_settings::COLOR_RED, 0);				
			indicator_text="Фильтр ВКЛ";
		} else {
			PtSetArg( &args[0], Pt_ARG_LABEL_IMAGE, g_system_settings.get_image(system_settings::BLOCK_GREY_LED), 0);
			PtSetArg( &args[1], Pt_ARG_COLOR, system_settings::COLOR_BLACK, 0);				
			indicator_text="Фильтр ВЫКЛ";
		};

		PtSetArg( &args[2], Pt_ARG_TEXT_STRING, indicator_text.c_str(),  indicator_text.size());

		PtSetResources (g_archive_log.get_filtration_state_indicator(),
									args.size(),
									&args[0]);	

	} else { // if (g_archive_log.get_filtration_state_indicator...
		g_system_settings.sys_message(	system_settings::ERROR_MSG,
															"Not found filtration_state_indicator in activate_LoadArchiveBtn");
	}; // if (g_archive_log.get_filtration_state_indicator..

	
	g_archive_log.prepare_to_display();

	return( Pt_CONTINUE );
	}


int
activate_on_off_filter_archive_log( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{
	PtCallbackInfo_t clbc_info;
	clbc_info.cbdata=&g_archive_log;

	g_archive_log.set_filtration_state_indicator(widget);

	ApCreateModule( ABM_LogFilterDlg, NULL, &clbc_info);


	return( Pt_CONTINUE );

	}

