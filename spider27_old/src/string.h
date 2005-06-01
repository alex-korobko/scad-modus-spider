#ifndef _STRING_H_
#define _STRING_H_

#include <stdlib.h>
#include <stdio.h>

class String
{
	protected:
		int			length;
		char*		str;		
	public:
	String();
	String(const char* string);
	virtual ~String();
	int Set(const char* string);
	int Get(char* string, int size);
	int Length() { return length; }
	int	Archive(FILE* handle);
	int	Load(FILE* handle);	
};

#endif
