/* Y o u r   D e s c r i p t i o n                       */
/*                            AppBuilder Photon Code Lib */
/*                                         Version 2.01  */

#include "global.h"

/*
Local functions
*/
static void fill_station_list()
{
metro_stations_container::iterator_metro_stations iter_station=g_stations.begin();
while (iter_station!=g_stations.end())
{
	PtListAddItems(ABW_StationList, (const char**)(iter_station->second.get_stl_name_string().c_str()), 1, 0);
	iter_station++;
};
}

static void re_calculate_command_items_count(PtWidget_t *cmd_pool_list)
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
		
		temp_lines_iterator->second.sort_stations_id();
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
	count=g_CommandPool.size();
	p_cmd_pool=g_CommandPool.begin();
	
	while(p_cmd_pool!=g_CommandPool.end() && curr_item)
		{
			if(curr_item->flags&Pt_LIST_ITEM_SELECTED>0)
						{
						(*p_cmd_pool).send();
						g_CommandPool.erase(p_cmd_pool);
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
			count=g_CommandPool.size();
			p_cmd_pool=g_CommandPool.begin();
	
			while(p_cmd_pool!=g_CommandPool.end() && curr_item)
				{
					if(curr_item->flags&Pt_LIST_ITEM_SELECTED>0)
						{
							g_CommandPool.erase(p_cmd_pool);
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
							if(cmd_pool_cont!=&g_CommandPool)
							{
								g_CommandPool.set_widget(cmd_pool_list);
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
	PtWidget_t *filter_dlg_wnd= ApCreateModule( ABM_LogFilterDlg, NULL, NULL);
	PtArg_t arg;

	if (filter_dlg_wnd!=NULL)
		{
			PtSetArg(&arg, Pt_ARG_POINTER, &g_main_log, 0);
			PtSetResources(filter_dlg_wnd, 1, &arg);											
		} else {
			string mess ="Can`t get  filter window";
			g_system_settings.sys_message(system_settings::ERROR_MSG, mess);			
			return( Pt_END );
		};

	PtWidget_t *tmp_widget_pointer=NULL; // pointer for widget instances

//begin: station and escalators range widgets setting
	tmp_widget_pointer=ApGetWidgetPtr(filter_dlg_wnd, ABN_StationList);
	// now tmp_widget_pointer point to  ABN__StationList instance in window
	if (tmp_widget_pointer==NULL) 
		{
			string mess ="Can`t get StationList for filter window";
			g_system_settings.sys_message(system_settings::ERROR_MSG, mess);			
			return( Pt_END );
		} else {
			vector<const char*> stations_names(g_stations.size()+1);
			stations_names[0]="Все станции";
			metro_stations_container::iterator_metro_stations iter_stat=g_stations.begin();			
			for (int i=1; iter_stat!=g_stations.end(); i++,  iter_stat++)
						stations_names[i]=iter_stat->second.get_c_name_string();
			
//			PtListAddItems(tmp_widget_pointer, (const char**)(), stations_names.size(), 0);

			PtListAddItems(tmp_widget_pointer, const_cast<const char**>(&stations_names[0]), stations_names.size(), 0);
			PtSetResource(tmp_widget_pointer, Pt_ARG_CBOX_SEL_ITEM, 1, 0);
		}; 
		
//end: station and escalators range widgets setting

//begin: messages types widgets setting
	int 	messages=g_main_log.filter.get_messages();
	tmp_widget_pointer=ApGetWidgetPtr(filter_dlg_wnd, ABN_LocalMsgBtn);
	// now tmp_widget_pointer point to  ABN_LocalMsgBtn instance in window
	if (tmp_widget_pointer==NULL) 
		{
			string mess ="Can`t get LocalMsgBtn for filter window";
			g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
			return( Pt_END );
		} else {
			long widget_flags;
			PtGetResource (tmp_widget_pointer, Pt_ARG_FLAGS, &widget_flags,0);
			(messages||log_filter::MSG_LOCAL)?
								PtSetResource(tmp_widget_pointer, Pt_ARG_FLAGS, (widget_flags & Pt_SET), 0) :
								PtSetResource(tmp_widget_pointer, Pt_ARG_FLAGS, (widget_flags & ~Pt_SET), 0);
		};

	tmp_widget_pointer=ApGetWidgetPtr(filter_dlg_wnd, ABN_BlockMsgBtn);
	// now tmp_widget_pointer point to  ABN_BlockMsgBtn instance in window
	if (tmp_widget_pointer==NULL) 
		{
			string mess ="Can`t get BlockMsgBtn for filter window";
			g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
			return( Pt_END );
		} else {
			long widget_flags;
			PtGetResource (tmp_widget_pointer, Pt_ARG_FLAGS, &widget_flags,0);
			(messages||log_filter::MSG_BLOCK)?
								PtSetResource(tmp_widget_pointer, Pt_ARG_FLAGS, (widget_flags & Pt_SET), 0) :
								PtSetResource(tmp_widget_pointer, Pt_ARG_FLAGS, (widget_flags & ~Pt_SET), 0);

		};

	tmp_widget_pointer=ApGetWidgetPtr(filter_dlg_wnd, ABN_SysMsgBtn);
	// now tmp_widget_pointer point to  ABN_SysMsgBtn instance in window
	if (tmp_widget_pointer==NULL) 
		{
			string mess ="Can`t get SysMsgBtn for filter window";
			g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
			return( Pt_END );
		} else {
			long widget_flags;
			PtGetResource (tmp_widget_pointer, Pt_ARG_FLAGS, &widget_flags,0);
			(messages||log_filter::MSG_SYSTEM)?
								PtSetResource(tmp_widget_pointer, Pt_ARG_FLAGS, (widget_flags & Pt_SET), 0) :
								PtSetResource(tmp_widget_pointer, Pt_ARG_FLAGS, (widget_flags & ~Pt_SET), 0);
		};

	tmp_widget_pointer=ApGetWidgetPtr(filter_dlg_wnd, ABN_AlertMsgBtn);
	// now tmp_widget_pointer point to  ABN_AlertMsgBtn instance in window
	if (tmp_widget_pointer==NULL) 
		{
			string mess ="Can`t get AlertMsgBtn for filter window";
			g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
			return( Pt_END );
		} else {
			long widget_flags;
			PtGetResource (tmp_widget_pointer, Pt_ARG_FLAGS, &widget_flags,0);
			(messages||log_filter::MSG_ALERT)?
								PtSetResource(tmp_widget_pointer, Pt_ARG_FLAGS, (widget_flags & Pt_SET), 0) :
								PtSetResource(tmp_widget_pointer, Pt_ARG_FLAGS, (widget_flags & ~Pt_SET), 0);
		};
// end: messages types widgets setting

//begin: timerange widgets settings
	tmp_widget_pointer=ApGetWidgetPtr(filter_dlg_wnd, ABN_CalendarStart); 
	// now tmp_widget_pointer point to  ABN_CalendarStart instance in window
	if (tmp_widget_pointer==NULL) 
		{
			string mess ="Can`t get CalendarStart for filter window";
			g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
			return( Pt_END );
		} else {
		time_t tmp_time=g_main_log.filter.get_start_time();
		struct tm cur_time_tm;
		if (tmp_time==0)
			{
			tmp_time=time(NULL);
			};
		localtime_r(&tmp_time, &cur_time_tm);
		PtSetResource(tmp_widget_pointer, Pt_ARG_CALENDAR_TIME_T, tmp_time, 0);
		tmp_widget_pointer=ApGetWidgetPtr(filter_dlg_wnd, ABN_StartHourNumeric); 
		// now tmp_widget_pointer point to  ABN_StartHourNumeric instance in window
		if (tmp_widget_pointer==NULL) 
			{
				string mess ="Can`t get StartHourNumeric for filter window";
				g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
				return( Pt_END );
			} else {
				PtSetResource(tmp_widget_pointer, Pt_ARG_NUMERIC_VALUE, cur_time_tm.tm_hour, 0);
			};
		PtSetResource(tmp_widget_pointer, Pt_ARG_CALENDAR_TIME_T, tmp_time, 0);
		tmp_widget_pointer=ApGetWidgetPtr(filter_dlg_wnd, ABN_StartMinNumeric); 
		// now tmp_widget_pointer point to  ABN_StartHourNumeric instance in window
		if (tmp_widget_pointer==NULL) 
			{
				string mess ="Can`t get StartMinNumeric for filter window";
				g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
				return( Pt_END );
			} else {
				PtSetResource(tmp_widget_pointer,  Pt_ARG_NUMERIC_VALUE, cur_time_tm.tm_min, 0);
			};
		};
		
	tmp_widget_pointer=ApGetWidgetPtr(filter_dlg_wnd, ABN_CalendarEnd);
	// now tmp_widget_pointer point to  ABN_CalendarEnd instance in window
	if (tmp_widget_pointer!=NULL) 
		{
			string mess ="Can`t get CalendarEnd for filter window";
			g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
			return( Pt_END );
		} else {
		time_t tmp_time=g_main_log.filter.get_stop_time();
		struct tm cur_time_tm;
		if (tmp_time==0)
			{
			tmp_time=time(NULL);
			};
		localtime_r(&tmp_time, &cur_time_tm);
		PtSetResource(tmp_widget_pointer, Pt_ARG_CALENDAR_TIME_T, tmp_time, 0);
		
		tmp_widget_pointer=ApGetWidgetPtr(filter_dlg_wnd, ABN_EndHourNumeric); 
		// now tmp_widget_pointer point to  ABN_EndHourNumeric instance in window
		if (tmp_widget_pointer==NULL) 
			{
				string mess ="Can`t get EndHourNumeric for filter window";
				g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
				return( Pt_END );
			} else {
				PtSetResource(tmp_widget_pointer, Pt_ARG_NUMERIC_VALUE, cur_time_tm.tm_hour, 0);
			};
		PtSetResource(tmp_widget_pointer, Pt_ARG_CALENDAR_TIME_T, tmp_time, 0);
		
		tmp_widget_pointer=ApGetWidgetPtr(filter_dlg_wnd, ABN_EndMinNumeric); 
		// now tmp_widget_pointer point to  ABN_EndMinNumeric instance in window
		if (tmp_widget_pointer==NULL) 
			{
				string mess ="Can`t get EndMinNumeric for filter window";
				g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
				return( Pt_END );
			} else {
				PtSetResource(tmp_widget_pointer,  Pt_ARG_NUMERIC_VALUE, cur_time_tm.tm_min, 0);
			};
		};


	tmp_widget_pointer=ApGetWidgetPtr(filter_dlg_wnd, ABN_AllTimeRangeBtn);
	// now tmp_widget_pointer point to  ABN_AllTimeRangeBtn instance in window
	if (tmp_widget_pointer==NULL) 
		{
			string mess ="Can`t get AllTimeRangeBtn for filter window";
			g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
			return( Pt_END );
		} else {
			long widget_flags;
			PtGetResource (tmp_widget_pointer, Pt_ARG_FLAGS, &widget_flags,0);
			(g_main_log.filter.get_all_times()==0)?
								PtSetResource(tmp_widget_pointer, Pt_ARG_FLAGS, (widget_flags & Pt_SET), 0) :
								PtSetResource(tmp_widget_pointer, Pt_ARG_FLAGS, (widget_flags & ~Pt_SET), 0);
		};

//end: time range widgets settings

	return( Pt_CONTINUE );
	}


int
realize_filter_dlg( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	return( Pt_CONTINUE );

	}



int
activate_LogFilterOK( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{
		vector<PtArg_t>	args;
		time_t*		date;
		int				elapsed = 0;
		int station_index;
		string	message = "Внимание! В журнале включен фильт ообщений";
/*
		g_logFilter.set_station_index(get_widget_scalar(ABW_StationList, Pt_ARG_CBOX_SEL_ITEM) - 1);
		
		PtSetArg(&args[0], Pt_ARG_FLAGS, 0, 0);
		PtGetResources(ABW_SysMsgBtn, 1, &args[0]);		
		if (args[0].value & Pt_SET)
		{
			g_logFilter.set_messages (g_logFilter.get_messages() | log_filter::MSG_SYSTEM);
		}
		else
		{
			g_logFilter.set_messages (g_logFilter.get_messages() | ! log_filter::MSG_SYSTEM);
		}
		
		PtSetArg(&args[0], Pt_ARG_FLAGS, 0, 0);
		PtGetResources(ABW_AlertMsgBtn, 1, &args[0]);		
		if (args[0].value & Pt_SET)
		{
			g_logFilter.set_messages (g_logFilter.get_messages() | log_filter::MSG_ALERT);
		}
		else
		{
			g_logFilter.set_messages (g_logFilter.get_messages() | ! log_filter::MSG_ALERT);
		}

		PtSetArg(&args[0], Pt_ARG_FLAGS, 0, 0);
		PtGetResources(ABW_BlockMsgBtn, 1, &args[0]);		
		if (args[0].value & Pt_SET)
		{
			g_logFilter.set_messages (g_logFilter.get_messages() | log_filter::MSG_BLOCK);
		}
		else
		{
			g_logFilter.set_messages (g_logFilter.get_messages() | ! log_filter::MSG_BLOCK);
		}

		PtSetArg(&args[0], Pt_ARG_FLAGS, 0, 0);
		PtGetResources(ABW_LocalMsgBtn, 1, &args[0]);		
		if (args[0].value & Pt_SET)
		{
			g_logFilter.set_messages (g_logFilter.get_messages() | log_filter::MSG_LOCAL);
		}
		else
		{
			g_logFilter.set_messages (g_logFilter.get_messages() | ! log_filter::MSG_LOCAL);
		}

		
		PtSetArg(&args[0], Pt_ARG_FLAGS, 0, 0);
		PtGetResources(ABW_AllTimeRangeBtn1, 1, &args[0]);		

		if (args[0].value & Pt_SET)
		{
			g_logFilter.set_all_times(1);
			g_logFilter.set_start_time(0);
			g_logFilter.set_stop_time (0);
		}
		else
		{
			g_logFilter.set_all_times(1);

			PtSetArg(&args[0], Pt_ARG_CALENDAR_TIME_T, &date, 0);
			PtGetResources(ABW_CalendarStart, 1, &args[0]);		
			
			g_logFilter.set_start_time((time_t) date);
			g_logFilter.set_stop_time((time_t) date);

			elapsed += 3600*args[0].value;

			PtGetResources(ABW_CalendarEnd, 1, &args[0]);		
			PtSetArg(&args[0], Pt_ARG_NUMERIC_VALUE, 0, 0);
			PtGetResources(ABW_StartHourNumeric, 1, &args[0]);

			PtSetArg(&args[0], Pt_ARG_NUMERIC_VALUE, 0, 0);
			PtGetResources(ABW_StartMinNumeric, 1, &args[0]);
			elapsed += 60*args[0].value;
		
			g_logFilter.set_start_time(g_logFilter.get_start_time()+elapsed);
		
		
			elapsed = 0;
			
			PtSetArg(&args[0], Pt_ARG_NUMERIC_VALUE, 0, 0);
			PtGetResources(ABW_EndHourNumeric, 1, &args[0]);
			elapsed += 3600*args[0].value;
			PtSetArg(&args[0], Pt_ARG_NUMERIC_VALUE, 0, 0);
			PtGetResources(ABW_EndMinNumeric, 1, &args[0]);
			elapsed += 60*args[0].value;		
		
			g_logFilter.set_stop_time( g_logFilter.get_stop_time()+elapsed);
		}
		station_index=g_logFilter.get_station_index();
		if (station_index>=0)
		{
		PtSetArg(&args[0], Pt_ARG_FLAGS, 0, 0);
		PtGetResources(ABW_OneEscalatorBtn, 1, &args[0]);
		
		if (args[0].value & Pt_SET)
		{
			PtSetArg(&args[0], Pt_ARG_NUMERIC_VALUE, 0, 0);
			PtGetResources(ABW_OneEscalatorNumeric, 1, &args[0]);

			g_logFilter.set_first_escalator_num(args[0].value); 
			g_logFilter.set_last_escalator_num(args[0].value);
		}
		else
		{
			PtSetArg(&args[0], Pt_ARG_NUMERIC_VALUE, 0, 0);
			PtGetResources(ABW_RangeStartNumeric, 1, &args[0]);
			g_logFilter.set_first_escalator_num(args[0].value);
			
			PtSetArg(&args[0], Pt_ARG_NUMERIC_VALUE, 0, 0);
			PtGetResources(ABW_RangeEndNumeric, 1, &args[0]);
			g_logFilter.set_last_escalator_num(args[0].value);
		}
		}
		
//	mainLog.FilterLogWnd();	

	if (is_filter_changed(g_logFilter))
	{
		PtSetArg(&args[0], Pt_ARG_TEXT_STRING,  &message[0], message.size());
		PtSetArg(&args[1], Pt_ARG_COLOR,  Pg_RED, 0);	
		PtSetResources(ABW_log_status, 2, &args[0]);
	} else {
	ResetFilter( NULL, NULL, NULL);
	};

		printf("Live\n");
*/			
	return( Pt_CONTINUE );

	}

int
combobox_close_StationList( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
	{

	PtWidget_t   *filter_dlg_wnd, *tmp_widget_pointer, *widget_tree_cursor;
	int tree_return_value, first_escalators_in_station=1, count_escalators_in_station=0;	
	unsigned short index;
	long new_state_widgets_flag=0, widget_flags;
	
	filter_dlg_wnd=ApGetInstance(	widget);
	if (filter_dlg_wnd==NULL) {
		string mess ="Can`t get filter window";
		g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
		return( Pt_END );
	};
	
	PtGetResource(widget, Pt_ARG_CBOX_SEL_ITEM, &index, 0);
	// indexes of combo box items begining at 1  and item number 1 that is "All stations"
	if (index<2 || index>g_stations.size())
			{
				  new_state_widgets_flag=Pt_GHOST  | Pt_BLOCKED;
	 		 } else {
				metro_stations_container::iterator_metro_stations iter_stat=g_stations.begin();
				advance(iter_stat, index-2);

	 		 	count_escalators_in_station=iter_stat->second.size_escalators_id();
				new_state_widgets_flag=~Pt_GHOST & ~Pt_BLOCKED;  	 
			};

	if (first_escalators_in_station>count_escalators_in_station)  first_escalators_in_station=count_escalators_in_station;

	vector<PtArg_t> args(2);

	tmp_widget_pointer=ApGetWidgetPtr(filter_dlg_wnd, ABN_OneEscalatorNumeric);
	// now tmp_widget_pointer point to  ABN_OneEscalatorNumeric instance in window
	if (tmp_widget_pointer==NULL) 
		{
			string mess ="Can`t get OneEscalatorNumeric for filter window";
			g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
			return( Pt_END );
		} else {
			PtSetArg(&args[0], Pt_ARG_NUMERIC_VALUE, &first_escalators_in_station, 0);
			PtSetArg(&args[1], Pt_ARG_NUMERIC_MAX, &count_escalators_in_station, 0);

			PtSetResources (tmp_widget_pointer, args.size(), &args[0]);
		};

	tmp_widget_pointer=ApGetWidgetPtr(filter_dlg_wnd, ABN_RangeStartNumeric);
	// now tmp_widget_pointer point to  ABN_RangeStartNumeric instance in window
	if (tmp_widget_pointer==NULL) 
		{
			string mess ="Can`t get RangeStartNumeric for filter window";
			g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
			return( Pt_END );
		} else {
			PtSetArg(&args[0], Pt_ARG_NUMERIC_VALUE, &first_escalators_in_station, 0);
			PtSetArg(&args[1], Pt_ARG_NUMERIC_MAX, &count_escalators_in_station, 0);

			PtSetResources (tmp_widget_pointer, args.size(), &args[0]);
		};

	tmp_widget_pointer=ApGetWidgetPtr(filter_dlg_wnd, ABN_RangeEndNumeric);
	// now tmp_widget_pointer point to  ABN_RangeEndNumeric instance in window
	if (tmp_widget_pointer==NULL) 
		{
			string mess ="Can`t get RangeEndNumeric for filter window";
			g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
			return( Pt_END );
		} else {
			PtSetArg(&args[0], Pt_ARG_NUMERIC_VALUE, &first_escalators_in_station, 0);
			PtSetArg(&args[1], Pt_ARG_NUMERIC_MAX, &count_escalators_in_station, 0);

			PtSetResources (tmp_widget_pointer, args.size(), &args[0]);
		};

	tmp_widget_pointer=ApGetWidgetPtr(filter_dlg_wnd, ABN_StationEscalatorsPanel);
	// now tmp_widget_pointer point to  ABN_StationEscalatorsPanel instance in window
	if (tmp_widget_pointer==NULL) 
		{
			string mess ="Can`t get StationEscalatorsPanel for filter window";
			g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
			return( Pt_END );
		} else {
			widget_tree_cursor=tmp_widget_pointer;
			tree_return_value=Pt_TRAVERSE_START;

			while( ( tree_return_value=PtWidgetTree( tmp_widget_pointer,
																			&widget_tree_cursor,
																			tree_return_value ) 
						) !=Pt_TRAVERSE_DONE)
			{
			PtGetResource (widget_tree_cursor, Pt_ARG_FLAGS, &widget_flags,0);
			PtSetResource(widget_tree_cursor, Pt_ARG_FLAGS, (widget_flags & new_state_widgets_flag), 0);
     		};

		};
	
	return( Pt_CONTINUE );
	}


int
activate_AllRangeBtn( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{
		unsigned short index;
		PtWidget_t *filter_dlg_wnd, *tmp_widget_pointer;
		int first_escalators_in_station=1, count_escalators_in_station=0;	
		
		filter_dlg_wnd=ApGetInstance(widget);

		if (filter_dlg_wnd==NULL) {
			string mess ="Can`t get filter window";
			g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
			return( Pt_END );
			};

		tmp_widget_pointer=ApGetWidgetPtr(filter_dlg_wnd, ABN_StationList);
		// now tmp_widget_pointer point to  ABN_StationList instance in window
		if (tmp_widget_pointer==NULL) 
			{
				string mess ="Can`t get StationList for filter window";
				g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
				return( Pt_END );
			} else {
							
				PtGetResource(tmp_widget_pointer, Pt_ARG_CBOX_SEL_ITEM, &index, 0);

				tmp_widget_pointer=ApGetWidgetPtr(filter_dlg_wnd, ABN_RangeStartNumeric);
				// now tmp_widget_pointer point to  ABN_RangeStartNumeric instance in window
				if (tmp_widget_pointer==NULL) 
				{
					string mess ="Can`t get RangeStartNumeric for filter window";
					g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
					return( Pt_END );
				} else {
					PtGetResource(tmp_widget_pointer, Pt_ARG_NUMERIC_MAX, &count_escalators_in_station, 0);
					if (first_escalators_in_station>count_escalators_in_station) first_escalators_in_station=count_escalators_in_station;
					PtSetResource(tmp_widget_pointer, Pt_ARG_NUMERIC_VALUE, &first_escalators_in_station, 0);
				};

				tmp_widget_pointer=ApGetWidgetPtr(filter_dlg_wnd, ABN_RangeEndNumeric);
				// now tmp_widget_pointer point to  ABN_RangeEndNumeric instance in window
				if (tmp_widget_pointer==NULL) 
				{
					string mess ="Can`t get RangeEndNumeric for filter window";
					g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
					return( Pt_END );
				} else {
					PtGetResource(tmp_widget_pointer, Pt_ARG_NUMERIC_MAX, &count_escalators_in_station, 0);
					PtSetResource(tmp_widget_pointer, Pt_ARG_NUMERIC_VALUE, &count_escalators_in_station, 0);
				};
			};
		
	return( Pt_CONTINUE );

	}


int
activate_ResetFilter( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
	{
	string message("Фильтр сообщений выключен");
	vector<PtArg_t>	args(2);
	
	g_main_log.filter.set_filter_off();

	PtSetArg(&args[0], Pt_ARG_TEXT_STRING,  message.c_str(), 0);
	PtSetArg(&args[1], Pt_ARG_COLOR,  Pg_BLACK, 0);	
	PtSetResources(ABW_log_status, args.size(), &args[0]);

	return( Pt_CONTINUE );
	}


int
activate_EscalatorsGroup( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
	{
	enum { SINGLE_ESCALATOR=0,
				RANGE_ESCALATORS
				};
	
	PtWidget_t   *filter_dlg_wnd, *tmp_widget_pointer, *widget_tree_cursor;
	int index, tree_return_value;	
	long widget_flags, range_widgets_flag, single_widgets_flag;
	
	filter_dlg_wnd=ApGetInstance(	widget);
	if (filter_dlg_wnd==NULL) {
		string mess ="Can`t get filter window";
		g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
		return( Pt_END );
	};
	
	PtGetResource(widget, Pt_ARG_USER_DATA, &index, 0);
	
	switch (index)
	{
		case RANGE_ESCALATORS:
			range_widgets_flag=~Pt_GHOST & ~Pt_BLOCKED;
			single_widgets_flag=Pt_GHOST  | Pt_BLOCKED;
			break;
		case SINGLE_ESCALATOR:		
		default:
			range_widgets_flag=Pt_GHOST  | Pt_BLOCKED;
			single_widgets_flag=~Pt_GHOST & ~Pt_BLOCKED;
			break;
	};

	tmp_widget_pointer=ApGetWidgetPtr(filter_dlg_wnd, ABN_SingleEscalatorsGroup);
	// now tmp_widget_pointer point to  ABN_SingleEscalatorsGroup instance in window
	if (tmp_widget_pointer==NULL) 
		{
			string mess ="Can`t get SingleEscalatorsGroup for filter window";
			g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
			return( Pt_END );
		} else {

			widget_tree_cursor=tmp_widget_pointer;
			tree_return_value=Pt_TRAVERSE_START;

			while( ( tree_return_value=PtWidgetTree( tmp_widget_pointer,
																			&widget_tree_cursor,
																			tree_return_value ) 
						) !=Pt_TRAVERSE_DONE)
			{
			PtGetResource (widget_tree_cursor, Pt_ARG_FLAGS, &widget_flags,0);
			PtSetResource(widget_tree_cursor, Pt_ARG_FLAGS, (widget_flags & single_widgets_flag), 0);
     		};

		};

	tmp_widget_pointer=ApGetWidgetPtr(filter_dlg_wnd, ABN_RangeEscalatorsGroup);
	// now tmp_widget_pointer point to  ABN_RangeEscalatorsGroup instance in window
	if (tmp_widget_pointer==NULL) 
		{
			string mess ="Can`t get RangeEscalatorsGroup for filter window";
			g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
			return( Pt_END );
		} else {

			widget_tree_cursor=tmp_widget_pointer;
			tree_return_value=Pt_TRAVERSE_START;

			while( ( tree_return_value=PtWidgetTree( tmp_widget_pointer,
																			&widget_tree_cursor,
																			tree_return_value ) 
						) !=Pt_TRAVERSE_DONE)
			{
			PtGetResource (widget_tree_cursor, Pt_ARG_FLAGS, &widget_flags,0);
			PtSetResource(widget_tree_cursor, Pt_ARG_FLAGS, (widget_flags & range_widgets_flag), 0);
     		};

		};


	return( Pt_CONTINUE );
	}


int
activate_AllTimeRangeBtn( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
	{
	
	PtWidget_t   *filter_dlg_wnd, *tmp_widget_pointer, *widget_tree_cursor;
	int tree_return_value;	
	long widget_flags, new_state_widgets_flag=0;
	
	filter_dlg_wnd=ApGetInstance(	widget);
	if (filter_dlg_wnd==NULL) {
		string mess ="Can`t get filter window";
		g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
		return( Pt_END );
	};
	
	PtGetResource(widget, Pt_ARG_FLAGS, &widget_flags, 0);
	
	if (widget_flags & Pt_SET)
			{ new_state_widgets_flag=(Pt_GHOST  | Pt_BLOCKED);}
			else 
			{new_state_widgets_flag=(~Pt_GHOST & ~Pt_BLOCKED);};
	

	tmp_widget_pointer=ApGetWidgetPtr(filter_dlg_wnd, ABN_StartTimePanel);
	// now tmp_widget_pointer point to  ABN_StartTimePanel instance in window
	if (tmp_widget_pointer==NULL) 
		{
			string mess ="Can`t get StartTimePanel for filter window";
			g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
			return( Pt_END );
		} else {

			widget_tree_cursor=tmp_widget_pointer;
			tree_return_value=Pt_TRAVERSE_START;

			while( ( tree_return_value=PtWidgetTree( tmp_widget_pointer,
																			&widget_tree_cursor,
																			tree_return_value ) 
						) !=Pt_TRAVERSE_DONE)
			{
			PtGetResource (widget_tree_cursor, Pt_ARG_FLAGS, &widget_flags,0);
			PtSetResource(widget_tree_cursor, Pt_ARG_FLAGS, (widget_flags & new_state_widgets_flag), 0);
     		};

		};


	tmp_widget_pointer=ApGetWidgetPtr(filter_dlg_wnd, ABN_EndTimePanel);
	// now tmp_widget_pointer point to  ABN_EndTimePanel instance in window
	if (tmp_widget_pointer==NULL) 
		{
			string mess ="Can`t get EndTimePanel for filter window";
			g_system_settings.sys_message(system_settings::ERROR_MSG, mess);
			return( Pt_END );
		} else {

			widget_tree_cursor=tmp_widget_pointer;
			tree_return_value=Pt_TRAVERSE_START;

			while( ( tree_return_value=PtWidgetTree( tmp_widget_pointer,
																			&widget_tree_cursor,
																			tree_return_value ) 
						) !=Pt_TRAVERSE_DONE)
			{
			PtGetResource (widget_tree_cursor, Pt_ARG_FLAGS, &widget_flags,0);
			PtSetResource(widget_tree_cursor, Pt_ARG_FLAGS, (widget_flags & new_state_widgets_flag), 0);
     		};

		};


	return( Pt_CONTINUE );

	}

/*
Callbacks for ArchiveLog  and ArchiveDlg window
*/

int
realize_ArchiveLog( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{
	g_archive_log.set_widget (widget);
	return( Pt_CONTINUE );
	}


int
unrealize_ArchiveLog( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{
	g_archive_log.set_widget(NULL);
	g_archive_log.erase_all();
	
	return( Pt_CONTINUE );

	}

int
realized_main_log_wnd( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	g_main_log.set_widget (widget);
	return( Pt_CONTINUE );

	}


int
unrealized_main_log_wnd( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	g_main_log.set_widget(NULL);
	g_main_log.erase_all();

	return( Pt_CONTINUE );

	}



int
activate_LoadArchiveBtn( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{
	
	g_archive_log.erase_all();
	g_archive_log.load (string("remove_it.ini"));

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
	// создаем схему
	CreateScheme();
	// загружаем журнал
 	g_main_log.load(g_system_settings.get_main_log_name());
	
	// добавляем сообщение
 	g_main_log.set_records_autoincrement(g_main_log.get_records_autoincrement()+1); 
	g_main_log.insert(log_record (
												 g_main_log.get_records_autoincrement(),
												 143, //	message_id  Program started
												 0, // station_id -  broadcast
												 0, // device_id -  broadcast
												 time(NULL)
												)
							);

	return( Pt_CONTINUE );

	}


int
CloseMainWnd( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	g_system_settings.set_main_window(NULL);

	// добавляем сообщение
	g_main_log.set_records_autoincrement(g_main_log.get_records_autoincrement()+1);
	g_main_log.insert(log_record (
												 g_main_log.get_records_autoincrement(),
												 143, //	message_id Program closed
												 0, // station_id -  broadcast
												 0, // device_id -  broadcast
												 time(NULL)
												)
							);



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
		g_system_settings.message_window (system_settings::ERROR_MSG, string("Не удсь сохранить направления движеня эскалаторов в файле !"));
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
g_CommandPool.push_back(cmd);
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
				g_CommandPool.push_back(cmd);
				};
		}
		else if ((esc_iter->second.get_pref_direction() == system_settings::DIRECTION_DOWN)
					||(esc_iter->second.get_direction() == system_settings::DIRECTION_DOWN))
		{
			if (esc_iter->second.confirm_direction(system_settings::DIRECTION_UP) == 1)
				{
				cmd.set_item_color(Pg_RED);
				g_CommandPool.push_back(cmd);	
				};
		}
		else	
			{
				cmd.set_item_color(Pg_PURPLE);
				g_CommandPool.push_back(cmd);
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
g_CommandPool.push_back(cmd);
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
				g_CommandPool.push_back(cmd);
				};
		}	else if ((esc_iter->second.get_pref_direction() == system_settings::DIRECTION_UP)
					||(esc_iter->second.get_direction() == system_settings::DIRECTION_UP))
		{
			if (esc_iter->second.confirm_direction(system_settings::DIRECTION_DOWN)==1)
				{
				cmd.set_item_color(Pg_RED);
				g_CommandPool.push_back(cmd);
				};
		}else	{
			cmd.set_item_color(Pg_DGREEN);
			g_CommandPool.push_back(cmd);
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
			return( Pt_HALT );

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
		directions_strings[REVERSE]="реверсивный";
	
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
			
			esc_types_container::iterator_esc_types iter_types;
			
			iter_types=g_metro_escalator_types.find(paneled_escalator->second.get_type());
			if (iter_types!=g_metro_escalator_types.end()) 
				{
				PtWidget_t* panelTabs = ApGetWidgetPtr(widget, ABN_SignalPanel);
				
				for ( metro_escalator_type::iterator_blocks iter_blocks=iter_types->second.blocks_begin();
						iter_blocks!=iter_types->second.blocks_end();
						iter_blocks++)
						iter_blocks->second.create_panel(panelTabs);
						
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

