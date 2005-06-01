#ifndef __CONT_METRO_LINES_H__
#define __CONT_METRO_LINES_H__

/*****************************************************************************
Container class for metro lines - 
wrapper of STL container  map
*****************************************************************************/

class metro_lines_container {
private :
struct ltint
	{
		  bool operator() (const int i1, const int i2) const
 			 {
   			 return (i1<i2) ;
  			}
	};
typedef  map <int, metro_line, ltint> metro_lines_map;	
metro_lines_map 	container_metro_lines;

public :
typedef 	metro_lines_map::iterator iterator_metro_lines;	
typedef 	metro_lines_map::value_type pair_metro_lines;	
typedef 	metro_lines_map::size_type size_type_metro_lines;	

private :
iterator_metro_lines current_line;
PtWidget_t		*morning_start_state_indicator;
public :

metro_lines_container(): 
		morning_start_state_indicator(NULL)
		{
			current_line=container_metro_lines.end();
		};

/*
get_ and set_  metods for private data members
*/
iterator_metro_lines get_current_line() {return(current_line);};
void set_current_line (iterator_metro_lines new_current_line) {current_line=new_current_line;};

PtWidget_t* get_morning_start_state_indicator() {return morning_start_state_indicator;}
void set_morning_start_state_indicator( PtWidget_t *new_morning_start_state_indicator);

/*
wrappers of current STL lines`s container metods
*/
iterator_metro_lines begin() {return container_metro_lines.begin();};
iterator_metro_lines end() {return container_metro_lines.end();};

iterator_metro_lines find(const int key) { return container_metro_lines.find(key); };
iterator_metro_lines upper_bound(const int key) { return container_metro_lines.upper_bound(key); };

void erase (iterator_metro_lines iter) { container_metro_lines.erase(iter); };
void erase (iterator_metro_lines iter_beg, 
					iterator_metro_lines iter_end ) { container_metro_lines.erase(iter_beg, iter_end); };

iterator_metro_lines insert (iterator_metro_lines iter, const pair_metro_lines& obj) 
{
  return (container_metro_lines.insert(iter, obj)); 
}

bool empty() const { return container_metro_lines.empty();}
size_type_metro_lines size() const {return container_metro_lines.size();};

void load_line_parameters()
  throw (spider_exception);

void load(string file_name)
  throw (spider_exception);
};


#endif