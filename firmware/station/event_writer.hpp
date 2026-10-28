#pragma once

#include <string>

#include "station_controller.hpp"

namespace station {

// One event as one line of the serial protocol (spec/serial-protocol.md), "\n" included:
//   <seq>,<event>,<studentnummer>,<utstyr_navn>,<state>,<result>
std::string format_event(const StationEvent& event);

}  // namespace station
