#pragma once

#include <optional>

#include "station_controller.hpp"

namespace station {

// One reading of the inputs, taken by the device every few milliseconds.
struct InputSample
{
    bool button_a_pressed;
    bool button_b_pressed;
    int tilt_x_mg;   // accelerometer x axis in milli-g: negative = tilted left, positive = tilted right
};

// Turns a stream of input readings into Commands (REQ-HW-01 to REQ-HW-03). No hardware here,
// so it is unit-tested on the PC.
//
//   button B released after a press -> Borrow        button A released after a press -> Return
//   tilted left  (x < -500 mg)      -> NextStudent   tilted right (x > 500 mg)       -> NextItem
//
// A tilt counts once: the board must come back to flat (|x| < 300 mg) before the next tilt
// counts. The board must also be flat once after start-up (assumption A-13).
class InputDecoder
{
public:
    std::optional<Command> update(const InputSample& sample);

    static constexpr int TILT_ON_MG = 500;    // about 30 degrees
    static constexpr int TILT_OFF_MG = 300;   // about 17 degrees

private:
    bool a_was_pressed_ = false;
    bool b_was_pressed_ = false;
    bool tilted_ = true;   // true until the board has been flat once
};

}  // namespace station
