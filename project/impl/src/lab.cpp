#include "labutlaan/lab.hpp"

#include <algorithm>
#include <utility>

namespace labutlaan {

void Lab::legg_til_utstyr(std::shared_ptr<Lab_Utstyr> utstyr)
{
    utstyrsliste_.push_back(std::move(utstyr));
}

void Lab::registrer_student(std::shared_ptr<Student> student)
{
    student_liste_.push_back(std::move(student));
}

Utlaan* Lab::laan_ut(Student& student, Lab_Utstyr& utstyr)
{
    // Sequence diagram: find the item and check it is available (finnObjekt, erTilgjengelig) ...
    if (!finn_tilgjengelig_utstyr(utstyr))
    {
        return nullptr;
    }
    // ... then find the student among the lab's students (finnStudent, assumption A-11) ...
    auto is_this_student = [&student](const std::shared_ptr<Student>& s) { return s.get() == &student; };
    if (std::none_of(student_liste_.begin(), student_liste_.end(), is_this_student))
    {
        return nullptr;
    }
    // ... then put the loan on the student (settStudentPaaLaan).
    utstyr.reduser_antall_tilgjengelig();
    utlaansliste_.push_back(std::make_unique<Utlaan>(student, utstyr, 1));   // one unit per loan (A-04)
    Utlaan& utlaan = *utlaansliste_.back();
    student.legg_til_laan(utlaan);
    return &utlaan;
}

void Lab::registrer_retur(Utlaan& utlaan)
{
    auto is_this_loan = [&utlaan](const std::unique_ptr<Utlaan>& loan) { return loan.get() == &utlaan; };
    auto loan = std::find_if(utlaansliste_.begin(), utlaansliste_.end(), is_this_loan);
    if (loan == utlaansliste_.end())
    {
        return;   // not a loan of this lab: nothing to return
    }
    utlaan.get_utstyr().oek_antall_tilgjengelig();
    utlaan.get_laanetaker().fjern_laan(utlaan);
    utlaansliste_.erase(loan);   // destroys the loan: `utlaan` must not be used after this
}

bool Lab::finn_tilgjengelig_utstyr(const Lab_Utstyr& utstyr) const
{
    // Available here means: registered in this lab (assumption A-08) and at least one unit left.
    for (const auto& item : utstyrsliste_)
    {
        if (item.get() == &utstyr)
        {
            return utstyr.er_tilgjengelig();
        }
    }
    return false;
}

}  // namespace labutlaan
