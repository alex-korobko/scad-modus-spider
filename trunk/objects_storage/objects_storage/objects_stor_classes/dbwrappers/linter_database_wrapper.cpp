using namespace std;

#include <functional>
#include <vector>
#include <sstream>

#include <pthread.h>
#include <inttypes.h>

#include "storage_common.h"
#include "objects_storage_exception.h"
#include "objects_storage_logger.h"

#include <linapi.h>

#include "linter_database_wrapper.h"

database_wrapper& 
          database_wrapper::get_instance() throw (objects_storage_exception){
    static database_wrapper dbwrapper_instance;
    return dbwrapper_instance;
};


string
       database_wrapper::linter_error_decoding (LONG return_code,
                                                                 WORD connection_id,
                                                                 WORD cursor_id,
                                                                WORD statement_id){
  LONG linapi_error_ret_code;
  LONG api_error = 0, error = 0, system_err = 0;
  short int  error_message_buff_size=100;
  vector<char> error_message_buff(error_message_buff_size);
  string error_message;
  ostringstream return_string;

  return_string<<" ";

  if (return_code == LINAPI_ERROR){
     if ((linapi_error_ret_code = LINTER_Error(connection_id,
                                                       cursor_id,
                                                       statement_id,
                                                       &api_error,
                                                       &error,
                                                       &system_err, 
                                                       &error_message_buff[0],
                                                       &error_message_buff_size))!=0) {
           return_string<<"Error execution LINTER_Error error code  : "<<linapi_error_ret_code;
       } else  { //     if ((linapi_error_ret_code = LINTER_Error(connection_id,
            if (error == 2) 
               return return_string.str();

            error_message=&error_message_buff[0];

           return_string<<"API error "<<api_error
                                <<" error "<<error
                                <<" system error  "<<system_err;

            if (!error_message.empty())
                    return_string<<" error message: "<<error_message;
       }; //  if ((linapi_error_ret_code = LINTER_Error(connection_id,
   } else { //if (return_code == LINAPI_ERROR)
       return_string <<"linter return code "<<return_code;
   }; ////if (return_code == LINAPI_ERROR)
 
  return return_string.str();
};


  database_wrapper::database_wrapper() throw (objects_storage_exception):
  occupation_answer_data(1),
  occupation_needed_ids(1) {
   string query;
   vector<char> query_string_buffer;

  if (LINTER_Connect("SYSTEM", 0, "MANAGER", 0, NULL,
                            mOptimistic,  /* transaction mode */
                            &linter_connection_id)!=LINAPI_SUCCESS) 
                  throw  objects_storage_exception("Connection  to LINTER failed");

  if (LINTER_OpenCursor(linter_connection_id,
                                         &linter_cursor_id,
                                         NULL, 0, 
                                         mOptimistic)!=LINAPI_SUCCESS)
                  throw objects_storage_exception ("Openinig cursor linter_cursor_id failed");

//occupation_1 statement

query="select channel_id
  from channels
  where 
     channel_right_side=:RIGHT_SIDE_PARAM and
     channel_number=:CHANNEL_INTERVAL_PARAM and
     channel_right_side-channel_left_side=1;";

  query_string_buffer.assign(query.begin(),
                             query.end());
  if (LINTER_CreateStatement(linter_connection_id, 
                                               &query_string_buffer[0],
                                               query_string_buffer.size(),
                                                &pri_channel_occupation_statement_1)!=LINAPI_SUCCESS)
                  throw objects_storage_exception ("Statement pri_channel_occupation_statement_1 creation failed");

   size_of_param=sizeof(right_side_param);
   if (LINTER_BindParameter(linter_cursor_id,
                                    pri_channel_occupation_statement_1,
                                    1,
                                    NULL,
                                    NULL,
                                    &right_side_param,
                                    1,
                                    0,
                                    tInt,
                                    NULL,
                                    &size_of_param)!=LINAPI_SUCCESS)
      throw objects_storage_exception("Binding right_side_param for pri_channel_occupation_statement_1 failed");

   size_of_param=sizeof(channel_interval_param);
   if (LINTER_BindParameter(linter_cursor_id,
                                    pri_channel_occupation_statement_1,
                                    2,
                                    NULL,
                                    NULL,
                                    &channel_interval_param,
                                    1,
                                    0,
                                    tInt,
                                    NULL,
                                    &size_of_param)!=LINAPI_SUCCESS)
      throw objects_storage_exception("Binding channel_interval_param for pri_channel_occupation_statement_1 failed");

  if (LINTER_BindAnswer(linter_cursor_id,
                               pri_channel_occupation_statement_1,
                               1,
                               &(&occupation_needed_ids[0])->channel_id,
                               NULL, 
                               sizeof(struct needed_id),
                               tInt,
                               sizeof(LONG),
                               0, 0, NULL)!=LINAPI_SUCCESS)
        throw objects_storage_exception("Binding answer about channel_id for pri_channel_occupation_statement_1 failed");

//occupation_2 statement
query="select channeld_id
              from channel_d
              where channelb_id = :CHANNELB_ID;";

  query_string_buffer.assign(query.begin(),
                             query.end());
  if (LINTER_CreateStatement(linter_connection_id, 
                                               &query_string_buffer[0],
                                               query_string_buffer.size(),
                                                &pri_channel_occupation_statement_2)!=LINAPI_SUCCESS)
                  throw objects_storage_exception ("Statement pri_channel_occupation_statement_2 creation failed");

   size_of_param=sizeof(channel_id_param);
   if (LINTER_BindParameter(linter_cursor_id,
                                    pri_channel_occupation_statement_2,
                                    1,
                                    NULL,
                                    NULL,
                                    &channel_id_param,
                                    1,
                                    0,
                                    tInt,
                                    NULL,
                                    &size_of_param)!=LINAPI_SUCCESS)
      throw objects_storage_exception("Binding channel_id_param for pri_channel_occupation_statement_2 failed");

  if (LINTER_BindAnswer(linter_cursor_id,
                               pri_channel_occupation_statement_2,
                               1,
                               &(&occupation_needed_ids[0])->channel_id,
                               NULL, 
                               sizeof(struct needed_id),
                               tInt,
                               sizeof(LONG),
                               0, 0, NULL)!=LINAPI_SUCCESS)
        throw objects_storage_exception("Binding answer about channel_id for pri_channel_occupation_statement_2 failed");

//occupation_3 statement
query="select channelb_id
              from channel_b_d
              where channeld_id = :CHANNELD_ID and 
                         channelb_number=:CHANNEL_NUMBER;";

  query_string_buffer.assign(query.begin(),
                             query.end());
  if (LINTER_CreateStatement(linter_connection_id, 
                                               &query_string_buffer[0],
                                               query_string_buffer.size(),
                                                &pri_channel_occupation_statement_3)!=LINAPI_SUCCESS)
                  throw objects_storage_exception ("Statement pri_channel_occupation_statement_3 creation failed");

   size_of_param=sizeof(channel_id_param);
   if (LINTER_BindParameter(linter_cursor_id,
                                    pri_channel_occupation_statement_3,
                                    1,
                                    NULL,
                                    NULL,
                                    &channel_id_param,
                                    1,
                                    0,
                                    tInt,
                                    NULL,
                                    &size_of_param)!=LINAPI_SUCCESS)
      throw objects_storage_exception("Binding channel_id_param for pri_channel_occupation_statement_3 failed");

   size_of_param=sizeof(channel_number_param);
   if (LINTER_BindParameter(linter_cursor_id,
                                    pri_channel_occupation_statement_3,
                                    2,
                                    NULL,
                                    NULL,
                                    &channel_number_param,
                                    1,
                                    0,
                                    tInt,
                                    NULL,
                                    &size_of_param)!=LINAPI_SUCCESS)
      throw objects_storage_exception("Binding channel_number_param for pri_channel_occupation_statement_3 failed");


  if (LINTER_BindAnswer(linter_cursor_id,
                               pri_channel_occupation_statement_3,
                               1,
                               &(&occupation_needed_ids[0])->channel_id,
                               NULL, 
                               sizeof(struct needed_id),
                               tInt,
                               sizeof(LONG),
                               0, 0, NULL)!=LINAPI_SUCCESS)
        throw objects_storage_exception("Binding answer about channel_id for pri_channel_occupation_statement_3 failed");

//occupation_4 statement
query="select max(channel_id)
              from channels 
              where channel_id in (
                     select channelb_id
                     from channel_b_d
                     where channeld_id = :CHANNELD_ID) 
                     and is_busy=0;";

  query_string_buffer.assign(query.begin(),
                             query.end());
  if (LINTER_CreateStatement(linter_connection_id, 
                                               &query_string_buffer[0],
                                               query_string_buffer.size(),
                                                &pri_channel_occupation_statement_4)!=LINAPI_SUCCESS)
                  throw objects_storage_exception ("Statement pri_channel_occupation_statement_3 creation failed");

   size_of_param=sizeof(channel_id_param);
   if (LINTER_BindParameter(linter_cursor_id,
                                    pri_channel_occupation_statement_4,
                                    1,
                                    NULL,
                                    NULL,
                                    &channel_id_param,
                                    1,
                                    0,
                                    tInt,
                                    NULL,
                                    &size_of_param)!=LINAPI_SUCCESS)
      throw objects_storage_exception("Binding channel_id_param for pri_channel_occupation_statement_4 failed");

  if (LINTER_BindAnswer(linter_cursor_id,
                               pri_channel_occupation_statement_4,
                               1,
                               &(&occupation_needed_ids[0])->channel_id,
                               NULL, 
                               sizeof(struct needed_id),
                               tInt,
                               sizeof(LONG),
                               0, 0, NULL)!=LINAPI_SUCCESS)
        throw objects_storage_exception("Binding answer about channel_id for pri_channel_occupation_statement_3 failed");

//occupation_5 statement
query="select channels.channel_id,
                         channels.channel_right_side,
                         channels.channel_number,
                         channel_b_d.channelb_number
                from channels, channel_b_d
                where channels.channel_id=:CHANNEL_ID and
                           channels.channel_right_side-channels.channel_left_side=1 and
                           channels.channel_id=channel_b_d.channelb_id;";

  query_string_buffer.assign(query.begin(),
                             query.end());
  if (LINTER_CreateStatement(linter_connection_id, 
                                               &query_string_buffer[0],
                                               query_string_buffer.size(),
                                                &pri_channel_occupation_statement_5)!=LINAPI_SUCCESS)
                  throw objects_storage_exception ("Statement pri_channel_occupation_statement_5 creation failed");

   size_of_param=sizeof(channel_id_param);
   if (LINTER_BindParameter(linter_cursor_id,
                                    pri_channel_occupation_statement_5,
                                    1,
                                    NULL,
                                    NULL,
                                    &channel_id_param,
                                    1,
                                    0,
                                    tInt,
                                    NULL,
                                    &size_of_param)!=LINAPI_SUCCESS)
      throw objects_storage_exception("Binding channel_id_param for pri_channel_occupation_statement_5 failed");

  if (LINTER_BindAnswer(linter_cursor_id,
                               pri_channel_occupation_statement_5,
                               1,
                               &(&occupation_answer_data[0])->channel_id,
                               NULL, 
                               sizeof(struct occupation_answer),
                               tInt,
                               sizeof(LONG),
                               0, 0, NULL)!=LINAPI_SUCCESS)
        throw objects_storage_exception("Binding answer about channel_right_side for pri_channel_occupation_statement_5 failed");

  if (LINTER_BindAnswer(linter_cursor_id,
                               pri_channel_occupation_statement_5,
                               2,
                               &(&occupation_answer_data[0])->channel_right_side,
                               NULL, 
                               sizeof(struct occupation_answer),
                               tInt,
                               sizeof(LONG),
                               0, 0, NULL)!=LINAPI_SUCCESS)
        throw objects_storage_exception("Binding answer about channel_right_side for pri_channel_occupation_statement_5 failed");

  if (LINTER_BindAnswer(linter_cursor_id,
                               pri_channel_occupation_statement_5,
                               3,
                               &(&occupation_answer_data[0])->channel_interval,
                               NULL, 
                               sizeof(struct occupation_answer),
                               tInt,
                               sizeof(LONG),
                               0, 0, NULL)!=LINAPI_SUCCESS)
        throw objects_storage_exception("Binding answer about channel_interval for pri_channel_occupation_statement_5 failed");

  if (LINTER_BindAnswer(linter_cursor_id,
                               pri_channel_occupation_statement_5,
                               4,
                               &(&occupation_answer_data[0])->channel_number,
                               NULL, 
                               sizeof(struct occupation_answer),
                               tInt,
                               sizeof(LONG),
                               0, 0, NULL)!=LINAPI_SUCCESS)
        throw objects_storage_exception("Binding answer about channel_number for pri_channel_occupation_statement_5 failed");

//occupation_6 statement
  query="update channels set is_busy=:TID where channel_id=:CHANNEL_ID and is_busy=0; ";

  query_string_buffer.assign(query.begin(),
                             query.end());
  if (LINTER_CreateStatement(linter_connection_id, 
                                                  &query_string_buffer[0],
                                                  query_string_buffer.size(),
                                                  &pri_channel_occupation_statement_6)!=LINAPI_SUCCESS)
                  throw objects_storage_exception ("Statement pri_channel_occupation_statement_6 creation failed");

   size_of_param=sizeof(tid_param);
   if (LINTER_BindParameter(linter_cursor_id,
                                    pri_channel_occupation_statement_6,
                                    1,
                                    NULL,
                                    NULL,
                                    &tid_param,
                                    1,
                                    0,
                                    tInt,
                                    NULL,
                                    &size_of_param)!=LINAPI_SUCCESS)
      throw objects_storage_exception("Binding tid_param parameter failed");

   size_of_param=sizeof(channel_id_param);
   if (LINTER_BindParameter(linter_cursor_id,
                                    pri_channel_occupation_statement_6,
                                    2,
                                    NULL,
                                    NULL,
                                    &channel_id_param,
                                    1,
                                    0,
                                    tInt,
                                    NULL,
                                    &size_of_param)!=LINAPI_SUCCESS)
      throw objects_storage_exception("Binding channel_id_param parameter failed");

// free statement
  query="update channels set is_busy=0 where 
                                    channel_right_side=:RIGHT_SIDE_VALUE 
                                    and channel_right_side-channel_left_side=1 
                                    and is_busy=:TID;";

  query_string_buffer.assign(query.begin(),
                             query.end());
  if (LINTER_CreateStatement(linter_connection_id, 
                                               &query_string_buffer[0],
                                               query_string_buffer.size(),
                                                &pri_channel_free_statement_1)!=LINAPI_SUCCESS)
                  throw objects_storage_exception ("Statement pri_channel_free_statement_1 creation failed");

   size_of_param=sizeof(right_side_param);
   if (LINTER_BindParameter(linter_cursor_id,
                                    pri_channel_free_statement_1,
                                    1,
                                    NULL,
                                    NULL,
                                    &right_side_param,
                                    1,
                                    0,
                                    tInt,
                                    NULL,
                                    &size_of_param)!=LINAPI_SUCCESS)
      throw objects_storage_exception("Binding right_side_param for pri_channel_free_statement_1 failed");

   size_of_param=sizeof(tid_param);
   if (LINTER_BindParameter(linter_cursor_id,
                                    pri_channel_free_statement_1,
                                    2,
                                    NULL,
                                    NULL,
                                    &tid_param,
                                    1,
                                    0,
                                    tInt,
                                    NULL,
                                    &size_of_param)!=LINAPI_SUCCESS)
      throw objects_storage_exception("Binding tid_param for pri_channel_free_statement_1 failed");
};

vector<byte> 
     database_wrapper::prepare_channel_attributes_by_right_side(
                     LONG channel_right_side,
                     word channel_number,
                      pthread_t pthread_id) {
int upo_distance, ie1_distance, e1_distance, chb_distance;
upo_distance=2*database_settings::IE1_COUNT+
                        2*database_settings::IE1_COUNT*database_settings::E1_COUNT+
                        2*database_settings::IE1_COUNT*database_settings::E1_COUNT*database_settings::CHANNELB_COUNT+
                        1;
ie1_distance=2*database_settings::E1_COUNT+
                        2*database_settings::E1_COUNT*database_settings::CHANNELB_COUNT+
                        1;
e1_distance=2*database_settings::CHANNELB_COUNT+1;
chb_distance=1;

vector<byte> ret_buff(0);
byte upo_index, ie1_index, e1_index, channelb_index;

 if (database_settings::UPO_COUNT*upo_distance
      <=channel_right_side) return ret_buff; // index overflow

   upo_index = 0;
   while(upo_index <= database_settings::UPO_COUNT) {
            if ((upo_index+1)*(upo_distance+1)>=channel_right_side) 
                            break;
            upo_index++;
        };

   channel_right_side-=(upo_index*upo_distance);
   ie1_index = 0;
   while(ie1_index <= database_settings::IE1_COUNT) {
            if ( (ie1_index+1)*(ie1_distance+2)>=channel_right_side) 
                            break;
            ie1_index++;
        };

   channel_right_side-=(ie1_index*ie1_distance);
   e1_index = 0;
   while(e1_index <= database_settings::E1_COUNT) {
            if ((e1_index+1)*(e1_distance+3)>=channel_right_side) 
                            break;
            e1_index++;
        };

   channel_right_side-=e1_index*e1_distance;
   channel_right_side-=(upo_index+e1_index+ie1_index);
   channelb_index=(channel_right_side-5)/2;

   vector<byte> type_bytes;
   type_bytes=bytes_of_type<pthread_t>(pthread_id);
   ret_buff.insert(ret_buff.end(),
                                type_bytes.rbegin(),
                                type_bytes.rend() );

      ret_buff.push_back(upo_index);
      ret_buff.push_back(ie1_index);
      ret_buff.push_back(e1_index);
      ret_buff.push_back(channelb_index);

       type_bytes=bytes_of_type<word>(channel_number);
       ret_buff.insert(ret_buff.end(),
                                type_bytes.rbegin(),
                                type_bytes.rend() );

//==============delete it==========
/*
{
      ostringstream message;
      message<<"In database_wrapper::prepare_channel_attributes_by_right_side(...):"
                          <<" upo "<<static_cast<int>(upo_index)
                          <<" ie1 "<<static_cast<int>(ie1_index)
                          <<" e1 "<<static_cast<int>(e1_index)
                          <<" channel_interval "<<static_cast<int>(channelb_index)
                          <<" pthread_t "<<pthread_id
                          <<" channel_right_side "<< channel_right_side
                          <<" channel_number "<<channel_number;
       objects_storage_logger* logger_inst=
               objects_storage_logger::get_instance();
       logger_inst->log_message
               (objects_storage_logger::INFO_MSG,
                 message.str());
};
*/
//=============================


return ret_buff;
};

// PRI SUPPORT METODS
vector<byte>
       database_wrapper::pri_occupate_channel_only(byte upo_number, 
                                                byte ie1_number,
                                                byte e1_number,
                                                byte channel_interval,
                                               word channelb_number,
                                                pthread_t pthread_id) {
LONG  rows_count, fetched_rows_count, return_code;
LONG upo_distance, ie1_distance, e1_distance, chb_distance;
upo_distance=2*database_settings::IE1_COUNT+
                        2*database_settings::IE1_COUNT*database_settings::E1_COUNT+
                        2*database_settings::IE1_COUNT*database_settings::E1_COUNT*database_settings::CHANNELB_COUNT+
                        1;
ie1_distance=2*database_settings::E1_COUNT+
                        2*database_settings::E1_COUNT*database_settings::CHANNELB_COUNT+
                        1;
e1_distance=2*database_settings::CHANNELB_COUNT+1;
chb_distance=1;

left_side_param=upo_number*upo_distance+ie1_number*ie1_distance+e1_number*e1_distance+channel_interval*chb_distance+upo_number+ie1_number+e1_number+channel_interval+4;
right_side_param=left_side_param+1;

tid_param=pthread_id;
channel_interval_param=channel_interval;
channel_number_param=channelb_number;

//==============delete it==========
/*
{
      ostringstream message;
      message<<"In database_wrapper::pri_occupate_channel_only(...):"
                          <<" upo "<<static_cast<int>(upo_number)
                          <<" ie1 "<<static_cast<int>(ie1_number)
                          <<" e1 "<<static_cast<int>(e1_number)
                          <<" channel_interval "<<static_cast<int>(channel_interval)
                          <<" channelb_number "<<channelb_number
                          <<" pthread_t "<<pthread_id
                          <<" left_side_id "<< left_side_param
                         <<" right_side_id "<< right_side_param;

       objects_storage_logger* logger_inst=
               objects_storage_logger::get_instance();
       logger_inst->log_message
               (objects_storage_logger::INFO_MSG,
                 message.str());
};
*/
//=============================

      try {
      if ((return_code=LINTER_ExecuteStatement(linter_cursor_id,
                                     pri_channel_occupation_statement_1,
                                     NULL, NULL, NULL))!=LINAPI_SUCCESS)
         throw  objects_storage_exception("Can`t execute statement pri_channel_occupation_statement_1"+
                                    database_wrapper::linter_error_decoding (return_code, 
                                                                 linter_connection_id,
                                                                 linter_cursor_id,
                                                                pri_channel_occupation_statement_1));

        size_of_param=sizeof(rows_count);
      if ((return_code=LINTER_GetCursorOption(linter_cursor_id,
                                     cSelectRowCount,
                                     1,
                                     &rows_count,
                                     &size_of_param))!=LINAPI_SUCCESS)
         throw  objects_storage_exception("Can`t get cursor option LINTER_GetCursorOption"+
                                                         database_wrapper::linter_error_decoding (return_code, 
                                                                 linter_connection_id,
                                                                 linter_cursor_id,
                                                                 0));

               if (rows_count!=1) {
                  if (rows_count>1){ 
                        ostringstream message;
                         message<<"In database_wrapper::pri_occupate_channel_only(...) trying to select many than 1 channel:"
                                        <<" upo "<<static_cast<int>(upo_number)
                                        <<" ie1 "<<static_cast<int>(ie1_number)
                                        <<" e1 "<<static_cast<int>(e1_number)
                                        <<" channel_interval "<<static_cast<int>(channel_interval)
                                        <<" channelb_number "<<channelb_number
                                         <<" pthread_t "<<pthread_id
                                        <<" left_side_id "<< left_side_param
                                        <<" right_side_id "<< right_side_param;

                      throw  objects_storage_exception(message.str());
                  }; //  if (rows_count>1){
                       return vector<byte>(0);
               }; // if (rows_count!=1) {


                  if (LINTER_Fetch(linter_cursor_id,
                       toAbsNumber,  /* direction of travelling     */
                       1,    /* starting position in the selection */
                      rows_count,  /* count of required strings       */
                      &fetched_rows_count,  /* count of gotten strings      */
                      NULL,
                      NULL)!=LINAPI_SUCCESS)
                                 throw  objects_storage_exception("Can`t fetch result of execution statement pri_channel_occupation_statement_1");

                      if (rows_count!=
                           fetched_rows_count)
                              throw  objects_storage_exception("Can`t fetched rows of result of execution statement pri_channel_occupation_statement_1 less than rows in result");

      channel_id_param=occupation_needed_ids[0].channel_id;

     if ((return_code=LINTER_ExecuteStatement(linter_cursor_id,
                                                        pri_channel_occupation_statement_2,
                                                   NULL, NULL, NULL))!=LINAPI_SUCCESS)
                     throw  objects_storage_exception("Can`t execute statement pri_channel_occupation_statement_2"+
                                        database_wrapper::linter_error_decoding (return_code, 
                                                                 linter_connection_id,
                                                                 linter_cursor_id,
                                                                pri_channel_occupation_statement_2));

        size_of_param=sizeof(rows_count);
      if ((return_code=LINTER_GetCursorOption(linter_cursor_id,
                                     cSelectRowCount,
                                     1,
                                     &rows_count,
	                                  &size_of_param))!=LINAPI_SUCCESS)
         throw  objects_storage_exception("Can`t get cursor option LINTER_GetCursorOption"+
                                                         database_wrapper::linter_error_decoding (return_code, 
                                                                 linter_connection_id,
                                                                 linter_cursor_id,
                                                                 0));

           if (rows_count!=1) {
                  if (rows_count>1){ 
                        ostringstream message;
                         message<<"In database_wrapper::pri_occupate_channel_only(...) trying to select many than 1 channel:"
                                        <<" upo "<<static_cast<int>(upo_number)
                                        <<" ie1 "<<static_cast<int>(ie1_number)
                                        <<" e1 "<<static_cast<int>(e1_number)
                                        <<" channel_interval "<<static_cast<int>(channel_interval)
                                        <<" channelb_number "<<channelb_number
                                         <<" pthread_t "<<pthread_id
                                        <<" left_side_id "<< left_side_param
                                        <<" right_side_id "<< right_side_param;

                       throw  objects_storage_exception(message.str());
                  }; //  if (rows_count>1){
                       return vector<byte>(0);
               }; // if (rows_count!=1) {

             if (LINTER_Fetch(linter_cursor_id,
                   toAbsNumber,  /* direction of travelling     */
                   1,    /* starting position in the selection */
                   rows_count,  /* count of required strings       */
                   &fetched_rows_count,  /* count of gotten strings      */
                  NULL,
                  NULL)!=LINAPI_SUCCESS)
                        throw  objects_storage_exception("Can`t fetch result of execution statement pri_channel_occupation_statement_1");

             if (rows_count!=
                      fetched_rows_count)
                          throw  objects_storage_exception("Can`t fetched rows of result of execution statement pri_channel_occupation_statement_1 less than rows in result");

     channel_id_param=occupation_needed_ids[0].channel_id;

     if ((return_code=LINTER_ExecuteStatement(linter_cursor_id,
                                                        pri_channel_occupation_statement_3,
                                                   NULL, NULL, NULL))!=LINAPI_SUCCESS)
                     throw  objects_storage_exception("Can`t execute statement pri_channel_occupation_statement_3 "+
                                        database_wrapper::linter_error_decoding (return_code, 
                                                                 linter_connection_id,
                                                                 linter_cursor_id,
                                                                pri_channel_occupation_statement_3));

        size_of_param=sizeof(rows_count);
      if ((return_code=LINTER_GetCursorOption(linter_cursor_id,
                                     cSelectRowCount,
                                     1,
                                     &rows_count,
	                                  &size_of_param))!=LINAPI_SUCCESS)
         throw  objects_storage_exception("Can`t get cursor option LINTER_GetCursorOption"+
                                                         database_wrapper::linter_error_decoding (return_code, 
                                                                 linter_connection_id,
                                                                 linter_cursor_id,
                                                                 0));

           if (rows_count!=1) {
                  if (rows_count>1){ 
                        ostringstream message;
                         message<<"In database_wrapper::pri_occupate_channel_only(...) trying to select many than 1 channel:"
                                        <<" upo "<<static_cast<int>(upo_number)
                                        <<" ie1 "<<static_cast<int>(ie1_number)
                                        <<" e1 "<<static_cast<int>(e1_number)
                                        <<" channel_interval "<<static_cast<int>(channel_interval)
                                        <<" channelb_number "<<channelb_number
                                         <<" pthread_t "<<pthread_id
                                        <<" left_side_id "<< left_side_param
                                        <<" right_side_id "<< right_side_param;

                         throw  objects_storage_exception(message.str());
                  }; //  if (rows_count>1){
                       return vector<byte>(0);
               }; // if (rows_count!=1) {

             if (LINTER_Fetch(linter_cursor_id,
                   toAbsNumber,  /* direction of travelling     */
                   1,    /* starting position in the selection */
                   rows_count,  /* count of required strings       */
                   &fetched_rows_count,  /* count of gotten strings      */
                  NULL,
                  NULL)!=LINAPI_SUCCESS)
                        throw  objects_storage_exception("Can`t fetch result of execution statement pri_channel_occupation_statement_3");

             if (rows_count!=
                      fetched_rows_count)
                          throw  objects_storage_exception("Can`t fetched rows of result of execution statement pri_channel_occupation_statement_1 less than rows in result");

        channel_id_param=occupation_needed_ids[0].channel_id;

     if ((return_code=LINTER_ExecuteStatement(linter_cursor_id,
                                                        pri_channel_occupation_statement_5,
                                                   NULL, NULL, NULL))!=LINAPI_SUCCESS)
                     throw  objects_storage_exception("Can`t execute statement pri_channel_occupation_statement_5"+
                                        database_wrapper::linter_error_decoding (return_code, 
                                                                 linter_connection_id,
                                                                 linter_cursor_id,
                                                                pri_channel_occupation_statement_5));

        size_of_param=sizeof(rows_count);
      if ((return_code=LINTER_GetCursorOption(linter_cursor_id,
                                     cSelectRowCount,
                                     1,
                                     &rows_count,
	                                  &size_of_param))!=LINAPI_SUCCESS)
         throw  objects_storage_exception("Can`t get cursor option LINTER_GetCursorOption"+
                                                         database_wrapper::linter_error_decoding (return_code, 
                                                                 linter_connection_id,
                                                                 linter_cursor_id,
                                                                 0));

           if (rows_count!=1) {
                  if (rows_count>1){ 
                        ostringstream message;
                         message<<"In database_wrapper::pri_occupate_channel_only(...) trying to select many than 1 channel:"
                                        <<" upo "<<static_cast<int>(upo_number)
                                        <<" ie1 "<<static_cast<int>(ie1_number)
                                        <<" e1 "<<static_cast<int>(e1_number)
                                        <<" channel_interval "<<static_cast<int>(channel_interval)
                                        <<" channelb_number "<<channelb_number
                                         <<" pthread_t "<<pthread_id
                                        <<" left_side_id "<< left_side_param
                                        <<" right_side_id "<< right_side_param;

                         throw  objects_storage_exception(message.str());
                  }; //  if (rows_count>1){
                       return vector<byte>(0);
               }; // if (rows_count!=1) {

             if (LINTER_Fetch(linter_cursor_id,
                   toAbsNumber,  /* direction of travelling     */
                   1,    /* starting position in the selection */
                   rows_count,  /* count of required strings       */
                   &fetched_rows_count,  /* count of gotten strings      */
                  NULL,
                  NULL)!=LINAPI_SUCCESS)
                        throw  objects_storage_exception("Can`t fetch result of execution statement pri_channel_occupation_statement_1");

             if (rows_count!=
                      fetched_rows_count)
                          throw  objects_storage_exception("Can`t fetched rows of result of execution statement pri_channel_occupation_statement_1 less than rows in result");

            channel_id_param=occupation_answer_data[0].channel_id;

           if ((return_code=LINTER_ExecuteStatement(linter_cursor_id,
                                                   pri_channel_occupation_statement_6,
                                                   NULL, NULL, NULL))!=LINAPI_SUCCESS)
                            throw  objects_storage_exception("Can`t execute statement pri_channel_occupation_statement_6"+
                                         database_wrapper::linter_error_decoding (return_code, 
                                                                 linter_connection_id,
                                                                 linter_cursor_id,
                                                                pri_channel_occupation_statement_6));

                        size_of_param=sizeof(rows_count);
            if (LINTER_GetCursorOption(linter_cursor_id,
                                                            cExecRow,
                                                            1,
                                                            &rows_count,
                                                            &size_of_param)!=LINAPI_SUCCESS)
                                 throw  objects_storage_exception("Can`t get cursor option LINTER_GetCursorOption ");

            if (rows_count!=1) {
                  if (rows_count>1){ 
                              ostringstream message;
                              message<<"In database_wrapper::pri_occupate_channel_only(...) trying to update many than 1 channel:"
                                        <<" upo "<<static_cast<int>(upo_number)
                                        <<" ie1 "<<static_cast<int>(ie1_number)
                                        <<" e1 "<<static_cast<int>(e1_number)
                                        <<" channel_interval "<<static_cast<int>(channel_interval)
                                        <<" channelb_number "<<channelb_number
                                         <<" pthread_t "<<pthread_id
                                        <<" left_side_id "<< left_side_param
                                        <<" right_side_id "<< right_side_param;

                           if (LINTER_RollBack(linter_connection_id,
                                                NULL, NULL)!=LINAPI_SUCCESS)
                                    message<<" Can`t rollback transaction in trying to update many than 1 channel";

                           throw  objects_storage_exception(message.str());
                          }; // if (rows_count>1)

                       return vector<byte>(0);
           };// if (rows_count!=1)

           if (LINTER_Commit(linter_connection_id,
                            NULL, NULL)!=LINAPI_SUCCESS)
                         throw  objects_storage_exception("Can`t commit transaction ito update 1 channel");

              return prepare_channel_attributes_by_right_side(occupation_answer_data[0].channel_right_side,
                                                                                                occupation_answer_data[0].channel_number,
                                                                                                pthread_id);

          } catch (objects_storage_exception obj_stor_exc) {
                  string message("In database_wrapper::pri_occupate_channel_only(...) catched exception: ");
                  message+=obj_stor_exc.get_description();
                  objects_storage_logger* logger_inst=
                      objects_storage_logger::get_instance();
                  logger_inst->log_message
                    (objects_storage_logger::ERROR_MSG,
                     message);
         };

          return vector<byte>(0);
};

vector<byte> 
         database_wrapper::pri_occupate_channel_any(byte upo_number, 
                                                byte ie1_number,
                                                byte e1_number,
                                                byte channel_interval,
                                                word channelb_number,
                                                pthread_t pthread_id) {

LONG  channeld_id, rows_count, fetched_rows_count, return_code;
LONG upo_distance, ie1_distance, e1_distance, chb_distance;
upo_distance=2*database_settings::IE1_COUNT+
                        2*database_settings::IE1_COUNT*database_settings::E1_COUNT+
                        2*database_settings::IE1_COUNT*database_settings::E1_COUNT*database_settings::CHANNELB_COUNT+
                        1;
ie1_distance=2*database_settings::E1_COUNT+
                        2*database_settings::E1_COUNT*database_settings::CHANNELB_COUNT+
                        1;
e1_distance=2*database_settings::CHANNELB_COUNT+1;
chb_distance=1;

left_side_param=upo_number*upo_distance+ie1_number*ie1_distance+e1_number*e1_distance+channel_interval*chb_distance+upo_number+ie1_number+e1_number+channel_interval+4;
right_side_param=left_side_param+1;

tid_param=pthread_id;
channel_interval_param=channel_interval;
channel_number_param=channelb_number;

//==============delete it==========
/*
{
      ostringstream message;
      message<<"In database_wrapper::pri_occupate_channel_any(...):"
                          <<" upo "<<static_cast<int>(upo_number)
                          <<" ie1 "<<static_cast<int>(ie1_number)
                          <<" e1 "<<static_cast<int>(e1_number)
                          <<" channel_interval "<<static_cast<int>(channel_interval)
                          <<" channelb_number "<<channelb_number
                          <<" pthread_t "<<pthread_id
                          <<" left_side_id "<< left_side_param
                         <<" right_side_id "<< right_side_param;

       objects_storage_logger* logger_inst=
               objects_storage_logger::get_instance();
       logger_inst->log_message
               (objects_storage_logger::INFO_MSG,
                 message.str());
};
*/
//=============================


      try {
      if ((return_code=LINTER_ExecuteStatement(linter_cursor_id,
                                     pri_channel_occupation_statement_1,
                                     NULL, NULL, NULL))!=LINAPI_SUCCESS)
         throw  objects_storage_exception("Can`t execute statement pri_channel_occupation_statement_1"+
                                    database_wrapper::linter_error_decoding (return_code, 
                                                                 linter_connection_id,
                                                                 linter_cursor_id,
                                                                pri_channel_occupation_statement_1));

        size_of_param=sizeof(rows_count);
      if ((return_code=LINTER_GetCursorOption(linter_cursor_id,
                                     cSelectRowCount,
                                     1,
                                     &rows_count,
	                                  &size_of_param))!=LINAPI_SUCCESS)
         throw  objects_storage_exception("Can`t get cursor option LINTER_GetCursorOption"+
                                                         database_wrapper::linter_error_decoding (return_code, 
                                                                 linter_connection_id,
                                                                 linter_cursor_id,
                                                                 0));

               if (rows_count!=1) {
                  if (rows_count>1){ 
                        ostringstream message;
                         message<<"In database_wrapper::pri_occupate_channel_only(...) trying to select many than 1 channel:"
                                        <<" upo "<<static_cast<int>(upo_number)
                                        <<" ie1 "<<static_cast<int>(ie1_number)
                                        <<" e1 "<<static_cast<int>(e1_number)
                                        <<" channel_interval "<<static_cast<int>(channel_interval)
                                        <<" channelb_number "<<channelb_number
                                         <<" pthread_t "<<pthread_id
                                        <<" left_side_id "<< left_side_param
                                        <<" right_side_id "<< right_side_param;

                      throw  objects_storage_exception(message.str());
                  }; //  if (rows_count>1){
                       return vector<byte>(0);
               }; // if (rows_count!=1) {


                  if (LINTER_Fetch(linter_cursor_id,
                       toAbsNumber,  /* direction of travelling     */
                       1,    /* starting position in the selection */
                      rows_count,  /* count of required strings       */
                      &fetched_rows_count,  /* count of gotten strings      */
                      NULL,
                      NULL)!=LINAPI_SUCCESS)
                                 throw  objects_storage_exception("Can`t fetch result of execution statement pri_channel_occupation_statement_1");

                      if (rows_count!=
                           fetched_rows_count)
                              throw  objects_storage_exception("Can`t fetched rows of result of execution statement pri_channel_occupation_statement_1 less than rows in result");

      channel_id_param=occupation_needed_ids[0].channel_id;

     if ((return_code=LINTER_ExecuteStatement(linter_cursor_id,
                                                        pri_channel_occupation_statement_2,
                                                   NULL, NULL, NULL))!=LINAPI_SUCCESS)
                     throw  objects_storage_exception("Can`t execute statement pri_channel_occupation_statement_2"+
                                        database_wrapper::linter_error_decoding (return_code, 
                                                                 linter_connection_id,
                                                                 linter_cursor_id,
                                                                pri_channel_occupation_statement_3));

        size_of_param=sizeof(rows_count);
      if ((return_code=LINTER_GetCursorOption(linter_cursor_id,
                                     cSelectRowCount,
                                     1,
                                     &rows_count,
	                                  &size_of_param))!=LINAPI_SUCCESS)
         throw  objects_storage_exception("Can`t get cursor option LINTER_GetCursorOption"+
                                                         database_wrapper::linter_error_decoding (return_code, 
                                                                 linter_connection_id,
                                                                 linter_cursor_id,
                                                                 0));

           if (rows_count!=1) {
                  if (rows_count>1){ 
                        ostringstream message;
                         message<<"In database_wrapper::pri_occupate_channel_only(...) trying to select many than 1 channel:"
                                        <<" upo "<<static_cast<int>(upo_number)
                                        <<" ie1 "<<static_cast<int>(ie1_number)
                                        <<" e1 "<<static_cast<int>(e1_number)
                                        <<" channel_interval "<<static_cast<int>(channel_interval)
                                        <<" channelb_number "<<channelb_number
                                         <<" pthread_t "<<pthread_id
                                        <<" left_side_id "<< left_side_param
                                        <<" right_side_id "<< right_side_param;

                       throw  objects_storage_exception(message.str());
                  }; //  if (rows_count>1){
                       return vector<byte>(0);
               }; // if (rows_count!=1) {

             if (LINTER_Fetch(linter_cursor_id,
                   toAbsNumber,  /* direction of travelling     */
                   1,    /* starting position in the selection */
                   rows_count,  /* count of required strings       */
                   &fetched_rows_count,  /* count of gotten strings      */
                  NULL,
                  NULL)!=LINAPI_SUCCESS)
                        throw  objects_storage_exception("Can`t fetch result of execution statement pri_channel_occupation_statement_1");

             if (rows_count!=
                      fetched_rows_count)
                          throw  objects_storage_exception("Can`t fetched rows of result of execution statement pri_channel_occupation_statement_1 less than rows in result");

      channel_id_param=occupation_needed_ids[0].channel_id;
      channeld_id=channel_id_param;
     if ((return_code=LINTER_ExecuteStatement(linter_cursor_id,
                                                        pri_channel_occupation_statement_3,
                                                   NULL, NULL, NULL))!=LINAPI_SUCCESS)
                     throw  objects_storage_exception("Can`t execute statement pri_channel_occupation_statement_3 "+
                                        database_wrapper::linter_error_decoding (return_code, 
                                                                 linter_connection_id,
                                                                 linter_cursor_id,
                                                                pri_channel_occupation_statement_3));

        size_of_param=sizeof(rows_count);
      if ((return_code=LINTER_GetCursorOption(linter_cursor_id,
                                     cSelectRowCount,
                                     1,
                                     &rows_count,
	                                  &size_of_param))!=LINAPI_SUCCESS)
         throw  objects_storage_exception("Can`t get cursor option LINTER_GetCursorOption"+
                                                         database_wrapper::linter_error_decoding (return_code, 
                                                                 linter_connection_id,
                                                                 linter_cursor_id,
                                                                 0));

           if (rows_count!=1) {
                  if (rows_count>1){ 
                        ostringstream message;
                         message<<"In database_wrapper::pri_occupate_channel_only(...) trying to select many than 1 channel:"
                                        <<" upo "<<static_cast<int>(upo_number)
                                        <<" ie1 "<<static_cast<int>(ie1_number)
                                        <<" e1 "<<static_cast<int>(e1_number)
                                        <<" channel_interval "<<static_cast<int>(channel_interval)
                                        <<" channelb_number "<<channelb_number
                                         <<" pthread_t "<<pthread_id
                                        <<" left_side_id "<< left_side_param
                                        <<" right_side_id "<< right_side_param;

                         throw  objects_storage_exception(message.str());
                  }; //  if (rows_count>1){
                       return vector<byte>(0);
               }; // if (rows_count!=1) {

             if (LINTER_Fetch(linter_cursor_id,
                   toAbsNumber,  /* direction of travelling     */
                   1,    /* starting position in the selection */
                   rows_count,  /* count of required strings       */
                   &fetched_rows_count,  /* count of gotten strings      */
                  NULL,
                  NULL)!=LINAPI_SUCCESS)
                        throw  objects_storage_exception("Can`t fetch result of execution statement pri_channel_occupation_statement_3");

             if (rows_count!=
                      fetched_rows_count)
                          throw  objects_storage_exception("Can`t fetched rows of result of execution statement pri_channel_occupation_statement_1 less than rows in result");

        channel_id_param=occupation_needed_ids[0].channel_id;
     if ((return_code=LINTER_ExecuteStatement(linter_cursor_id,
                                                        pri_channel_occupation_statement_5,
                                                   NULL, NULL, NULL))!=LINAPI_SUCCESS)
                     throw  objects_storage_exception("Can`t execute statement pri_channel_occupation_statement_5"+
                                        database_wrapper::linter_error_decoding (return_code, 
                                                                 linter_connection_id,
                                                                 linter_cursor_id,
                                                                pri_channel_occupation_statement_3));

        size_of_param=sizeof(rows_count);
      if ((return_code=LINTER_GetCursorOption(linter_cursor_id,
                                     cSelectRowCount,
                                     1,
                                     &rows_count,
	                                  &size_of_param))!=LINAPI_SUCCESS)
         throw  objects_storage_exception("Can`t get cursor option LINTER_GetCursorOption"+
                                                         database_wrapper::linter_error_decoding (return_code, 
                                                                 linter_connection_id,
                                                                 linter_cursor_id,
                                                                 0));

           if (rows_count!=1) {
                  if (rows_count>1){ 
                        ostringstream message;
                         message<<"In database_wrapper::pri_occupate_channel_only(...) trying to select many than 1 channel:"
                                        <<" upo "<<static_cast<int>(upo_number)
                                        <<" ie1 "<<static_cast<int>(ie1_number)
                                        <<" e1 "<<static_cast<int>(e1_number)
                                        <<" channel_interval "<<static_cast<int>(channel_interval)
                                        <<" channelb_number "<<channelb_number
                                         <<" pthread_t "<<pthread_id
                                        <<" left_side_id "<< left_side_param
                                        <<" right_side_id "<< right_side_param;

                         throw  objects_storage_exception(message.str());
                  }; //  if (rows_count>1){
                  //rows_count==0

                channel_id_param=channeld_id;
                if ((return_code=LINTER_ExecuteStatement(linter_cursor_id,
                                                                      pri_channel_occupation_statement_4,
                                                                    NULL, NULL, NULL))!=LINAPI_SUCCESS)
                         throw  objects_storage_exception("Can`t execute statement pri_channel_occupation_statement_4 "+
                                                                 database_wrapper::linter_error_decoding (return_code, 
                                                                 linter_connection_id,
                                                                 linter_cursor_id,
                                                                pri_channel_occupation_statement_3));

                     size_of_param=sizeof(rows_count);
                 if ((return_code=LINTER_GetCursorOption(linter_cursor_id,
                                                                  cSelectRowCount,
                                                                  1,
                                                                  &rows_count,
                                                                  &size_of_param))!=LINAPI_SUCCESS)
                                     throw  objects_storage_exception("Can`t get cursor option LINTER_GetCursorOption"+
                                                                               database_wrapper::linter_error_decoding (return_code, 
                                                                               linter_connection_id,
                                                                               linter_cursor_id,
                                                                               0));

                         if (rows_count!=1) {
                              if (rows_count>1){ 
                                ostringstream message;
                                message<<"In database_wrapper::pri_occupate_channel_only(...) trying to select many than 1 channel:"
                                        <<" upo "<<static_cast<int>(upo_number)
                                        <<" ie1 "<<static_cast<int>(ie1_number)
                                        <<" e1 "<<static_cast<int>(e1_number)
                                        <<" channel_interval "<<static_cast<int>(channel_interval)
                                        <<" channelb_number "<<channelb_number
                                         <<" pthread_t "<<pthread_id
                                        <<" left_side_id "<< left_side_param
                                        <<" right_side_id "<< right_side_param;

                               throw  objects_storage_exception(message.str());
                        }; //  if (rows_count>1){
                            return vector<byte>(0);
                        }; // if (rows_count!=1) {

                    if (LINTER_Fetch(linter_cursor_id,
                                      toAbsNumber,  /* direction of travelling     */
                                      1,    /* starting position in the selection */
                                     rows_count,  /* count of required strings       */
                                    &fetched_rows_count,  /* count of gotten strings      */
                                    NULL,
                                   NULL)!=LINAPI_SUCCESS)
                        throw  objects_storage_exception("Can`t fetch result of execution statement pri_channel_occupation_statement_3");

                    if (rows_count!=
                              fetched_rows_count)
                       throw  objects_storage_exception("Can`t fetched rows of result of execution statement pri_channel_occupation_statement_1 less than rows in result");

                     channel_id_param=occupation_needed_ids[0].channel_id;
                    if ((return_code=LINTER_ExecuteStatement(linter_cursor_id,
                                                            pri_channel_occupation_statement_5,
                                                           NULL, NULL, NULL))!=LINAPI_SUCCESS)
                     throw  objects_storage_exception("Can`t execute statement pri_channel_occupation_statement_5"+
                                        database_wrapper::linter_error_decoding (return_code, 
                                                                 linter_connection_id,
                                                                 linter_cursor_id,
                                                                pri_channel_occupation_statement_3));

                     size_of_param=sizeof(rows_count);
                    if ((return_code=LINTER_GetCursorOption(linter_cursor_id,
                                                   cSelectRowCount,
                                                   1,
                                                   &rows_count,
                                                   &size_of_param))!=LINAPI_SUCCESS)
                              throw  objects_storage_exception("Can`t get cursor option LINTER_GetCursorOption"+
                                                                    database_wrapper::linter_error_decoding (return_code, 
                                                                                             linter_connection_id,
                                                                                             linter_cursor_id,
                                                                                             0));

                         if (rows_count!=1) {
                              if (rows_count>1){ 
                                ostringstream message;
                                message<<"In database_wrapper::pri_occupate_channel_only(...) trying to select many than 1 channel:"
                                        <<" upo "<<static_cast<int>(upo_number)
                                        <<" ie1 "<<static_cast<int>(ie1_number)
                                        <<" e1 "<<static_cast<int>(e1_number)
                                        <<" channel_interval "<<static_cast<int>(channel_interval)
                                        <<" channelb_number "<<channelb_number
                                         <<" pthread_t "<<pthread_id
                                        <<" left_side_id "<< left_side_param
                                        <<" right_side_id "<< right_side_param;

                               throw  objects_storage_exception(message.str());
                        }; //  if (rows_count>1){
                            return vector<byte>(0);
                        }; // if (rows_count!=1) {

               }; // if (rows_count!=1) {

             if (LINTER_Fetch(linter_cursor_id,
                   toAbsNumber,  /* direction of travelling     */
                   1,    /* starting position in the selection */
                   rows_count,  /* count of required strings       */
                   &fetched_rows_count,  /* count of gotten strings      */
                  NULL,
                  NULL)!=LINAPI_SUCCESS)
                        throw  objects_storage_exception("Can`t fetch result of execution statement pri_channel_occupation_statement_1");

             if (rows_count!=
                      fetched_rows_count)
                          throw  objects_storage_exception("Can`t fetched rows of result of execution statement pri_channel_occupation_statement_1 less than rows in result");

            channel_id_param=occupation_answer_data[0].channel_id;

           if ((return_code=LINTER_ExecuteStatement(linter_cursor_id,
                                                   pri_channel_occupation_statement_6,
                                                   NULL, NULL, NULL))!=LINAPI_SUCCESS)
                            throw  objects_storage_exception("Can`t execute statement pri_channel_occupation_statement_6"+
                                         database_wrapper::linter_error_decoding (return_code, 
                                                                 linter_connection_id,
                                                                 linter_cursor_id,
                                                                pri_channel_occupation_statement_3));

                        size_of_param=sizeof(rows_count);
            if (LINTER_GetCursorOption(linter_cursor_id,
                                                            cExecRow,
                                                            1,
                                                            &rows_count,
                                                            &size_of_param)!=LINAPI_SUCCESS)
                                 throw  objects_storage_exception("Can`t get cursor option LINTER_GetCursorOption ");

            if (rows_count!=1) {
                  if (rows_count>1){ 
                              ostringstream message;
                              message<<"In database_wrapper::pri_occupate_channel_only(...) trying to update many than 1 channel:"
                                        <<" upo "<<static_cast<int>(upo_number)
                                        <<" ie1 "<<static_cast<int>(ie1_number)
                                        <<" e1 "<<static_cast<int>(e1_number)
                                        <<" channel_interval "<<static_cast<int>(channel_interval)
                                        <<" channelb_number "<<channelb_number
                                         <<" pthread_t "<<pthread_id
                                        <<" left_side_id "<< left_side_param
                                        <<" right_side_id "<< right_side_param;

                           if (LINTER_RollBack(linter_connection_id,
                                                NULL, NULL)!=LINAPI_SUCCESS)
                                    message<<" Can`t rollback transaction in trying to update many than 1 channel";

                           throw  objects_storage_exception(message.str());
                          }; // if (rows_count>1)

                       return vector<byte>(0);
           };// if (rows_count!=1)

           if (LINTER_Commit(linter_connection_id,
                            NULL, NULL)!=LINAPI_SUCCESS)
                         throw  objects_storage_exception("Can`t commit transaction ito update 1 channel");

              return prepare_channel_attributes_by_right_side(occupation_answer_data[0].channel_right_side,
                                                                                                occupation_answer_data[0].channel_number,
                                                                                                pthread_id);

          } catch (objects_storage_exception obj_stor_exc) {
                  string message("In database_wrapper::pri_occupate_channel_only(...) catched exception: ");
                  message+=obj_stor_exc.get_description();
                  objects_storage_logger* logger_inst=
                      objects_storage_logger::get_instance();
                  logger_inst->log_message
                    (objects_storage_logger::ERROR_MSG,
                     message);
         };

          return vector<byte>(0);
}; 


bool 
        database_wrapper::pri_free_channel(byte upo_number, 
                                     byte ie1_number,
                                     byte e1_number,
                                     byte channel_interval,
                                     word channelb_number,
                                     pthread_t  pthread_id){
bool return_value=false;
LONG rows_count, return_code;
LONG upo_distance, ie1_distance, e1_distance, chb_distance;
upo_distance=2*database_settings::IE1_COUNT+
                        2*database_settings::IE1_COUNT*database_settings::E1_COUNT+
                        2*database_settings::IE1_COUNT*database_settings::E1_COUNT*database_settings::CHANNELB_COUNT+
                        1;
ie1_distance=2*database_settings::E1_COUNT+
                        2*database_settings::E1_COUNT*database_settings::CHANNELB_COUNT+
                        1;
e1_distance=2*database_settings::CHANNELB_COUNT+1;
chb_distance=1;

left_side_param=upo_number*upo_distance+ie1_number*ie1_distance+e1_number*e1_distance+channel_interval*chb_distance+upo_number+ie1_number+e1_number+channel_interval+4;
right_side_param=left_side_param+1;

 tid_param=pthread_id;

      try {
      if (LINTER_ExecuteStatement(linter_cursor_id,
                                     pri_channel_free_statement_1,
                                     NULL, NULL, NULL)!=LINAPI_SUCCESS)
         throw  objects_storage_exception("Can`t execute statement pri_channel_free_statement_1"+
                                                                database_wrapper::linter_error_decoding (return_code, 
                                                                 linter_connection_id,
                                                                 linter_cursor_id,
                                                                 pri_channel_free_statement_1));

      size_of_param=sizeof(rows_count);
      if (LINTER_GetCursorOption(linter_cursor_id,
                                     cExecRow,
                                     1,
                                     &rows_count,
                                     &size_of_param)!=LINAPI_SUCCESS)
         throw  objects_storage_exception("Can`t get cursor option LINTER_GetCursorOption");

        if (rows_count==1) {
                         if (LINTER_Commit(linter_connection_id,
                              NULL, NULL)!=LINAPI_SUCCESS)
                                   throw  objects_storage_exception("Can`t commit transaction in update one channel");
                          return_value=true;
                } else if (rows_count>1){ //if (rows_count==1) {
                        ostringstream message;
                         message<<"In database_wrapper::pri_free_channel(...) trying to update many than 1 channel:"
                                        <<" upo "<<static_cast<int>(upo_number)
                                        <<" ie1 "<<static_cast<int>(ie1_number)
                                        <<" e1 "<<static_cast<int>(e1_number)
                                        <<" channel_interval "<<static_cast<int>(channel_interval)
                                        <<" channelb_number "<<channelb_number
                                         <<" pthread_t "<<pthread_id
                                        <<" left_side_id "<< left_side_param
                                        <<" right_side_id "<< right_side_param;

                        objects_storage_logger* logger_inst=
                                objects_storage_logger::get_instance();
                        logger_inst->log_message
                                (objects_storage_logger::ERROR_MSG,
                                 message.str());

                         if (LINTER_RollBack(linter_connection_id,
                              NULL, NULL)!=LINAPI_SUCCESS)
                                   throw  objects_storage_exception("Can`t rollback transaction in trying to update many than 1 channel");
                }; // }else if (rows_count>1){
                // if (rows_count==0) return return_value==false
          } catch (objects_storage_exception obj_stor_exc) {
                  string message("In  database_wrapper::pri_free_channel(...)  catched exception: ");
                  message+=obj_stor_exc.get_description();
                  objects_storage_logger* logger_inst=
                      objects_storage_logger::get_instance();
                  logger_inst->log_message
                    (objects_storage_logger::ERROR_MSG,
                     message);
         };


return return_value;
};

