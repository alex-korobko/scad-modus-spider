#ifndef __CONT_METRO_ESCALATORS_H__
#define  __CONT_METRO_ESCALATORS_H__

/*****************************************************************************
Container class for metro escalators - 
wrapper of STL container  map
*****************************************************************************/

class metro_escalators_container {
private :
struct ltint
	{
		  bool operator() (const int i1, const int i2) const
 			 {
   			 return (i1<i2) ;
  			}
	};

typedef  map <int, metro_escalator, ltint> metro_escalators_map;	
metro_escalators_map	container_metro_escalators;

public :
typedef metro_escalators_map::iterator  iterator_metro_escalators;	
typedef metro_escalators_map::value_type pair_metro_escalators;	

private :
iterator_metro_escalators current_escalator;
iterator_metro_escalators current_escalator_in_directions;

int 	escalators_count;

public :

metro_escalators_container (): 
		current_escalator(container_metro_escalators.end()), 
		current_escalator_in_directions(container_metro_escalators.end()),
		escalators_count(0)
	{ };

/*
get_ and set_  metods for private data members
*/

iterator_metro_escalators get_current_escalator() {return (current_escalator);};
void set_current_escalator(iterator_metro_escalators new_current_escalator) {current_escalator=new_current_escalator;};

iterator_metro_escalators get_current_escalator_in_directions() {return (current_escalator_in_directions);};
void set_current_escalator_in_directions(iterator_metro_escalators new_current_escalator_in_directions) {current_escalator_in_directions=new_current_escalator_in_directions;};

int get_escalators_autoincrement() {return (escalators_count);};
void set_escalators_autoincrement(int new_escalators_count) {escalators_count=new_escalators_count;};

/*
wrappers of current STL escalator`s container metods
*/

iterator_metro_escalators begin() {return (container_metro_escalators.begin());};
iterator_metro_escalators end() {return (container_metro_escalators.end());};

iterator_metro_escalators find(const int key) { return (container_metro_escalators.find(key)); };
iterator_metro_escalators upper_bound(const int key) { return container_metro_escalators.upper_bound(key); };

void erase (iterator_metro_escalators iter) { container_metro_escalators.erase(iter); };
iterator_metro_escalators insert (iterator_metro_escalators iter, const pair_metro_escalators& obj) 
{
  return (container_metro_escalators.insert(iter, obj)); 
}

bool empty() const { return (container_metro_escalators.empty());}
int size() const {return (container_metro_escalators.size());};
/*
Other functions
*/
bool save_directions (string file_name)
{
return (true);
};

bool load_directions (string file_name)
{
return (true);
};

bool load_escalators (string file_name)
{
return (true);
};

}; // class metro_escalators_container

#endif