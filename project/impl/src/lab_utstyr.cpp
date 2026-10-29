#include "labutlaan/lab_utstyr.hpp"

#include <utility>

namespace labutlaan {

Lab_Utstyr::Lab_Utstyr(std::string utstyr_navn, int antall_totalt)
    : utstyr_navn_(std::move(utstyr_navn)), antall_totalt_(antall_totalt),
      antall_tilgjengelig_(antall_totalt)
{
    // Nothing to lend from the start: the state is UTLÅNT, as for a fully lent item (A-05).
    if (antall_tilgjengelig_ <= 0)
    {
        handle(Event::regUtlaan);
    }
}

bool Lab_Utstyr::er_tilgjengelig() const { return antall_tilgjengelig_ > 0; }

void Lab_Utstyr::oek_antall_tilgjengelig()
{
    // Never more available than the lab owns (assumption A-09).
    if (antall_tilgjengelig_ < antall_totalt_)
    {
        ++antall_tilgjengelig_;
        // The first unit back makes the item LEDIG again (A-05).
        if (antall_tilgjengelig_ == 1)
        {
            handle(Event::regRetur);
        }
    }
}

void Lab_Utstyr::reduser_antall_tilgjengelig()
{
    // Never fewer than zero available (assumption A-09).
    if (antall_tilgjengelig_ > 0)
    {
        --antall_tilgjengelig_;
        // Lending the last unit makes the item UTLÅNT (A-05).
        if (antall_tilgjengelig_ == 0)
        {
            handle(Event::regUtlaan);
        }
    }
}

std::string Lab_Utstyr::get_navn() const { return utstyr_navn_; }

void Lab_Utstyr::handle(Event event)
{
    for (const Transition& t : transitions)
    {
        if (t.from != state_ || t.event != event)
        {
            continue;
        }
        if (t.guard != nullptr && !(this->*t.guard)())
        {
            continue;
        }
        if (t.action != nullptr)
        {
            (this->*t.action)();
        }
        state_ = t.to;
        return;
    }
    // No transition for this event in this state: the event is ignored.
}

Lab_Utstyr::State Lab_Utstyr::get_state() const { return state_; }

int Lab_Utstyr::get_antall_tilgjengelig() const { return antall_tilgjengelig_; }

int Lab_Utstyr::get_antall_totalt() const { return antall_totalt_; }

}  // namespace labutlaan
