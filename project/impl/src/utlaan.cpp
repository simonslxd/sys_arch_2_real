#include "labutlaan/utlaan.hpp"

namespace labutlaan {

Utlaan::Utlaan(Student& laanetaker, Lab_Utstyr& utlaant_utstyr, int antall)
    : laanetaker_(&laanetaker), utlaant_utstyr_(&utlaant_utstyr), antall_(antall)
{
}

Student& Utlaan::get_laanetaker() const { return *laanetaker_; }

Lab_Utstyr& Utlaan::get_utstyr() const { return *utlaant_utstyr_; }

int Utlaan::get_antall() const { return antall_; }

}  // namespace labutlaan
