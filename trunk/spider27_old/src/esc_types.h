#ifndef __ESCALATOR_TYPES_H__
#define  __ESCALATOR_TYPES_H__


class escalator_signal
{
private:
	int			signal_index;
	string		name;
	string 		hint;	
	PtWidget_t*	led;

	escalator_signal();
public:
	escalator_signal(int new_index, 
								string new_name,
								string new_hint,
								PtWidget_t*	new_led=NULL): 
								signal_index (new_index),
								name(new_name),
								hint(new_hint)
	{led=new_led;};

	~escalator_signal()
		{
			PtArg_t arg;
			if (led!=NULL) 
			{
				PtSetArg(&arg, 
							Pt_ARG_POINTER , 
							NULL, 
							0);

			 PtSetResources( led,
					                    1,
                    						&arg);
			};
		};

/*
get_ and set_ metods for private data members
*/

int  get_index() {return(signal_index);};

string  get_name() {return(name);};
string  get_hint() {return(hint);};

PtWidget_t* get_led() { return (led);};

void set_led(PtWidget_t *new_led) 
	{
	PtArg_t arg;
	if (led!=NULL) 
		{
				PtSetArg(&arg, 
							Pt_ARG_POINTER , 
							NULL, 
							0);

			 PtSetResources( led,
					                    1,
                    						&arg);
 		};

	if (new_led!=NULL) 
		{
				PtSetArg(&arg, 
							Pt_ARG_POINTER , 
							this, 
							0);

			 PtSetResources( new_led,
					                    1,
                    						&arg);
 		};
	};

/*
Other metods
*/

	int create_led(system_settings *sys_sett_obj, PtWidget_t* parent, int column, int row);
};

class escalator_block
{
private:
	struct ltint
		{
		  bool operator() (const int i1, const int i2) const
 			 {
   			 return (i1<i2) ;
  			}
		};


	typedef vector<int>  signals_id_container;
	typedef signals_id_container::iterator iterator_signals_id;
	typedef signals_id_container::size_type size_type_signals_id;


	signals_id_container signals_id;
	
	int id;
	string name;

	PtWidget_t  *panel;

	escalator_block();
public:
	/*
		constructors & destructors
	*/
	escalator_block(  int new_id,
								string new_name,
								PtWidget_t  *new_panel=NULL) :
								id(new_id),
								name(new_name)
								 {panel=new_panel;};

	~escalator_block()
		{
			PtArg_t arg;
	
			if (panel!=NULL) 
			{
				PtSetArg(&arg, 
							Pt_ARG_POINTER , 
							NULL, 
							0);

			 PtSetResources( panel,
					                    1,
                    						&arg);
			}

		};

/*
get_ & set_ metods for private data members
*/

int get_id() {return id;};
string get_name() {return (name); }


void set_panel(PtWidget_t *new_panel) 
	{
	PtArg_t arg;
	
	if (panel!=NULL) 
		{
				PtSetArg(&arg, 
							Pt_ARG_POINTER , 
							NULL, 
							0);

			 PtSetResources( panel,
					                    1,
                    						&arg);
 		}

	if (new_panel!=NULL) 
		{
				PtSetArg(&arg, 
							Pt_ARG_POINTER , 
							this, 
							0);

			 PtSetResources( new_panel,
					                    1,
                    						&arg);
 		}

	panel=new_panel;
	};

/*
wrapper metods for STL container signals
*/
iterator_signals_id signals_id_begin() { return signals_id.begin();};
iterator_signals_id signals_id_end() {return signals_id.end();};

void signals_id_push_back(int new_signal_id) { signals_id.push_back(new_signal_id);};
void signals_id_erase ( iterator_signals_id signal_id_to_removing) { signals_id.erase(signal_id_to_removing);};

bool signals_id_empty() {return signals_id.empty();};
size_type_signals_id signals_id_size () { return signals_id.size();};
};


class metro_escalator_type
{
private:
	struct ltint {
		  bool operator() (const int i1, const int i2) const
 			 {
   			 return (i1<i2) ;
  			}
		};

	int id;
	string		name;

	typedef map<int, escalator_signal, ltint> signals_container;
	typedef signals_container::iterator iterator_signals;
	typedef signals_container::value_type pair_signals;
	typedef signals_container::size_type  size_signals;

	typedef map<int, escalator_block, ltint> blocks_container;
	typedef blocks_container::iterator iterator_blocks;
	typedef blocks_container::value_type pair_blocks;
	typedef blocks_container::size_type  size_blocks;
	


	signals_container  type_signals;
	blocks_container	type_blocks;
	
	metro_escalator_type() ;
	
	class  led_create : public unary_function<int, void>  
	{
	private:
		int row, column;
		PtWidget_t* parent;
		system_settings *sys_set_obj;
		signals_container *signals_of_type;

		led_create();
			
	public:
	led_create (PtWidget_t* new_parent,
						system_settings *new_sys_set_obj,
						signals_container *new_signals_of_type
						): 
									row(0), column(0)
									{
										parent=new_parent; 
										sys_set_obj=new_sys_set_obj;
										signals_of_type=new_signals_of_type;
									};
			
	void operator() (int  signal_id);
				
	};


	class  panel_create : public unary_function<pair_blocks, void>  
	{
	private:
		PtWidget_t* parent;
		system_settings *sys_set_obj;
		signals_container *signals_of_type;		
		panel_create();
			
	public:
		panel_create (PtWidget_t* new_parent,
								system_settings *new_sys_set_obj,
								signals_container *new_signals_of_type
							)
									{
										parent=new_parent; 
										sys_set_obj=new_sys_set_obj;
										signals_of_type=new_signals_of_type;										
									};
			
	void operator() (pair_blocks  block_pair);
				
	};
	
	
public:
	msg_dict_container  type_messages;

/*
constructors & destructors
*/
	metro_escalator_type (int new_id, string new_name): id(new_id),  name(new_name) {};

/*
get_ & set_ metods for private data members
*/
	int get_id () { return (id);}
	string get_name() { return(name);}
	msg_dict_container* get_messages() { return(&type_messages);}

/*
wrapper metods for STL container for blocks
*/
	iterator_blocks blocks_begin() { return type_blocks.begin();}
	iterator_blocks blocks_end() { return type_blocks.end();}
	
	void blocks_erase (iterator_blocks block_to_removing) { type_blocks.erase(block_to_removing);}
	iterator_blocks blocks_insert (iterator_blocks iter_blocks, const pair_blocks& new_block_pair )  { return  type_blocks.insert(iter_blocks, new_block_pair) ; }

	iterator_blocks blocks_find(int	id_block ) { return type_blocks.find(id_block); }
	iterator_blocks blocks_upper_bound(int	id_block ) { return type_blocks.upper_bound(id_block); }
	
	bool blocks_empty() const { return (type_blocks.empty()); }
	size_blocks blocks_size() const { return (type_blocks.size()); } 

/*
wrapper metods for STL container for signals
*/
	iterator_signals signals_begin() { return (type_signals.begin());}
	iterator_signals signals_end() { return (type_signals.end());}
	
	void signals_erase (iterator_signals signal_to_removing) { type_signals.erase(signal_to_removing);}
	iterator_signals signals_insert (iterator_signals iter_signals, const pair_signals& new_signal_pair )  { return  type_signals.insert(iter_signals, new_signal_pair) ; }

	iterator_signals signals_find(int	id_signal ) { return type_signals.find(id_signal); }
	iterator_signals signals_upper_bound(int	id_signal ) { return type_signals.upper_bound(id_signal); }
	
	bool signals_empty() const { return (type_signals.empty()); }
	size_signals signals_size() const { return (type_signals.size()); } 

/*
Other metods
*/
	bool create_panels(PtWidget_t* parent,
									system_settings *sys_set_obj);

};


#endif
