using namespace std;

#include <inttypes.h>

#include <iostream.h> 
#include <ios> 
#include <numeric>
#include <vector>
#include <string> 
#include <sstream> 

#include "../defines.h"

#include "../dbobjects/dbobject_exception.h"
#include "../dbobjects/dbobject_interface.h"

#include "test_exception.h"
#include "test_interface.h"
#include "test_1.h"

test_1::test_1(string test_description, 
	    int min_data_block, //kbytes
	    int max_data_block, //kbytes
		int data_block_split_count,
		int buffer_split_count,
		int repeats_count
	    ): 
    test_interface(test_description),
	min_data_block(min_data_block),
	max_data_block(max_data_block),
	data_block_split_count(data_block_split_count),
	buffer_split_count(buffer_split_count),
	repeats_count(repeats_count) {};

void test_1::run(dbobject_interface *dbobject) throw (test_exception){
  ostringstream exception_description;
  int  step_data_block, current_data_block, step_buffer, current_buffer, counter_of_repeats;
  vector<uint64_t> test_insertion_sample, test_turncation_sample;
  vector<byte> insertion_buffer;

  step_data_block=(max_data_block-min_data_block)*1024/data_block_split_count;
  current_data_block=min_data_block*1024;

  while (current_data_block<=max_data_block*1024) {
  step_buffer = current_data_block/buffer_split_count;
  insertion_buffer.assign(step_buffer, 0xFF);

      counter_of_repeats=0;
      while(counter_of_repeats<repeats_count) {

         current_buffer=step_buffer;
         while (current_buffer<=current_data_block) {
         	try {
			test_insertion_sample.push_back(
                   dbobject->test_1_insert_blob(insertion_buffer));         
    		  	} catch (dbobject_exception dbj_exc) {
    		  		exception_description<<"Inserting "<<current_data_block<<" bytes by parts of ";
    		  		exception_description<<step_buffer<<" raise exception: \n"<<dbj_exc.get_description();
    		  		throw (test_exception(exception_description.str()));
 		  	};

          current_buffer+=step_buffer;
          }; //while (current_buffer<=
			try {
	   		test_turncation_sample.push_back(
 		  				dbobject->test_1_turncate_table());
 		  	} catch (dbobject_exception dbj_exc) {
    		  		exception_description<<"Turncating after insert "<<current_data_block<<" bytes by parts of ";
    		  		exception_description<<step_buffer<<" raise exception: \n"<<dbj_exc.get_description();
    		  		throw (test_exception(exception_description.str()));
 		  	};
		counter_of_repeats++;
      }; //while(counter_of_repeats

	test_results.push_back(
       test_iteration_result(
                dbobject->get_description(),
                current_data_block,
                step_buffer,
                accumulate(test_insertion_sample.begin(), test_insertion_sample.end(), double(0.0))*1000/(repeats_count*test_interface::cycles_per_second),
                (test_interface::calculate_sample_mean(test_insertion_sample)*1000)/test_interface::cycles_per_second,
                (test_interface::calculate_standart_deviation(test_insertion_sample)*1000)/test_interface::cycles_per_second,
                (test_interface::calculate_sample_mean(test_turncation_sample)*1000)/test_interface::cycles_per_second,
	            (test_interface::calculate_standart_deviation(test_turncation_sample)*1000)/test_interface::cycles_per_second   )
         );

        test_insertion_sample.clear();
        test_turncation_sample.clear();
		current_data_block+=step_data_block;
}; //while (current_data_block<

};

string test_1::html_table_report() throw (test_exception){
ostringstream return_string, test_description;

test_description<<"<br><center>"<<this->get_description()<<"<br><small>блок данных от "<<min_data_block<<"K до "<<max_data_block<<"K<br>";
test_description<<"тестрируются "<<data_block_split_count<<" разных размера диапазона<br>";
test_description<<"вставка производится из буффера размером 1/"<<buffer_split_count<<" от размера блока данных<br>";
test_description<<"выполняется "<<repeats_count<<" повторений</small></center><br>";

return_string<<test_description.str();

return_string<<"<table width=\"95%\" border=\"1\" align=\"center\">\n";

return_string<<"\t<tr>\n";
return_string<<"\t\t<td align = \"center\" width = \"60%\">";
return_string<<"Описание теста";
return_string<<"</td>\n";

return_string<<"\t\t<td align = \"center\">";
return_string<<"Общее время вставки<br>(миллисек)";
return_string<<"</td>\n";

return_string<<"\t\t<td align = \"center\">";
return_string<<"Среднее время вставки<br>(миллисек)";
return_string<<"</td>\n";

return_string<<"\t\t<td align = \"center\">";
return_string<<"Оценка стандартного отклонения времени вставки<br>(миллисек)";
return_string<<"</td>\n";

return_string<<"\t\t<td align = \"center\">";
return_string<<"Среднее время очистки<br>(миллисек)";
return_string<<"</td>\n";

return_string<<"\t\t<td align = \"center\">";
return_string<<"Оценка стандартного отклонения времени очистки<br>(миллисек)";
return_string<<"</td>\n";

return_string<<"\t</tr>\n";

ios::fmtflags old_flags = return_string.flags();
return_string.flags(ios_base::fixed|ios_base::dec);

test_results_sample_iterator res_iter=test_results.begin();
while (res_iter!=test_results.end()) {
	
			return_string<<"\n\t<tr>\n";
			return_string<<"\t\t<td align = \"center\" width = \"60%\">";
			return_string<<res_iter->dbobject_description;
			return_string<<"<br>размер данных "<<res_iter->iteration_current_data_block<<" байт";
            return_string<<"<br>размер буффера "<<res_iter->iteration_current_buffer<<" байт";
			return_string<<"</td>\n";

		   return_string<<"\t\t<td align = \"center\">";
		   return_string<<res_iter->summary_insertion_time;
		   return_string<<"</td>\n";

		   return_string<<"\t\t<td align = \"center\">";
		   return_string<<res_iter->insertion_sample_mean;
		   return_string<<"</td>\n";

		   return_string<<"\t\t<td align = \"center\">";
		   return_string<<res_iter->insertion_sample_variance;
		   return_string<<"</td>\n";

		   return_string<<"\t\t<td align = \"center\">";
		   return_string<<res_iter->turncation_sample_mean;
		   return_string<<"</td>\n";

           return_string<<"\t\t<td align = \"center\">";
           return_string<<res_iter->turncation_sample_variance;
           return_string<<"</td>\n";

           return_string<<"\t</tr>\n";

res_iter++;
};

return_string.flags(old_flags);

return_string<<"</table>\n";
				
return return_string.str();
};
  
