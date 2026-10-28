#pragma once

#include "MicroBit.h"

// Shows the outcome of an input on the LED matrix and the speaker (REQ-HW-04, REQ-HW-05).
class FeedbackOutput
{
public:
    explicit FeedbackOutput(MicroBit& uBit);

    void show_result(bool ok);                     // ✓ + high beep, or ✗ + low beep
    void show_selection(const ManagedString& label);   // scrolls e.g. "S2" (assumption A-02)

private:
    void beep(int frequency_hz, int duration_ms);

    MicroBit& uBit_;
};
