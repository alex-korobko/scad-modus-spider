/*******************************************************************************
 Programming - Alexander Klimenko
 Project - MetroNET
 Started at 27.12.00
 Last updated at 16.01.01
 Copyright (C) 2000, 2001 SCAD Ltd. (software development group)
*******************************************************************************/

#ifndef __LINE_H__
#define __LINE_H__

#include <Pt.h>
#include <Ph.h>
#include <Ap.h>
#include "station.h"

// metropoliten line class
class metro_line
{
protected:   
   PtWidget_t* wnd;
   PtWidget_t *left_scroll, *right_scroll;
   PhImage_t *left_up_image, *right_up_image, *left_disable_image,*right_disable_image;
   int first_visible, right_clipped;
   
public:
	PgColor_t	color;
	PhPoint_t	*points;
	int				stationCount;
	PtWidget_t		*line;
	String	Name;
	int			id;
	
   metro_line();
   ~metro_line();
};

#endif

// that's all folks
