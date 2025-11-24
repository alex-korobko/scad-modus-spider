#ifndef __CONT_METRO_STATIONS_H__
#define __CONT_METRO_STATIONS_H__

/*****************************************************************************
Container class for metro stations - 
wrapper of STL container  map
*****************************************************************************/

class metro_stations_container {
private :
struct ltint
	{
		  bool operator() (const int i1, const int i2) const
 			 {
   			 return (i1<i2) ;
  			}
	};
typedef  map <int,  metro_station, ltint> metro_stations_map;	
metro_stations_map	container_metro_stations;
typedef 	metro_stations_map::iterator iterator_metro_stations;	
typedef 	metro_stations_map::value_type pair_metro_stations;	
typedef 	metro_stations_map::size_type size_type_metro_stations;	

private :
int stations_count;
iterator_metro_stations current_station;

public :
metro_stations_container () : stations_count(0){current_station=container_metro_stations.end();};

/*
get_ and set_  metods for private data members
*/
int get_stations_autoincrement() {return (stations_count);}
void set_stations_autoincrement(int new_stations_count) {stations_count=new_stations_count;}

iterator_metro_stations get_current_station() {return(current_station);};
void set_current_station(iterator_metro_stations new_current_station) { current_station=new_current_station;};

/*
wrappers of current STL stations`s container metods
*/
iterator_metro_stations begin() {return container_metro_stations.begin();};
iterator_metro_stations end() {return container_metro_stations.end();};

iterator_metro_stations find(const int key) { return container_metro_stations.find(key); };
iterator_metro_stations upper_bound(const int key) { return container_metro_stations.upper_bound(key); };

void erase (iterator_metro_stations iter) { container_metro_stations.erase(iter); };
void erase (iterator_metro_stations iter_beg,
					iterator_metro_stations iter_end) { container_metro_stations.erase(iter_beg, iter_end); };

iterator_metro_stations insert (iterator_metro_stations iter, const pair_metro_stations& obj) {
  return (container_metro_stations.insert(iter,obj)); 
}

bool empty() const { return container_metro_stations.empty();}
size_type_metro_stations size() const {return container_metro_stations.size();};

void 
	load_station_parameters 
	( metro_lines_container* metro_lines_obj)  throw (spider_exception);

void load ( string file_name,
        metro_lines_container *metro_lines_cont) throw (spider_exception);

};

#endif