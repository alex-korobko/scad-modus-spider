#ifndef __CONT_DEVICES_TYPES__
#define __CONT_DEVICES_TYPES__


/*****************************************************************************
Container class for metro devices types - 
wrapper of STL container  map
*****************************************************************************/

class metro_devices_types_container {
private :
enum  data_unit_type {SIGNAL=0, PARAMETER};

typedef  hash_map<int, metro_device_type*> devices_type_container;
typedef  devices_type_container::iterator iterator;	
typedef  devices_type_container::value_type value_type;	
typedef devices_type_container::size_type size_type;	

devices_type_container	container_devices_types;

iterator current_devices_type;
metro_device_type::iterator_blocks current_device_type_block;

metro_devices_types_container(){};

//static members
static metro_devices_types_container* metro_devices_types_container_instance; 
public :

iterator begin() {return container_devices_types.begin();};
iterator end() {return container_devices_types.end();};

iterator find(const int key) { return container_devices_types.find(key); };
iterator upper_bound(const int key) { return container_devices_types.upper_bound(key); };

void erase (iterator iter) { container_devices_types.erase(iter); };
void erase (iterator iter_beg,
					iterator iter_end) { container_devices_types.erase(iter_beg, iter_end); };

iterator insert (iterator iter, const value_type& obj) {
  return (container_devices_types.insert(iter, obj)); 
}

bool empty() const { return (container_devices_types.empty());}
size_type size() const {return (container_devices_types.size());};


//load parameters from config file
void load_message_parameters () throw(spider_exception);
void load_input_circut_for_message_parameters () throw(spider_exception);
void load_data_unit_parameters (data_unit_type unit_type)  throw(spider_exception);
void load_block_parameters ()  throw(spider_exception);
void load_type_parameters ()  throw(spider_exception);
void load_device_type_file (string file_name)  throw(spider_exception);
void load (string file_name)  throw(spider_exception);

//static functions
static metro_devices_types_container* get_instance();
};

#endif
