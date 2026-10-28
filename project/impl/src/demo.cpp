#include <iostream>
#include <memory>

#include "labutlaan/lab.hpp"

int main()
{
    labutlaan::Lab lab;
    auto multimeter = std::make_shared<labutlaan::Lab_Utstyr>("Multimeter", 1);
    auto student = std::make_shared<labutlaan::Student>("S001", "Eksempel Student");
    lab.legg_til_utstyr(multimeter);
    lab.registrer_student(student);

    labutlaan::Utlaan* utlaan = lab.laan_ut(*student, *multimeter);
    std::cout << "laan_ut: " << (utlaan ? "OK" : "FEIL") << ", "
              << multimeter->get_navn() << " tilgjengelig: " << multimeter->er_tilgjengelig() << "\n";

    labutlaan::Utlaan* andre = lab.laan_ut(*student, *multimeter);
    std::cout << "laan_ut igjen: " << (andre ? "OK" : "FEIL") << "\n";

    lab.registrer_retur(*utlaan);
    std::cout << "registrer_retur: " << multimeter->get_navn()
              << " tilgjengelig: " << multimeter->er_tilgjengelig() << "\n";
    return 0;
}
