/*******************************************************************************
Programming - Alexander Klimenko
Project - MetroNET
Started at 27.12.00
Last updated at 03.01.01
Copyright (C) 2000, 2001 SCAD Ltd. (software development group)
*******************************************************************************/

#include <Ph.h>
#include <Pt.h>
#include <Ap.h>

#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "abimport.h"
#include "global.h"
#include "log.h"
#include "system.h"
#include "dictionary.h"

Log::Log()
{
	wnd = NULL;
	memset(&lastAccessTime, 0, sizeof(lastAccessTime));
}

Log::~Log()
{
	LogRecord* curRecord;
	
	records.first();
	while((curRecord = records.Pop()) != NULL)
		delete curRecord;
}

int Log::AddMessage(int msgID, int stationID, int escalatorID, time_t* saveTime, dword param)
{
	LogRecord *newRecord;

	newRecord = new LogRecord;
	
	if (!newRecord)
	{
		SysMessage(ERROR_MSG, "Memory allocation");
		return 0;
	}

	newRecord->ID = 0;
	newRecord->msgID = msgID;
	newRecord->stationID = stationID;
	newRecord->escalatorID = escalatorID;
	newRecord->listItem.prev = NULL;
	newRecord->listItem.next = NULL;
	newRecord->listItem.flags = 0;
	newRecord->listItem.size.h = rowHeight;
	newRecord->listItem.size.w= 0;
	if (saveTime)
		newRecord->saveTime = *saveTime;
	else
		newRecord->saveTime = time(NULL);
		
	if (param)
		newRecord->msgID |= (param << 16);
		
	if (wnd && CheckFilter(newRecord))
		PtGenListAddItems(wnd, &newRecord->listItem, PtGenListLastItem(wnd));		

	records.Push(newRecord);

	ScrollDown();
		
	return 1;
}

int Log::Save(const char* fileName)
{
	int		logFile;
	dword	size;
	char	signature[] = "LOGDAT";
	LogRecord* curRecord;
	
	printf ("\nSaving journal\n");
	
	if ((logFile = open(fileName, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH)) < 0)
	{
		SysMessage(ERROR_MSG, "Fail to open log file [%s]", strerror(errno));
		return 0;
	}
		
	write(logFile, signature, strlen(signature));
	size = records.GetSize();
	write(logFile, &size, sizeof(size));
	records.first();
	while((curRecord = records.next()) != NULL)
	{
		write(logFile, &curRecord->msgID, sizeof(curRecord->msgID));
		write(logFile, &curRecord->stationID, sizeof(curRecord->stationID));
		write(logFile, &curRecord->escalatorID, sizeof(curRecord->escalatorID));
		write(logFile, &curRecord->saveTime, sizeof(curRecord->saveTime));			
	}
	close(logFile);
	SysMessage(INFO_MSG, "Save current log");

	return 1;
}

int Log::Load(const char* fileName)
{
	int		logFile;
	dword	size;
	char	signature[7];
	LogRecord record;
	int	msgID, stationID, escalatorID;
	time_t	saveTime;
	time_t	curTime;
	struct	stat fileStat;
	int		justCreated = 0;
	
	if ((logFile = open(fileName, O_RDONLY, O_BINARY)) < 0)
	{
		if ((logFile = open(fileName, O_CREAT, O_BINARY)) < 0)
		{
			SysMessage(ERROR_MSG, "can't create log file [%s]", strerror(errno));
			return 0;
		}
		else
			INFO_BOX("Создан новый файл журнала");
		curTime = time(NULL);
		localtime_r(&curTime, &lastAccessTime); 
		justCreated = 1;
	}
	if(stat(fileName, &fileStat) != -1)
      	localtime_r(&fileStat.st_mtime, &lastAccessTime);
	else
		SysMessage(ERROR_MSG, "can't get log file access time [%s]", strerror(errno));

	read(logFile, signature, 6);
	signature[6] = '\0';
	if (!justCreated)
	{
		if (!strcmp(signature, "LOGDAT"))
		{
			read(logFile, &size, sizeof(size));
		
			for(dword i=0; i<size; i++)
			{
				read(logFile, &msgID, sizeof(msgID));
				read(logFile, &stationID, sizeof(stationID));
				read(logFile, &escalatorID, sizeof(escalatorID));
				read(logFile, &saveTime, sizeof(saveTime));
				AddMessage(msgID, stationID, escalatorID, &saveTime);
			}
		}
		else
		{
			ERROR_BOX("Файл журнала поврежден");
			SysMessage(ERROR_MSG, "Incompatible log file format expected");
			return 0;
		}
	}

	close(logFile);
	
	return 1;
}

void Log::ScrollDown()
{
	int visibleCount, itemsNum, topItem;
	
	if (wnd)
	{
		visibleCount = get_widget_scalar(wnd, Pt_ARG_VISIBLE_COUNT);
		itemsNum = records.GetSize();
		topItem = (visibleCount >= itemsNum)? 1: itemsNum - visibleCount + 1;
		PtSetResource(wnd, Pt_ARG_TOP_ITEM_POS, topItem, 0);
	}
}

int RealizeLogWnd( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	PtSetResource(widget, Pt_ARG_TOP_ITEM_POS, 2, 0);

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}

void Log::FilterLogWnd()
{
	LogRecord* curRecord;

	if (wnd)
	{
		PtGenListRemoveItems(wnd, NULL, NULL);

		records.first();
		while((curRecord = records.next()) != NULL)
		{
			if (CheckFilter(curRecord))
			{
				curRecord->listItem.prev = NULL;
				curRecord->listItem.next = NULL;
				curRecord->listItem.flags = 0;
				curRecord->listItem.size.h = rowHeight;
				curRecord->listItem.size.w= 0;
				PtGenListAddItems(wnd, &curRecord->listItem, PtGenListLastItem(wnd));
			}
		}
		ScrollDown();
	}
}

void Log::Clear()
{
	LogRecord* curRecord;

	if (wnd)
		PtGenListRemoveItems(wnd, NULL, NULL);

	records.first();
	while((curRecord = records.Pop()) != NULL)
		delete curRecord;
}

int CheckFilter(const LogRecord* record)
{
	int				color;
	int				msg = 0, station = 0, savetime = 0, escalator = 0;
	int				msgID;
	DictEntry*		curEntry = NULL;
	
	msgID = record->msgID & 0xFFFF;
	
	if (!(msgID & 0x8000))
	{
		curEntry = g_msgDictionary.GetByKey(msgID);
		if (!curEntry)
			return 0;
		if (g_logFilter.messages.localMsg && (msgID >= 143))
		{
			msg = 1;
			station = 1;
			escalator = 1;
		}
		else if (g_logFilter.messages.sysMsg && ((curEntry->color == COLOR_BLACK) || (curEntry->color == COLOR_BLUE)))
			msg |= 1;
		else if (g_logFilter.messages.blockMsg && (curEntry->color == COLOR_RED))
			msg |= 1;
		else if (g_logFilter.messages.alertMsg && (curEntry->color == COLOR_GREEN))
			msg |= 1;
		
	}
	else if (g_logFilter.messages.blockMsg)
			msg |= 1;

	if (!g_logFilter.stationIndex)
		station = 1;
	else if (g_logFilter.stationIndex == record->stationID)
		station = 1;
		
	if (!g_logFilter.firstEscalatorNum)
		escalator = 1;
	else if ((record->escalatorID >= g_logFilter.firstEscalatorNum) && (record->escalatorID <= g_logFilter.lastEscalatorNum))
		escalator = 1;
		
	if (g_logFilter.allTime)
		savetime = 1;
	else if (record->saveTime >= g_logFilter.startTime && record->saveTime < g_logFilter.endTime)
		savetime |= 1;

	return msg & station & savetime & escalator;
}

int Log::Rotate()
{
	time_t			curTime;
	struct	tm		localTime;
	char			fileName[11];

	curTime = time(NULL);
	localtime_r(&curTime, &localTime);
	if ((localTime.tm_mon != lastAccessTime.tm_mon) && !(localTime.tm_mon % 3))
	{
		int quartal = int(lastAccessTime.tm_mon / 3) + 1;
		sprintf(fileName, "%d-%d.log", quartal, lastAccessTime.tm_year + 1900);
		Save(fileName);
		memcpy(&lastAccessTime, &localTime, sizeof(lastAccessTime));
		Clear();
		SysMessage(INFO_MSG, "Rotating log");
	}

	return 1;
}	

int Log::AttachWnd(PtWidget_t* wnd)
{
	LogRecord* curRecord;

	if (!wnd)
	{
		SysMessage(ERROR_MSG, "Invalid log window pointer");
		return 0;
	}

	this->wnd = wnd;
	
	records.first();
	while((curRecord = records.next()) != NULL)
	{
		if (CheckFilter(curRecord))
			PtGenListAddItems(wnd, &curRecord->listItem, PtGenListLastItem(wnd));		
	}	
	
	return 1;
}	
