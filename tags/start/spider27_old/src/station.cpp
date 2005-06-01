/*******************************************************************************
 Programming - Alexander Klimenko
 Project - MetroNET
 Started at 27.12.00
 Last updated at 16.01.01
 Copyright (C) 2000, 2001 SCAD Ltd. (software development group)
*******************************************************************************/

#include <Ph.h>
#include <stdio.h>
#include <stdlib.h>
#include "global.h"
#include "line.h"
#include "station.h"

int metro_station::reference = 0;

metro_station::metro_station()
{
	wnd = NULL;
	number = 0;
	reference++;	
	id = reference;
	escalatorNum = 0;

	lineID = -1;
	xcoord = ycoord = 0;
}

metro_station::~metro_station()
{
	reference--;
}

int metro_station::create_wnd(ApDBase_t* dbase, PtWidget_t* parent, int x, int y, int width, int height)
{
    PtArg_t args[2];
	int count = 0;
	metro_escalator* cur_escalator;
	char	str[128];
	int		strSize;
	PhRect_t	extent;

	width = 2 + 4;

	for(int i=0; i<g_escalatorNum; i++)
    {
    		if (g_escalators[i].stationID == id)
    			width += (45 + 2);
    	}    	
    	
    strSize = Name.Get(str, 128);
    PtSetParentWidget(parent);
	PfExtentText(&extent, NULL, "Caption08b", str, strSize);
	PtSetArg(&args[0], Pt_ARG_WIDTH, __max(extent.lr.x + 15, width), 0);
    	PtSetArg(&args[1], Pt_ARG_TITLE, str, 0);
    wnd = ApCreateWidget(dbase, "station_wnd", xcoord*x/100 - 10, xcoord*y/100, 2, args);
    if (!wnd)
		return 0;
	
	
    for(int i=0; i<g_escalatorNum; i++)
    {
    		if (g_escalators[i].stationID == id)
    		{
    			g_escalators[i].CreateWnd(dbase, wnd, count*(45 + 2) + 3, 3);
    			count++;
    		}
    	}
    	
    	if (!wnd || PtRealizeWidget(wnd) == -1)
      		return 0;
 
    return 1;
}
	
// that's all folks
