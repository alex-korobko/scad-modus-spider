#ifndef __METRO_LINES_CONTAINER__
#define __METRO_LINES_CONTAINER__

/*****************************************************************************
Container class for metro lines - 
wrapper of STL container  map
*****************************************************************************/

class metro_lines_container {
public:
typedef  hash_map <int, metro_line > contain_metro_lines;	
typedef 	contain_metro_lines::iterator iterator;	
typedef 	contain_metro_lines::value_type value_type;	
typedef 	contain_metro_lines::size_type size_type;	

private :
contain_metro_lines 	container_metro_lines;
iterator current_line;

//static members
static metro_lines_container* metro_lines_container_instance; 
public :
metro_lines_container(): 
 container_metro_lines(),
		current_line(container_metro_lines.end())
		{};


//get_ and set_  metods for private data members
iterator get_current_line() {return(current_line);};
void set_current_line (iterator new_current_line) {current_line=new_current_line;};

//wrappers of current STL lines`s container metods
iterator begin() {return container_metro_lines.begin();};
iterator end() {return container_metro_lines.end();};

iterator find(const int key) { return container_metro_lines.find(key); };
iterator upper_bound(const int key) { return container_metro_lines.upper_bound(key); };

void erase (iterator iter) { container_metro_lines.erase(iter); };
void erase (iterator iter_beg, 
   					iterator iter_end )
 { container_metro_lines.erase(iter_beg, iter_end); };

iterator insert (const value_type& obj) {
   return (container_metro_lines.insert(obj).first); 
}

bool empty() const { return container_metro_lines.empty();}
size_type size() const {return container_metro_lines.size();};

void load_line_parameters()
  throw (spider_exception);

void load(string file_name)
  throw (spider_exception);

//static metods
static metro_lines_container* get_instance(); 
};


#endif
