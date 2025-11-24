#ifndef __MSG_TYPES_CONT_H__
#define __MSG_TYPES_CONT_H__

class msg_type
{
private:
int id_msg_type;
string descr_msg_type;
PgColor_t color_msg_type;

msg_type();

public:
msg_type (int new_id_msg_type,
				string new_descr_msg_type,
				PgColor_t new_color_msg_type
				) :
				id_msg_type(new_id_msg_type),
				descr_msg_type(new_descr_msg_type),	
				color_msg_type(new_color_msg_type)
				{};

string get_type_description() {return descr_msg_type;};
int get_type_id() {return id_msg_type;};
int get_type_color() {return color_msg_type;};
};


/*****************************************************************************
Container class for messages types - 
wrapper of STL container  map
*****************************************************************************/

class msg_types_container {
private :
struct ltint
	{
		  bool operator() (const int i1, const int i2) const
 			 {
   			 return (i1<i2) ;
  			}
	};

typedef  map <int, msg_type, ltint> msg_types_cont;	
typedef  msg_types_cont::iterator msg_types_iterator;	
typedef  msg_types_cont::value_type msg_types_pair;	
typedef  msg_types_cont::size_type msg_types_size_type;	

msg_types_cont	container_msg_types;

public :
enum {DEFAULT_COLOR=Pg_BLACK};

msg_types_iterator begin() {return container_msg_types.begin();};
msg_types_iterator end() {return container_msg_types.end();};

msg_types_iterator find(const int key) { return container_msg_types.find(key); };
msg_types_iterator upper_bound(const int key) { return container_msg_types.upper_bound(key); };
void erase (msg_types_iterator iter) { container_msg_types.erase(iter); };
void erase (msg_types_iterator iter_beg,
					msg_types_iterator iter_end) { container_msg_types.erase(iter_beg, iter_end); };

msg_types_iterator insert (msg_types_iterator iter, const msg_types_pair& obj) {
  return (container_msg_types.insert(iter, obj)); 
}

bool empty() const { return container_msg_types.empty();}
msg_types_container::msg_types_size_type size() const {return container_msg_types.size();};

void load_type_parameters () 
        throw (spider_exception);
void load (
     string file_name) throw (spider_exception);
};

#endif