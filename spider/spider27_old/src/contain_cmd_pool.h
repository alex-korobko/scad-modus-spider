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

public :
typedef  cmd_pool_list::iterator cmd_pool_iterator;

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
cmd_pool_iterator begin() {return (cmd_pool_cont.begin());};
cmd_pool_iterator end() {return (cmd_pool_cont.end());};

void push_back (command& cmd) { cmd_pool_cont.push_back(cmd);};

void erase(const cmd_pool_iterator iter) { cmd_pool_cont.erase(iter);};
bool empty() const { return (cmd_pool_cont.empty());}
int size() const{return (cmd_pool_cont.size());};

/*
preparing to display
*/
void prepare_to_display() throw (spider_exception);
};

#endif