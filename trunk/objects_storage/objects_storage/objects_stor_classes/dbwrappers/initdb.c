#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "kgtsdb.h"
#include "initdb.h"

int insert_UPO ( mco_db_h db,  uint1 id, uint1 number ) {

	/* insert UPO */
	MCO_RET           rc;	
	mco_trans_h       t;
	UPO               obj;

	/* begin a read-write transaction */
	if ((rc = mco_trans_start(db, MCO_READ_WRITE, MCO_TRANS_FOREGROUND, &t)) == MCO_S_OK) {

		/* make the object */
		UPO_new( t, &obj );
		
		/* fillup the object */
		UPO_id_upo_put ( &obj, id );
		UPO_exist_put  ( &obj, 1 );
		UPO_number_put ( &obj, number );

		/* commit the transaction */
		rc=mco_trans_commit(t);

	} else {
		fprintf( stderr, "Can't open a transaction. Error code: %d\n", rc);
	};

	return rc == MCO_S_OK ? 0 : 1;
};

int insert_IE1 ( mco_db_h db, uint1 id_upo, uint1 id_ie1, uint1 number ) {

	/* insert IE1 */
	MCO_RET           rc;	
	mco_trans_h       t;
	IE1               obj;

	/* begin a read-write transaction */
	if ((rc = mco_trans_start(db, MCO_READ_WRITE, MCO_TRANS_FOREGROUND, &t)) == MCO_S_OK) {

		/* make the object */
		IE1_new( t, &obj );
		
		/* fillup the object */
		IE1_id_ie1_put ( &obj, id_ie1 );
		IE1_id_upo_put ( &obj, id_upo );
		IE1_exist_put  ( &obj, 1 );
		IE1_number_put ( &obj, number );

		/* commit the transaction */
		rc=mco_trans_commit(t);
	} else {
		fprintf( stderr, "Can't open a transaction. Error code: %d\n", rc);
	};

	return rc == MCO_S_OK ? 0 : 1;
};

int insert_E1 ( mco_db_h db,  uint1 id_ie1, uint2 id_e1, uint2 number) {

	/* insert E1 */
	MCO_RET           rc;	
	mco_trans_h       t;
	E1               obj;

	/* begin a read-write transaction */
	if ((rc = mco_trans_start(db, MCO_READ_WRITE, MCO_TRANS_FOREGROUND, &t)) == MCO_S_OK) {

		/* make the object */
		E1_new( t, &obj );
		
		/* fillup the object */
		E1_id_e1_put  ( &obj, id_e1 );
		E1_id_ie1_put ( &obj, id_ie1 );
		E1_number_put ( &obj, number );

		/* commit the transaction */
		rc=mco_trans_commit(t);
	} else {
		fprintf( stderr, "Can't open a transaction. Error code: %d\n", rc);
	};

	return rc == MCO_S_OK ? 0 : 1;
};


int insert_CHANNEL_B(  mco_db_h db,  uint4 id_channel_B, uint2 id_E1, uint4 number  ) {
	/* insert Channel b */
	MCO_RET           rc;	
	mco_trans_h       t;
	CHANNEL_B          obj;

	/* begin a read-write transaction */
	if ((rc = mco_trans_start(db, MCO_READ_WRITE, MCO_TRANS_FOREGROUND, &t)) == MCO_S_OK) {

		/* make the object */
		CHANNEL_B_new( t, &obj );
		
		/* fillup the object */
		CHANNEL_B_id_channel_b_put( &obj, id_channel_B );
		CHANNEL_B_id_e1_put( &obj, id_E1 );
		CHANNEL_B_nki_put( &obj, number );

		/* commit the transaction */
		rc=mco_trans_commit(t);
	} else {
		fprintf( stderr, "Can't open a transaction. Error code: %d\n", rc);
	};

	return rc == MCO_S_OK ? 0 : 1;
}

int insert_CHANNEL_D( mco_db_h db, uint4 id_channel_B, uint4 id_channel_D, uint2 id_E1, uint4 max_call_pointer  ) {
	/* insert Channel d */
	MCO_RET           rc;	
	mco_trans_h       t;
	CHANNEL_D          obj;
	CH_B_D   link_obj;

	/* begin a read-write transaction */
	if ((rc = mco_trans_start(db, MCO_READ_WRITE, MCO_TRANS_FOREGROUND, &t)) == MCO_S_OK) {

		/* make the object */
		CHANNEL_D_new( t, &obj );
	
		/* fillup the object */
		CHANNEL_D_id_channel_d_put ( &obj, id_channel_D );
		CHANNEL_D_id_e1_put( &obj, id_E1 );
		CHANNEL_D_max_call_pointer_put( &obj, max_call_pointer );
		 CHANNEL_D_last_call_pointer_put( &obj, 0 );    
		CHANNEL_D_call_pointers_alloc(  &obj, max_call_pointer );
		for ( uint4 i = 0; i < max_call_pointer;  i++ ) {
                CHANNEL_D_call_pointers_put ( &obj, i, 0 ); 
          }       
		/* commit the transaction */
		rc=mco_trans_commit(t);
	} else {
		fprintf( stderr, "Can't open a transaction. Error code: %d\n", rc);
	};
      /* begin a read-write transaction */
      for( int  i=0; i <=1; i++ ) {
		if ((rc = mco_trans_start(db, MCO_READ_WRITE, MCO_TRANS_FOREGROUND, &t)) == MCO_S_OK) {
			CH_B_D_new( t, &link_obj );
			 CH_B_D_id_channel_b_put( &link_obj , id_channel_B );
   		      CH_B_D_id_channel_d_put( &link_obj,  id_channel_D );
     		 CH_B_D_numb_b_put( &link_obj, i );  
			/* commit the transaction */
			rc=mco_trans_commit(t);

			} else {

				fprintf( stderr, "Can't open a transaction. Error code: %d\n", rc);
			};
	 }  		
	return rc == MCO_S_OK ? 0 : 1;
}

int insert_CH_B_TRUNK_GR ( mco_db_h db, uint2 id_trunk_gr, uint4 id_channel_b ) {

     assert( id_trunk_gr < 7 && id_trunk_gr > 0 );
	/* insert CHANNEL_B */
	MCO_RET           rc;	
	mco_trans_h       t;
	CH_B_TRUNK_GR     obj;

	/* begin a read-write transaction */
	if ((rc = mco_trans_start(db, MCO_READ_WRITE, MCO_TRANS_FOREGROUND, &t)) == MCO_S_OK) {

		/* make the object */
		CH_B_TRUNK_GR_new( t, &obj );
		/* fillup the object */
	     CH_B_TRUNK_GR_id_trunk_gr_put ( &obj, id_trunk_gr );
		CH_B_TRUNK_GR_id_channel_b_put( &obj, id_channel_b );

		/* commit the transaction */
		rc=mco_trans_commit(t);
	} else {
		fprintf( stderr, "insert_CH_B_TRUNK_GR::Can't open a transaction. Error code: %d\n", rc);
	};

	return rc == MCO_S_OK ? 0 : 1;
};

int insert_TRUNK_GROUP (    mco_db_h db,  uint2 id_trunk_group, uint4 id_direction, uint2 inout,
                                                         uint2 slzsl, uint2 isdn, double hi_limit, double low_limit ) {


	/* insert TRUNK_GROUP */
	MCO_RET           rc;	
	mco_trans_h       t;
	TRUNK_GROUP    obj;

	/* begin a read-write transaction */
	if ((rc = mco_trans_start(db, MCO_READ_WRITE, MCO_TRANS_FOREGROUND, &t)) == MCO_S_OK) {

		/* make the object */
		TRUNK_GROUP_new( t, &obj );

		/* fillup the object */
	
	     TRUNK_GROUP_id_trunk_group_put           (  &obj, id_trunk_group );
          TRUNK_GROUP_id_direction_put         	     (  &obj, id_direction );
          TRUNK_GROUP_inout_put                              (  &obj, inout );
          TRUNK_GROUP_slzsl_put						(  &obj, slzsl );
          TRUNK_GROUP_isdn_put						(  &obj, isdn );
          TRUNK_GROUP_cng_hi_limit_put                (  &obj, hi_limit );
          TRUNK_GROUP_cng_low_limit_put                (  &obj, low_limit );
		TRUNK_GROUP_b_total_put 				     (  &obj, 0 );
		/* commit the transaction */
		rc=mco_trans_commit(t);
	} else {
		fprintf( stderr, "Can't open a transaction. Error code: %d\n", rc);
	};

	return rc == MCO_S_OK ? 0 : 1;
};

int insert_DESTINATION (   mco_db_h db,    uint4 id_destination, const char  *name  ) {


	/* insert DESTINATION */
	MCO_RET           rc;	
	mco_trans_h       t;
	DESTINATION    obj;
	
	/* begin a read-write transaction */
	if ((rc = mco_trans_start(db, MCO_READ_WRITE, MCO_TRANS_FOREGROUND, &t)) == MCO_S_OK) {

		/* make the object */
		DESTINATION_new( t, &obj );

		/* fillup the object */
         DESTINATION_id_destination_put  ( &obj, id_destination );
         DESTINATION_name_put                 ( &obj, name, strlen(name)) ;

		/* commit the transaction */
		rc=mco_trans_commit(t);
	} else {
		fprintf( stderr, "Can't open a transaction. Error code: %d\n", rc);
	};

	return rc == MCO_S_OK ? 0 : 1;
};

int insert_DIRECTION (   mco_db_h db,   uint4 id_direction, const char  *name,  uint1  to_amts,  uint1 lpr      ) {


	/* insert DIRECTION */
	MCO_RET           rc;	
	mco_trans_h       t;
	DIRECTION    obj;
	
	/* begin a read-write transaction */
	if ((rc = mco_trans_start(db, MCO_READ_WRITE, MCO_TRANS_FOREGROUND, &t)) == MCO_S_OK) {

		/* make the object */
		DIRECTION_new( t, &obj );

		/* fillup the object */
         DIRECTION_id_direction_put  (  &obj, id_direction );
         DIRECTION_name_put             (  &obj, name, strlen(name) ) ;

          DIRECTION_to_amts_put        (  &obj, to_amts  );
          DIRECTION_lpr_put                  (  &obj, lpr   );
          
		/* commit the transaction */
		rc=mco_trans_commit(t);
	} else {
		fprintf( stderr, "Can't open a transaction. Error code: %d\n", rc);
	};

	return rc == MCO_S_OK ? 0 : 1;
};


int insert_DIRECTION_DESTINATION  ( mco_db_h db,     uint4 id_direction, uint4 id_destination,  uint4 points  ) {


	/* insert DIRECTION_DESTINATION */
	MCO_RET           rc;	
	mco_trans_h       t;
	DIRECTION_DESTINATION   obj;
	
	/* begin a read-write transaction */
	if ((rc = mco_trans_start(db, MCO_READ_WRITE, MCO_TRANS_FOREGROUND, &t)) == MCO_S_OK) {

		/* make the object */
		DIRECTION_DESTINATION_new( t, &obj );

		/* fillup the object */
         DIRECTION_DESTINATION_id_direction_put  (  &obj, id_direction );    
         DIRECTION_DESTINATION_id_destination_put    (  &obj, id_destination );
         DIRECTION_DESTINATION_points_put     (  &obj, points );

		/* commit the transaction */
		rc=mco_trans_commit(t);
	} else {
		fprintf( stderr, "Can't open a transaction. Error code: %d\n", rc);
	};

	return rc == MCO_S_OK ? 0 : 1;
};

int load_database( mco_db_h db ) {

	uint1 UPO=0, IE1=0;
     uint2  E1=0;
     uint4  CH_B = 0,  CH_D =0;
	uint2 TR_GRP =0;
	uint4 i, j, k, m;

     // Insert TRUNK GROUPS
	if ( 0 !=  insert_TRUNK_GROUP(  db,  1, 1, 1, 3, 0, 91, 89  ) ) {
			fprintf( stderr, "Can't insert TRUNK GROUP 1 link .\n" );
			return 1;
     };
     if ( 0 !=insert_TRUNK_GROUP (   db, 2, 1, 2, 1, 0, 91, 89   ) ) {
			fprintf( stderr, "Can't insert TRUNK GROUP 2 link .\n" );
			return 1;
     };
     if ( 0 !=insert_TRUNK_GROUP (  db,  3, 1, 2, 2, 0, 91, 89   ) ) {
			fprintf( stderr, "Can't insert TRUNK GROUP 3 link .\n" );
			return 1;
     };
     if ( 0 !=insert_TRUNK_GROUP (  db,  4, 2, 1, 3, 0, 91, 89   ) ) {
			fprintf( stderr, "Can't insert TRUNK GROUP 4 link .\n" );
			return 1;
     };
     if ( 0 !=insert_TRUNK_GROUP (  db,   5, 2, 2, 1, 0, 91, 89   ) ) {
			fprintf( stderr, "Can't insert TRUNK GROUP 5 link .\n" );
			return 1;
     };
     if ( 0 !=insert_TRUNK_GROUP (  db,  6, 2, 2, 2, 0, 91, 89   ) ) {
			fprintf( stderr, "Can't insert TRUNK GROUP 6 link .\n" );
			return 1;
     };
   
     bool out_trunk_group = false;
	/* Insert UPOs */
	for ( i=0; i<4; i++ ) {

		if ( 0 != insert_UPO( db, UPO, UPO ) ) {
			
			fprintf( stderr, "Can't insert UPO.\n" );
			return 1;
		};

		/* Insert IE1s */
		for ( j = 0; j < 16; j++ ) {

		
			
			if ( 0 != insert_IE1( db, UPO, IE1, j ) ) {

				fprintf( stderr, "Can't insert IE1.\n" );
				return 1;
			};

			/* Insert E1s */
			for ( k = 0; k < 16; k++ ) {

				if ( 0 != insert_E1( db, IE1, E1, k  ) ) {

					fprintf( stderr, "Can't insert E1.\n" );
					return 1;
				} 
                    out_trunk_group = ! out_trunk_group;				
				for (  m = 0; m < 32; m++ ) {
				       
					if ( 0 != insert_CHANNEL_B( db,  CH_B, E1, m  ) ) {
        					fprintf( stderr, "Can't insert CHANNEL B.\n" );
   						return 1;
				     } 
				     
				    if (  m != 0 && m!= 16 ) {
				          if ( 0 != insert_CHANNEL_D( db, CH_B, CH_D, E1, 255  ) ) {
   	      					fprintf( stderr, "Can't insert CHANNEL D.\n" );
							return 1;
			    	           } 
			     	           CH_D++;
					}	
				   
					TR_GRP = 0;
				     if (m >= 1 && m<=15  ) {
				           if( out_trunk_group) TR_GRP = 1;
						else  TR_GRP = 4;
				     }
				     else if  (m >= 17 && m<=26  ) {
				           if( out_trunk_group) TR_GRP = 2;
						else  TR_GRP = 5;
				     }
				     else if  (m >= 27 && m<=31  ) {
				           if( out_trunk_group) TR_GRP = 3;
						else  TR_GRP = 6;
				     }
				 
				      // link channel_b with trunk_grp
				      if( TR_GRP != 0 )
						if ( 0 != insert_CH_B_TRUNK_GR( db, TR_GRP, CH_B ) ) {
								fprintf( stderr, "Can't insert TRUNK CHANNEL B link .\n" );
								return 1;
					};
				 	CH_B++;
								
				}
		
				E1++;
			
			};

			IE1++;
		};

		UPO ++;
	};

     if ( 0 !=insert_DESTINATION (   db,   1, "Destination 1"  )) {
			fprintf( stderr, "Can't insert Destination 1.\n" );
			return 1;
     };
     if ( 0 !=insert_DESTINATION (   db,   2, "Destination 2"  )) {
			fprintf( stderr, "Can't insert Destination 2 .\n" );
			return 1;
     };
     if ( 0 !=insert_DIRECTION (     db,  1, "Direction 1",  0, 0      )) {
			fprintf( stderr, "Can't insert Direction 1 link .\n" );
			return 1;
     };
     if ( 0 !=insert_DIRECTION (  db,    2, "Direction 2",  0, 0      )) {
			fprintf( stderr, "Can't insert Direction 2 .\n" );
			return 1;
     };
     
     if ( 0 !=insert_DIRECTION_DESTINATION  (  db,    2, 2, 0  ) ) {
			fprintf( stderr, "Can't insert DIRECTION DESTINATION link .\n" );
			return 1;
     };
     if ( 0 !=insert_DIRECTION_DESTINATION  (   db,  1, 1, 0  ) ) {
			fprintf( stderr, "Can't insert DIRECTION DESTINATION link .\n" );
			return 1;
     };
    
	return 0;

};
