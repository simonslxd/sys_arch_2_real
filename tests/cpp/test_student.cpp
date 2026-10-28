#include "check.hpp"
#include "labutlaan/lab_utstyr.hpp"
#include "labutlaan/student.hpp"
#include "labutlaan/utlaan.hpp"

using labutlaan::Lab_Utstyr;
using labutlaan::Student;
using labutlaan::Utlaan;

std::vector<Test> student_tests()
{
    return {
        {"Student: new student has no loans",
         [] {
             Student student("S001", "Eksempel Student");
             CHECK(student.get_laan().empty());
         }},
        {"Student: add and remove a loan",
         [] {
             Student student("S001", "Eksempel Student");
             Lab_Utstyr item("Multimeter", 1);
             Utlaan utlaan(student, item, 1);
             student.legg_til_laan(utlaan);
             CHECK(student.get_laan().size() == 1);
             CHECK(student.get_laan()[0] == &utlaan);
             student.fjern_laan(utlaan);
             CHECK(student.get_laan().empty());
         }},
    };
}
