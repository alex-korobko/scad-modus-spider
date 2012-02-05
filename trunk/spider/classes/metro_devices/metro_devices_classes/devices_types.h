#ifndef __DEVICES_TYPES__
#define  __DEVICES_TYPES__

class device_input_circut_for_message {
  private:
	string		name;
	string		hint;
  public:
  device_input_circut_for_message (
       string  new_name,
       string  new_hint ) : name(new_name), hint(new_hint){};

  string get_name() {return name;};
  string get_hint() {return hint;};
};

class device_data_unit {
private:
    int device_data_unit_id;
	int	 device_data_unit_index;
	string		name;
	string		hint;
    bool visibility;
	PtWidget_t*	widget;

public:

	enum {DATA_UNIT_SIGNAL,
	            DATA_UNIT_PARAMETER};

	device_data_unit (int new_id,
	                            int new_index, 
								string new_name,
								string new_hint,
                                bool new_visibility): 
								device_data_unit_id(new_id),
								device_data_unit_index (new_index),
								name(new_name),
								hint(new_hint),
                                visibility(new_visibility)
	{widget=NULL;};

	virtual ~device_data_unit(){};

/*
get_ and set_ metods for private data members
*/
int  get_id() {return(device_data_unit_id);};
int  get_index() {return(device_data_unit_index);};
string  get_name() {return(name);};
string  get_hint() {return(hint);};
bool get_visibility() {return (visibility);};

PtWidget_t* get_widget() { return (widget);};
void set_widget(PtWidget_t*new_widget) { widget=new_widget;};
/*
virtual metods
*/
// virtual int data_unit_type_description()=0;
virtual void create_widget(ApDBase_t *widgets_dbase,
                                        PtWidget_t* parent,
                                        data_block *data,
                                        int column,
                                        int row) throw (spider_exception)=0;
virtual void update_widget(data_block *data_blk,
                                         system_settings_spider *sys_sett,
                                         bool online) throw (spider_exception)=0;
};

class device_signal  : public device_data_unit{
public:
       device_signal(
                          int new_id,
	                      int new_index, 
						  string new_name,
						  string new_hint,
                          bool new_visibility) :
			device_data_unit ( new_id,
	                            new_index, 
								new_name,
								new_hint,
                                new_visibility) {};


	virtual ~device_signal(){};

/*
virtual metods
*/
// virtual int data_unit_type_description();
virtual void create_widget(ApDBase_t *widgets_dbase,
                                         PtWidget_t* parent,
                                         data_block *data,
                                         int column,
                                         int row) throw (spider_exception);
virtual void update_widget(data_block *data_blk,
                                         system_settings_spider *sys_sett,
                                         bool online_and_no_errors) throw (spider_exception);

/*
static metods
*/
static PhImage_t* signal_image(data_block *data_blk,
                                                 system_settings_spider *sys_sett,
                                                 int signal_index);
};

class device_parameter : public device_data_unit{
public:
     device_parameter(
                                int new_id,
	                            int new_index, 
								string new_name,
                                string new_hint,
                                bool new_visibility):
             device_data_unit (new_id,
                                        new_index,
                                        new_name, 
                                        new_hint,
                                        new_visibility) {};


	virtual ~device_parameter(){};
/*
virtual metods
*/
//virtual int data_unit_type_description();
virtual void create_widget(ApDBase_t *widgets_dbase,
                                        PtWidget_t* parent,
                                        data_block *data,
                                        int column,
                                        int row)  throw (spider_exception);
virtual void update_widget (data_block *data_blk,
                                          system_settings_spider *sys_sett,
                                          bool online_and_no_errors) throw (spider_exception);

/*
static metods
*/
static bool value_is_norma(data_block *data, int parameter_index);
static string parameter_text_of_value(data_block *data, 
                                                               system_settings_spider *sys_sett,
                                                               device_parameter *dev_par);
};


class device_block{
private:
	typedef vector<int>  ids_container;
	typedef ids_container::iterator iterator_ids;
	typedef ids_container::size_type size_type_ids;


	ids_container data_units_id;

	int id;
	string name;

	PtWidget_t  *panel;

	device_block();
public:
	/*
		constructors & destructors
	*/
	device_block(  int new_id,
								string new_name) :
								id(new_id),
								name(new_name)
								 {panel=NULL;};

	~device_block(){};

/*
get_ & set_ metods for private data members
*/

int get_id() {return id;};
string get_name() {return (name); }

void set_panel(PtWidget_t *new_panel){panel=new_panel;};

/*
wrapper metods for STL container signals
*/
iterator_ids id_begin() { return data_units_id.begin();};
iterator_ids id_end() {return data_units_id.end();};

void id_push_back(int new_id) { data_units_id.push_back(new_id);};
void id_erase ( iterator_ids id_to_removing) { data_units_id.erase(id_to_removing);};

bool id_empty() {return data_units_id.empty();};
size_type_ids id_size () { return data_units_id.size();};
};


class metro_device_type{
private:
	int id;
	string		name;

    struct ltint {
		  bool operator() (const int i1, const int i2) const{
   			 return (i1<i2) ;
  			}
	};

	typedef hash_map<int, device_data_unit*> data_unit_container;
	typedef data_unit_container::iterator data_unit_iterator;
	typedef data_unit_container::value_type data_unit_pair;
	typedef data_unit_container::size_type  data_unit_size;

	typedef hash_map<int, device_block> blocks_container;
	typedef blocks_container::iterator iterator_blocks;
	typedef blocks_container::value_type pair_blocks;
	typedef blocks_container::size_type  size_blocks;

	typedef vector<device_input_circut_for_message> circuts_for_message_container;
	typedef circuts_for_message_container::iterator iterator_circuts_for_message;
	typedef circuts_for_message_container::value_type value_type_circuts_for_message;
	typedef circuts_for_message_container::size_type  size_circuts_for_message;

	data_unit_container  type_data_units;
	blocks_container	type_blocks;
    circuts_for_message_container circuts_for_messages;

	metro_device_type() ;
	
	class  widget_create : public unary_function<int, void>  {
	private:
		int row, column;
		PtWidget_t* parent;
	    ApDBase_t *widgets_dbase;
		data_unit_container *data_units_of_type;
		data_block *data_blk;

		widget_create();
			
	public:
	widget_create (PtWidget_t* new_parent,
							ApDBase_t *new_widgets_dbase,
						data_unit_container *new_data_units_of_type,
						data_block* new_data_block
						): row(0), column(0){
										parent=new_parent; 
										widgets_dbase=new_widgets_dbase;
										data_units_of_type=new_data_units_of_type;
										data_blk=new_data_block;
									};
			
	void operator() (int  data_unit_id);
	};

class  panel_create : public unary_function<pair_blocks, void>  {
	private:
		PtWidget_t* parent;
		ApDBase_t *widgets_dbase;
		data_unit_container *data_units_of_type;
		data_block *data_blk;
		
		panel_create();
			
	public:
		panel_create (PtWidget_t* new_parent,
								ApDBase_t *new_widgets_dbase,
								data_block *new_data_blk,
 						        data_unit_container *new_data_units_of_type )	{
										parent=new_parent; 
										widgets_dbase=new_widgets_dbase;
										data_blk=new_data_blk;
										data_units_of_type=new_data_units_of_type;
									};
			
	void operator() (pair_blocks  block_pair);
	};
	
public:
	msg_dict_container  type_messages;

	//devices types 
	enum {DEVICE_TYPE_ESCALATOR=1,
				DEVICE_TYPE_SHAVR,
                DEVICE_TYPE_DOOR,
                DEVICE_TYPE_UDKU
				};

/*
constructors & destructors
*/
	metro_device_type (int new_id, string new_name): id(new_id),  name(new_name) {};
	virtual ~metro_device_type ();

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
wrapper metods for STL container for data units
*/
	data_unit_iterator data_units_begin() { return (type_data_units.begin());}
	data_unit_iterator data_units_end() { return (type_data_units.end());}
	
	
	void data_units_erase (data_unit_iterator data_units_to_removing) { 
	                                                 delete(data_units_to_removing->second);
	                                                 type_data_units.erase(data_units_to_removing);}
	data_unit_iterator data_units_insert (data_unit_iterator iter_data_unit, 
														    const data_unit_pair new_data_unit) 
              { return  type_data_units.insert(iter_data_unit, new_data_unit) ; }

	data_unit_iterator data_units_find(int id_data_unit ) { return type_data_units.find(id_data_unit); }
	data_unit_iterator data_units_upper_bound(int	id_data_unit ) { return type_data_units.upper_bound(id_data_unit); }
	
	bool data_units_empty() const { return (type_data_units.empty()); }
	data_unit_size data_units_size() const { return (type_data_units.size()); } 


/*
wrapper metods for STL circuts_for_message_container circuts_for_messages;
*/
    iterator_circuts_for_message circuts_begin() { return circuts_for_messages.begin();}
    iterator_circuts_for_message circuts_end() { return circuts_for_messages.end();}
	
	void circuts_erase (iterator_circuts_for_message circut_to_removing) { circuts_for_messages.erase(circut_to_removing);}
	iterator_circuts_for_message circuts_insert (iterator_circuts_for_message iter_circuts, const value_type_circuts_for_message& new_circut_for_message )  { return  circuts_for_messages.insert(iter_circuts, new_circut_for_message); };

    device_input_circut_for_message& at_circut (const size_circuts_for_message index_of_circut) { return circuts_for_messages [index_of_circut];};

	bool circuts_empty() const { return (circuts_for_messages.empty()); }
	size_circuts_for_message circuts_size() const { return (circuts_for_messages.size()); } 

/*
other metods
*/
	bool create_panels(PtWidget_t* parent,
									ApDBase_t *widgets_dbase,
									data_block *data_blk);

	bool update_widgets(system_settings_spider *sys_sett,
									data_block *data,
									bool device_online);

};


#endif
