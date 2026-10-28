#include "check.hpp"
#include "labutlaan/lab_utstyr.hpp"

using labutlaan::Lab_Utstyr;

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
    };
}
