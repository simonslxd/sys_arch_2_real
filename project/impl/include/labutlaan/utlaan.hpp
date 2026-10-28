#pragma once

namespace labutlaan {

class Lab_Utstyr;
class Student;

class Utlaan
{
public:
    Utlaan(Student& laanetaker, Lab_Utstyr& utlaant_utstyr, int antall);

    Student& get_laanetaker() const;
    Lab_Utstyr& get_utstyr() const;
    int get_antall() const;

private:
    Student* laanetaker_;          // raw pointers -> associations: a loan refers to its borrower
    Lab_Utstyr* utlaant_utstyr_;   // and to the borrowed item, it owns neither
    int antall_;
};

}  // namespace labutlaan
