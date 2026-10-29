#include <fstream>
#include <string>
#include <vector>

#include "check.hpp"
#include "demo_data.hpp"
#include "event_writer.hpp"
#include "station_controller.hpp"

using station::Command;
using station::StationController;

namespace {

// The protocol lines of spec/serial-protocol-examples.txt: every line that is not a comment.
std::vector<std::string> golden_lines()
{
    std::ifstream file(GOLDEN_LINES_FILE);
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line))
    {
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();   // the file may have Windows line endings after a git checkout
        }
        if (!line.empty() && line[0] != '#')
        {
            lines.push_back(line + "\n");
        }
    }
    return lines;
}

}  // namespace

std::vector<Test> event_writer_tests()
{
    return {
        {"event_writer: formats every field in protocol order",
         [] {
             station::StationEvent event{7, "LAAN_UT", "S002", "Multimeter", "UTLÅNT", false, 0, 2};
             CHECK(station::format_event(event) == "7,LAAN_UT,S002,Multimeter,UTLÅNT,FEIL,0,2\n");
         }},
        {"Golden lines: the input sequence in spec/serial-protocol-examples.txt gives exactly its lines",
         [] {
             // The inputs listed in the comments of the golden file, in order.
             const Command inputs[] = {
                 Command::Borrow,      Command::Borrow,      Command::Borrow,   Command::NextStudent,
                 Command::Return,      Command::NextStudent, Command::NextStudent, Command::Return,
                 Command::NextItem,    Command::Borrow,      Command::NextItem, Command::NextItem,
                 Command::Status,
             };
             StationController controller(station::demo_students(), station::demo_utstyr());
             std::vector<std::string> sent = {station::format_event(controller.start())};
             for (Command input : inputs)
             {
                 for (const station::StationEvent& event : controller.execute(input))
                 {
                     sent.push_back(station::format_event(event));
                 }
             }

             const std::vector<std::string> expected = golden_lines();
             CHECK(expected.size() == 16);
             CHECK(sent.size() == expected.size());
             for (std::size_t i = 0; i < expected.size(); ++i)
             {
                 CHECK(sent[i] == expected[i]);
             }
         }},
    };
}
