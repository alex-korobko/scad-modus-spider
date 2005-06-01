class mock_generator_test {
private:
  string name;
  mock_generator gen_for_test;

  mock_generator_test();

public:
  mock_generator_test( string new_name , int channel_id);
  string get_name() { return name;};

  void run_tests();
};
