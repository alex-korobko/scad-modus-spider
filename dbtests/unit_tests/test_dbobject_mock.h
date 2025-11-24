#ifndef __TEST_DBOBJECT_MOCK__
#define __TEST_DBOBJECT_MOCK__

class test_dbobject_mock : public CppUnit::TestFixture {
private:
string dbobject_description, 
	right_username, 
	right_password, 
	wrong_username, 
	wrong_password;

void test_object_right();
void test_object_wrong();

public:
void setUp();
void tearDown();
static CppUnit::TestSuite* suite();
};

#endif