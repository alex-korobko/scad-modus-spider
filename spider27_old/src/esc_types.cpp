/*******************************************************************************
 Programming - Alexander Klimenko
 Project - MetroNET
 Started at 27.12.00
 Last updated at 16.01.01
 Copyright (C) 2000, 2001 SCAD Ltd. (software development group)
*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <photon/PxProto.h>
#include <Pt.h>

#include "global.h"
#include "esc_types.h"


Type::Type()
{
	blocks = NULL;
	size = 0;
}

Type::~Type()
{
	if (blocks)
		delete[] blocks;
	size = 0;
}

Block::Block()
{
	signals = NULL;
	panel = NULL;
	size = 0;
}

int Block::CreatePanel(PtWidget_t* parent)
{
	char	buffer[128];
	PtArg_t args[3];
	int column=0, row=0, flg;

	name.Get(buffer, 128);
	flg=PtEnter(0);
	if (flg<0) {
		SysMessage(ERROR_MSG, "In Block::CreatePanel: %s", strerror(-flg ));
		return (Pt_CONTINUE);
					};

	PtSetArg(&args[0], Pt_ARG_TITLE, buffer, 0);
	PtSetArg(&args[1], Pt_ARG_FILL_COLOR, 0x00BBC1D9, 0);
	PtSetArg(&args[2], Pt_ARG_OUTLINE_COLOR, 0x00A8A9D8, 0);
	panel = PtCreateWidget(PtPane, parent, 3, args);
	PtLeave(flg);
	
	for(int i=0; i<size; i++)
	{		
		signals[i].CreateLed(panel, column, row);
		row++;
		if (row == 10)
		{
			column++;	
			row = 0;
		}
	}

	if (panel) {
		flg=PtEnter(0);
		if (flg<0) {
		SysMessage(ERROR_MSG, "In Block::CreatePanel: %s", strerror(-flg ));
		return (Pt_CONTINUE);
						};
		PtRealizeWidget(panel);
		PtLeave(flg);
					};
	return 1;
}

Block::~Block()
{
	if (signals)
		delete[] signals;
	size = 0;
}

Signal::Signal()
{
	index = 0;
	led = NULL;
}

int Signal::CreateLed(PtWidget_t* parent, int column, int row)
{
	PtArg_t args[6];
	char		buffer[128];
	char		balloon[128];
	int flg;
	
	name.Get(buffer, 128);
	flg=PtEnter(0);
	if (flg<0) {
			SysMessage(ERROR_MSG, "In Block::CreatePanel: %s", strerror(-flg ));
			return (Pt_CONTINUE);
					};
					
	PtSetParentWidget(parent);
	PtSetArg(&args[0], Pt_ARG_TEXT_STRING, buffer, 0);
	PtSetArg(&args[1], Pt_ARG_LABEL_TYPE, Pt_TEXT_IMAGE, 0);
	PtSetArg(&args[2], Pt_ARG_TEXT_IMAGE_SPACING,	 5, 0);
	led = ApCreateWidget(widget_dbase, "block_grey_led", 5+65*column, 4+24*row, 3, args);
	PtLeave(flg);
	
//	hint.Get(balloon, 128);		
//	PtSetArg(&args[0], Pt_ARG_TEXT_STRING, "jhvjvhvjhhjvjh", 0);	
//	ApCreateWidget(widget_dbase, "led_balloon", 5+65*column, 4+24*row, 1, args);

	return 1;
}

Signal::~Signal()
{
	index = 0;
	led = NULL;
}

void FreeEscalatorTypes()
{

	delete[] g_escTypes;
}

int LoadEscalatorLibrary(const char* fileName)
{
	Block*		curBlock = NULL;
	Type*		curType = NULL;
	Signal*		curSignal = NULL;
	char	buffer[256];
	char	temp[40];
	FILE*	handle;
   	char 	*val_name, *val_data;
    char  	tokens[] = "=";
	int	   	mode = 0;
	int 	   	intTemp, i;
	int		typeCount = -1, blockCount = -1, signalCount = -1;

	handle = fopen(fileName, "rt");
	if (!handle)
	{
		SysMessage(ERROR_MSG, "can't open escalators' library [%s]", strerror(errno));
		return 0;
	}
		
	while(fgets(buffer, 256, handle))
	{
		if (extract_string(buffer, temp, '[', ']'))
		{
			if (!strcmp("type", temp))
			{
				mode = 1;
				typeCount++;
				if (typeCount >= g_escTypeNum)
				{
					typeCount = -1;
					mode = 0;
					SysMessage(ERROR_MSG, "Parsing escalator library file");
					fclose(handle);
					return 0;
				}
				signalCount = -1;
				blockCount = -1;
			}
			else if (!strcmp("block", temp))
			{
				mode = 2;
				blockCount++;
				signalCount = -1;
			}
			else if (!strcmp("signal", temp))
			{
				mode = 3;
				signalCount++;
			}
		}
		else if (buffer[0] != '#')
		{
			val_name = strtok(buffer, tokens);
			val_data = strtok(NULL, tokens);
			if (val_name && val_data)
			{
				if (!strcmp("typeNum", val_name))
				{					
					g_escTypeNum = atoi(val_data);
					if (g_escTypeNum)
						g_escTypes = new Type[g_escTypeNum];
				}
				else if (!strcmp("blockNum", val_name))
				{	
					if (g_escTypes)
					{
						g_escTypes[typeCount].size = atoi(val_data);
						g_escTypes[typeCount].blocks = new Block[g_escTypes[typeCount].size];
					}
				}
				else if (!strcmp("signalNum", val_name))
				{					
					if (g_escTypes[typeCount].blocks)
					{
						g_escTypes[typeCount].blocks[blockCount].size = atoi(val_data);
						g_escTypes[typeCount].blocks[blockCount].signals = new Signal[g_escTypes[typeCount].blocks[blockCount].size];
					}
				}
				else if (!strcmp("num", val_name))
				{					
					g_escTypes[typeCount].blocks[blockCount].signals[signalCount].index = atoi(val_data);
				}
				else if (!strcmp("description", val_name))
				{					
					g_escTypes[typeCount].blocks[blockCount].signals[signalCount].hint.Set(val_data);
				}
				else if (!strcmp("name", val_name))
				{
					switch(mode)
					{
						case 1:
							g_escTypes[typeCount].name.Set(val_data);
							break;
						case 2:
							g_escTypes[typeCount].blocks[blockCount].name.Set(val_data);
							break;
						case 3:
							g_escTypes[typeCount].blocks[blockCount].signals[signalCount].name.Set(val_data);
							break;
						default:
							break;
					}
				}
			}
		}
	}

	fclose(handle);
	
	return 1;
}
