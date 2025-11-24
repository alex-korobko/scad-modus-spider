#ifndef __LINE_H__
#define __LINE_H__

// metropoliten line class
class metro_line
{
private:   
 	typedef vector<int> station_id_container;
 	typedef string stl_string;
 	
	metro_line();
	
   int			id;
   stl_string 	name;
   PgColor_t	color;   
   PtWidget_t* line;

	station_id_container stations_id;     
	
public:
	typedef station_id_container::iterator iterator_stations_id;
	typedef station_id_container::size_type size_of_stations_id;
	typedef stl_string::size_type size_of_stl_string;
	
	enum {DEFAULT_COLOR=Pg_RED};

	contain_morning_start morning_start;
	
	metro_line (
				int new_line_id,
				stl_string new_line_name,
				PgColor_t	new_line_color
				) :
		id (new_line_id),
		name (new_line_name),
		color(new_line_color)
			{
			line=NULL;
			}

/*
get_ and set_  metods for private data members
*/
	PtWidget_t* get_line_widget() {return(line);};
	void set_line_widget( PtWidget_t *new_line_widget) {line=new_line_widget;};	

	PgColor_t get_color() {return(color);};
	void set_color(PgColor_t new_color) {color=new_color;};

	void set_stl_name_string(stl_string new_line_name) {name=new_line_name;};
	const char* get_c_name_string() { return name.c_str(); }
	size_of_stl_string  get_c_name_string_size() { return name.size(); }
	stl_string get_stl_name_string() { return name; }


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
	
	size_of_stations_id size_stations_id() {return (stations_id.size());};
	bool empty_stations_id() {return (stations_id.empty());};

	iterator_stations_id find_stations_id(int station_id) {return (find(stations_id.begin(), stations_id.end(), station_id));};
};

#endif
