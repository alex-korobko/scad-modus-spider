using namespace std;
#include <string>
#include <vector>
#include <map>
#include <numeric>
#include <algorithm>
#include <sstream>

#include <iostream.h>

#include <inttypes.h>
#include <math.h>
#include <sys/neutrino.h>
#include <sys/syspage.h>
#include <pthread.h>

#include "generator_exception.h"
#include "mock_generator.h"
#include "mock_generator_test.h"


mock_generator_test::mock_generator_test
 (string new_name,
   int channel_id):
   name(new_name),
   gen_for_test(channel_id)   
{};

void mock_generator_test::run_tests() {
}
