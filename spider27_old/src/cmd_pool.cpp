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
	PtWidget_t *cmd_pool_list=NULL;
	unsigned short buffer[SEL_BUFFER_LEN];
	Command *tmp_cmd;

	ApGetWidgetPtr(cmd_pool_list, ABN_CommandPoolRwLst);

	if (cmd_pool_list!=NULL)
	{
	printf ("\nIn OnClickSendCommandBtn\n");
		if (PtGenListGetSelIndexes(cmd_pool_list, buffer)!=NULL) 
			{
			for (int i=0; i<SEL_BUFFER_LEN; i++)
				{
				printf ("\nsel index  %d\n", buffer[i]);
				if (buffer[i]!=0)
					if (g_CommandPool.GetSize()>static_cast<unsigned int>(buffer[i]-1))
					{
					tmp_cmd=g_CommandPool[buffer[i]-1];
					if (tmp_cmd!=NULL) 	tmp_cmd->Send();			
					}; //if (g_CommandPo
				};// for (int i=0; i<SEL_BU
			};  // if (PtGenListGetSelIndexes(c
	}; // if (ApGetWidgetPtr(cmd_pool_lis
	return( Pt_CONTINUE );
	}


int ReleazeCmdPoolList( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	PtGenListItem_t  *prev_item, *next_item, *list_item=new(PtGenListItem_t) , *first_item;	
	
	int size=g_CommandPool.GetSize();
//	printf("Size of Command  Pool %d\n", size);

	PtGenListRemoveItems(widget, NULL, NULL);	
	
	if (size>0)
	{
	first_item=list_item;

	list_item->prev = NULL;
	list_item->next = NULL;
	list_item->flags = 0;
	list_item->size.h = ROW_HEIGHT;
	list_item->size.w= 0;

	for (int i=1; i<size;i++)
	{
		prev_item=list_item;
		list_item->next=new(PtGenListItem_t);
		list_item=list_item->next;
		list_item->prev = prev_item;
		list_item->next = NULL;
		list_item->flags = 0;
		list_item->size.h = ROW_HEIGHT;
		list_item->size.w= 0;
	}

	PtGenListAddItems(widget, first_item, NULL);	
	}	
	return( Pt_CONTINUE );
}
