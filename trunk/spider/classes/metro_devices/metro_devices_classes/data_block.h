#ifndef _DATA_BLOCK_
#define _DATA_BLOCK_

class data_block{
private:
    typedef byte signal_data_type;
    typedef word message_data_type;
    typedef dword parameter_data_type;
    typedef  vector<signal_data_type> signals_container;
    typedef  vector<message_data_type> messages_container;
    typedef  vector<parameter_data_type> parameters_container;

	signals_container signals;
	messages_container  messages;
	parameters_container parameters;

 pthread_mutex_t data_block_mutex;

 data_block();
public:
	enum { DATA_BLOCK_ESCALATOR=1,
                  DATA_BLOCK_SHAVR,
                  DATA_BLOCK_DOOR,
                  DATA_BLOCK_UDKU};
/*
constructors and destructor
*/
	data_block( signals_container::size_type signals_size,
	                   messages_container::size_type messages_size,
	                   parameters_container::size_type parameters_size) throw (spider_exception);

	data_block
	        (const data_block &copy_data_block)
	                          throw (spider_exception);


  virtual ~data_block( );

/*
reloading parameters
*/

data_block&  operator=(const data_block& copy_data_block);

/*
other metods
*/
  signals_container::size_type get_signals_size();
  parameters_container::size_type get_parameters_size();
  messages_container::size_type get_messages_size();

   signal_data_type 
           get_signal_value 
           (signals_container::size_type signal_index)
                                     throw (spider_exception);
   parameter_data_type
           get_parameter_value
             (parameters_container::size_type parameter_index)
                                                     throw (spider_exception);

   message_data_type
           get_message_value
             (messages_container::size_type message_index)
                                                     throw (spider_exception);
   
   void set_signal_value (
                  signals_container::size_type signal_index,
                  signal_data_type signal_value) 
                               throw (spider_exception);
   void set_parameter_value (
                  parameters_container::size_type parameter_index,
                  parameter_data_type parameter_value)
                                     throw (spider_exception);

   void set_new_data 
             (signals_container::size_type begin_signal_index,
              signals_container new_signals,
              parameters_container::size_type begin_parameters_index,
              parameters_container new_parameters,
              messages_container::size_type begin_messages_index,
              messages_container new_messages) throw (spider_exception);
/*
virtual functions
*/
   virtual int get_data_block_type_description()=0;
};

#endif
