#ifndef __BUFFER_H__
#define __BUFFER_H__

#include "types.h"

class Buffer
{
public:
	int			length;
	byte*		buffer;
	Buffer*	next;

	Buffer();
	~Buffer();
	int Set(byte* ptr, int size);
	int Get(byte* ptr, int size);
};

class CmdPool
{

protected:
	Buffer*	head;
	Buffer*	tail;	
	int			length;
public:
	CmdPool();
	~CmdPool();
	int Put(byte* ptr, int size);
	int Get(byte* ptr, int size);
};

#endif