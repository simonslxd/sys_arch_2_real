#include "station_controller.hpp"

namespace station {

StationController::StationController(const std::vector<DemoStudent>& students,
                                     const std::vector<DemoUtstyr>& utstyr)
{
    for (const DemoStudent& demo : students)
    {
        auto student = std::make_shared<labutlaan::Student>(demo.studentnummer, demo.navn);
        lab_.registrer_student(student);
        students_.push_back({demo.studentnummer, student});
    }
    for (const DemoUtstyr& demo : utstyr)
    {
        auto item = std::make_shared<labutlaan::Lab_Utstyr>(demo.utstyr_navn, demo.antall_totalt);
        lab_.legg_til_utstyr(item);
        utstyr_.push_back(item);
    }
}

StationEvent StationController::start() { return make_event("START", true, *utstyr_[current_utstyr_]); }

std::vector<StationEvent> StationController::execute(Command command)
{
    const labutlaan::Lab_Utstyr& selected = *utstyr_[current_utstyr_];
    switch (command)
    {
    case Command::Borrow:
    {
        const bool ok = borrow();
        return {make_event("LAAN_UT", ok, selected)};
    }
    case Command::Return:
    {
        const bool ok = give_back();
        return {make_event("RETUR", ok, selected)};
    }
    case Command::NextStudent:
        current_student_ = (current_student_ + 1) % students_.size();
        return {make_event("VELG", true, selected)};
    case Command::NextItem:
        current_utstyr_ = (current_utstyr_ + 1) % utstyr_.size();
        return {make_event("VELG", true, *utstyr_[current_utstyr_])};
    case Command::Status:
    {
        std::vector<StationEvent> events;
        for (const auto& item : utstyr_)
        {
            events.push_back(make_event("STATUS", true, *item));
        }
        return events;
    }
    }
    return {};   // not reached: every Command is handled above
}

std::size_t StationController::student_number() const { return current_student_ + 1; }

std::size_t StationController::utstyr_number() const { return current_utstyr_ + 1; }

bool StationController::borrow()
{
    labutlaan::Student& student = *students_[current_student_].student;
    labutlaan::Lab_Utstyr& item = *utstyr_[current_utstyr_];
    return lab_.laan_ut(student, item) != nullptr;
}

bool StationController::give_back()
{
    // Return the selected student's loan of the selected item (assumption A-03).
    labutlaan::Student& student = *students_[current_student_].student;
    labutlaan::Lab_Utstyr& item = *utstyr_[current_utstyr_];
    for (labutlaan::Utlaan* utlaan : student.get_laan())
    {
        if (&utlaan->get_utstyr() == &item)
        {
            lab_.registrer_retur(*utlaan);   // changes student.get_laan(), so stop looping now
            return true;
        }
    }
    return false;
}

StationEvent StationController::make_event(const std::string& event, bool ok, const labutlaan::Lab_Utstyr& item)
{
    const bool ledig = item.get_state() == labutlaan::Lab_Utstyr::State::LEDIG;
    return {next_seq_++,
            event,
            students_[current_student_].studentnummer,
            item.get_navn(),
            ledig ? "LEDIG" : "UTLÅNT",
            ok,
            item.get_antall_tilgjengelig(),
            item.get_antall_totalt()};
}

}  // namespace station
