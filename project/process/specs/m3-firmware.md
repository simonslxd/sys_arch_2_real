# M3 spec: micro:bit firmware

Sources: the approved architecture (module table), [requirements.md](requirements.md) (REQ-HW-01 to 05,
REQ-COM-01, A-01 to A-03, A-06, A-12, A-13) and the serial protocol model
[spec/serial-protocol.md](../../../spec/serial-protocol.md). The domain model is used unchanged from
`project/impl/`.

## Modules (one responsibility each)

| Module | Folder | Responsibility | Tested by |
|---|---|---|---|
| `StationController` | `firmware/station/` | Holds the `Lab`, the demo data and the current selection; turns a `Command` into domain calls and a `StationEvent` | unit tests (6) + golden lines + replay |
| `InputDecoder` | `firmware/station/` | Turns raw readings (A pressed, B pressed, x in mg) into `Command`s | unit tests (6) |
| `event_writer` | `firmware/station/` | `StationEvent` → one protocol line | unit test + golden lines |
| `demo_data.hpp` | `firmware/station/` | The fixed demo data (A-01) | golden lines |
| `InputHandler` | `firmware/device/` | Reads buttons and accelerometer every 20 ms, feeds `InputDecoder` | hardware check |
| `FeedbackOutput` | `firmware/device/` | ✓ / ✗ image + high / low beep; scrolls the new selection | hardware check |
| `main.cpp` | `firmware/device/` | Wiring: wait for a Command → execute → send line → show feedback | hardware check |

Only `firmware/device/` includes CODAL (`MicroBit.h`). Everything in `firmware/station/` is plain C++20 and is
built into the PC tests from the very same files.

## Build

`firmware/build.sh` downloads CODAL once into `build/codal/` (samples v0.3.0, `codal-microbit-v2` v0.3.5,
both pinned), copies `firmware/device/`, `firmware/station/` and `project/impl/` (without `demo.cpp`) into
`build/codal/source/`, and builds `build/MICROBIT.hex`. CODAL compiles as C++11; `firmware/cxx20.cmake` compiles
our files as C++20, like the verified PC build (D-10). The build has no warnings since M6 (see the correction
under Results).

## Input handling: why polling (D-11)

The first firmware listened for button and tilt events on CODAL's message bus. On the board, nothing reacted. Three
diagnostic builds (in `build/codal/source/` only, never committed) showed:

1. The program ran: start-up finished, the listeners were registered, and a heartbeat kept coming. But no handler
   was ever called.
2. Polling `isPressed()` in the main loop saw every press, while the button listeners received no events at all.
   After the tilt listeners were added (which starts the accelerometer), some events started arriving, a few
   seconds late.

CODAL queues events and runs non-urgent handlers only from its idle task; in this setup that queue was not
processed reliably. The root cause inside CODAL was not identified. Polling every 20 ms works on every press,
needs no event queue, no fibers and no "busy" flag, and lets the decision logic (`InputDecoder`) be unit-tested.

## Results

- 34 of 34 unit tests pass (domain + station), including the 13 golden protocol lines.
- Hardware: 2 recorded runs, each 13 lines; `replay_log` reproduced **13 of 13** lines of both runs
  ([tests/hardware/checklist.md](../../../tests/hardware/checklist.md)).
- Firmware build: 113 KB of 364 KB flash. **Correction (M6):** this spec first said "0 warnings". The polling
  `InputHandler` actually produced 2 `-Wnarrowing` warnings (`isPressed()` returns `int`, put into a `bool` in
  braces). The warning count in M3 was taken before the build had finished, so it read an old log. Fixed in M6
  (`isPressed() != 0`, same behaviour), and the build now has 0 warnings. The firmware tested on the hardware in
  M3 was the build with these 2 warnings.
