#ifndef __STATION_H__
#define __STATION_H__

/*
 metropoliten station class
*/

class metro_station
{
private:
 	typedef vector<int> escalators_id_container;
 	typedef string stl_string;
 	
   	metro_station();

   	PtWidget_t *wnd;
   	int			id;
	int			line_id;
	int			xcoord, ycoord;
   	stl_string			name;
   	stl_string			wav_file_name;
   	
	escalators_id_container escalators_id;     
public:
	typedef escalators_id_container::iterator iterator_escalators_id;
	typedef escalators_id_container::size_type size_type_escalators_id;
	typedef stl_string::size_type size_of_stl_string;

	metro_station(int new_id_station,
						  int new_id_line,
						  int new_x_coord,
						  int new_y_coord,
						  stl_string new_name,
						  stl_string new_wav_file_name
  						  ):
	id(new_id_station),
	line_id(new_id_line),
	xcoord(new_x_coord),
	ycoord(new_y_coord),
	name(new_name),
	wav_file_name(new_wav_file_name)
	{
		wnd = NULL;
	}


	~metro_station()
	{
	}

   
   	int create_wnd(PtWidget_t* parent, int x, int y, int width=0);

/*
get_ and set_  metods for private data members
*/
	int 	get_id() { return id; }
	void set_id(int new_station_id) {id=new_station_id;};
	
	PtWidget_t* 	get_wnd() { return wnd; }

	void set_stl_name_string(stl_string new_station_name) {name=new_station_name;};
	const char* get_c_name_string() { return name.c_str(); }
	stl_string get_stl_name_string() { return name; }
	size_of_stl_string  get_c_name_string_size() { return name.size(); }

	stl_string get_wav_file_name() { return wav_file_name; }

	int get_line_id() {return(line_id);};
	void set_line_id(int new_line_id) { line_id=new_line_id;};

	int get_x_coord() {return(xcoord);};
	void set_x_coord(int new_xcoord) { xcoord=new_xcoord;};

	int get_y_coord() {return(ycoord);};
	void set_y_coord (int new_ycoord) { ycoord=new_ycoord;};

/*
wrappers of current STL escalators`s iterators`s  container metods
*/
  	iterator_escalators_id begin_escalators_id()  {return(escalators_id.begin());};
    	iterator_escalators_id end_escalators_id()  {return(escalators_id.end());};
   	
	void push_back_escalators_id(int new_esc_id)  {escalators_id.push_back(new_esc_id);};
	
	void remove_escalators_id(iterator_escalators_id iter)  {escalators_id.erase(iter);};
	void remove_escalators_id(iterator_escalators_id beg_iter, iterator_escalators_id end_iter)  {escalators_id.erase(beg_iter, end_iter);};
	
	size_type_escalators_id size_escalators_id() {return (escalators_id.size());};
	bool empty_escalators_id() {return (escalators_id.empty());};
	
	iterator_escalators_id find_escalators_id(int escalator_id) {return (find(escalators_id.begin(), escalators_id.end(), escalator_id));};	

}; //class metro station defenition

#endif
