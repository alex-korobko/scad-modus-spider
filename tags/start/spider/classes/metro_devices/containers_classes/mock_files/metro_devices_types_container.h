#ifndef __METRO_DEVICES_TYPES_CONTAINER__
#define __METRO_DEVICES_TYPES_CONTAINER__

class metro_devices_types_container {
private:
typedef  map<int, metro_device_type*, less<int> > metro_dev_types_container;
typedef  metro_dev_types_container::iterator iterator;
typedef  metro_dev_types_container::value_type value_type;
typedef  metro_dev_types_container::key_type key_type;
typedef  metro_dev_types_container::size_type size_type;

metro_dev_types_container metro_dev_types_cont;
public:
iterator find(key_type key) { return metro_dev_types_cont.find(key);}

iterator begin() { return metro_dev_types_cont.begin();}
iterator end() { return metro_dev_types_cont.end();}

size_type size() {return metro_dev_types_cont.size();}
bool empty() {return metro_dev_types_cont.empty();}

void insert(iterator pos, value_type new_item) {metro_dev_types_cont.insert(pos, new_item);}

virtual ~metro_devices_types_container() {

   iterator iter=metro_dev_types_cont.begin();
   while (iter!=metro_dev_types_cont.end()){
       delete(iter->second);
       iter++;
       };
 };
 

};

#endif