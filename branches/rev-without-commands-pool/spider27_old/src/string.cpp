#include <stdio.h>
#include <string.h>
#include "string.h"

String::String()
{
	length = 0;
	str = NULL;
}

String::String(const char* string)
{
	if (string)
	{
		length = strlen(string);
		str = new char[length + 1];
		if (str)
			strcpy(str, string);
	}
}

String::~String()
{
	if (str)
	{
		delete[] str;
		str = NULL;
	}
}


int String::Set(const char* string)
{
	if (string)
	{
		if (str)
			delete[] str;
		length = strlen(string);
		str = new char[length + 1];
		if (str)
		{
			strcpy(str, string);
			return 1;
		}
		else
			return 0;
	}
	
	return 0;
}

int String::Get(char* string, int size)
{
	if (size <(length+1))
	{
		string = NULL;
		return 0;
	}
	strcpy(string, str);
	return length;
}
		
int String::Archive(FILE* handle)
{
	fwrite(&length, sizeof(length), 1, handle);
	fwrite(str, length, 1, handle);
	
	return 1;
}

int String::Load(FILE* handle)
{
	fread(&length, sizeof(length), 1, handle);
	str = new char[length + 1];
	if (str)
	{
		fread(str, length, 1, handle);
		str[length] = '\0';
	}
	else
	{
		length = 0;
		return 0;
	}
		
	return 1;
}
