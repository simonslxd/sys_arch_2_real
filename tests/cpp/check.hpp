#pragma once

// A minimal test runner, so the tests need no framework.
//
// A test is a named function. Inside it, CHECK(condition) stops that test with a failure
// when the condition is false. run_tests() runs every test, prints one line per test and
// ends with the summary line "RESULT: <passed> of <total> tests passed", which
// fom/measure_fom.py reads.

#include <functional>
#include <iostream>
#include <string>
#include <vector>

struct CheckFailed
{
    std::string message;
};

#define CHECK(condition)                                                                    \
    do                                                                                      \
    {                                                                                       \
        if (!(condition))                                                                   \
        {                                                                                   \
            throw CheckFailed{std::string(__FILE__) + ":" + std::to_string(__LINE__) +      \
                              ": CHECK(" #condition ") failed"};                            \
        }                                                                                   \
    } while (false)

struct Test
{
    std::string name;
    std::function<void()> run;
};

// Returns the number of failed tests, so main() can return it as the exit code.
inline int run_tests(const std::vector<Test>& tests)
{
    int passed = 0;
    for (const Test& test : tests)
    {
        try
        {
            test.run();
            ++passed;
            std::cout << "PASS " << test.name << "\n";
        }
        catch (const CheckFailed& failure)
        {
            std::cout << "FAIL " << test.name << "\n     " << failure.message << "\n";
        }
    }
    std::cout << "RESULT: " << passed << " of " << tests.size() << " tests passed\n";
    return static_cast<int>(tests.size()) - passed;
}
