using namespace std;
#include <Ph.h>
#include <Ap.h>
#include <Pt.h>
#include <sys/neutrino.h>

#include <vector>
#include <string>
#include <map>
#include <functional>

#include "defines.h"
#include "spider_exception.h"
#include "system_settings.h"
#include "system_settings_spider.h"
#include "command.h"


bool command::operator == 
 (command command_to_equal) {
         if (this->device_id!=command_to_equal.device_id) return false;
         if (this->device_type_id!=command_to_equal.device_type_id) return false;
         if (this->command_code!=command_to_equal.command_code) return false;
         if (this->device_number!=command_to_equal.device_number) return false;
         if (this->station_id!=command_to_equal.station_id) return false;
         //item_color??  if (this->item_color!=command_to_equal.item_color) return false;
         return true;
	};

