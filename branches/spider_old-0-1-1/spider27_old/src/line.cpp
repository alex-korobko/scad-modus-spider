/*******************************************************************************
 Programming - Alexander Klimenko
 Project - MetroNET
 Started at 27.12.00
 Last updated at 16.01.01
 Copyright (C) 2000, 2001 SCAD Ltd. (software development group)
*******************************************************************************/

#include <stdio.h>
#include "station.h"
#include "line.h"

metro_line::metro_line()
{
	color = 0;
	stationCount = 0;
	points = NULL;
	line = NULL;
    wnd = NULL;
	left_scroll = right_scroll = NULL;
	first_visible = 0;
	right_clipped = 0;
	id = -1;
}

metro_line::~metro_line()
{
}

// that's all folks
