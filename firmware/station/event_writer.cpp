#include "event_writer.hpp"

#include <cstdio>

namespace station {

namespace {

// std::to_string is missing in some ARM toolchains, so numbers are written with snprintf.
std::string number(int value)
{
    char text[12];
    std::snprintf(text, sizeof text, "%d", value);
    return text;
}

}  // namespace

std::string format_event(const StationEvent& event)
{
    return number(event.seq) + "," + event.event + "," + event.studentnummer + "," + event.utstyr_navn + "," +
           event.state + "," + (event.ok ? "OK" : "FEIL") + "," + number(event.antall_tilgjengelig) + "," +
           number(event.antall_totalt) + "\n";
}

}  // namespace station
