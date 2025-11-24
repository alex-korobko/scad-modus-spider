#ifndef __1_TEST_TEST__
#define __1_TEST_TEST__

class test_test_1 : public CppUnit::TestFixture {
private:
dbobject_interface *tested_dbobject_mock;
test_interface *tested_test_1;
string test_description, dbobject_description;

int min_data_block;
int max_data_block;
int data_block_split_count;
int buffer_split_count;
int repeats_count;

void test_of_test_1();

public:
void setUp();
void tearDown();
static CppUnit::TestSuite* suite();
};

#endif