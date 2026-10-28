#include <memory>

#include "check.hpp"
#include "labutlaan/lab.hpp"

using labutlaan::Lab;
using labutlaan::Lab_Utstyr;
using labutlaan::Student;
using labutlaan::Utlaan;

namespace {

// A lab with one student and one multimeter (1 unit), as most tests need.
struct LabWithOneItem
{
    Lab lab;
    std::shared_ptr<Lab_Utstyr> multimeter = std::make_shared<Lab_Utstyr>("Multimeter", 1);
    std::shared_ptr<Student> student = std::make_shared<Student>("S001", "Eksempel Student");

    LabWithOneItem()
    {
        lab.legg_til_utstyr(multimeter);
        lab.registrer_student(student);
    }
};

}  // namespace

std::vector<Test> lab_tests()
{
    return {
        {"Lab: an item that is not in the lab is not found as available (A-08)",
         [] {
             Lab lab;
             Lab_Utstyr other("Oscilloskop", 1);
             CHECK(!lab.finn_tilgjengelig_utstyr(other));
         }},
        {"Lab: a registered item with units is found as available",
         [] {
             LabWithOneItem setup;
             CHECK(setup.lab.finn_tilgjengelig_utstyr(*setup.multimeter));
         }},
        {"Lab: laan_ut creates a loan of one unit and links it to the student",
         [] {
             LabWithOneItem setup;
             Utlaan* utlaan = setup.lab.laan_ut(*setup.student, *setup.multimeter);
             CHECK(utlaan != nullptr);
             CHECK(&utlaan->get_laanetaker() == setup.student.get());
             CHECK(&utlaan->get_utstyr() == setup.multimeter.get());
             CHECK(utlaan->get_antall() == 1);
             CHECK(setup.student->get_laan().size() == 1);
             CHECK(!setup.multimeter->er_tilgjengelig());
         }},
        {"Lab: laan_ut of an unavailable item returns nullptr and changes nothing",
         [] {
             LabWithOneItem setup;
             setup.lab.laan_ut(*setup.student, *setup.multimeter);
             Utlaan* second = setup.lab.laan_ut(*setup.student, *setup.multimeter);
             CHECK(second == nullptr);
             CHECK(setup.student->get_laan().size() == 1);
         }},
        {"Lab: registrer_retur makes the item available and removes the loan from the student",
         [] {
             LabWithOneItem setup;
             Utlaan* utlaan = setup.lab.laan_ut(*setup.student, *setup.multimeter);
             setup.lab.registrer_retur(*utlaan);
             CHECK(setup.multimeter->er_tilgjengelig());
             CHECK(setup.student->get_laan().empty());
         }},
        {"Lab: registrer_retur of a loan that belongs to another lab changes nothing",
         [] {
             LabWithOneItem setup;
             Utlaan* utlaan = setup.lab.laan_ut(*setup.student, *setup.multimeter);
             Lab other_lab;
             other_lab.registrer_retur(*utlaan);
             CHECK(!setup.multimeter->er_tilgjengelig());
             CHECK(setup.student->get_laan().size() == 1);
         }},
    };
}
