#include "check.hpp"
#include "labutlaan/lab_utstyr.hpp"
#include "labutlaan/student.hpp"
#include "labutlaan/utlaan.hpp"

using labutlaan::Lab_Utstyr;
using labutlaan::Student;
using labutlaan::Utlaan;

std::vector<Test> utlaan_tests()
{
    return {
        {"Utlaan: getters return the borrower, the item and the count",
         [] {
             Student student("S001", "Eksempel Student");
             Lab_Utstyr item("Multimeter", 1);
             Utlaan utlaan(student, item, 1);
             CHECK(&utlaan.get_laanetaker() == &student);
             CHECK(&utlaan.get_utstyr() == &item);
             CHECK(utlaan.get_antall() == 1);
         }},
    };
}
