#ifndef __METRO_DEVICES_CONTAINER__
#define __METRO_DEVICES_CONTAINER__
class metro_devices_container {
private:
typedef  map<int, metro_device*, less<int> > metro_dev_container;
typedef  metro_dev_container::iterator iterator;
typedef  metro_dev_container::value_type value_type;
typedef  metro_dev_container::key_type key_type;
typedef  metro_dev_container::size_type size_type;

metro_dev_container metro_dev_cont;
public:
iterator find(key_type key) { return metro_dev_cont.find(key);}

iterator begin() { return metro_dev_cont.begin();}
iterator end() { return metro_dev_cont.end();}

size_type size() {return metro_dev_cont.size();}
bool empty() {return metro_dev_cont.empty();}

void insert(iterator pos, value_type new_item) {metro_dev_cont.insert(pos, new_item);}

virtual ~metro_devices_container() {
   iterator iter=metro_dev_cont.begin();

   while (iter!=metro_dev_cont.end()) {
       delete(iter->second);
       iter++;
      }; 
   }; 
};
#endif