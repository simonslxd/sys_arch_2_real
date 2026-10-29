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

        // Characters from the dashboard: read(ASYNC) returns at once, negative when none are waiting.
        for (int received = uBit_.serial.read(ASYNC); received >= 0; received = uBit_.serial.read(ASYNC))
        {
            if (std::optional<station::Command> command = serial_reader_.push(static_cast<char>(received)))
            {
                return *command;
            }
        }

        // isPressed() returns an int: 1 while the button is held down.
        const station::InputSample sample{uBit_.buttonA.isPressed() != 0, uBit_.buttonB.isPressed() != 0,
                                          uBit_.accelerometer.getX()};
        if (std::optional<station::Command> command = decoder_.update(sample))
        {
            return *command;
        }
    }
}
