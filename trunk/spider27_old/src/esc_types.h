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

	int create_led(PtWidget_t* parent, int column, int row);
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

	typedef map<int, escalator_signal, ltint> signals_container;

	signals_container signals;

	string name;
	int signals_counter; //autoincrement counter

	PtWidget_t  *panel;

	escalator_block();
public:
	typedef signals_container::iterator iterator_signals;
	typedef signals_container::value_type pair_signals;
	typedef signals_container::size_type size_type_signals;

private:	

	class  led_create : public unary_function<pair_signals, void>  
	{
	private:
		int row, column;
		PtWidget_t* parent;
		led_create();
			
	public:
	explicit led_create (PtWidget_t* new_parent): row(0), column(0){parent=new_parent;};
			
	void operator() (pair_signals sign_pair) 
		{
			sign_pair.second.create_led(parent, row,column);
			if (++row>=10) 
				{
					column++;	
					row = 0;
				};
		};
				
	};


	
public:

	/*
		constructors & destructors
	*/
	escalator_block(string new_name,
								PtWidget_t  *new_panel=NULL) :
								name(new_name),
								signals_counter(0) {panel=new_panel;};
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
string get_name() {return (name); }

int get_signals_counter() { return(signals_counter);}
void set_signals_counter(int new_signals_counter)
	{ signals_counter=new_signals_counter;} 

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
iterator_signals signals_begin() { return(signals.begin());}
iterator_signals signals_end() {return(signals.end());}

bool signals_insert(pair_signals new_signal_pair) { return(signals.insert(new_signal_pair).second);}
void signals_erase ( iterator_signals signal_to_removing) { signals.erase(signal_to_removing);}

iterator_signals signals_find(int id_signal) { return(signals.find(id_signal));}

bool signals_empty() {return(signals.empty());}
size_type_signals signals_size() { return(signals.size());}

/*
Other metods
*/
	int create_panel(PtWidget_t* parent);
};

class metro_escalator_type
{
private:
	struct ltint
		{
		  bool operator() (const int i1, const int i2) const
 			 {
   			 return (i1<i2) ;
  			}
		};
	int blocks_counter; //autoincrement counter
	string		name;

	typedef map<int, escalator_block, ltint> blocks_container;
	blocks_container	type_blocks;

	metro_escalator_type() ;
public:
	typedef blocks_container::iterator iterator_blocks;
	typedef blocks_container::value_type pair_blocks;
	typedef blocks_container::size_type  size_blocks;

	msg_dict_container  block_messages_container;

/*
constructors & destructors
*/
	explicit metro_escalator_type(string new_name): blocks_counter(0), name(new_name) {};

/*
get_ & set_ metods for private data members
*/
	string get_name() { return(name);}
/*
wrapper metods for STL container for blocks
*/
	iterator_blocks blocks_begin() { return (type_blocks.begin());}
	iterator_blocks blocks_end() { return (type_blocks.end());}
	
	void blocks_erase (iterator_blocks block_to_removing) { type_blocks.erase(block_to_removing);}
	bool blocks_insert (pair_blocks new_block_pair )  { return( type_blocks.insert(new_block_pair).second); }

	iterator_blocks blocks_find(int	id_block ) { return(type_blocks.find(id_block)); }
	
	bool blocks_empty() const { return (type_blocks.empty()); }
	size_blocks blocks_size() const { return (type_blocks.size()); } 
};

#endif
