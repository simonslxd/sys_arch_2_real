#include <optional>
#include <string>

#include "check.hpp"
#include "serial_command_reader.hpp"

using station::Command;
using station::SerialCommandReader;

namespace {

// Feeds every character of `text` and returns the last command the reader gave, if any.
std::optional<Command> feed(SerialCommandReader& reader, const std::string& text)
{
    std::optional<Command> last;
    for (char received : text)
    {
        if (std::optional<Command> command = reader.push(received))
        {
            last = command;
        }
    }
    return last;
}

}  // namespace

std::vector<Test> serial_command_reader_tests()
{
    return {
        {"Serial input: the line STATUS is a status request (REQ-COM-03)",
         [] {
             SerialCommandReader reader;
             CHECK(feed(reader, "STATUS\n") == Command::Status);
         }},
        {"Serial input: a request split over several reads, or ending in \\r\\n, still counts",
         [] {
             SerialCommandReader reader;
             CHECK(!feed(reader, "STA").has_value());
             CHECK(feed(reader, "TUS\r\n") == Command::Status);
         }},
        {"Serial input: other lines are ignored, and don't spoil the next request",
         [] {
             SerialCommandReader reader;
             CHECK(!feed(reader, "status\n").has_value());
             CHECK(!feed(reader, "STATUS NOW\n").has_value());
             CHECK(!feed(reader, std::string(100, 'x') + "\n").has_value());
             CHECK(feed(reader, "STATUS\n") == Command::Status);
         }},
    };
}
