/* Header "global" for probe Application */

#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#define __DEBUG__

#ifdef DEFINE_GLOBALS

#define GLOBAL
#define INIT(x) = x

#else

#define GLOBAL extern
#define INIT(x)

#endif

/* global variables */

#include <sys/socket.h>

#include "map.h"
#include "list.h"
#include "log.h"
#include "router.h"

// pre-release

#include "defines.h"
#include "esc_types.h"
#include "alert.h"
#include "filter.h"
#include "dictionary.h"
#include "sound.h"
#include "line.h"
#include "station.h"
#include "escalator.h"
#include "cmd_pool.h"

//void SlidePanel();

GLOBAL ApDBase_t*  widget_dbase;
GLOBAL struct PxTransCtrl* translate_set; 
//GLOBAL int panelized INIT(0);
GLOBAL Log mainLog;
GLOBAL Log 		g_archiveLog;
GLOBAL metro_line*			g_lines INIT(NULL);
GLOBAL metro_station*		g_stations INIT(NULL);
GLOBAL metro_escalator*		g_escalators INIT(NULL);
GLOBAL int						g_lineNum INIT(0);
GLOBAL int						g_stationNum  INIT(0);
GLOBAL int						g_escalatorNum  INIT(0);
GLOBAL Map<DictEntry>	g_msgDictionary;
GLOBAL Sound					g_sound;
GLOBAL	 Router					g_router;
GLOBAL	 Map<Command>		g_CommandPool;

// pre-release
GLOBAL Type 	*g_escTypes INIT(0);
GLOBAL int 		g_escTypeNum INIT(0);
GLOBAL LogFilter		g_logFilter;

GLOBAL AlertQueue		g_alertQueue;
GLOBAL uchar_t*			g_msgDialogFont INIT(0);
GLOBAL int				g_chanID INIT(-1);
GLOBAL FILE*			g_debugFile INIT(0);

#define		ERROR_MSG	0
#define		DEBUG_MSG	1
#define		INFO_MSG		2

// COLORS
#define		RED_BTN_COLOR				PgRGB(210,0,0)
#define		BLUE_TEXT_COLOR				PgRGB(63,58,110)
#define		BLUE_BTN_COLOR				PgRGB(61,87,181)
#define		LIGHTBLUE_BTN_COLOR		PgRGB(182,202,255)
#define		GREEN_BTN_COLOR				PgRGB(52, 134, 34)
#define		YELLOW_TEXT_COLOR		PgRGB(254, 254, 85)
#define		DISABLE_BTN_COLOR			PgRGB(190, 190, 190)
#define		DISABLE_TEXT_COLOR		PgRGB(100, 100, 100)
#define		PANEL_COLOR					PgRGB(170, 185, 202)

#define		ALL_ESCALATORS 			for(int i=0; i<g_escalatorNum; i++)

#define		SYSTEM_TIMER			_PULSE_CODE_MINAVAIL
#define		PING_TIMEOUT			2000000 // мкс
#define		SYSTEM_TICK				1 // c

#define		DIRECTION_STOP		0
#define		DIRECTION_UP			1
#define		DIRECTION_DOWN		2
#define		DIRECTION_REVERSE		3

#define		STATE_UP_PE			2
#define		STATE_DOWN_PE		3
#define		STATE_UP_SE			4
#define		STATE_DOWN_SE		5
#define		STATE_STOP				6

#define CHECK_ADDRESS(addr) if (addr > 247) { SysMessage(ERROR_MSG, "Incorrect device address (%d)", addr); return 0; }
#define CHECK_ORDER(lo, hi)	if (hi < lo) { SysMessage(ERROR_MSG, "Last address of requested block less then first"); return 0; }

#endif
