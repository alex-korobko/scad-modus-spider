#ifndef __COMMAND_TEST__
#define __COMMAND_TEST__

class command_test : public CppUnit::TestFixture {
private:

void test_command_object();

public:
void setUp();
void tearDown();
static CppUnit::TestSuite* suite();
};

#endif