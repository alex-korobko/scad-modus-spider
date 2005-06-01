#ifndef __CONTAIN_TIMER_COMMAND__
#define __CONTAIN_TIMER_COMMAND__

class timer_command {
private:
command cmd;
int timer_hour;
int timer_minute;
int execution_mode;
bool enabled_excecution;

timer_command();

public:
timer_command(
	command new_cmd,
	int  new_timer_hour,
	int new_timer_minute,
	int new_execution_mode,
	bool new_enabled_excecution
	) : cmd(new_cmd),
	timer_hour(new_timer_hour),
	timer_minute(new_timer_minute),
	execution_mode(new_execution_mode),
	enabled_excecution(new_enabled_excecution)	{};

int get_timer_hour() { return timer_hour; };
int get_timer_minute() { return timer_minute; };
int get_execution_mode() { return execution_mode; };
bool is_enabled_excecution() { return enabled_excecution; };
command get_command();
bool operator == (timer_command timer_command_to_equal);
};

class contain_timer_command {
private :

typedef  list<timer_command> timer_command_container;	
typedef  timer_command_container::iterator iterator_timer_command;	
typedef  timer_command_container::const_iterator const_iterator_timer_command;	
typedef  timer_command_container::size_type size_type_timer_command;	

timer_command_container container_timer_command;
bool was_created_in_this_day, only_execution_mode_enabled_now;
PtWidget_t *widget;
iterator_timer_command filtered_end;

/*
functor class for partial sort to selection only morning starts with 
start_enabled==true
*/

class filter_timer_command_enabled : public unary_function<timer_command, bool>{
public:
	bool operator () (timer_command timer_cmd) const {
		return timer_cmd.is_enabled_excecution();
	};

};

class timer_cmd_sorter : 
 public binary_function <const  timer_command,
	 									const  timer_command,
 										bool>{
public:
bool operator () (timer_command first_timer_command,
						 timer_command second_timer_command) const;
	
};

public:
/*
constructor
*/
contain_timer_command():
container_timer_command(),
was_created_in_this_day(false),
only_execution_mode_enabled_now(false),
widget(NULL),
filtered_end (container_timer_command.end())
{};

/*
copy constructor
*/
contain_timer_command(const contain_timer_command& cont_timer_command);
/*
get_ and set_ metods 
*/
bool get_was_created_in_this_day() {return was_created_in_this_day;};
void set_was_created_in_this_day(bool new_was_created_in_this_day)
				{was_created_in_this_day=new_was_created_in_this_day;};

bool only_execution_mode_enabled() {return only_execution_mode_enabled_now;}
				
PtWidget_t* get_widget() {return widget;};
void set_widget(PtWidget_t *new_widget);

/*
interface to useful metods of stl container
*/
iterator_timer_command begin() {return container_timer_command.begin();};
iterator_timer_command end() {return filtered_end;};

void erase (iterator_timer_command iter){ 
    if (filtered_end==iter) filtered_end=container_timer_command.end();
    container_timer_command.erase(iter);
};

void erase_all(){ 
				container_timer_command.erase(container_timer_command.begin(),
																container_timer_command.end());

				filtered_end=container_timer_command.end();
				only_execution_mode_enabled_now=false;
};

iterator_timer_command insert (iterator_timer_command iter, 
	const timer_command& obj) {
  return (container_timer_command.insert(iter, obj)); 
}

bool empty() const { return (container_timer_command.empty());}
size_type_timer_command size() {return distance(container_timer_command.begin(), filtered_end);};
size_type_timer_command max_size() const {return container_timer_command.size();};

/*
	sorting and filtration items
	sort_all by start time
*/
void sort_all (){
	only_execution_mode_enabled_now=false;
	timer_cmd_sorter sort_all_predicate;
	container_timer_command.sort (sort_all_predicate);
	filtered_end=container_timer_command.end();
	}

void only_enabled(){
filter_timer_command_enabled filter_only_enabled;
sort_all();
filtered_end=stable_partition(container_timer_command.begin(),
												container_timer_command.end(),
												filter_only_enabled
												);

only_execution_mode_enabled_now=true;
};


/*
reloading of operator=; operator== etc...
*/
contain_timer_command& operator=(const contain_timer_command& contain_timer_command);
bool operator == (contain_timer_command cont_timer_command_to_equal);

/*
other metods
*/
void prepare_to_display() throw (spider_exception);
};

#endif


