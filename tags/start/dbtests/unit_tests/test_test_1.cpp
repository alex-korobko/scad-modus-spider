using namespace std;

#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

#include <inttypes.h>

#include <iostream.h>

#include <vector>
#include <string>
#include <sstream>

#include "../src/defines.h"

#include "../src/dbobjects/dbobject_exception.h"
#include "../src/dbobjects/dbobject_interface.h"
#include "../src/dbobjects/dbobject_mock.h"

#include "../src/tests/test_exception.h"
#include "../src/tests/test_interface.h"
#include "../src/tests/test_1.h"

#include "test_test_1.h"

void test_test_1::test_of_test_1(){
	string report_of_test;
   	ostringstream expected_table, test_description_stream;
    int  current_data_block, step_data_block,
			current_buffer, step_buffer;
	vector<byte> mock_vector(1);	

	test_description_stream<<test_description<<"</br><small>блок данных от "<<min_data_block<<" до "<<max_data_block<<"</br>";
	test_description_stream<<"тестрируются "<<data_block_split_count<<" разных размера диапазона</br>";
	test_description_stream<<"вставка производится из буффера размером 1/"<<buffer_split_count<<" от размера блока данных</br>";
	test_description_stream<<"производится"<<repeats_count<<"повторений</small>";

    expected_table<<"<table width=\"95%\" border=\"1\" align=\"center\">\n";

    expected_table<<"\t<th>\n";
    expected_table<<"\t";
    expected_table<<test_description_stream.str();
    expected_table<<"\n";
    expected_table<<"\t</th>\n";

    expected_table<<"\t<tr>\n";

    expected_table<<"\t\t<td align = \"center\" width = \"60%\">";
    expected_table<<"Описание теста";
    expected_table<<"</td>\n";

    expected_table<<"\t\t<td align = \"center\">";
    expected_table<<"Общее время вставки</br>(сек)";
    expected_table<<"</td>\n";

    expected_table<<"\t\t<td align = \"center\">";
    expected_table<<"Среднее время вставки</br>(сек)";
    expected_table<<"</td>\n";

    expected_table<<"\t\t<td align = \"center\">";
    expected_table<<"Дисперсия времении вставки</br>(сек)";
    expected_table<<"</td>\n";

    expected_table<<"\t\t<td align = \"center\">";
    expected_table<<"Среднее время очистки</br>(сек)";
    expected_table<<"</td>\n";

    expected_table<<"\t\t<td align = \"center\">";
    expected_table<<"Дисперсия времении очистки</br>(сек)";
    expected_table<<"</td>\n";

    expected_table<<"\t</tr>\n";

    step_data_block=(max_data_block-min_data_block)*1024/data_block_split_count;
	current_data_block=min_data_block*1024;

	while (current_data_block<=max_data_block*1024) {
		step_buffer = current_data_block/buffer_split_count;
		current_buffer=step_buffer;
		while (current_buffer<=current_data_block) {
			
			test_description_stream.str("");
			test_description_stream<<dbobject_description;
            test_description_stream<<"</br>размер данных "<<current_data_block;
            test_description_stream<<"</br>размер буффера "<<current_buffer;
	
			expected_table<<"\n\t<tr>\n";
			expected_table<<"\t\t<td align = \"center\" width = \"60%\">";
			expected_table<<test_description_stream.str();
			expected_table<<"</td>\n";

		   expected_table<<"\t\t<td align = \"center\">";
		   expected_table<<static_cast<long>(tested_dbobject_mock->test_1_insert_blob(mock_vector)*repeats_count);
		   expected_table<<"</td>\n";

		   expected_table<<"\t\t<td align = \"center\">";
		   expected_table<<static_cast<long>((tested_dbobject_mock->test_1_insert_blob(mock_vector)*repeats_count)/repeats_count);
		   expected_table<<"</td>\n";

		   expected_table<<"\t\t<td align = \"center\">";
		   expected_table<<0; //always return same insert time
		   expected_table<<"</td>\n";

		   expected_table<<"\t\t<td align = \"center\">";
		   expected_table<<static_cast<long>((tested_dbobject_mock->test_1_turncate_table()*repeats_count)/repeats_count);
		   expected_table<<"</td>\n";

           expected_table<<"\t\t<td align = \"center\">";
           expected_table<<0;
           expected_table<<"</td>\n";

           expected_table<<"\t</tr>\n";

			current_buffer+=step_buffer;
		};
		current_data_block+=step_data_block;
	};

    expected_table<<"</table>\n";

    try {
      tested_test_1->run(tested_dbobject_mock);
    } catch (test_exception tst_exc)  {
        CPPUNIT_FAIL("Unexpected exception:"+tst_exc.get_description());
    }

    try {
      report_of_test=tested_test_1->html_table_report();
    } catch (test_exception tst_exc)  {
        CPPUNIT_FAIL("Unexpected exception:"+tst_exc.get_description());
    }
    
    CPPUNIT_ASSERT_EQUAL(report_of_test,
			expected_table.str());

};

void test_test_1::setUp(){
min_data_block=4;
max_data_block=8;
data_block_split_count=3;
buffer_split_count=3;
repeats_count=10;

test_description = "Тестирование операции вставки в БД";
dbobject_description = "Объект-заглушка для тестирования и отладки ";

vector<string> connection_parameters;
	
connection_parameters.push_back("USERNAME=right_user");
connection_parameters.push_back("PASSWD=right_password");

tested_dbobject_mock = new dbobject_mock(dbobject_description, 
										 connection_parameters);

tested_test_1 = new test_1(test_description,
			    min_data_block,
			    max_data_block,
			    	data_block_split_count,
				buffer_split_count,
				repeats_count);
};

void test_test_1::tearDown(){
	delete (tested_dbobject_mock);
	delete (tested_test_1);
};

CppUnit::TestSuite* test_test_1::suite(){
CppUnit::TestSuite *suite_of_tests=new CppUnit::TestSuite("Test of test_1 object (using previosly tested dbobject_mock)");

suite_of_tests->addTest(new CppUnit::TestCaller<test_test_1>
    ( "test of run and  metod",
    &test_test_1::test_of_test_1));

return suite_of_tests;

};
