#ifndef __DISPATCHERS_PERSONS_CONTAINER__
#define __DISPATCHERS_PERSONS_CONTAINER__

class dispatcher_person
{
private:
ldword id_dispatcher_person;
string first_name_dispatcher_person;
string middle_name_dispatcher_person;
string last_name_dispatcher_person;

dispatcher_person();

public:
dispatcher_person (ldword new_id_dispatcher_person,
                string new_first_name_dispatcher_person,
                string new_middle_name_dispatcher_person,
                string new_last_name_dispatcher_person
				) :
				id_dispatcher_person(new_id_dispatcher_person),
                first_name_dispatcher_person(new_first_name_dispatcher_person),
                middle_name_dispatcher_person(new_middle_name_dispatcher_person),
                last_name_dispatcher_person(new_last_name_dispatcher_person)
				{};

string get_first_name() {return first_name_dispatcher_person;};
string get_middle_name() {return middle_name_dispatcher_person;};
string get_last_name() {return last_name_dispatcher_person;};
ldword get_id() {return id_dispatcher_person;};
};


/*****************************************************************************
Container class for dispatchers persons - 
wrapper of STL container  map
*****************************************************************************/

class dispatcher_persons_container {
private :
typedef  list <dispatcher_person> dispatcher_persons_cont;
typedef  dispatcher_persons_cont::iterator iterator;
typedef  dispatcher_persons_cont::value_type value_type;
typedef  dispatcher_persons_cont::size_type size_type;	

dispatcher_persons_cont container_dispatcher_persons;

PtWidget_t *dispatchers_list;
PtWidget_t *first_name_text;
PtWidget_t *middle_name_text;
PtWidget_t *last_name_text;

PtWidget_t *dispatchers_combobox;
PtWidget_t *dispatcher_smena_hour_numeric;
PtWidget_t *dispatcher_smena_minute_numeric;

//static members
static dispatcher_persons_container* dispatcher_persons_container_instance;

//Class functor for sorting by dispatcher_person.last_name  with Strict Weak Ordering
class stwo_dispatchers_sorter:
 public binary_function<const  dispatcher_person,
                                        const  dispatcher_person,
                                        bool> {
public:
bool operator () (dispatcher_person first_dispatcher, dispatcher_person second_dispatcher) const {
	return (first_dispatcher.get_last_name()<second_dispatcher.get_last_name() && 
                 first_dispatcher.get_first_name()<second_dispatcher.get_first_name() &&
                 first_dispatcher.get_middle_name()<second_dispatcher.get_middle_name());
	}
};

//Class functor for finding by log_record.get_id with Strict Weak Ordering
class 	stwo_finder: 
 public unary_function<const  dispatcher_person,
 									bool> {
private:
    ldword autoincrement_id_to_find;
    stwo_finder();
public:
    stwo_finder(ldword new_autoincrement_id_to_find):
         autoincrement_id_to_find(new_autoincrement_id_to_find) {};
    bool operator () (dispatcher_person dispatcher) const {
         return (dispatcher.get_id()==autoincrement_id_to_find);
    }
};


stwo_dispatchers_sorter dispatchers_sorter;
ldword dispatchers_count; //needed for automaic id generation
iterator current_dispatcher;

dispatcher_persons_container():
        container_dispatcher_persons(),
        dispatchers_list(NULL),
        first_name_text(NULL),
        middle_name_text(NULL),
        last_name_text(NULL),
        dispatchers_count(0),
        current_dispatcher(container_dispatcher_persons.end()){};
public :

iterator begin() {return container_dispatcher_persons.begin();};
iterator end() {return container_dispatcher_persons.end();};

iterator find(const int key);

iterator get_current_dispatcher() {return current_dispatcher;};

void erase (iterator iter) { container_dispatcher_persons.erase(iter); };
void erase (iterator iter_beg,
					iterator iter_end) { container_dispatcher_persons.erase(iter_beg, iter_end); };

void insert (const value_type& obj) { container_dispatcher_persons.insert(container_dispatcher_persons.end(), obj); };

bool empty() const { return container_dispatcher_persons.empty();}
size_type size() const {return container_dispatcher_persons.size();};

void load_dispatcher_parameters () throw (spider_exception);
void load_settings_parameters () throw (spider_exception);
void load (string file_name) throw (spider_exception);

void save_dispatcher_parameters (iterator dispatcher) throw (spider_exception);
void save_settings_parameters () throw (spider_exception);
void save (string file_name) throw (spider_exception);

void sort_all ();

//widgets 
PtWidget_t *get_dispatchers_list() { return dispatchers_list;};
void set_dispatchers_list(PtWidget_t *new_dispatchers_list) { dispatchers_list=new_dispatchers_list;};

PtWidget_t *get_dispatchers_first_name_text() {return first_name_text;};
void set_dispatchers_first_name_text(PtWidget_t *new_first_name_text) { first_name_text=new_first_name_text;};

PtWidget_t *get_dispatchers_middle_name_text() {return middle_name_text;};
void set_dispatchers_middle_name_text(PtWidget_t *new_middle_name_text) { middle_name_text=new_middle_name_text;};

PtWidget_t *get_dispatchers_last_name_text() {return last_name_text;};
void set_dispatchers_last_name_text(PtWidget_t *new_last_name_text) { last_name_text=new_last_name_text;};


PtWidget_t *get_dispatchers_combobox() {return dispatchers_combobox;};
void set_dispatchers_combobox(PtWidget_t *new_dispatchers_combobox) { dispatchers_combobox=new_dispatchers_combobox;};

PtWidget_t *get_dispatcher_smena_hour_numeric() {return dispatcher_smena_hour_numeric;};
void set_dispatcher_smena_hour_numeric(PtWidget_t *new_dispatcher_smena_hour_numeric) { dispatcher_smena_hour_numeric=new_dispatcher_smena_hour_numeric;};

PtWidget_t *get_dispatcher_smena_minute_numeric() {return dispatcher_smena_minute_numeric;};
void set_dispatcher_smena_minute_numeric(PtWidget_t *new_dispatcher_smena_minute_numeric) { dispatcher_smena_minute_numeric=new_dispatcher_smena_minute_numeric;};

//static metods
static dispatcher_persons_container* get_instance();

};

#endif


