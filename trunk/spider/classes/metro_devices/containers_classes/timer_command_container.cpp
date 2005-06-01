using namespace std;

#include <vector>
#include <string>
#include <hash_map>
#include <list>
#include <algorithm>
#include <functional>
#include <sstream>

#include <iostream.h>

#include <Ph.h>
#include <Ap.h>
#include <Pt.h>
#include <pthread.h>
#include <process.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/neutrino.h>
#include <sys/syspage.h>
#include <inttypes.h>
#include <fcntl.h>

#include "defines.h"
#include <sys/select.h> //#define FD_SETSIZE
#include "spider_exception.h"
#include "system_settings.h"
#include "system_settings_spider.h"

#include "command.h"
#include "timer_command_container.h"

timer_command_container* timer_command_container::timer_command_container_instance=NULL;

timer_command_container* timer_command_container::get_instance(){
  if (timer_command_container_instance==NULL)
         timer_command_container_instance= new timer_command_container();
  return timer_command_container_instance;
};

bool timer_command::operator == 
	(timer_command timer_command_to_equal){
	if (this->enabled_excecution!=timer_command_to_equal.enabled_excecution) return false;
	if (this->execution_mode!=timer_command_to_equal.execution_mode) return false;
	if (this->timer_minute!=timer_command_to_equal.timer_minute) return false;
	if (! (this->cmd==timer_command_to_equal.cmd) ) return false;
	return true;
};


bool timer_command_container::
              timer_cmd_sorter::operator () 
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


timer_command_container::timer_command_container(const timer_command_container& cont_timer_command) :
	container_timer_command(cont_timer_command.container_timer_command),
	was_created_in_this_day(cont_timer_command.was_created_in_this_day),
	only_execution_mode_enabled_now(cont_timer_command.only_execution_mode_enabled_now),
timer_commands_state_indicator(cont_timer_command.timer_commands_state_indicator){
		filtered_end=container_timer_command.begin();
		advance(filtered_end,
						distance<const_iterator_timer_command>(cont_timer_command.container_timer_command.begin(), 
																						cont_timer_command.filtered_end)
						);
	};


timer_command_container& 
     timer_command_container::operator=
        (const timer_command_container& contain_timer_command)	{
		if (this==&contain_timer_command)  return *this;

		container_timer_command=contain_timer_command.container_timer_command;

		filtered_end=container_timer_command.begin();
		advance(filtered_end,
						distance<const_iterator_timer_command>(contain_timer_command.container_timer_command.begin(), 
                                                                                         contain_timer_command.filtered_end));

		return *this;
	}; 

bool 
timer_command_container::operator == 
    (timer_command_container cont_timer_command_to_equal){
	if (this==&cont_timer_command_to_equal)  return true;	
	
	if (this->size()!=cont_timer_command_to_equal.size())  return false;	

	if (this->max_size()!=cont_timer_command_to_equal.max_size())  return false;	

	if (this->was_created_in_this_day!=
		cont_timer_command_to_equal.was_created_in_this_day)  return false;	
	
	timer_command_container::iterator_timer_command iter_timer_cmd=
				cont_timer_command_to_equal.container_timer_command.begin();

     timer_command_container::iterator_timer_command this_iter_timer_cmd=
								container_timer_command.begin();
 
	while(iter_timer_cmd!=
				cont_timer_command_to_equal.container_timer_command.end()) {
		if (!(*this_iter_timer_cmd==*iter_timer_cmd)) return false;

		iter_timer_cmd++;
		this_iter_timer_cmd++;
	};

	return true;
}

void
timer_command_container::prepare_to_display() throw (spider_exception){
	
	if (timer_commands_raw_list==NULL) {
		throw spider_exception("Not found list_widget for prepare_to_display");
		};

	PtGenListRemoveItems(timer_commands_raw_list, NULL,NULL);

	unsigned short size=this->size();

   if (size==0) return ;

	PtGenListItem_t *first, *last,  *list_item=new(PtGenListItem_t);

	first=list_item;

	first->prev=NULL;
	first->next=NULL;
	first->size.w=0;
	first->size.h=system_settings_spider::ROW_HEIGHT;
	first->flags=0;

	for (int i=1; i<size; i++){
			last=	new(PtGenListItem_t);
			last->prev=list_item;
			last->next=NULL;
			last->size.w=0;
			last->size.h=system_settings_spider::ROW_HEIGHT;
			last->flags=0;
			list_item->next=last;
				
			list_item=last;
		};

	 PtGenListAddItems(timer_commands_raw_list, first, NULL);
};


void timer_command_container::change_indicator_state(){
if (timer_commands_state_indicator==NULL)
               return;
system_settings_spider *spider_sys_sett=system_settings_spider::get_instance();
if(spider_sys_sett==NULL) {
            cout<<"In timer_command_container::change_indicator_state() can`t get instance of spider_sys_sett"<<endl;
           return;
};

vector<PtArg_t> args(2);
if (was_created_in_this_day) {
          if (empty()) {
            PtSetArg(&args[0], Pt_ARG_TEXT_STRING,
                      "УТРЕННИЙ ПУСК ВЫПОЛНЕН", 0);
             PtSetArg(&args[1], Pt_ARG_LABEL_IMAGE,
                              spider_sys_sett->get_image(system_settings_spider::BLOCK_GREY_LED), 0);
             } else { //empty())
                 if (timer_commands_accepted) {
                       PtSetArg(&args[0], Pt_ARG_TEXT_STRING,
                                     "УТРЕННИЙ ПУСК УТВЕРЖДЕН", 0);
                       PtSetArg(&args[1], Pt_ARG_LABEL_IMAGE,
                                spider_sys_sett->get_image(system_settings_spider::BLOCK_GREEN_LED), 0);
                   } else { //if (timer_commands_accepted
                       PtSetArg(&args[0], Pt_ARG_TEXT_STRING,
                                     "УТРЕННИЙ ПУСК НЕ УТВЕРЖДЕН", 0);
                       PtSetArg(&args[1], Pt_ARG_LABEL_IMAGE,
                                spider_sys_sett->get_image(system_settings_spider::BLOCK_RED_LED), 0);
                   }; //if (timer_commands_accepted
              }; ////if (empty())
      } else { //if (was_created_in_this_day
             PtSetArg(&args[0], Pt_ARG_TEXT_STRING,
                                     "УТРЕННИЙ ПУСК НЕ СОЗДАН", 0);
             PtSetArg(&args[1], Pt_ARG_LABEL_IMAGE,
                              spider_sys_sett->get_image(system_settings_spider::BLOCK_GREY_LED), 0);
     };

  PtSetResources(timer_commands_state_indicator, args.size(), &args[0]);

};
