#include <stdio.h>
#include <functional>
#include <vector>
#include <sstream>

#include <time.h>
#include <inttypes.h>

#include "kgtsdb.h"
#include "initdb.h"
#include "handlers.h"

using namespace std;

#include "storage_common.h"
#include "objects_storage_exception.h"
#include "objects_storage_logger.h"



#include "extreamedb_database_wrapper.h"

//static char state_random[32];

int database_wrapper::upo_count=4;
int database_wrapper::ie1_count=16;
int database_wrapper::e1_count=16;
int database_wrapper::channelb_count=32;
int database_wrapper::max_channel_number=4096;

#define PAGE_SIZE                            (uint2)128
#define DATABASE_SIZE                    1024*1024*20


char database_memory[ DATABASE_SIZE ];

char          * dbname = "kgtsdb";


/* Fatal error trap */
static void errhandler( int n )
{
  fprintf(stderr, "\n*** eXtremeDB runtime fatal error: %d\n", n );
  exit( -1 );
}

database_wrapper& 
          database_wrapper::get_instance() throw (objects_storage_exception){
    static database_wrapper dbwrapper_instance;
    return dbwrapper_instance;
};

database_wrapper::database_wrapper() throw (objects_storage_exception) :
          counter(0)
{
    MCO_RET           rc;	
    mco_runtime_info_t info;

    mco_get_runtime_info( &info);
     /* initialize eXtremeDB runtime */
     mco_runtime_start();
     /* setup fatal error handler */
     mco_error_set_handler( &errhandler );

    /* create the database */
    if ( MCO_S_OK != (rc=mco_db_open( dbname, kgtsdb_get_dictionary(), database_memory, DATABASE_SIZE,  PAGE_SIZE )) ) {
       printf( "Unable to open a database. Error code: %d\n", rc ); fflush( stdout );
        throw objects_storage_exception ("Unable to open a database." );
     };

	if ( MCO_S_OK != (rc=mco_db_connect( dbname, &db )) ) {
	    printf( "Unable to connect to the database. Error code: %d\n", rc ); fflush( stdout );
         throw objects_storage_exception (  "Unable to connect to the database" );
      };      
      if (MCO_S_OK!= register_events(db)) {
          printf("Error registring evenets, %d\n", rc);
      }    
       if ( 0 != load_database(db) ) {
           printf( "Unable to load the database.\n" ); fflush( stdout );
              throw objects_storage_exception ( "Unable to load the database" )    ;
        };
};



 database_wrapper::~database_wrapper() {
	MCO_RET           rc;
	unregister_events(db);
	
	/* disconnect from the database */
	if ( MCO_S_OK != (rc = mco_db_disconnect( db ))) {
		printf("mco_db_disconnect( db ) = %d\n", rc );
	};
	/* Close and shutdown the database */
	mco_db_close( dbname );
	/* Shutdown eXtremeDB engine */
	mco_runtime_stop( );

}

// call control metods
bytes database_wrapper::call_control_occupate_channel_side_a_only(  uint1 upo_number,	
                                                uint1 ie1_number,
                                                uint1 e1_number,
                                                uint1 interval, 
								        uint2 channel_b_number, 
									   uint2 uid,  
								       uint2 tid ) {
								       
									
	MCO_RET           rc;
	mco_trans_h       t;
	CHANNEL_B channel_b;
     uint4 channel_b_id;
    
     bool channel_ocupation_seccessfull=false;
     
     vector<byte> return_buffer(0);

	if ((rc = mco_trans_start(db, MCO_READ_WRITE, MCO_TRANS_FOREGROUND, &t)) == MCO_S_OK) {
	
          if(   find_channel_b_id_by_coordinates(  t, upo_number,  ie1_number,   e1_number, interval,  &channel_b_id   ) ) { 
		
   		     if(  find_channel_d_for_channel_b_only( t, channel_b_id,   channel_b_number ,   &channel_b ) )  {                  
		 			          					                            
				       // Занимаем канал
				      CHANNEL_B_tid_put( &channel_b, tid );
				      CHANNEL_B_userid_put ( &channel_b, uid );
				      channel_ocupation_seccessfull = true;
			}	 
		}           					        
		/* commit the transaction */
		rc=mco_trans_commit(t);
	} 
     else {
		fprintf( stderr, "Can't open a transaction. Error code: %d\n", rc); fflush(stderr);
	};
	/*
	Канал B занят успешно:
	номер байта       |    содержимое
	----------------------|---------------------
		1                     |      UPO   координата канала В
		2                     |       IE1    координата канала В
		3                     |       IE      координата канала В
		4				  |       KI      координата канала В

	if error return empty buffer
*/
  if( channel_ocupation_seccessfull ) {
    		    
    		    bytes temp_buffer =   get_channel_b_coordinates( channel_b_id );
    		    return_buffer.insert(   return_buffer.end(),  
										 temp_buffer.begin(),
                               				       temp_buffer.end() );
	}
    return return_buffer;
};

bytes database_wrapper::call_control_occupate_channel_side_a_any(	
                                                uint1 upo_number,	
                                                uint1 ie1_number,
                                                uint1 e1_number,
                                                uint1 interval, 
									uint2 channel_b_number, 
								     uint2 uid,  
									uint2 tid )  {
	MCO_RET           rc;
	mco_trans_h       t;
	CHANNEL_B channel_b;
     uint4 channel_b_id;
  //   uint4 result_channel_b_id_for_channel_d ;
     
     bool channel_ocupation_seccessfull=false;
     
     vector<byte> return_buffer(0);

	if ((rc = mco_trans_start(db, MCO_READ_WRITE, MCO_TRANS_FOREGROUND, &t)) == MCO_S_OK) {
	
          if(   find_channel_b_id_by_coordinates(  t, upo_number,  ie1_number,   e1_number, interval,  &channel_b_id   ) ) { 
		
   		     if(  find_channel_d_for_channel_b_any( t, channel_b_id,   &channel_b_number ,   &channel_b ) )  {                  
				      CHANNEL_B_tid_put( &channel_b, tid );
				      CHANNEL_B_userid_put ( &channel_b, uid );
				      channel_ocupation_seccessfull = true;
			}	 
		}           					        
		/* commit the transaction */
		rc=mco_trans_commit(t);
	} 
     else {
		fprintf( stderr, "Can't open a transaction. Error code: %d\n", rc); fflush(stderr);
	};
	/*
	Канал B занят успешно:
	номер байта       |    содержимое
	----------------------|---------------------
		1                     |      UPO   координата канала В
		2                     |       IE1    координата канала В
		3                     |       IE      координата канала В
		4				  |       KI      координата канала В

	if error return empty buffer
*/
  if( channel_ocupation_seccessfull ) {
    		    
    		    bytes temp_buffer =   get_channel_b_coordinates( channel_b_id );
    		    return_buffer.insert(   return_buffer.end(),  
										 temp_buffer.begin(),
                               				       temp_buffer.end() );
	}
    return return_buffer;

};


bytes database_wrapper::call_control_occupate_channel_side_b(
									int destination_p,		
                                               uint1 mg,  
									uint1 priority,
                                                uint1 isdn, 
									uint1  channels_count,
									uint2 uid, 
									uint2  tid)																																																						
 {
    
    uint4 destination = static_cast<uint1>( destination_p);		
	
    MCO_RET           rc;
    mco_trans_h       t;
    mco_cursor_t     c, c_tr_grp, destination_cursor;
    TRUNK_GROUP trunk_group;
    bool channel_ocupation_seccessfull = false;
    uint4 channel_b_id;
   uint2 result_channel_b_number;
   uint4 result_channel_b_id_for_channel_d;
   
	/* begin a read-write transaction */
	if ((rc = mco_trans_start(db, MCO_READ_WRITE, MCO_TRANS_FOREGROUND, &t)) == MCO_S_OK) {
 			
 			  DIRECTION_DESTINATION_by_id_destination_index_cursor( t, &destination_cursor );
                 if(  MCO_S_OK ==  ( rc= 
                       DIRECTION_DESTINATION_by_id_destination_search( t, &destination_cursor, MCO_EQ, destination ) ) )   {
                       do {
                           DIRECTION_DESTINATION dir_dest;
            	           DIRECTION_DESTINATION_from_cursor( t, &destination_cursor, &dir_dest);
                           uint4 temp_dest;
                           DIRECTION_DESTINATION_id_destination_get( &dir_dest, &temp_dest );
                           if ( temp_dest != destination) break;
                           uint4 direction;
                           DIRECTION_DESTINATION_id_direction_get ( &dir_dest, &direction ); 
 
 
                           TRUNK_GROUP_by_direction_index_cursor (  t,  &c );
 			  
     	                if ( MCO_S_OK ==  ( rc=   TRUNK_GROUP_by_direction_search(  t, &c, MCO_EQ, direction ) ) ) {
	                         do {
     					   TRUNK_GROUP_from_cursor( t, &c, &trunk_group );
						   uint4 temp_direction;
     					   TRUNK_GROUP_id_direction_get  ( &trunk_group, &temp_direction );
     	
 						   if( temp_direction != direction ) break;
  						   // Выбираем исходящие или двунаправленные
     					   uint1 inout ;
  						   TRUNK_GROUP_inout_get ( &trunk_group, &inout );
                                   if ( ! ( inout == 2 || inout == 3 )  )  {
								continue;             						                    
                                  }
   						  // Анализируем выбрабнную транкгруппу			
						
						  /// Отбрасывает перегруженные для приоритетных вызовов
						  if( priority == 0 )  {
						     uint1 overuse;
							TRUNK_GROUP_overuse_get ( &trunk_group,  &overuse );
							if ( overuse != 0 ) continue;
						  }
						  /// Отбрасывает транки не поддерживающие isdn

						 if( isdn == 3 )  {
						     	uint1 isdn_temp;
							TRUNK_GROUP_isdn_get ( &trunk_group,  &isdn_temp );
							if ( isdn_temp  ==0 ) continue;
						 }
						 uint1 slzsl;
						 TRUNK_GROUP_slzsl_get ( & trunk_group, &slzsl ); 
						 // требуется междугородняя
						 if ( (mg == 1 ) &&( ! (  slzsl  == 2 ||  slzsl == 3 )   ) )  {
								continue;             						                    
                                 } 
                                 // требуется местная 
						if ( (mg == 0 ) && ( ! (  slzsl  == 1 ||  slzsl == 3 )   ) )  {
								continue;             						                    
                             }
						uint4 last_channel_b;
						TRUNK_GROUP_last_occupied_channel_b_get ( &trunk_group, &last_channel_b );
						if(  last_channel_b == 0 ) 
						    TRUNK_GROUP_first_channel_b_id_get(  &trunk_group, &last_channel_b); 
						
                              uint1	trunk_group_id ;
						TRUNK_GROUP_id_trunk_group_get  ( &trunk_group, &trunk_group_id  ); 
					     uint4	max_channel_b_id_for_trunk;
						TRUNK_GROUP_max_channel_b_id_get ( &trunk_group, &max_channel_b_id_for_trunk);  	
						CH_B_TRUNK_GR_by_id_trunk_gr_id_channel_b_index_cursor  ( t,  &c_tr_grp);
						
							
						if ( CH_B_TRUNK_GR_by_id_trunk_gr_id_channel_b_search ( t, &c_tr_grp, MCO_EQ,  trunk_group_id, last_channel_b ) == MCO_S_OK ) 
						
						do {
							    CH_B_TRUNK_GR ch_b_trunk_gr;	
                                        CH_B_TRUNK_GR_from_cursor (  t, &c_tr_grp, & ch_b_trunk_gr  );
                                        
                                         uint1 trunk_id_temp;
                                         CH_B_TRUNK_GR_id_trunk_gr_get (  &ch_b_trunk_gr, &trunk_id_temp );
                                         if ( trunk_id_temp != trunk_group_id ) break;
                                         
                                        
                                        CH_B_TRUNK_GR_id_channel_b_get ( &ch_b_trunk_gr, &channel_b_id );
                                        
                                      //  fprintf( stderr, "  channel_b_id : %d max_channel_b_id_for_trunk: %d  last_channel_b: %d \n ", channel_b_id ,  max_channel_b_id_for_trunk,  last_channel_b  ); fflush(stderr);                      
                                        if ( channel_b_id > last_channel_b ) 
                                         {  
		                                        CHANNEL_B channel_b;
		                                        if ( MCO_S_OK == CHANNEL_B_by_id_channel_b_tid_find ( t, channel_b_id, 0, &channel_b ) ) {
		                                             // Ищем канал D
		                                             CHANNEL_B b_for_d;
		                                             if(  find_channel_d_for_channel_b_any( t, channel_b_id,   &result_channel_b_number ,   &b_for_d ) )  {                  
        											 CHANNEL_B_id_channel_b_get( &b_for_d, &result_channel_b_id_for_channel_d );
											 }
											CHANNEL_B_tid_put( &channel_b, tid );	
                								CHANNEL_B_userid_put ( &channel_b, uid );				
											channel_ocupation_seccessfull = true;
											TRUNK_GROUP_last_occupied_channel_b_put ( &trunk_group, channel_b_id );
											break;
									           //}		 
									   } //      if ( MCO_S_OK == CHANNEL_B_by_id_channel_b_tid_find ( t, channel_b_id, 0, &channel_b ) ) 
							   }  //  if ( channel_b_id > last_channel_b ) 
							     if ( channel_b_id == max_channel_b_id_for_trunk )  {
										 TRUNK_GROUP_first_channel_b_id_get(  &trunk_group, & last_channel_b); 
										 if( CH_B_TRUNK_GR_by_id_trunk_gr_id_channel_b_search ( t, &c_tr_grp, MCO_EQ,  trunk_group_id, last_channel_b ) == MCO_S_OK ) 
										                 continue;
													  
 							  }						 
               
                                        
						}  while ( MCO_S_OK == mco_cursor_next( t, &c_tr_grp ) ) ; 
      			      }  while ( MCO_S_OK ==  mco_cursor_next( t, &c ) ) ;
	            }  //    if ( MCO_S_OK ==  ( rc=   TRUNK_GROUP_by_direction_search(  t, &c, MCO_EQ, direction ) ) ) 
	            else {
					fprintf( stderr, "No trunkgroup for direction. Direction: %d\n", direction); fflush(stderr);
			};
			if( channel_ocupation_seccessfull) {
                        break;
               }         
			
		}    while ( MCO_S_OK ==  mco_cursor_next( t, &destination_cursor ) ) ;	
      } //                  if(  MCO_S_OK ==  ( rc= 
       //                DIRECTION_DESTINATION_by_id_destination_search( t, &destination_cursor, MCO_EQ, destination ) ) )   {
      
	 /* commit the transaction */
      rc=mco_trans_commit(t);
	
	}
	else {
		fprintf( stderr, "Can't open a transaction. Error code: %d\n", rc); fflush(stderr);
	};
      
      
      /*
Канал B занят успешно:
номер байта       |    содержимое
----------------------|---------------------
	1                     |   UPO   координата канала В
	2                     |   IE1    координата канала В
	3                     |   IE      координата канала В
	4				  |   KI      координата канала В
	5				  |	 Номер канала B
	6                     |   
     7                     |   Признак ОКСа 
     8                     |   УПО канала D
     9                     |   IE1  канала  
     10        	       |   E1 канала D
     11                   |   Номер канала D 
  
	if error return empty buffer
*/
  bytes return_buffer;
  if( channel_ocupation_seccessfull ) {
    		      		    
    		    bytes temp_buffer =   get_channel_b_coordinates( channel_b_id);
    		    return_buffer.insert(   return_buffer.end(),  
										 temp_buffer.begin(),
                               				       temp_buffer.end() );
             
    		   temp_buffer = bytes_of_type<uint2>( result_channel_b_number );

	 	   return_buffer.insert(return_buffer.end(),
                                      temp_buffer.begin(),
                                      temp_buffer.end() );

             return_buffer.push_back( 0 ); /// Признак ОКСа             

  	  	  temp_buffer =   get_channel_b_coordinates( result_channel_b_id_for_channel_d );

   	 	  return_buffer.insert(     return_buffer.end(),  
										 temp_buffer.begin(),
                               				       temp_buffer.end() );
	}
    return return_buffer;



};

bytes database_wrapper::call_control_occupate_call_pointer(
									 uint1 upo_number, 
                                                uint1 ie1_number,
                                                uint1 e1_number,
                                                uint1  interval)
{

	
	MCO_RET           rc;
	mco_trans_h       t;
	mco_cursor_t c;
	CH_B_D ch_b_d;
	uint4 channel_b_id;
     uint4 channel_d_id;
     uint2 call_pointer = 0;
     
     
   	/* begin a read-write transaction */
	if ((rc = mco_trans_start(db, MCO_READ_WRITE, MCO_TRANS_FOREGROUND, &t)) == MCO_S_OK) {
           
           /* get cursors by indexes */
      	CH_B_D_by_id_chanel_b_numb_b_index_cursor( t, &c ); 
      	
		/* find the first object with id specified */
		 if(   find_channel_b_id_by_coordinates(  t, upo_number,  ie1_number,   e1_number, interval,  &channel_b_id   ) ) { 
	             if ( MCO_S_OK ==  ( rc=CH_B_D_by_id_chanel_b_numb_b_search( t, &c,  MCO_EQ, channel_b_id, 0  ) ) )  {
                         // get channel D for chnannel B      					        
		              CH_B_D_from_cursor (  t, &c, &ch_b_d );
		              CH_B_D_id_channel_d_get ( &ch_b_d, &channel_d_id );
		           					       		 			          					       		           					                  
		             CHANNEL_D channel_d;
		             if ( MCO_S_OK ==  ( rc=CHANNEL_D_by_channel_d_hash_find(t, channel_d_id, &channel_d) ) )  {
 					 uint4 last_call_pointer;
					 uint4 max_call_pointer;
					 uint1 current_call_pointer_value;
					 uint2 current_call_pointer_index = 0;
					          
                  		bool pointer_not_found = true;
		   			   
     				CHANNEL_D_max_call_pointer_get (  &channel_d, &max_call_pointer );
					CHANNEL_D_last_call_pointer_get (   &channel_d, &last_call_pointer );
					if(  last_call_pointer >= max_call_pointer )  {
						last_call_pointer = 1;
					 }
					for( current_call_pointer_index  = last_call_pointer; current_call_pointer_index <= max_call_pointer; 
                                  current_call_pointer_index++ ) {
                                     CHANNEL_D_call_pointers_at ( &channel_d, current_call_pointer_index - 1, &current_call_pointer_value ); 
		                           if ( current_call_pointer_value == 0 ) {
		                              call_pointer = current_call_pointer_index;
			                         CHANNEL_D_call_pointers_put ( &channel_d, call_pointer  - 1, 1 );  
                                        CHANNEL_D_last_call_pointer_put(   &channel_d, call_pointer );
				                   pointer_not_found = false;
				                   break;
				                }              
	                       }
		                  // Просматриваем занятые укзатели вызова с нуля
		                  if( pointer_not_found) 
	                       for( current_call_pointer_index  = 1; current_call_pointer_index <= max_call_pointer; 
                                                            current_call_pointer_index++ ) {
			                CHANNEL_D_call_pointers_at ( &channel_d, current_call_pointer_index - 1, &current_call_pointer_value ); 
			                if ( current_call_pointer_value == 0 ) {
				              call_pointer = current_call_pointer_index ;
			                   CHANNEL_D_call_pointers_put ( &channel_d, call_pointer  - 1, 1 );  
		                        CHANNEL_D_last_call_pointer_put(   &channel_d, call_pointer );
				              pointer_not_found = false;
			                }              
			            }
			}           
		}
		 else {
			fprintf( stderr, "Can't find specified channel in . Error code: %d\n", rc); fflush(stderr);
		};

		} // if(   find_channel_b_id_by_coordinates(  t, upo_number,  ie1_number,   e1_number, interval,  &channel_b_id   ) )  
		/* commit the transaction */
		rc=mco_trans_commit(t);

	} else {
		fprintf( stderr, "Can't open a transaction. Error code: %d\n", rc); fflush(stderr);
	};
	/*
	Указатель вызова занят:
		номер байта       |    содержимое
	----------------------|---------------------
		1                     |      CP    указатель вызова мл.
		2                     ¦      CP    указатель вызова ст.
	if error return empty buffer
	*/
      bytes return_buffer;
  	if( call_pointer != 0 ) {
    		    
     		     bytes  type_bytes;
	    		    type_bytes=bytes_of_type<uint2>(call_pointer);

	 		   return_buffer.insert(return_buffer.end(),
                                      type_bytes.begin(),
                                      type_bytes.end() );
	
	}
    return return_buffer;

}


bool database_wrapper::call_control_free_channel(
											uint1 upo_number, uint1 ie1_number, uint1 e1_number , uint1 interval  ) {

	MCO_RET           rc;
	mco_trans_h      t;
	CHANNEL_B channel_b;
	uint4 channel_b_id;
     bool result = false;
	/* begin a read-write transaction */
	if ((rc = mco_trans_start(db, MCO_READ_WRITE, MCO_TRANS_FOREGROUND, &t)) == MCO_S_OK) {

		/* find the first object with id specified */
		 if(   find_channel_b_id_by_coordinates(  t, upo_number,  ie1_number,   e1_number, interval,  &channel_b_id   ) ) { 
	           if ( MCO_S_OK == ( rc=CHANNEL_B_by_id_channel_b_hash_find(t, channel_b_id, &channel_b  ) ) )  {
           	   CHANNEL_B_tid_put(  &channel_b, 0  );
                   CHANNEL_B_userid_put( &channel_b, 0 );	
           	   result = true;
           	}
		} 
		/* commit the transaction */
		rc=mco_trans_commit(t);

	} else { fprintf( stderr, "Can't open a transaction in call_control_free_channel . Error code: %d\n", rc); fflush(stderr); };

     return result;

}

bool database_wrapper::call_control_free_call_pointer(
									uint1 upo_number, 
                                                uint1  ie1_number,
                                                uint1  e1_number,
                                                uint1  interval,		
									uint2 call_pointer	
                                             )

{
	MCO_RET           rc;
	mco_trans_h       t;
	mco_cursor_t c;
	CH_B_D ch_b_d;
     uint4 channel_b_id;
     uint4 channel_d_id;
     bool result = false;
     
   	/* begin a read-write transaction */
	if ((rc = mco_trans_start(db, MCO_READ_WRITE, MCO_TRANS_FOREGROUND, &t)) == MCO_S_OK) {
           
           /* get cursors by indexes */
      	CH_B_D_by_id_chanel_b_numb_b_index_cursor( t, &c ); 
      	
		/* find the first object with id specified */
		 if(   find_channel_b_id_by_coordinates(  t, upo_number,  ie1_number,   e1_number, interval,  &channel_b_id   ) )  { 
                 if ( MCO_S_OK ==  ( rc=CH_B_D_by_id_chanel_b_numb_b_search( t, &c,  MCO_EQ, channel_b_id, 0  ) ) )   {
                       // get channel D for chnannel B      					        
                       CH_B_D_from_cursor (  t, &c, &ch_b_d );
		            CH_B_D_id_channel_d_get ( &ch_b_d, &channel_d_id );
		          
                      CHANNEL_D channel_d;
		           if ( MCO_S_OK ==  ( rc=CHANNEL_D_by_channel_d_hash_find(t, channel_d_id, &channel_d) ) )  {
                              uint4 max_call_pointer; 
		   			   CHANNEL_D_max_call_pointer_get (  &channel_d, &max_call_pointer );
					   if(  call_pointer > max_call_pointer )  {
                                    // Oшибка
                                    result = false;
					   }
					   else {
					       CHANNEL_D_call_pointers_put ( &channel_d, call_pointer  - 1, 0 );  
						  result = true; 
			               }       
				 }
     		       else {  fprintf( stderr, "Can't find specified channel_d in CHANNEL_D. Error code: %d\n", rc); fflush(stderr);   }       
  		       }
           	  else {  fprintf( stderr, "Can't find specified channel_d in CH_B_D. Error code: %d\n", rc); fflush(stderr);   }       
          }
		/* commit the transaction */
		rc=mco_trans_commit(t);

	} else {
		fprintf( stderr, "Can't open a transaction. Error code: %d\n", rc); fflush(stderr);
	};
      return result;

};

//proc control metods
bytes  database_wrapper::proc_control_check_state_b(byte upo_number,
                                     byte ie1_number,
                                     byte e1_number,
                                     byte interval ){
                                     
                                     
/*
unknown command don`t touch taht 
*/
bytes ret_buffer;
if (counter>2048) counter=0;
if ((counter++%7)==0) return ret_buffer;

ret_buffer.push_back(0);
return ret_buffer;
};


bytes  database_wrapper::get_channel_b_coordinates( uint4 id ) {

	MCO_RET           rc;	
	mco_trans_h       t;

	uint1             upo_number;
	uint1             ie1_number;
	uint1             e1_number;
	uint1            	 ki_number;

	/* begin a read-only transaction */
	if ((rc = mco_trans_start(db, MCO_READ_ONLY, MCO_TRANS_FOREGROUND, &t)) == MCO_S_OK) {

	      CHANNEL_B channel_b;
	      uint2 e1_id;
		 if (  MCO_S_OK == CHANNEL_B_by_id_channel_b_hash_find ( t, id, &channel_b ) ) {
			 CHANNEL_B_nki_get(  &channel_b, &ki_number  );			
			 CHANNEL_B_id_e1_get (  &channel_b, &e1_id  );			 
		 }
		E1 e1; 
		uint1 ie1_id;
		if (  MCO_S_OK ==E1_by_id_e1_find( t, e1_id, &e1 ) ) {
					E1_number_get( &e1, &e1_number );   
					E1_id_ie1_get( &e1, &ie1_id );
		}
 		IE1 ie1;
 		uint1 upo_id;
		if (  MCO_S_OK ==IE1_by_id_ie1_find( t, ie1_id, &ie1 ) ) {
					IE1_number_get( &ie1, &ie1_number );   
					IE1_id_upo_get( &ie1, &upo_id );
		}
		UPO upo;
		if (  MCO_S_OK ==UPO_by_id_upo_hash_find ( t, upo_id, &upo  ) ) {
					 UPO_number_get( &upo, &upo_number );
		}
		rc=mco_trans_commit(t);
	} else {
		printf( "Can't open a transaction. Error code: %d\n", rc);
	};
	bytes return_buffer(0);
	return_buffer.push_back( upo_number );
	return_buffer.push_back( ie1_number );	
     return_buffer.push_back( e1_number );
     return_buffer.push_back( ki_number  );
	return return_buffer;

};

bool database_wrapper::find_channel_d_for_channel_b_any (  mco_trans_h t, uint4 id_channel_b, /*OUT*/ uint2* channel_b_number,
                                                                                                                    /*OUT*/ CHANNEL_B *channel_b_id_for_d   )  {
     return find_channel_d_for_channel_b (  t,  id_channel_b,  0, channel_b_number,  channel_b_id_for_d, false   ) ;
}

bool database_wrapper::find_channel_d_for_channel_b_only (  mco_trans_h t, uint4 id_channel_b,  uint2 required_channel_b_number, 
                                                                                                                     /*OUT*/ CHANNEL_B *channel_b_id_for_d   )  {
     uint2 channel_b_mock;
     return find_channel_d_for_channel_b (  t,  id_channel_b,  required_channel_b_number, &channel_b_mock, channel_b_id_for_d, true   ) ;
}

bool database_wrapper::find_channel_d_for_channel_b (  mco_trans_h t, uint4 id_channel_b, uint2 required_channel_b,  
                                                                                                                     /*OUT*/ uint2* channel_b_number, /*OUT*/ CHANNEL_B * channel_b_id_for_d, bool only   )  {
   CH_B_D ch_b_d;
   bool result = false; 
   MCO_RET rc;
   uint4 channel_b_id;
   
   if ( MCO_S_OK ==  ( rc=CH_B_D_by_id_chanel_b_numb_b_find ( t,   id_channel_b, 0, &ch_b_d  ) ) )  {
        uint4 ch_d_id;
	  CH_B_D_id_channel_d_get(    &ch_b_d, &ch_d_id ); 
        mco_cursor_t c;
	  CH_B_D_by_id_chanel_d_index_cursor( t, &c );
	  if ( MCO_S_OK ==  ( rc=CH_B_D_by_id_chanel_d_search( t, &c,  MCO_EQ, ch_d_id  ) )) {
		 do {				
 	            CH_B_D_from_cursor ( t, &c, &ch_b_d );
			  uint2 num_b_temp;
			  CH_B_D_numb_b_get( &ch_b_d, &num_b_temp);
                  CH_B_D_id_channel_b_get(&ch_b_d, &channel_b_id ) ;
                  uint4 channel_d_id;
                  CH_B_D_id_channel_d_get(&ch_b_d, &channel_d_id ) ;
                  if( channel_d_id != ch_d_id )  break;
			  
			  if(  (num_b_temp != 0 && (!only) ) || ( num_b_temp == required_channel_b && only)   ) {
			           CH_B_D_id_channel_b_get(&ch_b_d, &channel_b_id ) ;
			           
				    	 // Проверяем свободен ли этот канал  
                           if ( MCO_S_OK == CHANNEL_B_by_id_channel_b_tid_find ( t, channel_b_id, 0, channel_b_id_for_d ) ) {			           
					   	 *channel_b_number = num_b_temp;
				 		 result =  true;				    	
			                 break;
			             } else if( only) break;    
			   } //  if( num_b_temp != 0 ) {
            }    while (   MCO_S_OK == mco_cursor_next( t, &c ) )	 ;
		 }   //  if ( MCO_S_OK ==  ( rc=CH_B_D_by_id_chanel_d_search( t, &c_d_cursor,  MCO_EQ, ch_d_id  ) ))
		 else    			 fprintf( stderr, "Cant' find channel d for channel d");
  }   //  if ( MCO_S_OK ==  ( rc=CH_B_D_by_id_chanel_b_numb_b_find ( t,   channel_b_id, 0, &ch_b_d  ) ) ) 

  return result;
}

bool database_wrapper::find_channel_b_id_by_coordinates(  mco_trans_h t, uint1 upo_number, uint1 ie1_number, 
                                                                                                                 uint1 e1_number, uint1 interval, /*OUT*/ uint4 * result_channel_b_id  )  
{
   bool result=false;
   MCO_RET rc;
   UPO upo;
   E1 e1;
   IE1 ie1;
   CHANNEL_B channel_b;
   uint1 upo_id;
   uint1 ie1_id;
   uint2 e1_id;
   /* find the first object with id specified */
   if ( MCO_S_OK == ( rc=UPO_by_number_hash_find    ( t, upo_number, &upo ))) {
        UPO_id_upo_get ( &upo, &upo_id );
        if ( MCO_S_OK == ( rc=IE1_by_id_upo_number_find ( t, upo_id, ie1_number, &ie1) ) )  {
      	    IE1_id_ie1_get  ( &ie1, &ie1_id );
        	    if ( MCO_S_OK == ( rc=E1_by_id_ie1_number_find( t, ie1_id, e1_number, &e1) ) )  {
                  E1_id_e1_get( &e1, &e1_id );
                  if ( MCO_S_OK == ( rc=CHANNEL_B_by_id_ie1_nki_find(t, e1_id, interval, &channel_b  ) ) )  {
                         CHANNEL_B_id_channel_b_get (  &channel_b, result_channel_b_id  );
                  }           					         
               }
          }
			 					         
   }
  return result;
}



