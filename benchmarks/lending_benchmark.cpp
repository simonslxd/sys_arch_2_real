// Benchmark: how long one loan + return takes in the domain model (REQ-TST-02).
//
//   lending_benchmark      prints one line per lab size, then "BENCH_NS: <n>" for the large lab
//
// Each measurement runs 10 000 pairs of Lab::laan_ut + Lab::registrer_retur and is repeated 7
// times; the median is reported, so a short disturbance on the PC doesn't decide the result.
// The student and the item are the last ones in the lab's lists, so laan_ut's searches
// (finn_tilgjengelig_utstyr, finnStudent) go through the whole list: the worst case.
//
// The numbers depend on the PC, so they are logged next to the FoM but are not part of it.
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <memory>
#include <string>
#include <vector>

#include "labutlaan/lab.hpp"

namespace {

const int PAIRS = 10000;
const int REPEATS = 7;

// Nanoseconds per laan_ut + registrer_retur pair, median of REPEATS runs, in a lab with
// `size` students and `size` items.
double nanoseconds_per_pair(int size)
{
    labutlaan::Lab lab;
    std::shared_ptr<labutlaan::Student> student;
    std::shared_ptr<labutlaan::Lab_Utstyr> item;
    for (int i = 0; i < size; ++i)
    {
        student = std::make_shared<labutlaan::Student>("S" + std::to_string(i), "Student");
        item = std::make_shared<labutlaan::Lab_Utstyr>("Utstyr " + std::to_string(i), 1);
        lab.registrer_student(student);
        lab.legg_til_utstyr(item);
    }

    std::vector<double> runs;
    for (int run = 0; run < REPEATS; ++run)
    {
        const auto start = std::chrono::steady_clock::now();
        for (int pair = 0; pair < PAIRS; ++pair)
        {
            labutlaan::Utlaan* utlaan = lab.laan_ut(*student, *item);
            if (utlaan == nullptr)
            {
                std::fprintf(stderr, "laan_ut failed: the benchmark is not measuring a loan\n");
                return -1;
            }
            lab.registrer_retur(*utlaan);
        }
        const std::chrono::duration<double, std::nano> elapsed = std::chrono::steady_clock::now() - start;
        runs.push_back(elapsed.count() / PAIRS);
    }
    std::sort(runs.begin(), runs.end());
    return runs[REPEATS / 2];
}

}  // namespace

int main()
{
    const double demo = nanoseconds_per_pair(3);     // the station's demo data size
    const double large = nanoseconds_per_pair(100);
    if (demo < 0 || large < 0)
    {
        return 1;
    }
    std::printf("lab with   3 students and   3 items: %.0f ns per laan_ut + registrer_retur\n", demo);
    std::printf("lab with 100 students and 100 items: %.0f ns per laan_ut + registrer_retur\n", large);
    std::printf("BENCH_NS: %.0f\n", large);
    return 0;
}
