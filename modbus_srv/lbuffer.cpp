#include <string.h>
#include "lbuffer.h"

Buffer::Buffer()
{
	length = 0;
	buffer = NULL;
	next = NULL;
}

Buffer::~Buffer()
{
	if ((length > 0) && (buffer != NULL))
	{
		delete[] buffer;
		length = 0;
		buffer = NULL;
		next = NULL;		
	}
}

int Buffer::Set(byte* ptr, int size)
{
	if (!size || !ptr)
		return 0;
		
	if (buffer)
		delete[] buffer;
	
	buffer = new byte[size];
	if (!buffer)
			return 0;
	
	memcpy(buffer, ptr, size);
	length = size;
	
	return size;
}

int Buffer::Get(byte* ptr, int size)
{
	if (!ptr || (size < length) || !buffer || !length)
		return 0;
	
	memcpy(ptr, buffer, length);
	
	return length;
}

CmdPool::CmdPool()
{
	head = tail = NULL;
	length = 0;
}

CmdPool::~CmdPool()
{
	Buffer*	curBuffer = head;
	
	while(length)
	{
		delete curBuffer;
		curBuffer = curBuffer->next;
		length--;
	}

	head = tail = NULL;
}

int CmdPool::Put(byte* ptr, int size)
{
	Buffer*	curBuffer = new Buffer();
	if (!curBuffer)
		return 0;
	
	if (!curBuffer->Set(ptr, size))
	{
		delete curBuffer;
		return 0;
	}
	
	if (!length)
		head = tail = curBuffer;
	else
	{
		tail->next = curBuffer;
		tail = curBuffer;
	}
	
	length++;
	
	return 1;
}
	
int CmdPool::Get(byte* ptr, int size)
{
	int	 getSize;
	
	Buffer*	curBuffer = head;
	if (!curBuffer)
		return 0;
		
	if (!(getSize = curBuffer->Get(ptr, size)))
		return 0;
	
	head = curBuffer->next;

	delete curBuffer;
	
	length--;
	
	return getSize;
}