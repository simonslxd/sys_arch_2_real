#include "check.hpp"
#include "station_controller.hpp"

using station::Command;
using station::StationController;

namespace {

// Two students, two items with one unit each: small enough to reason about in every test.
StationController small_station()
{
    return StationController({{"S001", "Ola Nordmann"}, {"S002", "Kari Nordmann"}},
                             {{"Multimeter", 1}, {"Oscilloskop", 1}});
}

}  // namespace

std::vector<Test> station_controller_tests()
{
    return {
        {"Station: start reports the first student and item, seq 0",
         [] {
             StationController station = small_station();
             station::StationEvent event = station.start();
             CHECK(event.seq == 0);
             CHECK(event.event == "START");
             CHECK(event.studentnummer == "S001");
             CHECK(event.utstyr_navn == "Multimeter");
             CHECK(event.state == "LEDIG");
             CHECK(event.ok);
         }},
        {"Station: borrow (REQ-HW-01) lends the selected item to the selected student",
         [] {
             StationController station = small_station();
             station::StationEvent event = station.execute(Command::Borrow).front();
             CHECK(event.event == "LAAN_UT");
             CHECK(event.ok);
             CHECK(event.state == "UTLÅNT");
             CHECK(!station.execute(Command::Borrow).front().ok);   // the only unit is lent
         }},
        {"Station: return (REQ-HW-02) gives back the selected student's loan of the selected item",
         [] {
             StationController station = small_station();
             station.execute(Command::Borrow);
             station::StationEvent event = station.execute(Command::Return).front();
             CHECK(event.event == "RETUR");
             CHECK(event.ok);
             CHECK(event.state == "LEDIG");
         }},
        {"Station: return without a matching loan fails (A-03)",
         [] {
             StationController station = small_station();
             station.execute(Command::Borrow);              // S001 borrows Multimeter
             station.execute(Command::NextStudent);         // S002 selected
             CHECK(!station.execute(Command::Return).front().ok);   // S002 has no loan of it
         }},
        {"Station: tilt left / right (REQ-HW-03) select the next student / item, with wrap-around",
         [] {
             StationController station = small_station();
             CHECK(station.execute(Command::NextStudent).front().studentnummer == "S002");
             CHECK(station.student_number() == 2);
             CHECK(station.execute(Command::NextStudent).front().studentnummer == "S001");
             CHECK(station.execute(Command::NextItem).front().utstyr_navn == "Oscilloskop");
             CHECK(station.utstyr_number() == 2);
             CHECK(station.execute(Command::NextItem).front().utstyr_navn == "Multimeter");
         }},
        {"Station: seq counts every event",
         [] {
             StationController station = small_station();
             CHECK(station.start().seq == 0);
             CHECK(station.execute(Command::NextItem).front().seq == 1);
             CHECK(station.execute(Command::Borrow).front().seq == 2);
         }},
        {"Station: every event carries the item's available and total count (REQ-UI-02)",
         [] {
             StationController station({{"S001", "Ola Nordmann"}}, {{"Multimeter", 2}});
             station::StationEvent event = station.execute(Command::Borrow).front();
             CHECK(event.antall_tilgjengelig == 1);
             CHECK(event.antall_totalt == 2);
             event = station.execute(Command::Return).front();
             CHECK(event.antall_tilgjengelig == 2);
         }},
        {"Station: Status gives one STATUS event per item, in order, with its counts (REQ-COM-03)",
         [] {
             StationController station = small_station();
             station.execute(Command::Borrow);   // S001 borrows the only Multimeter
             std::vector<station::StationEvent> events = station.execute(Command::Status);
             CHECK(events.size() == 2);
             CHECK(events[0].event == "STATUS");
             CHECK(events[0].utstyr_navn == "Multimeter");
             CHECK(events[0].antall_tilgjengelig == 0);
             CHECK(events[0].state == "UTLÅNT");
             CHECK(events[1].utstyr_navn == "Oscilloskop");
             CHECK(events[1].antall_tilgjengelig == 1);
             CHECK(events[1].seq == events[0].seq + 1);
         }},
    };
}
