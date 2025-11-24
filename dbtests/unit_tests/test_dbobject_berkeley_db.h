#ifndef __TEST_DBOBJECT_BERKELEY_DB__
#define __TEST_DBOBJECT_BERKELEY_DB__

class test_dbobject_berkeley_db : public CppUnit::TestFixture {
private:
string dbobject_description, database_file_name;
vector<string> supported_dbobject_flags, 
                    supported_database_types;

void test_object_right();
void test_object_wrong();

public:
void setUp();
void tearDown();
static CppUnit::TestSuite* suite();
};

#endif