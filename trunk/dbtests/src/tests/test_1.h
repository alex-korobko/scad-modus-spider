#ifndef __1_TEST__
#define __1_TEST__

class test_1 : public test_interface {
private:
    enum{DATA_BLOCK_SPLIT_COUNT=3,
    				BUFFER_SPLIT_COUNT=5, 
    				REPEATS_COUNT=10};

	struct test_iteration_result{
        string dbobject_description;
         int iteration_current_data_block;
         int iteration_current_buffer;
         double summary_insertion_time;
         double insertion_sample_mean;
         double insertion_sample_variance;
         double turncation_sample_mean;
         double turncation_sample_variance;

         test_iteration_result(string dbobject_description,
         								int iteration_current_data_block,
                                        int iteration_current_buffer,
                                        double summary_insertion_time,
                                        double insertion_sample_mean,
                                        double insertion_sample_variance,
                                        double turncation_sample_mean,
                                        double turncation_sample_variance):
                dbobject_description(dbobject_description),
                iteration_current_data_block(iteration_current_data_block),
                iteration_current_buffer(iteration_current_buffer),
                summary_insertion_time(summary_insertion_time),
                insertion_sample_mean(insertion_sample_mean),
                insertion_sample_variance(insertion_sample_variance),
                turncation_sample_mean(turncation_sample_mean),
                turncation_sample_variance(turncation_sample_variance) {};

	};

	typedef vector<test_iteration_result> test_results_sample;
	typedef vector<test_iteration_result>::iterator test_results_sample_iterator;

    int min_data_block;
    int max_data_block;
	int data_block_split_count;
	int buffer_split_count;
	int repeats_count;

	test_results_sample test_results;
	
public:
    test_1(string test_description, 
	    int min_data_block, //kbytes
	    int max_data_block, //kbytes
		int data_block_split_count,
		int buffer_split_count,
		int repeats_count );

	int get_min_data_block() { return min_data_block;};
	int get_max_data_block() { return max_data_block;};
	int get_buffer_split_count() { return buffer_split_count;};
	int get_repeats_count() { return repeats_count;};
	
    virtual void run(dbobject_interface *dbobject) throw (test_exception);
    virtual string html_table_report() throw (test_exception);
  
    virtual ~test_1(){};
};

#endif