class jobs_data_managers_container {
private : 
    typedef map<int , abstract_job_data_manager*, less<int> > container_type_of_managers;
    typedef container_type_of_managers::iterator iterator;
    typedef container_type_of_managers::size_type size_type;
    typedef container_type_of_managers::value_type value_type;

    container_type_of_managers  container_of_managers;

public:
      iterator begin() {return container_of_managers.begin();};
      iterator end() {return container_of_managers.end();};

      bool insert (const value_type& new_value) 
                         {pair<iterator,bool> ret_pair=container_of_managers.insert(new_value);
                           return ret_pair.second;};
      void erase (iterator where);
      iterator find(const int& discriminator) {return container_of_managers.find(discriminator);};

      ~jobs_data_managers_container();
};
