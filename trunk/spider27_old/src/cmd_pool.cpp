/*******************************************************************************
Programming - Alexej Korobko 
Project - Spider
Started at 20.08.03
Last updated at 20.08.03
Copyright (C) 2000, 2001 SCAD Ltd. (software development group)
*******************************************************************************/

#include <photon/PxProto.h>
#include <string.h>
#include "cmd_pool.h"

#include "map.h"
#include "global.h"
#include "abvars.h"


int Command::GetCmdCode(void)
{
return command_code;
}

int Command::GetEscNum(void)
{
return esc_number;
}

void 	Command::SetItemColor(const PgColor_t& color_of_item)
{
item_color=color_of_item;
}

PgColor_t& Command::GetItemColor(void)
{
return  item_color;
}


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
	PtWidget_t *cmd_pool_list=NULL, *cmd_pool_wnd;
	PtGenListItem_t *curr_item;
//	Command tmp_cmd;
	int i=0, count ;
	list<Command>::iterator p_cmd_pool;	

   cmd_pool_wnd = ApGetInstance( widget );
   cmd_pool_list = ApGetWidgetPtr( cmd_pool_wnd, ABN_CommandPoolRwLst);

	if (cmd_pool_list!=NULL)
	{
	curr_item = PtGenListFirstItem(cmd_pool_list);
	count=g_CommandPool.size();
	p_cmd_pool=g_CommandPool.begin();
	
	while(p_cmd_pool!=g_CommandPool.end() && curr_item)
		{
			if(curr_item->flags&Pt_LIST_ITEM_SELECTED>0)
						{
						(*p_cmd_pool).Send();
						g_CommandPool.erase(p_cmd_pool);
						}
		p_cmd_pool++;
		curr_item=curr_item->next;
		};
	};
	return( Pt_CONTINUE );
	}


int ReleazeCmdPoolList( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	PtGenListItem_t  *prev_item, *next_item, *list_item=new(PtGenListItem_t) , *first_item;	
	
	int size=g_CommandPool.size();
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
