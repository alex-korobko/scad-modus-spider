/*******************************************************************************
Programming - Alexander Klimenko
Project - MetroNET
Started at 27.12.00
Last updated at 03.01.01
Copyright (C) 2000, 2001 SCAD Ltd. (software development group)
*******************************************************************************/

#ifndef __LOG_H__
#define __LOG_H__

#include <Pt.h>
#include "list.h"
#include "system.h"

#define		OK     0
#define		ALERT  1
#define		NET    2

#define		COLOR_BLACK		0x00000000
#define		COLOR_BLUE		0x000000AA
#define		COLOR_RED		0x00EE0000
#define		COLOR_GREEN		0x0000AA00

struct LogMessage
{
	char* text; // text of message
	int   color; // color of message
	int   image; // image in list (not supported now)
};

class LogRecord
{
public:
	PtGenListItem_t listItem;
	int ID;
	int msgID;
	int stationID;
	int escalatorID;
	time_t saveTime;   
	explicit LogRecord () : ID(0), msgID(0), stationID(0), escalatorID(0), saveTime(time(NULL)) {};
};

class Log
{   
protected:
	enum { rowHeight = ROW_HEIGHT };
	List<LogRecord>	records;
	PtWidget_t*		wnd;
	struct	tm		lastAccessTime;
public:
	Log();
	virtual ~Log();
	int AddMessage(int msgID, int stationID, int escalatorID, time_t* saveTime = NULL, dword param = 0);
   	int AttachWnd(PtWidget_t* wnd);
   	void ScrollDown();
	void FilterLogWnd();
	int Save(const char* fileName);
	int Load(const char* fileName);
	int	 Rotate();
	void Clear();
	PtWidget_t* GetWindow() { return wnd; }
	void DetachWnd() { wnd = NULL; }
};

int CheckFilter(const LogRecord* record);

#endif
