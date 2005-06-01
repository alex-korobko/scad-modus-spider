
/* Event handlers (call back functions) implementaions */

#include <stdio.h>
#include <stdlib.h>

#include "kgtsdb.h"
#include "handlers.h"

int unregister_events(mco_db_h db) {
	MCO_RET rc;
	mco_trans_h t;

	mco_trans_start(db, MCO_READ_WRITE, MCO_TRANS_FOREGROUND, &t);
	
     mco_unregister_channel_b_tid_update_handler( t,   & update_channel_b_handler );
     mco_unregister_set_id_trunk_gr_handler  ( t, &new_channel_b_in_trunk );
	rc = mco_trans_commit(t);
	return rc;
}


int register_events(mco_db_h db) {
	MCO_RET rc;
	mco_trans_h t;

	mco_trans_start(db, MCO_READ_WRITE, MCO_TRANS_FOREGROUND, &t);
	
     mco_register_channel_b_tid_update_handler ( t, &update_channel_b_handler, (void *)0,  MCO_AFTER_UPDATE );
     mco_register_set_id_trunk_gr_handler  ( t, &new_channel_b_in_trunk, (void *)0,   MCO_AFTER_UPDATE );
    
	rc = mco_trans_commit(t);

	return rc;
}


MCO_RET update_channel_b_handler( mco_trans_h t,  CHANNEL_B * obj,  MCO_EVENT_TYPE et, /*INOUT*/ void *param) {
	MCO_RET           rc;
	uint2 tid;
	uint4 channel_b_id, total_channels_number;
	uint4 used_channels_number;
     uint1 trunk_group_id ;
     uint1 overused;
	CH_B_TRUNK_GR b_trunk_group;
	TRUNK_GROUP  trunk_group;
       
	CHANNEL_B_tid_get( obj, &tid);
	CHANNEL_B_id_channel_b_get ( obj, &channel_b_id );  

     if ( MCO_S_OK == (rc = CH_B_TRUNK_GR_by_id_channel_b_find ( t, channel_b_id, &b_trunk_group) ) ) {

                   CH_B_TRUNK_GR_id_trunk_gr_get( &b_trunk_group, &trunk_group_id);       
                   
                  TRUNK_GROUP_by_id_trunk_group_hash_find( t , trunk_group_id, &trunk_group );

                  TRUNK_GROUP_b_total_get ( &trunk_group, &total_channels_number);
                  
                  TRUNK_GROUP_b_used_get ( &trunk_group, &used_channels_number);
			
			 TRUNK_GROUP_overuse_get ( &trunk_group, &overused);
			  
		                  
                  if ( tid == 0 )  {
                          // Если текушее количество занятных == 0, то не делаем ничего
                          if ( used_channels_number != 0 ) {
                  				// Уменьшаем счетчик занятых
                  				used_channels_number--;
							TRUNK_GROUP_b_used_put ( &trunk_group, used_channels_number);                 		
  		                		// Если количестов занятых опустилось ниже нижней граници перегрузки
    			                      // то убираем состояние перегрузки, ecли оно было
    			                      double low_limit; 
    			                      if ( overused != 0 ) {
							  		TRUNK_GROUP_cng_low_limit_get ( &trunk_group,  &low_limit);
							  		if( ( used_channels_number / total_channels_number * 100 ) < low_limit ) 
							  				 TRUNK_GROUP_overuse_put ( &trunk_group, 0 );
                                       } 
    			                      
                          }
 
			   } else  {	
			   		// Если количество занятых каналов равно общему кол каналов транк-группы, то ничего не делаем
			   		if ( used_channels_number < total_channels_number )
					// увеличиваем счетчик занятых  
					 used_channels_number++;
					TRUNK_GROUP_b_used_put ( &trunk_group, used_channels_number);   
					TRUNK_GROUP_last_occupied_channel_b_put( &trunk_group, channel_b_id);                 		
				    // если количество занятых больше допустимой верхней границы,
				   	// то устанавливаем признак перегрузки
				   	if ( overused == 0 ) {
				   		double hi_limit; 
				   		TRUNK_GROUP_cng_hi_limit_get ( &trunk_group,  &hi_limit);
							  		if( ( used_channels_number / total_channels_number * 100 ) > hi_limit ) 
							  				 TRUNK_GROUP_overuse_put ( &trunk_group, 1 );
				   	}
			  }			
                   
	}
     else {
      //===================================== delete it =================================================
          uint1 nki;   
          CHANNEL_B_nki_get ( obj, &nki );  
	    fprintf( stderr, "Event: Can't find specified TRUNK GROUP for  channel_b %d nki : %d\n",  channel_b_id,  nki );
	    fflush(stderr);
	  //===================================== delete it =================================================   
	}

	return MCO_S_OK;
}



MCO_RET new_channel_b_in_trunk( mco_trans_h t, CH_B_TRUNK_GR * obj, MCO_EVENT_TYPE et, /*INOUT*/ void *param) {
      uint4 min_id = 1000000;
     TRUNK_GROUP trunk_group;
	uint1 trunk_group_id ;
	uint4 total_channels_number;
	CH_B_TRUNK_GR_id_trunk_gr_get( obj, &trunk_group_id);      
	uint4 channel_b;
	uint4 max_channel_b_id;
	CH_B_TRUNK_GR_id_channel_b_get( obj, &channel_b);      

	TRUNK_GROUP_by_id_trunk_group_hash_find( t , trunk_group_id, &trunk_group );
	TRUNK_GROUP_b_total_get ( &trunk_group, &total_channels_number);
	TRUNK_GROUP_max_channel_b_id_get( &trunk_group, &max_channel_b_id);
	 uint4 f_temp; 
	 TRUNK_GROUP_first_channel_b_id_get(  &trunk_group, & f_temp); 
	 if( f_temp != 0 ) min_id = f_temp;
  	

	// Увеличиваем общее количество каналов доступных трак-группе
	total_channels_number ++;
	TRUNK_GROUP_b_total_put ( &trunk_group, total_channels_number);
	if ( channel_b  > max_channel_b_id )
      	TRUNK_GROUP_max_channel_b_id_put ( &trunk_group, channel_b); 
   
	if ( channel_b  < min_id ) {
                  TRUNK_GROUP_first_channel_b_id_put(  &trunk_group, channel_b);         
     }
	return MCO_S_OK;
}; 
