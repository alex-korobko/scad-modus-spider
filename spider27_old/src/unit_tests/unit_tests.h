#ifndef __UNIT_TESTS__
#define __UNIT_TESTS__

class unit_test 
{ 
private:

CppUnit::TextUi::TestRunner runner;
public: 
 unit_test()
 {
  	runner.addTest(system_test::suite());
  	runner.addTest(sound_test::suite());
    	runner.addTest(msg_types_container_test::suite());

    	runner.addTest(msg_dict_container_test::suite());
 	runner.addTest(contain_log_rec_test::suite());

    	runner.addTest(line_test::suite());
    	runner.addTest(metro_lines_container_test::suite());
    	
    	runner.addTest(station_test::suite());
    	runner.addTest(metro_stations_container_test::suite());
    	
    	runner.addTest(devices_types_container_test::suite());
    	
    runner.addTest(escalator_test::suite());
    	runner.addTest(metro_escalators_container_test::suite());
 }
    
void run_tests() 
    {
    runner.run("", true);
    }
 };
 
 #endif