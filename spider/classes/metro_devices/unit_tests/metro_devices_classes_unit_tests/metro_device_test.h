#ifndef __METRO_DEVICE_TEST__
#define __METRO_DEVICE_TEST__

class metro_device_test : public CppUnit::TestFixture {
private:

void test_metro_device_object();

public:
void setUp();
void tearDown();
static CppUnit::TestSuite* suite();
};

#endif