#include "serial_command_reader.hpp"

namespace station {

std::optional<Command> SerialCommandReader::push(char received)
{
    if (received != '\n')
    {
        if (received != '\r' && line_.size() < MAX_LINE)
        {
            line_ += received;
        }
        return std::nullopt;
    }
    const bool is_status = line_ == "STATUS";
    line_.clear();
    if (is_status)
    {
        return Command::Status;
    }
    return std::nullopt;
}

}  // namespace station
