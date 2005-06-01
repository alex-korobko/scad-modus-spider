#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "goodies.h"

void ErrorMsg(const char* msg)
{
    printf("*** ERROR_MSG: %s", msg);
    if (errno)
    	printf(" <%s>", strerror(errno));
    printf(" ***\n");
}

int StripSpaces(char* str)
{
	int		count = 0, first = -1, last = -1;

	while(str[count])
	{
		if (str[count] != ' ')
		{
			first = count;
			break;
		}
		count++;
	}
	
	if (first == -1)
		return 0;
		
	count = strlen(str) - 1;
	
	while(str[count])
	{
		if (str[count] != ' ')
		{
			last = count + 1;
			break;
		}
		count--;
	}						
	
	if (last == -1)
		last = count;
	
	memmove(str, &str[first], last - first);
	str[last - first] = 0;
	
	return 1;
}

word CRC16(byte *buffer, int length)
{
  word data;
  word CRC = 0xFFFF;

  if (length == 0)
    return (~CRC);

  do
  {
     for(int i=0, data=(word)0xff & *buffer++; i<8; i++, data >>= 1)
     {
       if((CRC & 0x0001) ^ (data & 0x0001))
	     CRC = (CRC>>1)^0x8408;
       else
	     CRC >>= 1;
     }
  } while(--length);

  CRC = ~CRC;
  data = CRC;
  CRC = (CRC << 8) | ((data >> 8) & 0xff);

  return CRC;
}
