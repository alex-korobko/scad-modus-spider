/*******************************************************************************
 Programming - Alexander Klimenko
 Project - MetroNET
 Started at 27.12.00
 Last updated at 16.01.01
 Copyright (C) 2000, 2001 SCAD Ltd. (software development group)
*******************************************************************************/

#ifndef __STATION_H__
#define __STATION_H__

#include <Pt.h>
#include <Ap.h>
#include "string.h"
#include "escalator.h"
#include "system.h"

class metro_line;

// station class
class metro_station
{
public:
   	PtWidget_t* 	wnd;
   	int     			number;
   	static int 		reference;
   	int					id;
   	String			Name;
	int			escalatorNum;
	int			lineID;
	int			xcoord, ycoord;
   
   	metro_station();
   	~metro_station();   
   
   	int create_wnd(ApDBase_t* dbase, PtWidget_t* parent, int x = -1, int y = -1, int width = -1, int height = -1);

	int	GetID() { return id; }
//   	int realize() { return PtRealizeWidget(wnd); }
//   	int unrealize() { return PtUnrealizeWidget(wnd); }   
//   	long get_widget_flags();
//   	int get_widget_width();
//   	void set_widget_position(int x, int y, int width = 0, int height = 0);
};

#endif

// that's all folks
