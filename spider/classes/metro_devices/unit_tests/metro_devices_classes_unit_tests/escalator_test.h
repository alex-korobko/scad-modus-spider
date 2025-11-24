#ifndef __METRO_ESCALATOR_TEST__
#define __METRO_ESCALATOR_TEST__

class escalator_test : public CppUnit::TestFixture {
private:
int escalator_id,
 station_id,
 number,
 type_id,
 pref_direction,
 direction,
 start_day_mode,
 start_hour,
 start_minute,
 channel;
  bool enabled;
  in_addr_t ip;

metro_escalator *tested_escalator;

void test_escalator_object();

public:
void setUp();
void tearDown();
static CppUnit::TestSuite* suite();
};

#endif