/*******************************************************************************
Programming - Alexander Klimenko
Project - MetroNET
Started at 25.04.02
Last updated at 25.04.02
Copyright (C) 2000 - 2002 SCAD Ltd. (software development group)
*******************************************************************************/

#ifndef __CBUFFER_H__
#define __CBUFFER_H__

#include <string.h>
#include "defines.h"

class Cbuffer_c
{
protected:
	byte*		buffer;
	int			freeSize;
	int			allSize;
	int			tail;
	int			head;
	
public:
	Cbuffer_c(int size = 512);
	int Create(int size);
	virtual ~Cbuffer_c();
	int Add(byte* ptr, int size);
	int Get(byte* ptr);
};

#endif
