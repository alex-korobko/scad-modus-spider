/*******************************************************************************
 Programming - Alexander Klimenko
 Project - MetroNET
 Started at 27.12.00
 Last updated at 16.01.01
 Copyright (C) 2000, 2001 SCAD Ltd. (software development group)
*******************************************************************************/

#ifndef __DICTIONARY_H__
#define __DICTIONARY_H__

#include "goodies.h"

class		DictEntry
{
public:
	struct ltint
	{
		  bool operator() (const int i1, const int i2) const
 			 {
   			 return (i1<i2) ;
  			}
	};

	int			number;
	char*	text;
	dword	color;
};

int LoadDictionary(const char* fileName);

#endif

// that's all folks
