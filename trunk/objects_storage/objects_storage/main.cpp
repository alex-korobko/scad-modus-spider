using namespace std;

#include <functional>
#include <vector>
#include <map>
#include <string>

#include <iostream.h>

#include <inttypes.h>
#include <process.h>
#include <pthread.h>
#include <unistd.h>

#include "test_cond.h"

#include "storage_common.h"
#include "dbwrapper_definition.h"
#include "objects_storage_exception.h"
#include "objects_storage_logger.h"

#ifdef _MOCK_DBWRAPPER_
#include "mock_database_wrapper.h"
#else //#ifdef _MOCK_DBWRAPPER_
#ifdef _LINTER_DBWRAPPER_
#include <linapi.h>
#include "linter_database_wrapper.h"
#else
#ifdef _EXTREAME_DB_DBWRAPPER_
#include "extreamedb_database_wrapper.h"
#endif  //#ifdef _EXTREAME_DB_DBWRAPPER_
#endif  //#ifdef _LINTER_DBWRAPPER_
#endif  //#ifdef _MOCK_DBWRAPPER_

#include "call_control_data_manager.h"
#include "proc_control_data_manager.h"
#include "manager_of_data_storage.h"

#ifdef __UNIT_TESTING__
#endif

extern char * optarg;
extern int optind, opterr, optopt;

void usage(string program_name) {
   if (program_name.empty()) program_name="objects_storage";
   cout<<" Usage :\n"
           <<"    "<<program_name<<" -n node_id -p process_id -c channel_id\n"
           <<"    where :\n"
           <<"       node_id - id of node for requests generator\n"
           <<"       process_id - id of process for requests generator\n"
           <<"       channel_id - id of channel for requests generator\n"
           <<endl;
};

int main (int argc, char* argv[]) {
#ifdef __UNIT_TESTING__
#endif

  if (argc<4) {
       usage(argv[0]);
       return EXIT_FAILURE;
   };

	char opt;
	while ( ( opt = getopt( argc, argv, "n: p: c:" ) ) != -1 ) {
		switch( opt ) {
		case 'n' :
			if( sscanf( optarg, "%i", &manager_of_data_storage::node_id ) != 1 ) {
                   objects_storage_logger& logger_inst=
                              objects_storage_logger::get_instance();
                   logger_inst.log_message
                            (objects_storage_logger::ERROR_MSG,
                             "In main(...) n argument  isn`t node id");
                    return EXIT_FAILURE;
                  };
            break;
        case 'p' :
            if( sscanf( optarg, "%i", &manager_of_data_storage::process_id ) != 1 ) {
                   objects_storage_logger& logger_inst=
                              objects_storage_logger::get_instance();
                   logger_inst.log_message
                            (objects_storage_logger::ERROR_MSG,
                             "In main(...) p argument  isn`t process id");
                   return EXIT_FAILURE;
                 };
            break;
        case 'c' :
            if( sscanf( optarg, "%i", &manager_of_data_storage::channel_id ) != 1 ) {
                   objects_storage_logger& logger_inst=
                              objects_storage_logger::get_instance();
                   logger_inst.log_message
                            (objects_storage_logger::ERROR_MSG,
                             "In main(...) c argument  isn`t process id");
                   return EXIT_FAILURE;
                 };
            break;
        default :
                   objects_storage_logger& logger_inst=
                              objects_storage_logger::get_instance();
                   logger_inst.log_message
                            (objects_storage_logger::ERROR_MSG,
                             "In main(...) unrecognized argument ");
                   return EXIT_FAILURE;
            break;
        }
	};

try {
manager_of_data_storage::run();
 } catch (objects_storage_exception obj_stor_exc) {
        objects_storage_logger& logger_inst=
                      objects_storage_logger::get_instance();
         logger_inst.log_message
                    (objects_storage_logger::ERROR_MSG,
                     "In main(...) catched exception : "+obj_stor_exc.get_description());
 };

return 0;
};
