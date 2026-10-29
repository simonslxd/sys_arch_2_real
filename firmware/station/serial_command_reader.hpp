#pragma once

#include <optional>
#include <string>

#include "station_controller.hpp"

namespace station {

// Collects the characters the dashboard sends into lines (spec/serial-protocol.md). The line
// "STATUS" is a request for every item's status; any other line is ignored. No hardware here,
// so it is unit-tested on the PC.
class SerialCommandReader
{
public:
    // Call with each received character; returns Command::Status when a "STATUS" line is complete.
    std::optional<Command> push(char received);

private:
    static constexpr std::size_t MAX_LINE = 32;   // longer lines are not commands: no need to keep them

    std::string line_;
};

}  // namespace station
