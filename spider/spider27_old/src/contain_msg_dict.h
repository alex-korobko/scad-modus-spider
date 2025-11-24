#ifndef __MSG_DICT_CONT_H__
#define __MSG_DICT_CONT_H__

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
struct ltint
	{
		  bool operator() (const int i1, const int i2) const
 			 {
   			 return (i1<i2) ;
  			}
	};

typedef  map <int, dictionary_message, ltint> msg_dictionary;	
typedef  msg_dictionary::iterator msg_dict_iterator;	
typedef  msg_dictionary::value_type msg_dict_pair;	
typedef  msg_dictionary::size_type msg_dict_size_type;	

msg_dictionary	container_msg_dict;

public :
enum {MSG_PROGRAM_STARTED=143, 
			MSG_PROGRAM_CLOSED=144,
			MESSAGES_COUNT
			};

msg_dict_iterator begin() {return container_msg_dict.begin();};
msg_dict_iterator end() {return container_msg_dict.end();};

msg_dict_iterator find(const int key) { return container_msg_dict.find(key); };
msg_dict_iterator upper_bound(const int key) { return container_msg_dict.upper_bound(key); };

void erase (msg_dict_iterator iter) { container_msg_dict.erase(iter); };
void erase (msg_dict_iterator iter_beg,
					msg_dict_iterator iter_end) { container_msg_dict.erase(iter_beg, iter_end); };
msg_dict_iterator insert (msg_dict_iterator iter, const msg_dict_pair& obj) 
{
  return (container_msg_dict.insert(iter, obj)); 
}

bool empty() const { return container_msg_dict.empty();}
msg_dict_size_type size() const {return container_msg_dict.size();};

void
 load_message_parameters (
           msg_types_container *msg_types_cont) throw (spider_exception);

void load(
	  string file_name,
      msg_types_container *msg_types_cont) throw (spider_exception);
};

#endif