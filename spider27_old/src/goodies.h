#ifndef __GOODIES_H__
#define __GOODIES_H__

#define __DEBUG__

typedef unsigned long 	dword;
typedef unsigned short	word;
typedef unsigned char	byte;

void ErrorMsg(const char* msg);
int StripSpaces(char* str);
word CRC16(byte *buffer, int length);

/*#ifdef __DEBUG__
#define	ERROR(s)		ErrorMsg(s)
#else
#define	ERROR(s)
#endif*/

#include <netinet/in.h>

#endif