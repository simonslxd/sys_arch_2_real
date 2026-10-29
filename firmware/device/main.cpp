// The micro:bit lab-lending station.
//
// This file wires the modules together, in one loop, and picks the feedback for each Command
// (show_feedback):
//   InputHandler       waits for a button press, a tilt or a status request -> Command
//   StationController  runs the Command on the domain model (project/impl) -> StationEvents
//   event_writer       StationEvent -> one line on the USB serial port (spec/serial-protocol.md)
//   FeedbackOutput     ✓ / ✗ on the LEDs and a high / low beep, or the new selection
#include "MicroBit.h"
#include "demo_data.hpp"
#include "event_writer.hpp"
#include "feedback_output.hpp"
#include "input_handler.hpp"
#include "station_controller.hpp"

MicroBit uBit;

namespace {

void send(const station::StationEvent& event)
{
    uBit.serial.send(ManagedString(station::format_event(event).c_str()));
}

void show_feedback(FeedbackOutput& feedback, const station::StationController& controller,
                   station::Command command, const station::StationEvent& event)
{
    if (command == station::Command::Status)
    {
        return;   // a request from the dashboard: nothing to show on the board
    }
    if (command == station::Command::NextStudent)
    {
        feedback.show_selection(ManagedString("S") + ManagedString(int(controller.student_number())));
    }
    else if (command == station::Command::NextItem)
    {
        feedback.show_selection(ManagedString("U") + ManagedString(int(controller.utstyr_number())));
    }
    else
    {
        feedback.show_result(event.ok);
    }
}

}  // namespace

int main()
{
    uBit.init();

    station::StationController controller(station::demo_students(), station::demo_utstyr());
    FeedbackOutput feedback(uBit);
    InputHandler input(uBit);

    send(controller.start());
    while (true)
    {
        const station::Command command = input.wait_for_command();
        const std::vector<station::StationEvent> events = controller.execute(command);
        for (const station::StationEvent& event : events)
        {
            send(event);
        }
        show_feedback(feedback, controller, command, events.front());
    }
}
