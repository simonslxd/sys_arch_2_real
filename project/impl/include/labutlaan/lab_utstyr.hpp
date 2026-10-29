#pragma once

#include <string>

namespace labutlaan {

class Lab_Utstyr
{
public:
    // The states and events of diagrams/input/TilstandDiagram.drawio.
    enum class State { LEDIG, UTLÅNT };
    enum class Event { regUtlaan, regRetur };

    Lab_Utstyr(std::string utstyr_navn, int antall_totalt);

    bool er_tilgjengelig() const;
    void oek_antall_tilgjengelig();
    void reduser_antall_tilgjengelig();
    std::string get_navn() const;   // drawn below the class box; the verifier does not read it (DEV-05)

    void handle(Event event);
    State get_state() const;   // not drawn; lets tests, firmware and dashboard see the state (DEV-15)
    int get_antall_tilgjengelig() const;   // not drawn; the dashboard shows "x av y ledig" (DEV-17)
    int get_antall_totalt() const;         // not drawn (DEV-17)

private:
    std::string utstyr_navn_;
    int antall_totalt_;
    int antall_tilgjengelig_;

    State state_ = State::LEDIG;   // the initial state

    // The transition table: one row per arrow in the state diagram.
    struct Transition
    {
        State from;
        Event event;
        State to;
        bool (Lab_Utstyr::*guard)() const;   // nullptr: no guard
        void (Lab_Utstyr::*action)();        // nullptr: no action
    };
    static constexpr Transition transitions[] = {
        {State::LEDIG,  Event::regUtlaan, State::UTLÅNT, nullptr, nullptr},
        {State::UTLÅNT, Event::regRetur,  State::LEDIG,  nullptr, nullptr},
    };
};

}  // namespace labutlaan
