using namespace std;

#include <pthread.h>
#include <inttypes.h>

#include <iostream>

#include <vector>
#include <string>
#include <sstream>
#include <functional>

#include "defines.h"
#include "spider_exception.h"

#include "data_block.h"

	data_block::data_block( 
	                   signals_container::size_type signals_size,
	                   messages_container::size_type messages_size,
	                   parameters_container::size_type parameters_size)
	                                                          throw (spider_exception):
           signals(signals_size),
           messages(messages_size),
           parameters(parameters_size){
           int ret_value = pthread_mutex_init(&data_block_mutex, NULL);
           if (ret_value != 0 /*EOK*/ ){
                string exception_message("Can`t initialize data_block mutex: ");
                exception_message+=strerror(ret_value);
                throw spider_exception(exception_message);
           };
    };

	data_block::data_block
	       ( const data_block &copy_data_block)
	                        throw (spider_exception){
	pthread_mutex_t *ptr_to_copied_mutex=
	         const_cast<pthread_mutex_t*>(&(copy_data_block.data_block_mutex));
     int ret_value = pthread_mutex_init(&data_block_mutex, NULL);
     if (ret_value !=0 /*EOK*/ ){
          string exception_message("Can`t initialize data_block mutex: ");
          exception_message+=strerror(ret_value);
          throw spider_exception(exception_message);
           };

    pthread_mutex_lock(&data_block_mutex);
      pthread_mutex_lock(ptr_to_copied_mutex);
           signals=copy_data_block.signals;
           messages=copy_data_block.messages;
           parameters=copy_data_block.parameters;
      pthread_mutex_unlock(ptr_to_copied_mutex); 
    pthread_mutex_unlock(&data_block_mutex); 

	};

    data_block::~data_block( ){
       pthread_mutex_destroy(&data_block_mutex);
     };

data_block&  data_block::operator=
          (const data_block &copy_data_block) {
	pthread_mutex_t *ptr_to_copied_mutex=
	         const_cast<pthread_mutex_t*>(&(copy_data_block.data_block_mutex));
    pthread_mutex_lock(&data_block_mutex);
      pthread_mutex_lock(ptr_to_copied_mutex);
           signals=copy_data_block.signals;
           messages=copy_data_block.messages;
           parameters=copy_data_block.parameters;
      pthread_mutex_unlock(ptr_to_copied_mutex); 
    pthread_mutex_unlock(&data_block_mutex); 
	return *this;     
};



   data_block::signal_data_type
         data_block::get_signal_value
                             (data_block::signals_container::size_type signal_index)
                              throw (spider_exception){
      signal_data_type signal_value;
      pthread_mutex_lock(&data_block_mutex);
            if (signal_index>=signals.size()){
                  ostringstream exception_description;
                  exception_description<<
                        "In data_block::get_signal_value(...) signal index "<<
                         signal_index<<
                        " is out of range signals.size "<<
                        signals.size();
                  pthread_mutex_unlock(&data_block_mutex);

                  throw spider_exception(exception_description.str());
            };
      signal_value=signals[signal_index];
      pthread_mutex_unlock(&data_block_mutex);
      return  signal_value;
    };

   data_block::parameter_data_type
        data_block::get_parameter_value
                            (data_block::parameters_container::size_type parameter_index)
                             throw (spider_exception){
      parameter_data_type parameter_value;
      pthread_mutex_lock(&data_block_mutex);
            if (parameter_index>=parameters.size()){
                  ostringstream exception_description;
                  exception_description<<
                        "In data_block::get_parameter_value(...) parameter index "<<
                         parameter_index<<
                        " is out of range parameters.size "<<
                        parameters.size();
                  pthread_mutex_unlock(&data_block_mutex);

                  throw spider_exception(exception_description.str());
            };
      parameter_value=parameters[parameter_index];
      pthread_mutex_unlock(&data_block_mutex);
      return  parameter_value;
   };


  data_block::message_data_type
           data_block::get_message_value
             (messages_container::size_type message_index)
                                                     throw (spider_exception){
      message_data_type message_value;
      pthread_mutex_lock(&data_block_mutex);
            if (message_index>=messages.size()){
                  ostringstream exception_description;
                  exception_description<<
                        "In data_block::get_message_value(...) message index "<<
                         message_index<<
                        " is out of range messages.size "<<
                        messages.size();
                  pthread_mutex_unlock(&data_block_mutex);

                  throw spider_exception(exception_description.str());
            };
      message_value=messages[message_index];
      pthread_mutex_unlock(&data_block_mutex);
      return  message_value;
  };

   
   void 
      data_block::set_signal_value
                       (data_block::signals_container::size_type signal_index,
                       data_block::signal_data_type signal_value)
                       throw (spider_exception){

      pthread_mutex_lock(&data_block_mutex);
            if (signal_index>=signals.size()){
                  ostringstream exception_description;
                  exception_description<<
                        "In data_block::set_signal_value(...) signal index "<<
                         signal_index<<
                        " is out of range signals.size "<<
                        signals.size();
                  pthread_mutex_unlock(&data_block_mutex);

                  throw spider_exception(exception_description.str());
            };

      signals[signal_index]=signal_value;
      pthread_mutex_unlock(&data_block_mutex);
  };

   void 
       data_block::set_parameter_value 
                            (data_block::parameters_container::size_type parameter_index,
                            data_block::parameter_data_type parameter_value)
                            throw (spider_exception){

      pthread_mutex_lock(&data_block_mutex);
            if (parameter_index>=parameters.size()){
                  ostringstream exception_description;
                  exception_description<<
                        "In data_block::set_parameter_value(...) parameter index "<<
                         parameter_index<<
                        " is out of range parameters.size "<<
                        parameters.size();
                  pthread_mutex_unlock(&data_block_mutex);

                  throw spider_exception(exception_description.str());
            };
      parameters[parameter_index]=parameter_value;
      pthread_mutex_unlock(&data_block_mutex);
   };
 
 
    void 
        data_block::set_new_data 
             (data_block::signals_container::size_type begin_signals_index,
              data_block::signals_container new_signals,
              data_block::parameters_container::size_type begin_parameters_index,
              data_block::parameters_container new_parameters,
              data_block::messages_container::size_type begin_messages_index,
              data_block::messages_container new_messages) throw (spider_exception){

      pthread_mutex_lock(&data_block_mutex);

            if (begin_signals_index+new_signals.size()>signals.size()){
                  ostringstream exception_description;
                  exception_description<<
                        "In data_block::set_new_data(...) begin_signals_index+new_signals.size() "<<
                         begin_signals_index+new_signals.size()<<
                        " is out of range signals.size "<<
                        signals.size();
                  pthread_mutex_unlock(&data_block_mutex);

                  throw spider_exception(exception_description.str());
            };

            if (begin_parameters_index+new_parameters.size()>parameters.size()){
                  ostringstream exception_description;
                  exception_description<<
                        "In data_block::set_new_data(...) begin_parameters_index+new_parameters.size() "<<
                         begin_parameters_index+new_parameters.size()<<
                        " is out of range parameters.size "<<
                        parameters.size();
                  pthread_mutex_unlock(&data_block_mutex);

                  throw spider_exception(exception_description.str());
            };

            if (begin_messages_index+new_messages.size()>messages.size()){
                  ostringstream exception_description;
                  exception_description<<
                        "In data_block::set_new_data(...) begin_messages_index+new_messages.size() "<<
                         begin_messages_index+new_messages.size()<<
                        " is out of range messages.size "<<
                        messages.size();
                  pthread_mutex_unlock(&data_block_mutex);

                  throw spider_exception(exception_description.str());
            };

     if(!new_signals.empty()){
            signals_container::iterator sig_iter_beg=signals.begin(),
                                                     sig_iter_end=signals.begin();
             advance(sig_iter_beg, begin_signals_index);
             advance(sig_iter_end, begin_signals_index+new_signals.size());
             signals.erase(sig_iter_beg, sig_iter_end);
             signals.insert(sig_iter_end, new_signals.begin(), new_signals.end());
     };

    if(!new_parameters.empty()) {
               parameters_container::iterator par_iter_beg=parameters.begin(),
                                                                par_iter_end=parameters.begin();
               advance(par_iter_beg, begin_parameters_index);
               advance(par_iter_end, begin_parameters_index+new_parameters.size());
               parameters.erase(par_iter_beg, par_iter_end);
               parameters.insert(par_iter_end, new_parameters.begin(), new_parameters.end());
    };

    if (!new_messages.empty()) {
        messages_container::iterator msg_iter_beg=messages.begin(),
                                                          msg_iter_end=messages.begin();
        advance(msg_iter_beg, begin_messages_index);
        advance(msg_iter_end, begin_messages_index+new_messages.size());
        messages.erase(msg_iter_beg, msg_iter_end);
        messages.insert(msg_iter_end, new_messages.begin(), new_messages.end());
    };
    pthread_mutex_unlock(&data_block_mutex);
  };

  data_block::signals_container::size_type
           data_block::get_signals_size() {
     data_block::signals_container::size_type signals_size;
     pthread_mutex_lock(&data_block_mutex);
        signals_size=signals.size();
     pthread_mutex_unlock(&data_block_mutex);
     return signals_size;}

  data_block::parameters_container::size_type 
            data_block::get_parameters_size() {
     data_block::parameters_container::size_type parameters_size;
     pthread_mutex_lock(&data_block_mutex);
        parameters_size=parameters.size();
     pthread_mutex_unlock(&data_block_mutex);
     return parameters_size;}

  data_block::messages_container::size_type 
              data_block::get_messages_size() {
          data_block::messages_container::size_type messages_size;
     pthread_mutex_lock(&data_block_mutex);
        messages_size=messages.size();
     pthread_mutex_unlock(&data_block_mutex);
     return messages_size;}
