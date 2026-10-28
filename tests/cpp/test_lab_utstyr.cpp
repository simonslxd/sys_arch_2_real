#include "check.hpp"
#include "labutlaan/lab_utstyr.hpp"

using labutlaan::Lab_Utstyr;
using Event = Lab_Utstyr::Event;
using State = Lab_Utstyr::State;

std::vector<Test> lab_utstyr_tests()
{
    return {
        {"Lab_Utstyr: new item with units is available and keeps its name",
         [] {
             Lab_Utstyr item("Multimeter", 2);
             CHECK(item.er_tilgjengelig());
             CHECK(item.get_navn() == "Multimeter");
         }},
        {"Lab_Utstyr: item with zero units is not available",
         [] {
             Lab_Utstyr item("Oscilloskop", 0);
             CHECK(!item.er_tilgjengelig());
         }},
        {"Lab_Utstyr: reduce to zero makes it unavailable, increase makes it available again",
         [] {
             Lab_Utstyr item("Multimeter", 1);
             item.reduser_antall_tilgjengelig();
             CHECK(!item.er_tilgjengelig());
             item.oek_antall_tilgjengelig();
             CHECK(item.er_tilgjengelig());
         }},
        {"Lab_Utstyr: reduce never goes below zero (A-09)",
         [] {
             Lab_Utstyr item("Multimeter", 1);
             item.reduser_antall_tilgjengelig();
             item.reduser_antall_tilgjengelig();   // already 0: must stay 0, not -1
             item.oek_antall_tilgjengelig();
             CHECK(item.er_tilgjengelig());
         }},
        {"Lab_Utstyr: increase never goes above the total (A-09)",
         [] {
             Lab_Utstyr item("Multimeter", 1);
             item.oek_antall_tilgjengelig();   // already 1 of 1: must stay 1, not 2
             item.reduser_antall_tilgjengelig();
             CHECK(!item.er_tilgjengelig());
         }},

        // The state machine: the table itself, driven by handle().
        {"State machine: starts in LEDIG",
         [] {
             Lab_Utstyr item("Multimeter", 1);
             CHECK(item.get_state() == State::LEDIG);
         }},
        {"State machine: regUtlaan leads LEDIG -> UTLÅNT, regRetur leads UTLÅNT -> LEDIG",
         [] {
             Lab_Utstyr item("Multimeter", 1);
             item.handle(Event::regUtlaan);
             CHECK(item.get_state() == State::UTLÅNT);
             item.handle(Event::regRetur);
             CHECK(item.get_state() == State::LEDIG);
         }},
        {"State machine: an event with no arrow from the current state is ignored",
         [] {
             Lab_Utstyr item("Multimeter", 1);
             item.handle(Event::regRetur);   // no arrow LEDIG --regRetur-->
             CHECK(item.get_state() == State::LEDIG);
             item.handle(Event::regUtlaan);
             item.handle(Event::regUtlaan);   // no arrow UTLÅNT --regUtlaan-->
             CHECK(item.get_state() == State::UTLÅNT);
         }},

        // When the events fire: the state follows the available count (A-05).
        {"State follows count: lending the last unit gives UTLÅNT, the first back gives LEDIG",
         [] {
             Lab_Utstyr item("Multimeter", 2);
             item.reduser_antall_tilgjengelig();   // 1 of 2 left
             CHECK(item.get_state() == State::LEDIG);
             item.reduser_antall_tilgjengelig();   // 0 of 2 left
             CHECK(item.get_state() == State::UTLÅNT);
             item.oek_antall_tilgjengelig();       // 1 of 2 left
             CHECK(item.get_state() == State::LEDIG);
             item.oek_antall_tilgjengelig();       // 2 of 2 left
             CHECK(item.get_state() == State::LEDIG);
         }},
        {"State follows count: an item with zero units starts in UTLÅNT",
         [] {
             Lab_Utstyr item("Oscilloskop", 0);
             CHECK(item.get_state() == State::UTLÅNT);
         }},
    };
}
