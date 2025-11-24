	using namespace std;

#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <iostream.h>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <sstream>

#include "../system_settings_classes/defines.h"
#include "../com_port_class/comport_exception.h"
#include "../com_port_class/comport.h"

#include "test_comport.h"

void test_comport::test(){
bool exception_raised;
ostringstream exception_message;
int port_number;

exception_raised=false;
port_number=5;
{
	try {

	comport  tested_comport(port_number,
										baud_rate,
										parity_check,
										data_bits,
										stop_bits,
										delay_useconds);

	 } catch (comport_exception cmp_exc) {
		exception_raised=true;
		exception_message<<"Wrong com port number /dev/ser"<<port_number;
	 	CPPUNIT_ASSERT_EQUAL (cmp_exc.get_description(),
 								exception_message.str());
 	};
	 if (!exception_raised)  {
		CPPUNIT_FAIL("Exception did not raised");
		}
 }

exception_message.str("");
port_number=2;
{
	try {

  		comport  tested_comport(port_number,
										baud_rate,
										parity_check,
										data_bits,
										stop_bits,
										delay_useconds);

		byte rts_status =  comport::RTS_SET_1;
		tested_comport.set_rts_state(rts_status);
		CPPUNIT_ASSERT_EQUAL(rts_status,	tested_comport.get_rts_state());

		vector<byte> data_to_send, recieved_data;
		data_to_send.push_back(1);
		data_to_send.push_back(2);
		data_to_send.push_back(3);
		data_to_send.push_back(4);
		data_to_send.push_back(0);

		tested_comport.send(data_to_send);
		recieved_data=tested_comport.recieve(5);

		CPPUNIT_ASSERT(data_to_send==recieved_data);

		rts_status =  comport::RTS_SET_0;
		tested_comport.set_rts_state(rts_status);
		CPPUNIT_ASSERT_EQUAL(rts_status, tested_comport.get_rts_state());

	 } catch (comport_exception cmp_exc) {
		exception_message<<"Unexpected exception : "<<cmp_exc.get_description();
	 	CPPUNIT_FAIL (exception_message.str());
 	};

}

};

void test_comport::setUp(){
port_number=5;
baud_rate=comport::BAUD_19200;
parity_check = comport::PARITY_DISABLE;
data_bits = comport::DATA_BITS_8;
stop_bits=comport::STOP_BITS_1;
delay_useconds = 10000;
};

void test_comport::tearDown(){

};

CppUnit::TestSuite* test_comport::suite(){
CppUnit::TestSuite *suite_of_tests=new CppUnit::TestSuite("Test of comport object");

suite_of_tests->addTest(new CppUnit::TestCaller<test_comport>
    ( "Test of comport object",
    &test_comport::test));

return suite_of_tests;

};
