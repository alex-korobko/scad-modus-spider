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

private :
int lines_count;
iterator_metro_lines current_line;

public :

metro_lines_container(): lines_count(0){current_line=container_metro_lines.end();};

/*
get_ and set_  metods for private data members
*/
int get_lines_autoincrement() {return (lines_count);}
void set_lines_autoincrement(int new_lines_counter) {lines_count=new_lines_counter;}

iterator_metro_lines get_current_line() {return(current_line);};
void set_current_line (iterator_metro_lines new_current_line) {current_line=new_current_line;};

/*
wrappers of current STL lines`s container metods
*/
iterator_metro_lines begin() {return container_metro_lines.begin();};
iterator_metro_lines end() {return container_metro_lines.end();};

iterator_metro_lines find(const int key) { return container_metro_lines.find(key); };
iterator_metro_lines upper_bound(const int key) { return container_metro_lines.upper_bound(key); };

void erase (iterator_metro_lines iter) { container_metro_lines.erase(iter); };
iterator_metro_lines insert (iterator_metro_lines iter, const pair_metro_lines& obj) 
{
  return (container_metro_lines.insert(iter, obj)); 
}

bool empty() const { return container_metro_lines.empty();}
int size() const {return container_metro_lines.size();};

};


#endif