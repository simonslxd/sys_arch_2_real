#pragma once

#include <memory>
#include <vector>

#include "labutlaan/lab_utstyr.hpp"
#include "labutlaan/student.hpp"
#include "labutlaan/utlaan.hpp"

namespace labutlaan {

class Lab
{
public:
    void legg_til_utstyr(std::shared_ptr<Lab_Utstyr> utstyr);
    void registrer_student(std::shared_ptr<Student> student);
    Utlaan* laan_ut(Student& student, Lab_Utstyr& utstyr);   // nullptr when the item can't be lent
    void registrer_retur(Utlaan& utlaan);
    bool finn_tilgjengelig_utstyr(const Lab_Utstyr& utstyr) const;

private:
    std::vector<std::shared_ptr<Lab_Utstyr>> utstyrsliste_;   // aggregation "0..*"
    std::vector<std::shared_ptr<Student>> student_liste_;     // aggregation "0..*"
    std::vector<std::unique_ptr<Utlaan>> utlaansliste_;       // composition "0..*": the lab owns its loans
};

}  // namespace labutlaan
