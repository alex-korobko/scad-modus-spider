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
