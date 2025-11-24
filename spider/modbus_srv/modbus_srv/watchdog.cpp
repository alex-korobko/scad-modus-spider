#include <sys/neutrino.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>
#include <fcntl.h>

#include <hw/inout.h>
#include <string.h>
#include <string>
#include <inttypes.h>

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <sstream>

#include <iostream.h>

#include <Ap.h>
#include <Pt.h>

using namespace std;

#include "defines.h"
#include "program_settings.h"
#include "metro_device.h"
#include "watchdog.h"

void watchdog_timer::loop() throw (metro_device::metro_device_exception){
useconds_t usleep_interval;
uintptr_t port_nmap;
int ret_value;

if (!watchdog_enabled) 
        throw metro_device::metro_device_exception("In watchdog_timer::loop() watchdog_enabled false");

if (port_number==0)
        throw metro_device::metro_device_exception("In watchdog_timer::loop() Port number still 0  - not initialized");

if (reset_interval==0)
        throw metro_device::metro_device_exception("In watchdog_timer::loop() Reset interval still 0 - not initialized");

ret_value=ThreadCtl_r(_NTO_TCTL_IO, 0);
if (ret_value!=EOK) {
       string err_description("In watchdog_timer::loop() ThreadCtl_r(...) return error:");
       err_description+=strerror(ret_value);
       throw metro_device::metro_device_exception(err_description);
   };

port_nmap=mmap_device_io(1, port_number);
if (port_nmap<=0) {
       string err_description("In watchdog_timer::loop() mmap_device_io(...) return error:");
       err_description+=strerror(errno);
       throw metro_device::metro_device_exception(err_description);
   };

usleep_interval=(reset_interval*1000000)/4;
if (usleep_interval>1000000) usleep_interval=500000;

while(true){
  out8(port_nmap, reset_interval);
  usleep (usleep_interval);
  }
}

void watchdog_timer::load_watchdog() throw (metro_device::metro_device_exception) {
 enum {WATCHDOG_ENABLED=0, RESET_INTERVAL, PORT_NUMBER, ENTRIES_COUNT};

ostringstream exception_description;
string entry_name;
const char *entry_name_c_str;
vector<char> temp_str(512);
vector<string> entries_names(ENTRIES_COUNT);

entries_names[WATCHDOG_ENABLED]="watchdog enabled";
entries_names[RESET_INTERVAL]="reset interval";
entries_names[PORT_NUMBER]="port number";

entry_name_c_str=PxConfigNextString(&temp_str[0], 
                                                                   temp_str.size() );

while(entry_name_c_str!=NULL){
entry_name=entry_name_c_str;
     if (entry_name.compare(entries_names[WATCHDOG_ENABLED])==0) {
          string value_example="enabled";
          if (value_example.compare(&temp_str[0])==0) {
                      watchdog_enabled=true;
               } else   { //if (value_example.compare(value_example)==0)
                     value_example="disabled";
                       if (value_example.compare(&temp_str[0])==0) {
                              watchdog_enabled=false;
                           } else { //if (value_example.compare(value_example)==0)
                              exception_description<<"Uncorrect watchdog enabling value  "<<&temp_str[0]<<" supported only enabled, disabled";
                              throw metro_device::metro_device_exception(exception_description.str());
                           }; //if (value_example.compare(value_example)==0)
               }; //if (value_example.compare(value_example)==0)

       } else if (entry_name.compare(entries_names[RESET_INTERVAL])==0) {
			int temp_int = atoi(&temp_str[0]);
			if (temp_int<2||temp_int>20){
						exception_description<<"Wrong reset interval in seconds: "<<temp_int<<" now supported values from 2 to 20";
						throw metro_device::metro_device_exception(exception_description.str());
					};
              reset_interval=static_cast<byte>(temp_int);

       } else if (entry_name.compare(entries_names[PORT_NUMBER])==0) {
			uint64_t temp_int =static_cast<uint64_t>(atoi(&temp_str[0]));
			if (temp_int<1){
						exception_description<<"Wrong port number "<<temp_int;
						throw metro_device::metro_device_exception(exception_description.str());
					};
              port_number=temp_int;

         } else {
            exception_description<<"Unrecognized config entry  "<<entry_name;
            throw metro_device::metro_device_exception(exception_description.str());
         };

	entry_name_c_str=PxConfigNextString(&temp_str[0], 
							                        temp_str.size()
							                      );
	}; //while(entry_name_c_str!=NULL){
};


void watchdog_timer::load (string file_name) throw (metro_device::metro_device_exception) {
	ostringstream exception_description;
    enum {WATCHDOG_TIMER=0, SECTIONS_COUNT};
	string	section_name;
	const char *section_name_c_str;
	vector<string> sections_names(SECTIONS_COUNT);

	sections_names[WATCHDOG_TIMER]="watchdog timer";	
	if (PxConfigOpen( file_name.c_str(), PXCONFIG_READ)!=Pt_TRUE ){
        exception_description<<"Can`t open config file "<<file_name;
		throw metro_device::metro_device_exception(exception_description.str());
	};

     try {
                    section_name_c_str=PxConfigNextSection();
                    while (section_name_c_str!=NULL) {
                       section_name=section_name_c_str;

                      if (section_name.compare(sections_names[WATCHDOG_TIMER])==0) {
                               load_watchdog();
                        } else {
                           exception_description<<"Unrecognized config section "<<section_name;
                           throw metro_device::metro_device_exception(exception_description.str());
                        };
                      section_name_c_str=PxConfigNextSection();
                    };
	}catch  (metro_device::metro_device_exception dev_exc) {
           exception_description<<dev_exc.get_description();
           if  (PxConfigClose()!=Pt_TRUE) {
               exception_description<<"\nCan`t close file "<<file_name;
           };
           throw metro_device::metro_device_exception(exception_description.str());
 	};

     if  (PxConfigClose()!=Pt_TRUE) {
        exception_description<<"Can`t close file "<<file_name;
        throw metro_device::metro_device_exception(exception_description.str());
      };
};

