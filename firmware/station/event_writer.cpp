#include "event_writer.hpp"

#include <cstdio>

namespace station {

std::string format_event(const StationEvent& event)
{
    char seq[12];
    std::snprintf(seq, sizeof seq, "%d", event.seq);   // std::to_string is missing in some ARM toolchains
    return std::string(seq) + "," + event.event + "," + event.studentnummer + "," + event.utstyr_navn + "," +
           event.state + "," + (event.ok ? "OK" : "FEIL") + "\n";
}

}  // namespace station
