/*******************************************************************************
Programming - Alexej Korobko 
Project - Spider
Started at 20.08.03
Last updated at 20.08.03
Copyright (C) 2000, 2001 SCAD Ltd. (software development group)
*******************************************************************************/

#include <photon/PxProto.h>

#include "cmd_pool.h"

#include "map.h"
#include "global.h"
#include "abvars.h"

#define		SEL_BUFFER_LEN		4

int Command::Send()
{	
	int esc_found=0;
	CHECK_ADDRESS(id_escalator);
	ALL_ESCALATORS
	{
	if (g_escalators[i].id==id_escalator) 
		{
		g_escalators[i].SendCommand(command_code);
		esc_found=1;
		}
	}
	return esc_found;
}

int Command::GetStation()
{	
	return station_id;
}


int
OnClickSendCommandBtn( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
	{
	PtWidget_t *cmd_pool_list;
	unsigned short buffer[SEL_BUFFER_LEN];
	Command *tmp_cmd;
		
	if (ApGetWidgetPtr(cmd_pool_list, ABN_CommandPoolRwLst)!=NULL)	
		if (PtGenListGetSelIndexes(cmd_pool_list, buffer)!=NULL) 
			{
			for (int i=0; i<SEL_BUFFER_LEN; i++)
				if (buffer[i]!=0)
					if (g_CommandPool.GetSize()>buffer[i]-1)
					{
					tmp_cmd=g_CommandPool[buffer[i]-1];
					if (tmp_cmd!=NULL) 	tmp_cmd->Send();			
					};
				
			}; 

	return( Pt_CONTINUE );
	}


int ReleazeCmdPoolList( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	PtGenListItem_t newItem;	
	
	newItem.prev = NULL;
	newItem.next = NULL;
	newItem.flags = 0;
	newItem.size.h = LOG_ROW_HEIGHT;
	newItem.size.w= 0;
	
	int size=g_CommandPool.GetSize();
	
	printf("Size of Command  Pool %d\n", size);
	
	for (int i=0; i<size;i++)
	{
		g_CommandPool[i];
		PtGenListAddItems(widget, &newItem, PtGenListLastItem(widget));
	}
	
	return( Pt_CONTINUE );
}