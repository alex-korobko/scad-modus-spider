#ifndef __CONTAIN_MORNING_START__
#define __CONTAIN_MORNING_START__

class escalator_start {
private:
int escalator_id;
int escalator_pref_direction;
int escalator_direction;
int start_hour;
int start_minute;
int start_mode;
bool start_enabled;

escalator_start();

public:
escalator_start(
	int new_escalator_id,
	int new_escalator_pref_direction,
	int new_escalator_direction,
	int  new_start_hour,
	int new_start_minute,
	int new_start_mode,
	bool new_start_enabled
	) : escalator_id(new_escalator_id),
	escalator_pref_direction(new_escalator_pref_direction),
	escalator_direction(new_escalator_direction),
	start_hour(new_start_hour),
	start_minute(new_start_minute),
	start_mode(new_start_mode),
	start_enabled(new_start_enabled)
	{};

int get_escalator_id() { return escalator_id; };

int get_escalator_pref_direction() { return escalator_pref_direction; };
void set_escalator_pref_direction (int new_escalator_pref_direction)
	{ escalator_pref_direction=new_escalator_pref_direction; };

int get_escalator_direction() { return escalator_direction; };
void set_escalator_direction (int new_escalator_direction) 
	{ escalator_direction=new_escalator_direction; };

int get_start_hour() { return start_hour; };
void set_start_hour (int new_start_hour) 
	{ start_hour=new_start_hour; };

int get_start_minute() { return start_minute; };
void set_start_minute (int new_start_minute) 
	{ start_minute=new_start_minute; };

int get_start_mode() { return start_mode; };
void set_start_mode (int new_start_mode)
	{ start_mode=new_start_mode; };

bool get_start_enabled() { return start_enabled; };
void set_start_enabled (bool new_start_enabled)
	{ start_enabled=new_start_enabled; };

};

class contain_morning_start {
private :

typedef  list<escalator_start> morning_start_container;	
typedef  morning_start_container::iterator iterator_morining_start;	
typedef  morning_start_container::const_iterator const_iterator_morining_start;	
typedef  morning_start_container::size_type size_type_morining_start;	

morning_start_container container_morning_start;
bool was_created_in_this_day;
PtWidget_t *widget;
iterator_morining_start filtered_end;

/*
functor class for creating contain_morning_start from 
metro_escalators_container
*/	
	class make_morning_start
	{
	private:
		contain_morning_start *cont_morn_start;
		int day_now;

		make_morning_start();

	public:
		explicit make_morning_start(contain_morning_start *new_cont_morn_start) 
		{
			cont_morn_start=new_cont_morn_start;
			
			time_t time_now=time(NULL);
			struct tm that_day_tm;
			localtime_r(&time_now, &that_day_tm);

			that_day_tm.tm_mday%2==0 ?
			day_now=system_settings::START_DAY_MODE_EVEN :
			day_now=system_settings::START_DAY_MODE_ODD;
		}
		
		void operator() (metro_escalators_container::pair_metro_escalators pair_esc)
		{
			bool start_enabled=true;
			
			if (pair_esc.second.get_enabled()==
				system_settings::DISABLED) return;

			int esc_start_day_mode=pair_esc.second.get_start_day_mode();

			if (esc_start_day_mode==
				system_settings::START_DAY_MODE_NEVER ||
				(esc_start_day_mode!=
				system_settings::START_DAY_MODE_EVERYDAY &&
				esc_start_day_mode!=day_now) 
				 ) start_enabled=false; 
	
			int  direction=pair_esc.second.get_pref_direction();
			if (direction!=system_settings::DIRECTION_UP &&
				direction!=system_settings::DIRECTION_DOWN)
				{
					direction=pair_esc.second.get_direction();
					if (direction!=system_settings::DIRECTION_UP &&
						direction!=system_settings::DIRECTION_DOWN)
							start_enabled=false; // what can I do ??
				};

			cont_morn_start->insert(cont_morn_start->end(),
												  escalator_start(pair_esc.second.get_id(),
																		   pair_esc.second.get_pref_direction(),
   																		   pair_esc.second.get_direction(),
 		                 												   pair_esc.second.get_start_hour(),
																		   pair_esc.second.get_start_minute(),
																		   esc_start_day_mode,
																		   start_enabled
																			)
													);
		};
	};


/*
functor class for searching in contain_morning_start
escalators, wich be started in current hour and minutes and start it.
*/	

	class find_morning_start
	{
		private:
		metro_escalators_container *esc_container;
		int start_hour, start_minute;
		
		find_morning_start();
		public:
		find_morning_start(metro_escalators_container *new_esc_container,
									 int new_start_hour,
									 int new_start_minute
									) : start_hour(new_start_hour),
										start_minute(new_start_minute)
									{ esc_container=new_esc_container; };
		
		void operator() (escalator_start& morn_start)
		{
			if (morn_start.get_start_hour()!=start_hour ||
				morn_start.get_start_minute()!=start_minute) return;
		
			metro_escalators_container::iterator_metro_escalators iter_esc=
					esc_container->find(morn_start.get_escalator_id() );
					
			if (iter_esc!=esc_container->end())
				{
					int direction=morn_start.get_escalator_direction();

					if (direction==system_settings::DIRECTION_UP)
							iter_esc->second.send_command(system_settings::COMMAND_UP);

					if (direction==system_settings::DIRECTION_DOWN)
							iter_esc->second.send_command(system_settings::COMMAND_DOWN);
					
				};

		};
	};

/*
functor class for partial sort to selection only morning starts with 
start_enabled==true
*/

class filter_start_enabled : public unary_function<escalator_start, bool>
{
public:
	bool operator () (escalator_start esc_start) const
	{
		return esc_start.get_start_enabled();
	};

};

class mst_sorter : 
 public binary_function <const  escalator_start,
	 									const  escalator_start,
 										bool>
{
public:
bool operator () (escalator_start first_morn_start, escalator_start second_morn_start) const 
	{


	if (first_morn_start.get_start_hour()<second_morn_start.get_start_hour()) 
		{
			return true;
		} else	if (first_morn_start.get_start_hour()==second_morn_start.get_start_hour() &&
						first_morn_start.get_start_minute()<second_morn_start.get_start_minute()) 
						return true;

	if (first_morn_start.get_escalator_id()<second_morn_start.get_escalator_id()) return true;
	return false;	
	};
	
};

public:
/*
constructor
*/
contain_morning_start():
container_morning_start(),
was_created_in_this_day(false),
widget(NULL),
filtered_end (container_morning_start.end())
{};

/*
copy constructor
*/
contain_morning_start(const contain_morning_start& cont_morning_start) :
	container_morning_start(cont_morning_start.container_morning_start),
	was_created_in_this_day(cont_morning_start.was_created_in_this_day),
	widget(cont_morning_start.widget)
	{
		filtered_end=container_morning_start.begin();
		advance(filtered_end,
						distance<const_iterator_morining_start>(cont_morning_start.container_morning_start.begin(), 
																						cont_morning_start.filtered_end)
						);
	};

/*
get_ and set_ metods 
*/
bool get_was_created_in_this_day() {return was_created_in_this_day;};
void set_was_created_in_this_day(bool new_was_created_in_this_day)
				{was_created_in_this_day=new_was_created_in_this_day;};
				
PtWidget_t* get_widget() {return widget;};
void set_widget(PtWidget_t *new_widget)
	{
		if (widget!=NULL) 
			{
				PtSetResource(widget, Pt_ARG_POINTER, NULL, 0);				
			}
	
		if (new_widget!=NULL) 
			{
			PtSetResource(new_widget, Pt_ARG_POINTER, this, 0);				
			};		
	
		widget=new_widget;
	};

/*
interface to useful metods of stl container
*/
iterator_morining_start begin() {return container_morning_start.begin();};
iterator_morining_start end() {return filtered_end;};

void erase (iterator_morining_start iter)
		{ 
			if (filtered_end==iter) filtered_end=container_morning_start.end();
			container_morning_start.erase(iter);
		};

void erase_all()
			{ 
				container_morning_start.erase(container_morning_start.begin(),
																container_morning_start.end());

				filtered_end=container_morning_start.end();
			};

iterator_morining_start insert (iterator_morining_start iter, const escalator_start& obj) 
{
  return (container_morning_start.insert(iter, obj)); 
}

bool empty() const { return (container_morning_start.empty());}
size_type_morining_start size() {return distance(container_morning_start.begin(), filtered_end);};
size_type_morining_start max_size() const {return container_morning_start.size();};

/*
	sorting and filtration items
	sort_all by start time
*/
void sort_all ()
	{
	mst_sorter sort_all_predicate;
	container_morning_start.sort (sort_all_predicate);
	filtered_end=container_morning_start.end();
	}

void only_starting()
{
filter_start_enabled filter_only_starting;

sort_all();

filtered_end=stable_partition(container_morning_start.begin(),
												container_morning_start.end(),
												filter_only_starting
												);


};


/*
reloading of operator=; operator== etc...
*/
contain_morning_start& operator=(const contain_morning_start& contain_morning_start)
	{
		if (this==&contain_morning_start)  return *this;

		container_morning_start=contain_morning_start.container_morning_start;

		filtered_end=container_morning_start.begin();
		advance(filtered_end,
						distance<const_iterator_morining_start>(contain_morning_start.container_morning_start.begin(), contain_morning_start.filtered_end));

		return *this;
	}; 

bool operator == (contain_morning_start cont_morning_start_to_equal)
	{
	if (this==&cont_morning_start_to_equal)  return true;	
	
	if (this->size()!=cont_morning_start_to_equal.size())  return false;	

	if (this->max_size()!=cont_morning_start_to_equal.max_size())  return false;	

	if (this->was_created_in_this_day!=
		cont_morning_start_to_equal.was_created_in_this_day)  return false;	
	
	contain_morning_start::iterator_morining_start iter_morn_start=
				cont_morning_start_to_equal.container_morning_start.begin();

	contain_morning_start::iterator_morining_start this_iter_morn_start=
								container_morning_start.begin();
 
	while(iter_morn_start!=
				cont_morning_start_to_equal.container_morning_start.end())
	{
		if (this_iter_morn_start->get_escalator_id()!=
				iter_morn_start->get_escalator_id()) return false;

		if (this_iter_morn_start->get_escalator_pref_direction()!=
				iter_morn_start->get_escalator_pref_direction()) return false;

		if (this_iter_morn_start->get_escalator_direction()!=
				iter_morn_start->get_escalator_direction()) return false;

		if (this_iter_morn_start->get_start_hour()!=
				iter_morn_start->get_start_hour()) return false;

		if (this_iter_morn_start->get_start_minute()!=
				iter_morn_start->get_start_minute()) return false;

		if (this_iter_morn_start->get_start_mode()!=
				iter_morn_start->get_start_mode()) return false;

		if (this_iter_morn_start->get_start_enabled()!=
				iter_morn_start->get_start_enabled()) return false;
		
		iter_morn_start++;
		this_iter_morn_start++;
	};

	return true;
	}

/*
other metods
*/
void prepare_to_display()
	{
	if(widget==NULL) return;

	PtWidget_t *list_widget = ApGetWidgetPtr(widget, ABN_rwlst_config_escalators);
	
	if (list_widget==NULL) 
		{
			g_system_settings.sys_message(system_settings::ERROR_MSG,
														"Not found list_widget for prepare_to_display");
			return;
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
}



void prepare_morning_start(metro_escalators_container& cont_esc)
{
	make_morning_start maker_of_morning_start(this);

	this->erase_all();

	for_each(cont_esc.begin(),
				cont_esc.end(),
				maker_of_morning_start);

	sort_all();				
};

void execute_morning_start(metro_escalators_container& cont_esc,
											int start_hour,
											int start_minute)
{
	find_morning_start finder_morning_start(&cont_esc,
																	start_hour,
																	start_minute);

	for_each(this->begin(),
				   this->end(),
				   finder_morning_start);

};


};

#endif


