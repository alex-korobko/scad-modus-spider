using namespace std;

#include <Ap.h>
#include <Ph.h>
#include <Pt.h>

#include <arpa/inet.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/neutrino.h>
#include <sys/syspage.h>
#include <inttypes.h>
#include <fcntl.h>
#include <signal.h>
#include <iostream.h>

#include <map>
#include <vector>
#include <string>
#include <list>
#include <map>
#include <functional>
#include <algorithm>
#include <sstream>

#include "system_settings_classes/defines.h"
#include "system_settings_classes/spider_exception.h"
#include "system_settings_classes/system_settings.h"
#include "system_settings_classes/system_settings_spider.h"

#include "com_port_class/comport_exception.h"
#include "com_port_class/comport.h"

#include "metro_devices_classes/command.h"
#include "metro_devices_classes/data_block.h"
#include "metro_devices_classes/escalator_data_block.h"
#include "metro_devices_classes/shavr_data_block.h"

#include "containers_classes/msg_types_container.h"
#include "containers_classes/msg_dict_container.h"

#include "metro_devices_classes/devices_types.h"

#include "containers_classes/timer_command_container.h"
#include "containers_classes/metro_devices_types_container.h"
#include "metro_devices_classes/metro_device.h"

class metro_devices_container;
#include "metro_devices_classes/station.h"
#include "containers_classes/metro_stations_container.h"
#include "containers_classes/metro_devices_container.h"


extern metro_devices_container metro_devices;
extern metro_devices_types_container metro_devices_types;

extern ApDBase_t *widgets_db;
extern PtWidget_t *main_window;
extern PtWidget_t *station_window;
extern system_settings_spider *spider_sys_sett;

//PtInputCallbackProcF_t pulse_reciever_catcher
int pulse_reciever_catcher
                    (void *data,
                     int rcvid,
                     void *message,
                    size_t mbsize ){

	struct _pulse* pulse;
	pulse = (struct _pulse*)message;
	metro_devices_container::iterator 
	                  dev_iter=metro_devices.find(pulse->value.sival_int);

	if(dev_iter!=metro_devices.end()){
			switch(pulse->code){
					case system_settings_spider::PULSE_CODE_UPDATE_DEVICE : {
			              dev_iter->second->decode_answer_from_device_to_data_block
                                                                                                    ( &metro_devices_types,
                                                                                                      spider_sys_sett);

                           break;
						};
					default: {
					            	ostringstream error_description;
                                error_description<<"pulse_receiver_thread(...) unrecognized pulse code : "<<pulse->code;
                                spider_sys_sett->sys_message(system_settings::ERROR_MSG,  
											                                  error_description.str());
                                	return Pt_HALT;
                      };
				};
		} else {
            	ostringstream error_description;
            	error_description<<"pulse_receiver_thread(...) unrecognized device id : "<<pulse->value.sival_int;
            spider_sys_sett->sys_message(system_settings::ERROR_MSG,  
                                                          error_description.str());
		};

	return Pt_HALT;
}

void* comport_thread_function(void* comport_ptr){
	if (comport_ptr==NULL) return 0;

	comport *comport_device=static_cast<comport*>(comport_ptr);

    metro_devices_container::iterator dev_iter;

	metro_device::buffer_data_type
                telemetr_request_header(system_settings::MODBUS_HEADER_SIZE),
                telemetr_request,
                telemetr_answer;

    metro_device::buffer_data_type::iterator tmp_iter;
    word crc_value;

    system_settings::bytes device_crc;
    if (metro_devices.empty()) return 0;

    while (true) {
           telemetr_request.clear();
           try {
                    telemetr_request_header=comport_device->recieve(system_settings::MODBUS_HEADER_SIZE);
		    } catch (comport_exception cmp_exc) {
                  continue;
		    }; //catch (comport_exception cmp_exc

                     if (telemetr_request_header[system_settings::MODBUS_FUNCTION_CODE_INDEX]==4 ||
                                 telemetr_request_header[system_settings::MODBUS_FUNCTION_CODE_INDEX]==3 ||
                                 telemetr_request_header[system_settings::MODBUS_FUNCTION_CODE_INDEX]==6
                                 ) {
                            telemetr_request.resize(system_settings::MODBUS_CRC_SIZE+
                                                              system_settings::MODBUS_WRITE_COMMAND_ANSWER_FRAGMENT);
                     } else if (telemetr_request_header[system_settings::MODBUS_FUNCTION_CODE_INDEX]==16) {
                                 metro_device::buffer_data_type::size_type header_size=
                                               system_settings::MODBUS_FUNCTION_16_HEADER_SIZE-
                                               system_settings::MODBUS_HEADER_SIZE;

                                 metro_device::buffer_data_type previos_function_16_header(telemetr_request_header.begin(), 
                                                                                                       telemetr_request_header.end());
                                  try {
                                     telemetr_request_header=comport_device->recieve(header_size);
                                  } catch (comport_exception cmp_exc) {
                                        continue;
                                 }; //catch (comport_exception cmp_exc
                            telemetr_request_header.insert(
                                                    telemetr_request_header.begin(),
                                                    previos_function_16_header.begin(),
                                                    previos_function_16_header.end() );

                            telemetr_request.resize(system_settings::MODBUS_CRC_SIZE+
                                                              telemetr_request_header[system_settings::MODBUS_FUNCTION_16_REQUEST_BYTES_COUNT_INDEX]);
                     };

		   if (!telemetr_request.empty()) { 
                    try {
         			   telemetr_request=comport_device->recieve(telemetr_request.size());
                       telemetr_request.insert(telemetr_request.begin(),
                                                     telemetr_request_header.begin(), 
                                                     telemetr_request_header.end());

                        //CRC check
                        tmp_iter=telemetr_request.begin();
                        advance(tmp_iter, telemetr_request.size()-system_settings::MODBUS_CRC_SIZE);   
                        crc_value=spider_sys_sett->crc(metro_device::buffer_data_type(
                                                         telemetr_request.begin(),
                                                          tmp_iter));

                       device_crc=spider_sys_sett->bytes_of_type<word>(crc_value);
                       if (device_crc[1]!=*(tmp_iter) ||
                            device_crc[0]!=*(++tmp_iter) )
                                    throw comport_exception("Bad packet CRC");

                   } catch (comport_exception cmp_exc) {
		                   string exceptioin_message ("station emul comport_thread_function (... ) Fail reciving request to device : ");
		                    exceptioin_message+=cmp_exc.get_description();
                            spider_sys_sett->sys_message(system_settings::ERROR_MSG, exceptioin_message);

                           continue;
          		   }; //catch (comport_exception cmp_exc

                   //in that case (station emulation) device id equal device number
                  dev_iter = metro_devices.find(
                        telemetr_request[system_settings::MODBUS_DEVICE_NUMBER_INDEX]
                        ); 

                   if (dev_iter == metro_devices.end()) {
		              string error_message(" comport_thread_function (... ) not found device with number ");
		              vector<char> tmp_chars(40);
		              itoa (telemetr_request[system_settings::MODBUS_DEVICE_NUMBER_INDEX],
		                      &tmp_chars[0],
		                      10);
                      error_message+=&tmp_chars[0];
                      spider_sys_sett->sys_message(system_settings::ERROR_MSG, error_message);
                      continue;       
                   };
                 dev_iter->second->set_request_to_device_buffer (telemetr_request);
                 //in encoding sets answer_from_device_buffer and, if need, sends pulse to update device widget
                 dev_iter->second->encode_answer_to_telemetry_from_data_block
                                                                       (&metro_devices_types,
                                                                         spider_sys_sett);
                 usleep(50000);
                 telemetr_answer=dev_iter->second->get_answer_from_device_buffer();

            } else { // if (!telemetr_request.empty()) none of supported functions recognized
                   telemetr_answer=metro_device::generate_error_answer_for_function
                             ( telemetr_request[system_settings::MODBUS_DEVICE_NUMBER_INDEX],
                               telemetr_request_header[system_settings::MODBUS_FUNCTION_CODE_INDEX],
                               system_settings::MODBUS_EXCEPTION_ILLEGAL_FUNCTION,
                              spider_sys_sett);
           }; //if (!telemetr_request.empty())

        try {
            comport_device->send(telemetr_answer);
	    } catch (comport_exception cmp_exc) {
		          ostringstream exceptioin_message;
		          exceptioin_message<<"station emul comport_thread_function (... ) Fail sending answer from device ";
		          exceptioin_message<<dev_iter->second->get_number()<<" : "<<cmp_exc.get_description();
                  spider_sys_sett->sys_message(system_settings::ERROR_MSG, exceptioin_message.str());
	    }; //catch (comport_exception cmp_exc

    }; //while(true)

};
