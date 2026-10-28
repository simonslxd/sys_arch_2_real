#include "input_decoder.hpp"

namespace station {

std::optional<Command> InputDecoder::update(const InputSample& sample)
{
    // A button press counts when the button is released.
    const bool a_released = a_was_pressed_ && !sample.button_a_pressed;
    const bool b_released = b_was_pressed_ && !sample.button_b_pressed;
    a_was_pressed_ = sample.button_a_pressed;
    b_was_pressed_ = sample.button_b_pressed;
    if (b_released)
    {
        return Command::Borrow;
    }
    if (a_released)
    {
        return Command::Return;
    }

    // A tilt counts once; then the board must be flat again before the next one.
    if (tilted_)
    {
        if (sample.tilt_x_mg > -TILT_OFF_MG && sample.tilt_x_mg < TILT_OFF_MG)
        {
            tilted_ = false;
        }
        return std::nullopt;
    }
    if (sample.tilt_x_mg < -TILT_ON_MG)
    {
        tilted_ = true;
        return Command::NextStudent;
    }
    if (sample.tilt_x_mg > TILT_ON_MG)
    {
        tilted_ = true;
        return Command::NextItem;
    }
    return std::nullopt;
}

}  // namespace station
