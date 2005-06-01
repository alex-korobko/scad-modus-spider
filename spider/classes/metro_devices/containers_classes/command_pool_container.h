#ifndef __CMD_POOL_CONT__
#define __CMD_POOL_CONT__


/*****************************************************************************
*	Container class for command pool - wrapper of STL container list 			*
*****************************************************************************/

class cmd_pool_container {
private :
typedef 	list<command> cmd_pool_list;
cmd_pool_list cmd_pool_cont;

PtWidget_t *wnd; 
PtGenListItem_t *first;
static cmd_pool_container* cmd_pool_container_instance;

public :
typedef  cmd_pool_list::iterator iterator;

cmd_pool_container();
virtual ~cmd_pool_container();

/*
get_ and set_  metods for private data members
*/

PtWidget_t* get_widget() {return (wnd);}
void set_widget(PtWidget_t *new_wnd);

/*
Interface for useful metods to STL container list
*/
iterator begin() {return (cmd_pool_cont.begin());};
iterator end() {return (cmd_pool_cont.end());};

void push_back (command cmd);

void clear() { cmd_pool_cont.clear();};
void erase(const iterator iter) { cmd_pool_cont.erase(iter);};
bool empty() const { return (cmd_pool_cont.empty());}
int size() const{return (cmd_pool_cont.size());};

/*
preparing to display
*/
void prepare_to_display() throw (spider_exception);

//static metods
static cmd_pool_container* get_instance();
};

#endif
