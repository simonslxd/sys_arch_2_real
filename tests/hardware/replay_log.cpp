// Checks a serial log recorded from the micro:bit against the PC-tested station code.
//
//   replay_log <log file>
//
// The log is what the board sent (one protocol line per line, START first; protocol version 2).
// From each line the program works out which input the board received:
//   LAAN_UT -> Borrow, RETUR -> Return, VELG with a new student -> NextStudent, otherwise NextItem,
//   STATUS -> a status request (it answers with one STATUS line per item, so one request covers
//   that many lines).
// It replays those inputs on a fresh StationController with the same demo data and compares
// every line it produces with the recorded one. If all match, the board did exactly what the
// verified code does for those inputs, whatever inputs the person actually gave.
// Logs in protocol version 1 (iterations 3 to 6) are checked with that iteration's replay_log.
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "demo_data.hpp"
#include "event_writer.hpp"
#include "station_controller.hpp"

namespace {

std::vector<std::string> split(const std::string& line)
{
    std::vector<std::string> fields;
    std::stringstream stream(line);
    std::string field;
    while (std::getline(stream, field, ','))
    {
        fields.push_back(field);
    }
    return fields;
}

// The input that turned the previous line into this one. Fields: seq,event,studentnummer,...
station::Command received_input(const std::vector<std::string>& previous, const std::vector<std::string>& line)
{
    if (line[1] == "LAAN_UT")
    {
        return station::Command::Borrow;
    }
    if (line[1] == "RETUR")
    {
        return station::Command::Return;
    }
    if (line[1] == "STATUS")
    {
        return station::Command::Status;
    }
    return line[2] != previous[2] ? station::Command::NextStudent : station::Command::NextItem;
}

}  // namespace

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cerr << "usage: replay_log <log file>\n";
        return 2;
    }
    std::ifstream file(argv[1]);
    std::vector<std::string> recorded;
    std::string line;
    while (std::getline(file, line))
    {
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }
        if (!line.empty())
        {
            recorded.push_back(line);
        }
    }
    if (recorded.empty())
    {
        std::cerr << "no lines in " << argv[1] << "\n";
        return 2;
    }

    station::StationController controller(station::demo_students(), station::demo_utstyr());
    std::vector<std::string> replayed = {station::format_event(controller.start())};
    while (replayed.size() < recorded.size())
    {
        // The input behind the next recorded line; its answer may be several lines (STATUS).
        const std::size_t next = replayed.size();
        const station::Command input = received_input(split(recorded[next - 1]), split(recorded[next]));
        for (const station::StationEvent& event : controller.execute(input))
        {
            replayed.push_back(station::format_event(event));
        }
    }

    std::size_t matching = 0;
    for (std::size_t i = 0; i < recorded.size(); ++i)
    {
        const std::string expected = replayed[i].substr(0, replayed[i].size() - 1);   // without "\n"
        if (expected == recorded[i])
        {
            ++matching;
        }
        else
        {
            std::cout << "line " << i << " differs:\n  board:  " << recorded[i] << "\n  replay: " << expected << "\n";
        }
    }
    std::cout << "RESULT: " << matching << " of " << recorded.size() << " recorded lines reproduced\n";
    return matching == recorded.size() ? 0 : 1;
}
