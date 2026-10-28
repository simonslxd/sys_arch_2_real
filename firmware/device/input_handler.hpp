#pragma once

#include "MicroBit.h"
#include "input_decoder.hpp"

// Reads the buttons and the accelerometer and waits for the user's next Command
// (REQ-HW-01 to REQ-HW-03); InputDecoder decides what a reading means.
//
// It polls every 20 ms instead of listening on CODAL's message bus: on this board, events
// queued on the bus did not reach our handlers, while polling saw every press (decision D-11).
// Inputs given while the station is still showing a result are not read (assumption A-12).
class InputHandler
{
public:
    explicit InputHandler(MicroBit& uBit);

    station::Command wait_for_command();

private:
    MicroBit& uBit_;
    station::InputDecoder decoder_;
};
