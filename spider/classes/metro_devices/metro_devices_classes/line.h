#ifndef __METRO_LINE__
#define __METRO_LINE__

// metropoliten line class
class metro_line {
public:
   typedef set<int> stations_ids_container;
   typedef stations_ids_container::iterator stations_ids_interator;
   typedef stations_ids_container::size_type stations_ids_size_type;

private:
   int			id;
   string 	name;
   PgColor_t	 color;   
//   PtWidget_t *line_widget; //line widget not used in program, so, it present only in create_widger metod

   stations_ids_container stations_ids;
   metro_line();

public:
	enum {
            DEFAULT_COLOR=Pg_RED,
            LINE_WIDTH_IN_PIXELS=10};
	
	metro_line (
				int new_line_id,
				string new_line_name,
				PgColor_t	 new_line_color
				) :
		id (new_line_id),
		name (new_line_name),
		color(new_line_color){}

void create_widget(PtWidget_t* parent) throw (spider_exception);

//get_ and set_  metods for private data members
	PgColor_t get_color() {return(color);};
	void set_color(PgColor_t new_color) {color=new_color;};

     void set_line_name(string new_line_name) {name=new_line_name;};
     string get_line_name() { return name; }

     void set_id (int new_line_id) 	 {id=new_line_id;};
     int get_id() {return(id);};

//wrappers of current STL stations`s id`s  container metods
     stations_ids_interator begin_stations_id()  {return(stations_ids.begin());};
     stations_ids_interator end_stations_id()  {return(stations_ids.end());};
   	
	void insert_stations_id(int id_station)  {stations_ids.insert(id_station);};
	void erase_stations_id(stations_ids_interator iter)  { stations_ids.erase(iter);};
	void erase_stations_id(stations_ids_interator beg_iter,
                                    stations_ids_interator end_iter)
  {stations_ids.erase(beg_iter, end_iter);};
	
stations_ids_size_type size_stations_id() {return (stations_ids.size());};
	bool empty_stations_id() {return (stations_ids.empty());};

stations_ids_interator find_stations_id(int station_id) {return stations_ids.find(station_id);};
};

#endif
