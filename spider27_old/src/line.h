#ifndef __LINE_H__
#define __LINE_H__

// metropoliten line class
class metro_line
{
private:   
 	typedef vector<int> station_id_container;

	metro_line();
	
   int			id;
   string 	name;
   PgColor_t	color;   
   PtWidget_t* line;

	station_id_container stations_id;     
	
/*
functor class for sorting line`s stations by data member order
*/
	class stations_sorter:public binary_function<int, int, bool>
	{
	public:
	bool operator ()(const int& first_station_id, const int& second_station_id)
	  { 
	  metro_stations_container::iterator_metro_stations iter_first_station, iter_second_station;

	 iter_first_station=g_stations.find(first_station_id);
	  iter_second_station=g_stations.find(second_station_id);

	  if (  iter_first_station==iter_second_station ||
	  		iter_first_station==g_stations.end() ||
	  		iter_second_station==g_stations.end()
	  		) 
		  {
		  return (false);
		  } else  return(iter_first_station->second.get_order() < iter_second_station->second.get_order());
		  
	    }; //operator ()(int) defenition
	} sort_stations_predicate;	

	
public:
	typedef station_id_container::iterator iterator_stations_id;

	enum {DEFAULT_COLOR=Pg_RED};
	
	metro_line (
				int new_line_id,
				string new_line_name,
				PgColor_t	new_line_color
				) :
		id (new_line_id),
		name (new_line_name),
		color(new_line_color)
			{
			line=NULL;
			}

	metro_line::~metro_line()
		{
			if (line) {delete(line);}; //I don`t shure...
		}

/*
get_ and set_  metods for private data members
*/
	PtWidget_t* get_line_widget() {return(line);};
	void set_line_widget( PtWidget_t *new_line_widget) {line=new_line_widget;};	

	PgColor_t get_color() {return(color);};
	void set_color(PgColor_t new_color) {color=new_color;};

	void set_stl_name_string(string new_line_name) {name=new_line_name;};
	const char* get_c_name_string() { return name.c_str(); }
	int  get_c_name_string_size() { return name.size(); }
	string get_stl_name_string() { return name; }


	void set_id (int new_line_id) 	 {id=new_line_id;};
	int get_id() {return(id);};
/*
wrappers of current STL stations`s id`s  container metods
*/
   	iterator_stations_id begin_stations_id()  {return(stations_id.begin());};
    	iterator_stations_id end_stations_id()  {return(stations_id.end());};
   	
	void push_back_stations_id(int id_station)  {stations_id.push_back(id_station);};
	void remove_stations_id(iterator_stations_id iter)  { stations_id.erase(iter);};
	void remove_stations_id(iterator_stations_id beg_iter, iterator_stations_id end_iter)  {stations_id.erase(beg_iter, end_iter);};
	
	int size_stations_id() {return (stations_id.size());};
	bool empty_stations_id() {return (stations_id.empty());};
	
	void sort_stations_id() {sort(stations_id.begin(), stations_id.end(), sort_stations_predicate);}; 
	iterator_stations_id find_stations_id(int station_id) {return (find(stations_id.begin(), stations_id.end(), station_id));};
};

#endif
