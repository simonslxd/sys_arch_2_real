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
        const station::InputSample sample{uBit_.buttonA.isPressed(), uBit_.buttonB.isPressed(),
                                          uBit_.accelerometer.getX()};
        if (std::optional<station::Command> command = decoder_.update(sample))
        {
            return *command;
        }
    }
}
