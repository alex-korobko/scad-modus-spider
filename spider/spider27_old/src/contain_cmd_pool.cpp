#include "global.h"
#include "system_settings.h"
#include "spider_exception.h"
#include "data_block.h"
#include "metro_device.h"
#include "contain_msg_types.h"
#include "contain_msg_dict.h"
#include "devices_types.h"
#include "contain_metro_devices_types.h"
#include "contain_morning_start.h"
#include "line.h"
#include "contain_metro_lines.h"
#include "station.h"
#include "contain_metro_stations.h"
#include "escalator.h"
#include "contain_metro_escalators.h"
#include "command.h"
#include "contain_cmd_pool.h"


cmd_pool_container::cmd_pool_container(){
		wnd=NULL;
};

cmd_pool_container::~cmd_pool_container(){
	PtArg_t arg;
	
	if (wnd!=NULL) {
				PtSetArg(&arg, 
							Pt_ARG_POINTER , 
							NULL, 
							0);
			 PtSetResources( wnd,
					                    1,
                    						&arg);
 		};
};


void cmd_pool_container::set_widget(PtWidget_t *new_wnd) {
	PtArg_t arg;
	
	if (wnd!=NULL) {
				PtSetArg(&arg, 
							Pt_ARG_POINTER , 
							NULL, 
							0);

			 PtSetResources( wnd,
					                    1,
                    						&arg);
 		}

	if (new_wnd!=NULL) {
				PtSetArg(&arg, 
							Pt_ARG_POINTER , 
							this, 
							0);

			 PtSetResources( new_wnd,
					                    1,
                    						&arg);
 		}

	wnd=new_wnd;
};


void cmd_pool_container::prepare_to_display() throw (spider_exception){
	ostringstream exception_description;
	if(wnd==NULL) return;

	PtWidget_t *list_widget = ApGetWidgetPtr(wnd, ABN_CommandPoolRwLst);
	
	if (list_widget==NULL) {
		exception_description<<"Pointer to list widget is NULL";
		throw spider_exception(exception_description.str());
	};

	PtGenListRemoveItems(list_widget, NULL,NULL);

	unsigned short size=this->size();

   if (size==0) return ;

	PtGenListItem_t *first, *last,  *list_item=new(PtGenListItem_t);

	first=list_item;

	first->prev=NULL;
	first->next=NULL;
	first->size.w=0;
	first->size.h=system_settings::ROW_HEIGHT;
	first->flags=0;

	for (int i=1; i<size; i++)
		{
			last=	new(PtGenListItem_t);
			last->prev=list_item;
			last->next=NULL;
			last->size.w=0;
			last->size.h=system_settings::ROW_HEIGHT;
			last->flags=0;
			list_item->next=last;
				
			list_item=last;
		};

	 PtGenListAddItems(list_widget, first, NULL);
};

