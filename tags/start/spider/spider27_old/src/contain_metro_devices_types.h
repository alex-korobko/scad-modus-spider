#ifndef __CONT_DEVICES_TYPES__
#define __CONT_DEVICES_TYPES__


/*****************************************************************************
Container class for metro devices types - 
wrapper of STL container  map
*****************************************************************************/

class metro_devices_types_container {
private :
struct ltint{
		  bool operator() (const int i1, const int i2) const{
   			 return (i1<i2) ;
  			}
	};

typedef  map<int, metro_device_type, ltint> devices_type_container;	
typedef  devices_type_container::iterator iterator_devices_types;	
typedef  devices_type_container::value_type pair_devices_types;	
typedef devices_type_container::size_type size_devices_types;	

devices_type_container	container_devices_types;

iterator_devices_types current_devices_type;
metro_device_type::iterator_blocks current_device_type_block;

public :

iterator_devices_types begin() {return container_devices_types.begin();};
iterator_devices_types end() {return container_devices_types.end();};

iterator_devices_types find(const int key) { return container_devices_types.find(key); };
iterator_devices_types upper_bound(const int key) { return container_devices_types.upper_bound(key); };

void erase (iterator_devices_types iter) { container_devices_types.erase(iter); };
void erase (iterator_devices_types iter_beg,
					iterator_devices_types iter_end) { container_devices_types.erase(iter_beg, iter_end); };

iterator_devices_types insert (iterator_devices_types iter, const pair_devices_types& obj) 
{
  return (container_devices_types.insert(iter, obj)); 
}

bool empty() const { return (container_devices_types.empty());}
size_devices_types size() const {return (container_devices_types.size());};

/*
load parameters from config file
*/
void load_message_parameters (
              msg_types_container *msg_types_cont) throw(spider_exception);
void load_signal_parameters ()  throw(spider_exception);
void load_block_parameters ()  throw(spider_exception);
void load_type_parameters ()  throw(spider_exception);
void load (string file_name,
				msg_types_container *msg_types_cont)  throw(spider_exception);
};

#endif