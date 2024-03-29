#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <Ap.h>
#include <Ph.h>

#include <photon/PxProto.h>
#include <photon/Pf.h>

#include <sys/socket.h>
#include <sys/syspage.h>
#include <sys/dcmd_chr.h>
#include <sys/neutrino.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/asoundlib.h>
#include <sys/select.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>

#include <net/route.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <pthread.h>

#include <fcntl.h>
#include <devctl.h>
#include <inttypes.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include <string>
#include <map>
#include <list>
#include <vector>
#include <algorithm>
#include <set>

/* Local headers */
#include "ablibs.h"
#include "abimport.h"
#include "proto.h"

using namespace std;

//Automatically generated file that contain defenition __UNIT_TESTING__ if compiling with unit tests
#include "test_cond.h"

#include "defines.h"

#include "system.h"
extern system_settings g_system_settings;

#include "router.h"
extern router g_router;
/*****************************************************************************
*	Container class for messages dictionary - wrapper of STL container map 			*
*	Container class for metro devices types - wrapper of STL container map*
*	Container class for metro escalators - wrapper of STL container map 						*
*Container class for morning start items of escalators - wrapper of STL container map*
*	Container class for metro stations - wrapper of STL container map 								*
*	Container class for metro liness - wrapper of STL container map 										*
*	Container class for command pool - wrapper of STL container list 									*
*	Container class for log records - wrapper of STL container vector 			  *
*	Container class for alerts - wrapper of STL container vector with stack functions*
*****************************************************************************/
#include "contain_msg_types.h"
extern msg_types_container g_msg_types;

#include "contain_msg_dict.h"
extern msg_dict_container g_msg_dictionary;

#include "devices_types.h"
#include "contain_metro_devices_types.h"
extern devices_types_container g_metro_devices_types;

#include "contain_morning_start.h"
#include "line.h"
#include "contain_metro_lines.h"
extern metro_lines_container g_lines;

#include "station.h"
#include "contain_metro_stations.h"
extern metro_stations_container g_stations;

#include "escalator.h"
#include "contain_metro_escalators.h"
extern  metro_escalators_container g_escalators;

#include "command.h"
#include "contain_cmd_pool.h"
extern cmd_pool_container g_command_pool;

#include "contain_log_rec.h"
extern log_records_container  g_main_log, g_archive_log;

#include "sound.h"
extern sound g_sound;

#include "alert.h"
extern alerts_container g_alerts;

extern int g_chanID;
extern pthread_t	g_pingTID, g_timerTID;


#ifdef __UNIT_TESTING__

#include <cppunit/ui/text/TestRunner.h> 
#include <cppunit/TestFixture.h> 
#include <cppunit/TestSuite.h> 
#include <cppunit/TestCaller.h> 

#include "unit_tests/system_test.h"
#include "unit_tests/sound_test.h"

#include "unit_tests/contain_msg_types_test.h"
#include "unit_tests/contain_msg_dict_test.h"
#include "unit_tests/contain_log_rec_test.h"

#include "unit_tests/line_test.h"
#include "unit_tests/contain_metro_lines_test.h"
#include "unit_tests/station_test.h"
#include "unit_tests/contain_metro_stations_test.h"
#include "unit_tests/contain_metro_devices_types_test.h"
#include "unit_tests/escalator_test.h"
#include "unit_tests/contain_metro_escalators_test.h"
#include "unit_tests/contain_morning_start_test.h"

#include "unit_tests/unit_tests.h"

#endif



#endif
