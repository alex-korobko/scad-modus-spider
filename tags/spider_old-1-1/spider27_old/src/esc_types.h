/*******************************************************************************
Programming - Alexander Klimenko
Project - MetroNET
Started at 27.12.00
Last updated at 03.01.01
Copyright (C) 2000, 2001 SCAD Ltd. (software development group)
*******************************************************************************/

#ifndef _ESC_TYPES_H_
#define _ESC_TYPES_H_

#include <Pt.h>
#include "map.h"
#include "string.h"

void FreeEscalatorTypes();
int LoadEscalatorLibrary(const char* file_name);

class Signal
{
public:
	int			index;
	String		name;
	String 		hint;	
	PtWidget_t*	led;
	
	Signal();
	virtual ~Signal();
	int CreateLed(PtWidget_t* parent, int column, int row);
};

class Block
{
public:
	String		name;
	Signal		*signals;
	int			size;
	PtWidget_t	*panel;
	
	Block();
	virtual ~Block();
	int CreatePanel(PtWidget_t* parent);
};

class Type
{
public:
	String		name;
	Block		*blocks;
	int			size;
	
	Type();
	virtual ~Type();
};

#endif
