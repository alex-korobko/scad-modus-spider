using namespace std;
#include <functional>
#include <map>
#include <vector>

#include <inttypes.h>

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
#endif  //#ifdef _LINTER_DBWRAPPER_
#endif  //#ifdef _MOCK_DBWRAPPER_


#include "abstract_job_data_manager.h"
#include "jobs_data_managers_container.h"

void jobs_data_managers_container::erase (iterator where) {
      if (where!=container_of_managers.end()) {
              delete (where->second);
               container_of_managers.erase(where);
          };
 };

jobs_data_managers_container::~jobs_data_managers_container() {
   iterator container_iter=container_of_managers.begin();
   while (container_iter!=container_of_managers.end()) {
               delete (container_iter->second);
               container_iter++;
          } ; //while (container_iter!=container_of_managers.end()
  }; //  ~jobs_data_managers_container()

