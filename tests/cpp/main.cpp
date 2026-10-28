#include "check.hpp"

// Each test file returns its own list of tests.
std::vector<Test> lab_utstyr_tests();
std::vector<Test> student_tests();
std::vector<Test> utlaan_tests();
std::vector<Test> lab_tests();
std::vector<Test> station_controller_tests();
std::vector<Test> event_writer_tests();
std::vector<Test> input_decoder_tests();

int main()
{
    std::vector<Test> all;
    for (const auto& tests : {lab_utstyr_tests(), student_tests(), utlaan_tests(), lab_tests(),
                              station_controller_tests(), event_writer_tests(), input_decoder_tests()})
    {
        all.insert(all.end(), tests.begin(), tests.end());
    }
    return run_tests(all);
}
