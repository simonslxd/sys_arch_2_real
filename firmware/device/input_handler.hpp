#pragma once

#include "MicroBit.h"
#include "input_decoder.hpp"
#include "serial_command_reader.hpp"

// Reads the buttons, the accelerometer and the serial port, and waits for the next Command
// (REQ-HW-01 to REQ-HW-03, REQ-COM-03). InputDecoder decides what a button or tilt reading means,
// SerialCommandReader what the dashboard sent.
//
// It polls every 20 ms instead of listening on CODAL's message bus: on this board, events
// queued on the bus did not reach our handlers, while polling saw every press (decision D-11).
// Inputs given while the station is still showing a result are not read (assumption A-12);
// characters from the dashboard wait in CODAL's receive buffer until the next poll.
class InputHandler
{
public:
    explicit InputHandler(MicroBit& uBit);

    station::Command wait_for_command();

private:
    MicroBit& uBit_;
    station::InputDecoder decoder_;
    station::SerialCommandReader serial_reader_;
};
