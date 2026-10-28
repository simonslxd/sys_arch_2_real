#pragma once

#include <vector>

#include "station_controller.hpp"

namespace station {

// The station's fixed demo data (assumption A-01). Fictional placeholders, not real people or
// equipment. Names must not contain commas (spec/serial-protocol.md). The golden lines in
// spec/serial-protocol-examples.txt depend on this exact data.

inline std::vector<DemoStudent> demo_students()
{
    return {
        {"S001", "Ola Nordmann"},
        {"S002", "Kari Nordmann"},
        {"S003", "Per Nordmann"},
    };
}

inline std::vector<DemoUtstyr> demo_utstyr()
{
    return {
        {"Multimeter", 2},
        {"Oscilloskop", 1},
        {"Loddestasjon", 3},
    };
}

}  // namespace station
