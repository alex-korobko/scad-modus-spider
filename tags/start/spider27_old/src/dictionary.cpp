/*******************************************************************************
 Programming - Alexander Klimenko
 Project - MetroNET
 Started at 27.12.00
 Last updated at 16.01.01
 Copyright (C) 2000, 2001 SCAD Ltd. (software development group)
*******************************************************************************/

#include <stdio.h>
#include "dictionary.h"
#include "global.h"
#include "system.h"
#include "map.h"

int LoadDictionary(const char* fileName)
{
	FILE*		file;
	char		str[256];
	char		delims[] = "\t";
	char*	token;
	DictEntry*		newEntry = NULL;
	int			count = 0;
	
	file = fopen(fileName, "rt");
	if (file)
	{
		while(fgets(str, 256, file))
		{
			str[strlen(str) - 1] = 0;
				count = 0;
				token = strtok(str, delims);
			    while(token)
			    {
					 switch(count)
					 {
					 	case 0:
					 		newEntry = new DictEntry;
					 		if (newEntry)
					 		{
					 			newEntry->number = atoi(token);
					 		}
					 		else
					 		{
					 			SysMessage(ERROR_MSG, "Memory allocation");
					 			fclose(file);
					 			return 0;
					 		}
					 		break;
					 	case 1:
					 		newEntry->text = new char[(strlen(token) + 5) & ~3];
					 		if (newEntry->text)
					 		{
					 			strcpy(newEntry->text, token);
					 			strcat(newEntry->text, "\n");
					 		}
					 		else
					 		{
					 			SysMessage(ERROR_MSG, "Memory allocation");
					 			fclose(file);
					 			return 0;
					 		}
					 		break;
					 	case 2:
					 		if (!strcmp(token, "BLACK"))
					 			newEntry->color = COLOR_BLACK;
					 		else if (!strcmp(token, "RED"))
					 			newEntry->color = COLOR_RED;
					 		else if (!strcmp(token, "BLUE"))
					 			newEntry->color = COLOR_BLUE;
					 		else if (!strcmp(token, "GREEN"))
					 			newEntry->color = COLOR_GREEN;
					 		break;
					 	default:
					 		break;
					 }
      				token = strtok( NULL, delims );
      				count++;
    				}
    				g_msgDictionary.Push(newEntry, newEntry->number);
		}
		fclose(file);
	}
	else
	{
		SysMessage(ERROR_MSG, "fail to open '%s' dictionary file", fileName);
		return 0;
	}

	SysMessage(INFO_MSG, "Dictionary loaded");
	
	return 1;
}

// that's all folks
