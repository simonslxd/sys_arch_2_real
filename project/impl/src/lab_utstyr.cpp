#include "labutlaan/lab_utstyr.hpp"

#include <utility>

namespace labutlaan {

Lab_Utstyr::Lab_Utstyr(std::string utstyr_navn, int antall_totalt)
    : utstyr_navn_(std::move(utstyr_navn)), antall_totalt_(antall_totalt),
      antall_tilgjengelig_(antall_totalt)
{
}

bool Lab_Utstyr::er_tilgjengelig() const { return antall_tilgjengelig_ > 0; }

void Lab_Utstyr::oek_antall_tilgjengelig()
{
    // Never more available than the lab owns (assumption A-09).
    if (antall_tilgjengelig_ < antall_totalt_)
    {
        ++antall_tilgjengelig_;
    }
}

void Lab_Utstyr::reduser_antall_tilgjengelig()
{
    // Never fewer than zero available (assumption A-09).
    if (antall_tilgjengelig_ > 0)
    {
        --antall_tilgjengelig_;
    }
}

std::string Lab_Utstyr::get_navn() const { return utstyr_navn_; }

}  // namespace labutlaan
