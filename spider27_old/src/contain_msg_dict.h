#ifndef __MSG_DICT_CONT_H__
#define __MSG_DICT_CONT_H__

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

msg_dictionary	msg_dict_map;

public :
typedef  msg_dictionary::iterator msg_dict_iterator;	
typedef  msg_dictionary::value_type msg_dict_pair;	

msg_dict_iterator begin() {return msg_dict_map.begin();};
msg_dict_iterator end() {return msg_dict_map.end();};

msg_dict_iterator find(const int key) { return msg_dict_map.find(key); };
void erase (msg_dict_iterator iter) { msg_dict_map.erase(iter); };

msg_dict_iterator insert (msg_dict_iterator iter, const msg_dict_pair& obj) 
{
  return (msg_dict_map.insert(iter, obj)); 
}

bool empty() const { return msg_dict_map.empty();}
int size() const {return msg_dict_map.size();};
};

#endif