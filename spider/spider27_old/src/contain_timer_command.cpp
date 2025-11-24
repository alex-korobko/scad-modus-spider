#include "global.h"
#include "spider_exception.h"
#include "system_settings.h"
#include "contain_timer_command.h"

bool timer_command::operator == 
	(timer_command timer_command_to_equal){
	if (this->enabled_excecution!=timer_command_to_equal.enabled_excecution) return false;
	if (this->execution_mode!=timer_command_to_equal.execution_mode) return false;
	if (this->timer_minute!=timer_command_to_equal.timer_minute) return false;
	if (this->cmd!=timer_command_to_equal.cmd) return false;
	return true;
};


bool contain_timer_command::operator () 
           (timer_command first_timer_command,
            timer_command second_timer_command) const {

	if (first_timer_command.get_timer_hour()<second_timer_command.get_timer_hour()) 
		{
			return true;
		} else	if (first_timer_command.get_timer_hour()==second_timer_command.get_timer_hour() &&
						first_timer_command.get_timer_minute()<second_timer_command.get_timer_minute()) 
						return true; 

	return false;	
};


contain_timer_command::contain_timer_command(const contain_timer_command& cont_timer_command) :
	container_timer_command(cont_timer_command.container_timer_command),
	was_created_in_this_day(cont_timer_command.was_created_in_this_day),
	only_execution_mode_enabled_now(cont_timer_command.only_execution_mode_enabled_now),
	widget(cont_timer_command.widget){
		filtered_end=container_timer_command.begin();
		advance(filtered_end,
						distance<const_iterator_morining_start>(cont_timer_command.container_timer_command.begin(), 
																						cont_timer_command.filtered_end)
						);
	};


void contain_timer_command::set_widget(PtWidget_t *new_widget){
		if (widget!=NULL) {
				PtSetResource(widget, Pt_ARG_POINTER, NULL, 0);				
			}
	
		if (new_widget!=NULL) {
			PtSetResource(new_widget, Pt_ARG_POINTER, this, 0);				
			};		
	
		widget=new_widget;
};

contain_timer_command& 
     contain_timer_command::operator=
        (const contain_timer_command& contain_timer_command)	{
		if (this==&contain_timer_command)  return *this;

		container_timer_command=contain_timer_command.container_timer_command;

		filtered_end=container_timer_command.begin();
		advance(filtered_end,
						distance<const_iterator_morining_start>(contain_timer_command.container_timer_command.begin(), 
                                                                                         contain_timer_command.filtered_end));

		return *this;
	}; 

bool 
contain_timer_command::operator == 
    (contain_timer_command cont_timer_command_to_equal){
	if (this==&cont_timer_command_to_equal)  return true;	
	
	if (this->size()!=cont_timer_command_to_equal.size())  return false;	

	if (this->max_size()!=cont_timer_command_to_equal.max_size())  return false;	

	if (this->was_created_in_this_day!=
		cont_timer_command_to_equal.was_created_in_this_day)  return false;	
	
	contain_timer_command::iterator_morining_start iter_morn_start=
				cont_timer_command_to_equal.container_timer_command.begin();

	contain_timer_command::iterator_morining_start this_iter_morn_start=
								container_timer_command.begin();
 
	while(iter_morn_start!=
				cont_timer_command_to_equal.container_timer_command.end())
	{
		if (*this_iter_morn_start!=*iter_morn_start) return false;

		iter_morn_start++;
		this_iter_morn_start++;
	};

	return true;
}

void
contain_timer_command::prepare_to_display() throw (spider_exception){
	if(widget==NULL) return;
	ostringstream exception_message;
	PtWidget_t *list_widget = ApGetWidgetPtr(widget, ABN_rwlst_config_devices);
	
	if (list_widget==NULL) {
		exception_message<<"Not found list_widget for prepare_to_display";
		throw spider_exception(exception_message.str());
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

	for (int i=1; i<size; i++){
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
