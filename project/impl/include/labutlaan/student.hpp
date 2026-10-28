#pragma once

#include <string>
#include <vector>

namespace labutlaan {

class Utlaan;

class Student
{
public:
    Student(std::string studentnummer, std::string navn);

    void legg_til_laan(Utlaan& utlaan);
    void fjern_laan(Utlaan& utlaan);
    const std::vector<Utlaan*>& get_laan() const;

private:
    std::string studentnummer_;
    std::string navn_;
    std::vector<Utlaan*> mine_laan_;   // raw pointers, many -> association "0..*": the Lab owns the loans
};

}  // namespace labutlaan
