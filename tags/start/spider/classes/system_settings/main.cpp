using namespace std;

#include "test_cond.h"

#include <sys/types.h>
#include <sys/neutrino.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>

#include <iostream.h>

#include <string>
#include <vector>

#include <Ap.h>
#include <Pt.h>

#include "system_settings_classes/defines.h"
#include "system_settings_classes/system_settings.h"
#include "system_settings_classes/spider_exception.h"
#include "system_settings_classes/system_settings_spider.h"

#ifdef __UNIT_TESTING__

#include <cppunit/TestFixture.h> 
#include <cppunit/TestSuite.h> 
#include <cppunit/TestCaller.h>
#include <cppunit/ui/text/TestRunner.h> 

#include "unit_tests/system_settings_classes_unit_tests/system_settings_test.h"
#include "unit_tests/system_settings_classes_unit_tests/system_settings_spider_test.h"
#include "unit_tests/all_system_settings_classes_tests.h"

#endif

PtWidget_t *main_window=NULL;

int activate_start_test_button( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo ){
	#ifdef __UNIT_TESTING__
		cout<<all_system_settings_tests::run_tests()<<endl;
	#endif
	
		return( Pt_CONTINUE );
	};



int main (int argc , char* argv[]) 
{
vector<PtArg_t> args(6);
vector<PtCallback_t> callbacks(1);
PtCallback_t tmp_callback;
PhPoint_t position;

tmp_callback.event_f=activate_start_test_button;
tmp_callback.data=NULL;
callbacks.push_back(tmp_callback);

PtSetArg(&args[0], Pt_ARG_WINDOW_TITLE,
         "Test of system_settings_spider object", 0);
PtSetArg(&args[1], Pt_ARG_WINDOW_MANAGED_FLAGS,
         Pt_TRUE, Ph_WM_CLOSE);
PtSetArg(&args[2], Pt_ARG_WINDOW_MANAGED_FLAGS,
         Pt_FALSE, Ph_WM_TOFRONT );
PtSetArg(&args[3], Pt_ARG_WINDOW_MANAGED_FLAGS,
         Pt_TRUE, Ph_WM_TOBACK);
PtSetArg(&args[4], Pt_ARG_HEIGHT,
         300, 0);
PtSetArg(&args[5], Pt_ARG_WIDTH,
         500, 0);
         
main_window=PtAppInit( NULL,
                0,
                NULL ,
                args.size(),
                &args[0]);

 args.resize(8);
 
 position.x=130;
 position.y=60;
 string button_text("Start tests");
 
PtSetArg(&args[0], Pt_ARG_LABEL_TYPE,
         Pt_Z_STRING, 0);
PtSetArg(&args[1], Pt_ARG_TEXT_STRING,
         button_text.c_str(), 0);
PtSetArg(&args[3], Pt_ARG_HEIGHT,
         40, 0);
PtSetArg(&args[4], Pt_ARG_WIDTH,
         100, 0);
PtSetArg(&args[5], Pt_ARG_POS,
         &position, 0);      
PtSetArg(&args[6],  Pt_CB_ACTIVATE,
			  &callbacks[0],
			  callbacks.size());

PtCreateWidget(PtButton,
						main_window,
 		                args.size(),
  			            &args[0]);

PtRealizeWidget(main_window);
PtMainLoop();

 return (EXIT_SUCCESS);
}
