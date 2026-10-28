#include "input_handler.hpp"

namespace {

const int POLL_INTERVAL_MS = 20;

}  // namespace

InputHandler::InputHandler(MicroBit& uBit) : uBit_(uBit) {}

station::Command InputHandler::wait_for_command()
{
    while (true)
    {
        uBit_.sleep(POLL_INTERVAL_MS);
        // isPressed() returns an int: 1 while the button is held down.
        const station::InputSample sample{uBit_.buttonA.isPressed() != 0, uBit_.buttonB.isPressed() != 0,
                                          uBit_.accelerometer.getX()};
        if (std::optional<station::Command> command = decoder_.update(sample))
        {
            return *command;
        }
    }
}
