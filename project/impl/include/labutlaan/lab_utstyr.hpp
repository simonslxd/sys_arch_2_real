#pragma once

#include <string>

namespace labutlaan {

class Lab_Utstyr
{
public:
    Lab_Utstyr(std::string utstyr_navn, int antall_totalt);

    bool er_tilgjengelig() const;
    void oek_antall_tilgjengelig();
    void reduser_antall_tilgjengelig();
    std::string get_navn() const;   // drawn below the class box; the verifier does not read it (DEV-05)

private:
    std::string utstyr_navn_;
    int antall_totalt_;
    int antall_tilgjengelig_;
};

}  // namespace labutlaan
