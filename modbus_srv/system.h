/*******************************************************************************
Programming - Alexander Klimenko
Project - MetroNET
Started at 27.12.00
Last updated at 16.04.02
Copyright (C) 2000 - 2002 SCAD Ltd. (software development group)
*******************************************************************************/

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <stdio.h>
#include <stdarg.h>
#include "types.h"

#define LOBYTE(data)	(data & 0x00FF)
#define HIBYTE(data)	(data & 0xFF00) >> 8

enum {ERROR, INFO, DEBUG};

void Log(int type, const char* message, ...);
word CRC(const byte* buffer, int size);

#endif
