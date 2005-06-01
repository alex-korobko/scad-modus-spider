#ifndef __METRO_SHAVR_TEST__
#define __METRO_SHAVR_TEST__

class shavr_test : public CppUnit::TestFixture {
private:
int shavr_id,
 station_id,
 number,
 type_id,
 channel;
bool enabled;
in_addr_t ip;


void test_shavr_object();

public:
void setUp();
void tearDown();
static CppUnit::TestSuite* suite();
};

#endif