#ifndef __MSG_DICT_CONTAINER__
#define __MSG_DICT_CONTAINER__

class	 dictionary_message
{
private:
	int		id_msg;
	int 	type_msg;
	string	text_msg;
	
	dictionary_message();
public:
	dictionary_message(int new_number, 
									int new_msg_type,
									string new_text) : 
	id_msg(new_number),
	type_msg(new_msg_type),
	text_msg(new_text) {};

	string get_text () {return (text_msg);}
	int get_id () {return( id_msg);}
	int get_type () {return( type_msg);}
};

/*****************************************************************************
Container class for messages dictionary - 
wrapper of STL container  map
*****************************************************************************/

class msg_dict_container {
private :

typedef  hash_map <int, dictionary_message> msg_dictionary;	
typedef  msg_dictionary::iterator iterator;	
typedef  msg_dictionary::value_type value_type;	
typedef  msg_dictionary::size_type size_type;	

msg_dictionary	container_msg_dict;

public :
enum {MSG_PROGRAM_STARTED=143, 
			MSG_PROGRAM_CLOSED=144,
			MESSAGES_COUNT};

iterator begin() {return container_msg_dict.begin();};
iterator end() {return container_msg_dict.end();};

iterator find(const int key) { return container_msg_dict.find(key); };
iterator upper_bound(const int key) { return container_msg_dict.upper_bound(key); };

void erase (iterator iter) { container_msg_dict.erase(iter); };
void erase (iterator iter_beg,
					iterator iter_end) { container_msg_dict.erase(iter_beg, iter_end); };
iterator insert (iterator iter, const value_type& obj) {
  return (container_msg_dict.insert(iter, obj)); 
}

bool empty() const { return container_msg_dict.empty();}
size_type size() const {return container_msg_dict.size();};

void
 load_message_parameters () throw (spider_exception);

void load(
	  string file_name) throw (spider_exception);
};

#endif
