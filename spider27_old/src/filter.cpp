/* Y o u r   D e s c r i p t i o n                       */
/*                            AppBuilder Photon Code Lib */
/*                                         Version 2.01  */

/* Standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* Local headers */
#include "ablibs.h"
#include "global.h"
#include "abimport.h"
#include "proto.h"


LogFilter::LogFilter()
{
	allTime = 1;
	messages.sysMsg = 1;
	messages.alertMsg = 1;
	messages.blockMsg = 1;
	messages.localMsg = 1;
	stationIndex = 0;
}

LogFilter::~LogFilter()
{
}

void FillStationList()
{
	char *name;

	name = new char[128];
	for(int i=0; i<g_stationNum; i++)
	{
		g_stations[i].Name.Get(name, 128);

		PtListAddItems(ABW_StationList,(const char**)&name, 1, 0);
	}
// Уже было закомменчено		
//	PtSetResource(list_widget, Pt_ARG_POINTER, this, 0);
	delete[] name;
}

int SelectOneEscalator( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
	{
	PtArg_t	arg;
	
//	PtSetArg(&arg, Pt_ARG_ONOFF_STATE, 0, 0);
//	PtGetResources(ABW_OneEscalatorBtn, 1, &arg);
//	if (!arg.value)
//	{
//	}


	return( Pt_CONTINUE );

	}


int SelectRangeEscalator( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{
	PtArg_t	arg;

//	PtSetArg(&arg, Pt_ARG_ONOFF_STATE, 0, 0);
//	PtGetResources(ABW_RangeEscalatorBtn, 1, &arg);
//	if (!arg.value)
//	{
	

//	PtSetArg(&arg, Pt_ARG_ONOFF_STATE, 0, 0);	
//	PtSetResources(ABW_OneEscalatorBtn, 1, &arg);	
//	}
	return( Pt_CONTINUE );

	}


int CreateFilterDlg( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{
        PhPointerEvent_t *event;
		

 	    ApCreateModule(ABM_LogFilterDlg, widget, cbinfo);

        return( Pt_CONTINUE );
	}


int SetLogFilter( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{
		PtArg_t		args[2];
		time_t*		date;
		int				elapsed = 0, flg;
		int index;
		int				changed = 0;
		char			message[] = "Внимание! В журнале включен фильтр сообщений";

		index = get_widget_scalar(ABW_StationList, Pt_ARG_CBOX_SEL_ITEM) - 1;
/*		if (!index)
		{
			PtMessageBox(widget, "Error", "Don't select station", NULL, NULL);
			return Pt_END;
		}*/

	printf("Woof\n");		
		if (index)
			changed = 1;
		g_logFilter.stationIndex = index;
		PtSetArg(&args[0], Pt_ARG_FLAGS, 0, 0);
		PtGetResources(ABW_SysMsgBtn, 1, args);		
		
		if (args[0].value & Pt_SET)
			g_logFilter.messages.sysMsg = 1;
		else
		{
			changed = 1;
			g_logFilter.messages.sysMsg = 0;
		}
		PtSetArg(&args[0], Pt_ARG_FLAGS, 0, 0);
		PtGetResources(ABW_AlertMsgBtn, 1, args);		
		if (args[0].value & Pt_SET)
			g_logFilter.messages.alertMsg = 1;
		else
		{
			g_logFilter.messages.alertMsg = 0;
			changed = 1;
		}

		PtSetArg(&args[0], Pt_ARG_FLAGS, 0, 0);
		PtGetResources(ABW_BlockMsgBtn, 1, args);		
		
		if (args[0].value & Pt_SET)
			g_logFilter.messages.blockMsg = 1;
		else
		{
			g_logFilter.messages.blockMsg = 0;
			changed = 1;
		}

		PtSetArg(&args[0], Pt_ARG_FLAGS, 0, 0);
		PtGetResources(ABW_LocalMsgBtn, 1, args);		
		
		if (args[0].value & Pt_SET)
			g_logFilter.messages.localMsg = 1;
		else
		{
			g_logFilter.messages.localMsg = 0;
			changed = 1;
		}
		
		PtSetArg(&args[0], Pt_ARG_FLAGS, 0, 0);
		PtGetResources(ABW_AllTimeRangeBtn1, 1, args);		

		if (args[0].value & Pt_SET)
		{
			g_logFilter.allTime = 1;
			g_logFilter.startTime = 0;
			g_logFilter.endTime = 0;
		}
		else
		{
			g_logFilter.allTime = 0;
			changed = 1;
			PtSetArg(&args[0], Pt_ARG_CALENDAR_TIME_T, &date, 0);
			PtGetResources(ABW_CalendarStart, 1, args);		
			memcpy(&g_logFilter.startTime, date, sizeof(time_t));
			memcpy(&g_logFilter.endTime, date, sizeof(time_t));
			elapsed += 3600*args[0].value;

			PtGetResources(ABW_CalendarEnd, 1, args);		
			PtSetArg(&args[0], Pt_ARG_NUMERIC_VALUE, 0, 0);
			PtGetResources(ABW_StartHourNumeric, 1, args);

			PtSetArg(&args[0], Pt_ARG_NUMERIC_VALUE, 0, 0);
			PtGetResources(ABW_StartMinNumeric, 1, args);
			elapsed += 60*args[0].value;
		
			g_logFilter.startTime += elapsed;
		
		
			elapsed = 0;
			
			PtSetArg(&args[0], Pt_ARG_NUMERIC_VALUE, 0, 0);
			PtGetResources(ABW_EndHourNumeric, 1, args);
			elapsed += 3600*args[0].value;
			PtSetArg(&args[0], Pt_ARG_NUMERIC_VALUE, 0, 0);
			PtGetResources(ABW_EndMinNumeric, 1, args);
			elapsed += 60*args[0].value;		
		
			g_logFilter.endTime += elapsed;
		}

		if (g_logFilter.stationIndex)
		{
			changed = 1;
		PtSetArg(&args[0], Pt_ARG_FLAGS, 0, 0);
		PtGetResources(ABW_OneEscalatorBtn, 1, args);
		
		if (args[0].value & Pt_SET)
		{
			PtSetArg(&args[0], Pt_ARG_NUMERIC_VALUE, 0, 0);
			PtGetResources(ABW_OneEscalatorNumeric, 1, args);

			g_logFilter.firstEscalatorNum = g_logFilter.lastEscalatorNum = args[0].value;
		}
		else
		{
		
			PtSetArg(&args[0], Pt_ARG_NUMERIC_VALUE, 0, 0);
			PtGetResources(ABW_RangeStartNumeric, 1, args);

			g_logFilter.firstEscalatorNum =  args[0].value;
			PtSetArg(&args[0], Pt_ARG_NUMERIC_VALUE, 0, 0);
			PtGetResources(ABW_RangeEndNumeric, 1, args);

			g_logFilter.lastEscalatorNum =  args[0].value;
		}
		}
		
	mainLog.FilterLogWnd();	

	if (changed)
	{
		PtSetArg(&args[0], Pt_ARG_TEXT_STRING,  &message, strlen(message));
		PtSetArg(&args[1], Pt_ARG_COLOR,  Pg_RED, 0);	
		PtSetResources(ABW_log_status, 2, args);
	}

		printf("Live\n");
				
	return( Pt_CONTINUE );

	}


int RealizeFilterDlg( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{
	time_t	curTime;
	struct tm curTimeTM;
	PtArg_t		arg;
	char *str[] = {"Все станци"};
	int flg;
	PtListAddItems(ABW_StationList, (const char**)str, 1, 0);
	FillStationList();
	
	PtSetArg(&arg, Pt_ARG_CBOX_SEL_ITEM, 1, 0);
	PtSetResources(ABW_StationList, 1, &arg);

	g_logFilter.messages.localMsg? PtSetResource(ABW_LocalMsgBtn, Pt_ARG_FLAGS, Pt_TRUE, Pt_SET) :
										PtSetResource(ABW_LocalMsgBtn, Pt_ARG_FLAGS, Pt_FALSE, Pt_SET);
	g_logFilter.messages.blockMsg? PtSetResource(ABW_BlockMsgBtn, Pt_ARG_FLAGS, Pt_TRUE, Pt_SET) :
										PtSetResource(ABW_BlockMsgBtn, Pt_ARG_FLAGS, Pt_FALSE, Pt_SET);
	g_logFilter.messages.sysMsg? PtSetResource(ABW_SysMsgBtn, Pt_ARG_FLAGS, Pt_TRUE, Pt_SET) :
										PtSetResource(ABW_SysMsgBtn, Pt_ARG_FLAGS, Pt_FALSE, Pt_SET);
	g_logFilter.messages.alertMsg? PtSetResource(ABW_AlertMsgBtn, Pt_ARG_FLAGS, Pt_TRUE, Pt_SET) :
										PtSetResource(ABW_AlertMsgBtn, Pt_ARG_FLAGS, Pt_FALSE, Pt_SET);

	if (g_logFilter.startTime)
	{
		localtime_r(&g_logFilter.startTime, &curTimeTM);
	
		PtSetArg(&arg, Pt_ARG_NUMERIC_VALUE, curTimeTM.tm_hour, 0);
		PtSetResources(ABW_StartHourNumeric, 1, &arg);
		PtSetArg(&arg, Pt_ARG_NUMERIC_VALUE, curTimeTM.tm_min, 0);
		PtSetResources(ABW_StartMinNumeric, 1, &arg);
		PtSetResource(ABW_CalendarStart, Pt_ARG_CALENDAR_TIME_T, g_logFilter.startTime, sizeof(g_logFilter.startTime));
	}
	if (g_logFilter.endTime)
	{
		localtime_r(&g_logFilter.endTime, &curTimeTM);
		PtSetArg(&arg, Pt_ARG_NUMERIC_VALUE, curTimeTM.tm_hour, 0);
		PtSetResources(ABW_EndHourNumeric, 1, &arg);
		PtSetArg(&arg, Pt_ARG_NUMERIC_VALUE, curTimeTM.tm_min, 0);
		PtSetResources(ABW_EndMinNumeric, 1, &arg);
		PtSetResource(ABW_CalendarEnd, Pt_ARG_CALENDAR_TIME_T, g_logFilter.endTime, sizeof(g_logFilter.endTime));
	}
	else
	{
		curTime = time(NULL);
		localtime_r(&curTime, &curTimeTM);
		PtSetArg(&arg, Pt_ARG_NUMERIC_VALUE, curTimeTM.tm_hour, 0);
		PtSetResources(ABW_EndHourNumeric, 1, &arg);
		PtSetArg(&arg, Pt_ARG_NUMERIC_VALUE, curTimeTM.tm_min, 0);
		PtSetResources(ABW_EndMinNumeric, 1, &arg);
	}

	if (g_logFilter.allTime)
	{
		PtSetResource(ABW_AllTimeRangeBtn1, Pt_ARG_FLAGS, Pt_TRUE, Pt_SET);
		PtSetResource(ABW_AllTimeRangeBtn2, Pt_ARG_FLAGS, Pt_TRUE, Pt_SET);
	}
	else
	{
		PtSetResource(ABW_AllTimeRangeBtn1, Pt_ARG_FLAGS, Pt_FALSE, Pt_SET);
		PtSetResource(ABW_AllTimeRangeBtn2, Pt_ARG_FLAGS, Pt_FALSE, Pt_SET);
	}
	PtSetResource(ABW_StationList, Pt_ARG_CBOX_SEL_ITEM, g_logFilter.stationIndex + 1, 0);
	metro_station*	selectedStation =  &g_stations[g_logFilter.stationIndex];

	if (selectedStation)
	{
		PtSetResource(ABW_RangeEscalatorBtn, Pt_ARG_FLAGS, Pt_FALSE, Pt_GHOST | Pt_BLOCKED);
		PtSetResource(ABW_OneEscalatorBtn, Pt_ARG_FLAGS, Pt_FALSE, Pt_GHOST | Pt_BLOCKED);

		int maxNum = selectedStation->escalatorNum;
		PtSetResource(ABW_OneEscalatorNumeric, Pt_ARG_NUMERIC_MAX, maxNum, 0);
		PtSetResource(ABW_RangeStartNumeric, Pt_ARG_NUMERIC_MAX, maxNum, 0);
		PtSetResource(ABW_RangeEndNumeric, Pt_ARG_NUMERIC_MAX, maxNum, 0);
		
		if (g_logFilter.firstEscalatorNum == g_logFilter.lastEscalatorNum)
		{			

			PtSetResource(ABW_OneEscalatorLabel, Pt_ARG_FLAGS, Pt_FALSE, Pt_GHOST | Pt_BLOCKED);
			PtSetResource(ABW_OneEscalatorNumeric, Pt_ARG_FLAGS, Pt_FALSE, Pt_GHOST | Pt_BLOCKED);
			PtSetResource(ABW_OneEscalatorBtn, Pt_ARG_FLAGS, Pt_TRUE, Pt_SET);
			PtSetResource(ABW_OneEscalatorNumeric, Pt_ARG_NUMERIC_VALUE, g_logFilter.firstEscalatorNum, 0);
		}
		else
		{
			PtSetResource(ABW_RangeStartNumeric, Pt_ARG_FLAGS, Pt_FALSE, Pt_GHOST | Pt_BLOCKED);
			PtSetResource(ABW_RangeEndNumeric, Pt_ARG_FLAGS, Pt_FALSE, Pt_GHOST | Pt_BLOCKED);
			PtSetResource(ABW_AllRangeBtn, Pt_ARG_FLAGS, Pt_FALSE, Pt_GHOST | Pt_BLOCKED);
			PtSetResource(ABW_RangeStartLabel, Pt_ARG_FLAGS, Pt_FALSE, Pt_GHOST);
			PtSetResource(ABW_RangeEndLabel, Pt_ARG_FLAGS, Pt_FALSE, Pt_GHOST);
			PtSetResource(ABW_RangeEscalatorBtn, Pt_ARG_FLAGS, Pt_TRUE, Pt_SET);
			PtSetResource(ABW_RangeStartNumeric, Pt_ARG_NUMERIC_VALUE, g_logFilter.firstEscalatorNum, 0);
			PtSetResource(ABW_RangeEndNumeric, Pt_ARG_NUMERIC_VALUE, g_logFilter.lastEscalatorNum, 0);
		}
	}


	return( Pt_CONTINUE );

	}


int EnableAllTimeRange( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{
		PtArg_t		arg;
		
	PtSetArg(&arg, Pt_ARG_FLAGS, Pt_TRUE, Pt_GHOST | Pt_BLOCKED);
	PtSetResources(ABW_CalendarStart, 1, &arg);	
	PtSetResources(ABW_StartHourNumeric, 1, &arg);	
	PtSetResources(ABW_StartMinNumeric, 1, &arg);	
	PtSetResources(ABW_CalendarEnd, 1, &arg);	
	PtSetResources(ABW_EndHourNumeric, 1, &arg);	
	PtSetResources(ABW_EndMinNumeric, 1, &arg);

	PtSetArg(&arg, Pt_ARG_FLAGS, Pt_TRUE, Pt_SET);	
	if (widget == ABW_AllTimeRangeBtn1)
		PtSetResources(ABW_AllTimeRangeBtn2, 1, &arg);
	else if (widget == ABW_AllTimeRangeBtn2)
		PtSetResources(ABW_AllTimeRangeBtn1, 1, &arg);

	return( Pt_CONTINUE );

	}


int DisableAllTimeRange( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{
		PtArg_t		arg;

	PtSetArg(&arg, Pt_ARG_FLAGS, Pt_FALSE, Pt_GHOST | Pt_BLOCKED);
	PtSetResources(ABW_CalendarStart, 1, &arg);	
	PtSetResources(ABW_StartHourNumeric, 1, &arg);	
	PtSetResources(ABW_StartMinNumeric, 1, &arg);	
	PtSetResources(ABW_CalendarEnd, 1, &arg);	
	PtSetResources(ABW_EndHourNumeric, 1, &arg);	
	PtSetResources(ABW_EndMinNumeric, 1, &arg);

	PtSetArg(&arg, Pt_ARG_FLAGS, Pt_FALSE, Pt_SET);	
	if (widget == ABW_AllTimeRangeBtn1)
		PtSetResources(ABW_AllTimeRangeBtn2, 1, &arg);
	else if (widget == ABW_AllTimeRangeBtn2)
		PtSetResources(ABW_AllTimeRangeBtn1, 1, &arg);
	return( Pt_CONTINUE );

	}


int SelectFilterStation( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{
		PtArg_t		arg;
		
		metro_station*	selectedStation;

		int index = get_widget_scalar(ABW_StationList, Pt_ARG_CBOX_SEL_ITEM) - 2;
		if (index >= 0)
		{
		
			PtSetArg(&arg, Pt_ARG_FLAGS, Pt_FALSE, Pt_GHOST | Pt_BLOCKED);
			PtSetResources(ABW_RangeEscalatorBtn, 1, &arg);
			PtSetResources(ABW_OneEscalatorLabel, 1, &arg);
			PtSetResources(ABW_OneEscalatorNumeric, 1, &arg);
			PtSetArg(&arg, Pt_ARG_FLAGS, Pt_FALSE, Pt_GHOST);
			PtSetResources(ABW_OneEscalatorBtn, 1, &arg);

			PtSetArg(&arg, Pt_ARG_NUMERIC_VALUE, 1, 0);
			PtSetResources(ABW_OneEscalatorNumeric, 1, &arg);
			PtSetResources(ABW_RangeStartNumeric, 1, &arg);
			PtSetResources(ABW_RangeEndNumeric, 1, &arg);
			
			PtSetArg(&arg, Pt_ARG_NUMERIC_MAX, g_stations[index].escalatorNum, 0);
			PtSetResources(ABW_OneEscalatorNumeric, 1, &arg);
			PtSetResources(ABW_RangeStartNumeric, 1, &arg);
			PtSetResources(ABW_RangeEndNumeric, 1, &arg);
		}


	return( Pt_CONTINUE );

	}




int ActivateOneEscalator( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{
		PtArg_t		arg;

	PtSetArg(&arg, Pt_ARG_ONOFF_STATE, 0, 0);
	PtSetResources(ABW_RangeEscalatorBtn, 1, &arg);	

	PtSetArg(&arg, Pt_ARG_FLAGS, Pt_FALSE, Pt_BLOCKED);
	PtSetResources(ABW_RangeEscalatorBtn, 1, &arg);
	PtSetArg(&arg, Pt_ARG_FLAGS, Pt_TRUE, Pt_BLOCKED);
	PtSetResources(ABW_OneEscalatorBtn, 1, &arg);

	PtSetArg(&arg, Pt_ARG_FLAGS, Pt_TRUE, Pt_GHOST | Pt_BLOCKED);
	PtSetResources(ABW_RangeStartLabel, 1, &arg);
	PtSetResources(ABW_RangeEndLabel, 1, &arg);
	PtSetResources(ABW_RangeStartNumeric, 1, &arg);
	PtSetResources(ABW_RangeEndNumeric, 1, &arg);			
	PtSetResources(ABW_AllRangeBtn, 1, &arg);	

	PtSetArg(&arg, Pt_ARG_FLAGS, Pt_FALSE, Pt_GHOST | Pt_BLOCKED);
	PtSetResources(ABW_OneEscalatorLabel, 1, &arg);
	PtSetResources(ABW_OneEscalatorNumeric, 1, &arg);
	
	return( Pt_CONTINUE );

	}


int ActivateRangeEscalator( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{
		PtArg_t		arg;
		

	PtSetArg(&arg, Pt_ARG_ONOFF_STATE, 0, 0);	
	PtSetResources(ABW_OneEscalatorBtn, 1, &arg);	

	PtSetArg(&arg, Pt_ARG_FLAGS, Pt_FALSE, Pt_BLOCKED);
	PtSetResources(ABW_OneEscalatorBtn, 1, &arg);
	PtSetArg(&arg, Pt_ARG_FLAGS, Pt_TRUE, Pt_BLOCKED);
	PtSetResources(ABW_RangeEscalatorBtn, 1, &arg);


	PtSetArg(&arg, Pt_ARG_FLAGS, Pt_FALSE, Pt_GHOST | Pt_BLOCKED);
	PtSetResources(ABW_RangeStartLabel, 1, &arg);
	PtSetResources(ABW_RangeEndLabel, 1, &arg);
	PtSetResources(ABW_RangeStartNumeric, 1, &arg);
	PtSetResources(ABW_RangeEndNumeric, 1, &arg);			
	PtSetResources(ABW_AllRangeBtn, 1, &arg);	

	PtSetArg(&arg, Pt_ARG_FLAGS, Pt_TRUE, Pt_GHOST | Pt_BLOCKED);
	PtSetResources(ABW_OneEscalatorLabel, 1, &arg);
	PtSetResources(ABW_OneEscalatorNumeric, 1, &arg);

	return( Pt_CONTINUE );

	}


int SelectAllRange( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
		PtArg_t		arg;
		int index;
		
		index = get_widget_scalar(ABW_StationList, Pt_ARG_CBOX_SEL_ITEM) - 1;
		if (index >= 0)	
		{
		PtSetArg(&arg, Pt_ARG_NUMERIC_VALUE, 1, 0);
		PtSetResources(ABW_RangeStartNumeric, 1, &arg);
		PtSetArg(&arg, Pt_ARG_NUMERIC_VALUE, g_stations[index].escalatorNum, 0);
		PtSetResources(ABW_RangeEndNumeric, 1, &arg);
		};
		
	return( Pt_CONTINUE );
}


int ShowFilter( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	return( Pt_CONTINUE );

	}


int ResetFilter( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{
	char	message[] = "Фильтр сообщений выключен";
	PtArg_t	args[2];

	memset(&g_logFilter, 0, sizeof(g_logFilter));
	g_logFilter.allTime = 1;
	g_logFilter.messages.sysMsg = 1;
	g_logFilter.messages.alertMsg = 1;
	g_logFilter.messages.blockMsg = 1;
	g_logFilter.messages.localMsg = 1;

	PtSetArg(&args[0], Pt_ARG_TEXT_STRING,  &message, strlen(message));
	PtSetArg(&args[1], Pt_ARG_COLOR,  Pg_BLACK, 0);	
	PtSetResources(ABW_log_status, 2, args);
	
	mainLog.FilterLogWnd();	
	return( Pt_CONTINUE );

	}
