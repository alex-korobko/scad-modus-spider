#ifndef __TEST_COMPORT__
#define __TEST_COMPORT__

class test_comport : public CppUnit::TestFixture {
private:
int port_number;
int baud_rate;
byte parity_check;
byte data_bits;
byte stop_bits;
int delay_useconds;
void test();

public:
void setUp();
void tearDown();
static CppUnit::TestSuite* suite();
};

#endif