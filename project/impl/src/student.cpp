#include "labutlaan/student.hpp"

#include <utility>

namespace labutlaan {

Student::Student(std::string studentnummer, std::string navn)
    : studentnummer_(std::move(studentnummer)), navn_(std::move(navn))
{
}

void Student::legg_til_laan(Utlaan& utlaan) { mine_laan_.push_back(&utlaan); }

void Student::fjern_laan(Utlaan& utlaan) { std::erase(mine_laan_, &utlaan); }

const std::vector<Utlaan*>& Student::get_laan() const { return mine_laan_; }

}  // namespace labutlaan
