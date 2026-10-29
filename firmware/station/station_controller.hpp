#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "labutlaan/lab.hpp"

namespace station {

// What the station is asked to do: one per button press or tilt, or a status request from the
// dashboard (spec/serial-protocol.md).
enum class Command { Borrow, Return, NextStudent, NextItem, Status };

// One line of the serial protocol (spec/serial-protocol.md), before it is formatted.
struct StationEvent
{
    int seq;
    std::string event;           // START, LAAN_UT, RETUR, VELG or STATUS
    std::string studentnummer;   // the selected student
    std::string utstyr_navn;     // the item the line is about
    std::string state;           // that item's state: LEDIG or UTLÅNT
    bool ok;
    int antall_tilgjengelig;     // that item's available units
    int antall_totalt;           // that item's total units
};

// The station's demo data (assumption A-01).
struct DemoStudent
{
    std::string studentnummer;
    std::string navn;
};

struct DemoUtstyr
{
    std::string utstyr_navn;
    int antall_totalt;
};

// Holds the Lab and the current selection, and turns each Command into a call on the domain
// model and the events to report. No hardware here, so it is unit-tested on the PC.
class StationController
{
public:
    // Needs at least one student and one item.
    StationController(const std::vector<DemoStudent>& students, const std::vector<DemoUtstyr>& utstyr);

    StationEvent start();

    // One event for a button press or tilt; one STATUS event per item for Command::Status.
    std::vector<StationEvent> execute(Command command);

    // 1-based positions of the current selection, for the LED display: "S2", "U3".
    std::size_t student_number() const;
    std::size_t utstyr_number() const;

private:
    bool borrow();
    bool give_back();
    StationEvent make_event(const std::string& event, bool ok, const labutlaan::Lab_Utstyr& item);

    // The model has no getter for studentnummer, so the station keeps it next to the student (DEV-16).
    struct KnownStudent
    {
        std::string studentnummer;
        std::shared_ptr<labutlaan::Student> student;
    };

    labutlaan::Lab lab_;
    std::vector<KnownStudent> students_;
    std::vector<std::shared_ptr<labutlaan::Lab_Utstyr>> utstyr_;
    std::size_t current_student_ = 0;
    std::size_t current_utstyr_ = 0;
    int next_seq_ = 0;
};

}  // namespace station
