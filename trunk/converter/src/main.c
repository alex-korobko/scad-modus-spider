/* Converter old message files (QNX slogger format) to PostgreSQL database   */
/*                            AppBuilder Photon Code Lib */
/*                                         Version 2.01  */

/* Standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
/* Toolkit headers */
#include <Ph.h>
#include <Pt.h>
#include <Ap.h>

/* Local headers */
#include "libpq-fe.h"
#include "ablibs.h"
#include "abimport.h"
#include "proto.h"

int options_mode = 0;

ApDBase_t *database;

PhImage_t *image_on, *image_off;

int quit_app( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	if (database!=NULL) { ApCloseDBase(database);};
	PtExit(EXIT_SUCCESS);
	return( Pt_CONTINUE );

	}


int
main_WndOpenFile( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	PtFileSelectionInfo_t info;
	int DlgStatus=-1;



	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	memset (&info, 0, sizeof(PtFileSelectionInfo_t));

	DlgStatus=PtFileSelection (NULL, 
                     NULL,
                     "Selecting messages file", 
                     NULL, 
                     NULL, 
                     NULL,
                     NULL, 
                     NULL, 
                     &info, 
                     0 );

	if (DlgStatus==0 && info.ret==Pt_FSDIALOG_BTN1)
		{
		printf ("\nSelected file :\n%s\n",info.path);
		};

	return( Pt_CONTINUE );

	}


static void
exit_nicely(PGconn *conn)
{
	PQfinish(conn);
	exit(1);
}


int Move_Data (file_name)
{

char *pghost, *pgport, *pgoptions, *pgtty, *dbName, *query_text ;
int nFields;
int i, j;

#ifdef DEBUG
	FILE	   *debug;
#endif	 // DEBUG 

	PGconn	   *conn;
	PGresult   *res;

	/*
	 * begin, by setting the parameters for a backend connection if the
	 * parameters are null, then the system will try to use reasonable
	 * defaults by looking up environment variables or, failing that,
	 * using hardwired constants
	 */
	pghost = "127.0.0.1";				/* host name of the backend server */
	pgport = NULL;				/* port of the backend server */
	pgoptions = NULL;			/* special options to start up the backend
								 * server */
	pgtty = NULL;				/* debugging tty for the backend server */
	dbName = "spider";

	/* make a connection to the database */
	conn = PQsetdb(pghost, pgport, pgoptions, pgtty, dbName);

	/* check to see that the backend connection was successfully made */
	if (PQstatus(conn) == CONNECTION_BAD)
	{
		fprintf(stderr, "Connection to database '%s' failed.\n", dbName);
		fprintf(stderr, "%s", PQerrorMessage(conn));
		exit_nicely(conn);
	}

#ifdef DEBUG
	debug = fopen("/tmp/trace.out", "w");
	PQtrace(conn, debug);
#endif	 /* DEBUG */

	/* start a transaction block */
	query_text="BEGIN";
	res = PQexec(conn, query_text);
	if (PQresultStatus(res) != PGRES_COMMAND_OK)
	{
		fprintf(stderr, "\n%s command failed\n",query_text);
		PQclear(res);
		exit_nicely(conn);
	}

	/*
	 * should PQclear PGresult whenever it is no longer needed to avoid
	 * memory leaks
	 */
	PQclear(res);

	/*
	 * fetch instances from the pg_database, the system catalog of
	 * databases
	 */
//	res = PQexec(conn, "DECLARE myportal CURSOR FOR select * from pg_database");

	sprintf(query_text,"insert into blocing_names (short_name, long_name) values (");
	res = PQexec(conn, query_text);

	if (PQresultStatus(res) != PGRES_COMMAND_OK)
	{
		fprintf(stderr, "\n%s command failed\n", query_text);
		PQclear(res);
		exit_nicely(conn);
	}
	PQclear(res);

/*	res = PQexec(conn, "FETCH ALL in myportal");
	if (PQresultStatus(res) != PGRES_TUPLES_OK)
	{
		fprintf(stderr, "FETCH ALL command didn't return tuples properly\n");
		PQclear(res);
		exit_nicely(conn);
	}
*/
	/* first, print out the attribute names */
/*	nFields = PQnfields(res);
	for (i = 0; i < nFields; i++)
		printf("%-15s", PQfname(res, i));
	printf("\n\n");
*/
	/* next, print out the instances */
/*	for (i = 0; i < PQntuples(res); i++)
	{
		for (j = 0; j < nFields; j++)
			printf("%-15s", PQgetvalue(res, i, j));
		printf("\n");
	}

	PQclear(res);
*/
	/* close the portal */
/*	res = PQexec(conn, "CLOSE myportal");
	PQclear(res);
*/
	/* end the transaction */

	query_text="END";
	res = PQexec(conn, query_text);
	PQclear(res);

	/* close the connection to the database and cleanup */
	PQfinish(conn);

#ifdef DEBUG
	fclose(debug);
#endif	 /* DEBUG */

return (0);
}


int
switch_options( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{
PtWidget_t *tmp_widget;
	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

   if (ApName(widget) == ABN_block_names) 
      {
	options_mode=0;
       }
      else if (ApName(widget)==ABN_journal) 
       {
	options_mode=1;
       };

   if (	options_mode==0) 
      {
	ApModifyItemState(&options_menu,AB_ITEM_SET,ABN_block_names,NULL);
	ApModifyItemState(&options_menu,AB_ITEM_NORMAL,ABN_journal,NULL);
       }
      else 
       {
	ApModifyItemState(&options_menu,AB_ITEM_NORMAL,ABN_block_names,NULL);
	ApModifyItemState(&options_menu,AB_ITEM_SET,ABN_journal,NULL);
       };


	return( Pt_CONTINUE );

	return( Pt_CONTINUE );

	}


int
releazed_app( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
   if (	options_mode==0) 
      {
	ApModifyItemState(&options_menu,AB_ITEM_SET,ABN_block_names,NULL);
	ApModifyItemState(&options_menu,AB_ITEM_NORMAL,ABN_journal,NULL);
       }
      else 
       {
	ApModifyItemState(&options_menu,AB_ITEM_NORMAL,ABN_block_names,NULL);
	ApModifyItemState(&options_menu,AB_ITEM_SET,ABN_journal,NULL);
       };


	return( Pt_CONTINUE );

	}

