#ifndef __MSG_TYPES_CONTAINER__
#define __MSG_TYPES_CONTAINER__

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
public:
typedef  hash_map <int, msg_type> msg_types_cont;
typedef  msg_types_cont::iterator iterator;	
typedef  msg_types_cont::value_type value_type;	
typedef  msg_types_cont::size_type size_type;	

private :
msg_types_cont	container_msg_types;
msg_types_container(){};

//static members
static msg_types_container* msg_types_container_instance;
public :
enum {DEFAULT_COLOR=Pg_BLACK};

iterator begin() {return container_msg_types.begin();};
iterator end() {return container_msg_types.end();};

iterator find(const int key) { return container_msg_types.find(key); };
iterator upper_bound(const int key) { return container_msg_types.upper_bound(key); };
void erase (iterator iter) { container_msg_types.erase(iter); };
void erase (iterator iter_beg,
					iterator iter_end) { container_msg_types.erase(iter_beg, iter_end); };

iterator insert (iterator iter, const value_type& obj) {
  return (container_msg_types.insert(iter, obj)); 
}

bool empty() const { return container_msg_types.empty();}
size_type size() const {return container_msg_types.size();};

void load_type_parameters () 
        throw (spider_exception);
void load (
     string file_name) throw (spider_exception);
//static metods
static msg_types_container* get_instance();
};

#endif
