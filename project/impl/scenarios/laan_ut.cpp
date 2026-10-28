// The scenario of diagrams/input/Sekvens.drawio (verified as sequence-laan_ut.drawio).
//
// main() sets the objects up; scenario() makes the call the diagram's actor makes, and only
// the calls made from there on are recorded. The verifier runs this program in a traced build.
#include <memory>

#include "labutlaan/lab.hpp"

namespace {

void scenario(labutlaan::Lab& lab, labutlaan::Student& student, labutlaan::Lab_Utstyr& utstyr)
{
    lab.laan_ut(student, utstyr);   // Lab Ansatt -> Lab : Laan(Objekt, Student)
}

}  // namespace

int main()
{
    labutlaan::Lab lab;
    auto utstyr = std::make_shared<labutlaan::Lab_Utstyr>("Multimeter", 1);
    auto student = std::make_shared<labutlaan::Student>("S001", "Eksempel Student");
    lab.legg_til_utstyr(utstyr);
    lab.registrer_student(student);
    scenario(lab, *student, *utstyr);
    return 0;
}
