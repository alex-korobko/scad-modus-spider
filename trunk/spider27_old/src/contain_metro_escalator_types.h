#ifndef __CONT_ESCALATOR_TYPES_H_
#define __CONT_ESCALATOR_TYPES_H_


/*****************************************************************************
Container class for metro escalators types - 
wrapper of STL container  map
*****************************************************************************/

class esc_types_container {
private :
struct ltint
	{
		  bool operator() (const int i1, const int i2) const
 			 {
   			 return (i1<i2) ;
  			}
	};

typedef  map<int, metro_escalator_type, ltint> metro_escalator_type_container;	

metro_escalator_type_container	container_esc_types;

public :
typedef  metro_escalator_type_container::iterator iterator_esc_types;	
typedef  metro_escalator_type_container::value_type pair_esc_types;	
typedef  metro_escalator_type_container::size_type size_esc_types;	

iterator_esc_types begin() {return container_esc_types.begin();};
iterator_esc_types end() {return container_esc_types.end();};

iterator_esc_types find(const int key) { return container_esc_types.find(key); };
void erase (iterator_esc_types iter) { container_esc_types.erase(iter); };

iterator_esc_types insert (iterator_esc_types iter, const pair_esc_types& obj) 
{
  return (container_esc_types.insert(iter, obj)); 
}

bool empty() const { return (container_esc_types.empty());}
size_esc_types size() const {return (container_esc_types.size());};
};

#endif