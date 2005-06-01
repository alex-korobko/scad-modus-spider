/*******************************************************************************
Programming - Alexander Klimenko
Project - MetroNET
Started at 25.04.02
Last updated at 25.04.02
Copyright (C) 2000 - 2002 SCAD Ltd. (software development group)
*******************************************************************************/

#include <stdio.h>
#include "defines.h"
#include "cbuffer.h"

Cbuffer_c::Cbuffer_c(int size)
{
	buffer = new byte[size];
	if (!buffer)
		freeSize = allSize = 0;
	else
		freeSize = allSize = size;
	head = tail = 0;
}

Cbuffer_c::~Cbuffer_c()
{
	if (buffer)
		delete[] buffer;

	buffer = NULL;
	head = tail = allSize = freeSize = 0;
}

int Cbuffer_c::Create(int size)
{
	if (buffer)
		delete[] buffer;

	buffer = new byte[size];
	head = tail = 0;
	if (!buffer)
	{
		freeSize = allSize = 0;
		return 0;
	}
	else
	{
		freeSize = allSize = size;
		return 1;
	}	
}
	
int Cbuffer_c::Add(byte* ptr, int size)
{
	int rest, second;

	if (!size || (size > (freeSize - 1)))
			return 0;

    buffer[tail] = size;
    tail = (tail+1)%allSize;
	rest = allSize - tail;
	if (rest < size)
    {
        memcpy(&buffer[tail], ptr, rest);
		second = size - rest;
		memcpy(buffer, &ptr[rest], second);
		tail = second;
	}
	else
	{
		memcpy(&buffer[tail], ptr, size);
		tail += size;
	}

	freeSize -= (size + 1);

	return 1;
}

int Cbuffer_c::Get(byte* ptr)
{
	int	size, rest, second;

	if (freeSize == allSize)
		return 0;

	size = buffer[head];

	head = (head+1)%allSize;

	rest = allSize - head;
	if (rest < size)
    {
        memcpy(ptr, &buffer[head], rest);
		second = size - rest;
		memcpy(&ptr[rest], buffer, second);
		head = second;
	}
	else
	{
		memcpy(ptr, &buffer[head], size);
		head += size;
	}

	freeSize += (size + 1);
	
	return size;
}
