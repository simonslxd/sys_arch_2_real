#include "feedback_output.hpp"

namespace {

// 5 x 5 LED images, one row per line: 255 = LED on, 0 = off.
const char* const CHECKMARK = "0,0,0,0,0\n"
                              "0,0,0,0,255\n"
                              "0,0,0,255,0\n"
                              "255,0,255,0,0\n"
                              "0,255,0,0,0\n";
const char* const CROSS = "255,0,0,0,255\n"
                          "0,255,0,255,0\n"
                          "0,0,255,0,0\n"
                          "0,255,0,255,0\n"
                          "255,0,0,0,255\n";

// The two beeps (assumption A-06).
const int HIGH_BEEP_HZ = 1760;
const int HIGH_BEEP_MS = 150;
const int LOW_BEEP_HZ = 220;
const int LOW_BEEP_MS = 300;

}  // namespace

FeedbackOutput::FeedbackOutput(MicroBit& uBit) : uBit_(uBit) {}

void FeedbackOutput::show_result(bool ok)
{
    if (ok)
    {
        uBit_.display.print(MicroBitImage(CHECKMARK));
        beep(HIGH_BEEP_HZ, HIGH_BEEP_MS);
    }
    else
    {
        uBit_.display.print(MicroBitImage(CROSS));
        beep(LOW_BEEP_HZ, LOW_BEEP_MS);
    }
}

void FeedbackOutput::show_selection(const ManagedString& label) { uBit_.display.scroll(label); }

void FeedbackOutput::beep(int frequency_hz, int duration_ms)
{
    // The built-in speaker, driven as in CODAL's AudioTest sample: value first, then period.
    uBit_.audio.virtualOutputPin.setAnalogValue(512);
    uBit_.audio.virtualOutputPin.setAnalogPeriodUs(1000000 / frequency_hz);
    uBit_.sleep(duration_ms);
    uBit_.audio.virtualOutputPin.setAnalogValue(0);
}
