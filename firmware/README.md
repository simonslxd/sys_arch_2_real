# micro:bit firmware: the lab-lending station

The micro:bit V2 runs the domain model from `project/impl/` (the same code the verifier checks), driven by its
buttons and accelerometer, and reports every event over USB serial (`spec/serial-protocol.md`).

| Input | Command | Domain call | Feedback |
|---|---|---|---|
| button **B** (right) | Borrow | `Lab::laan_ut` | ✓ + high beep, or ✗ + low beep |
| button **A** (left) | Return | `Lab::registrer_retur` | ✓ + high beep, or ✗ + low beep |
| tilt **left** | NextStudent | — | scrolls `S1`, `S2`, `S3` |
| tilt **right** | NextItem | — | scrolls `U1`, `U2`, `U3` |
| the line `STATUS` from the dashboard | Status | `Lab_Utstyr::get_antall_tilgjengelig`, `get_antall_totalt` | none; sends one `STATUS` line per item |

"Left" is the side labelled **A**: hold the board with the USB plug pointing away from you.

## Build and flash (in WSL, from the repository root)

```bash
bash firmware/build.sh            # writes build/MICROBIT.hex
```

Then copy `build/MICROBIT.hex` onto the **MICROBIT** drive (in Windows Explorer, drag it there). The yellow light
on the back blinks while it flashes. If Windows says the drive "is corrupted and cannot be read", unplug the
micro:bit, wait 5 seconds, plug it in again and copy once more.

The first build downloads CODAL, the official micro:bit C++ runtime (pinned: `microbit-v2-samples` v0.3.0 in
`build.sh`, `codal-microbit-v2` v0.3.5 in `codal.json`) into `build/codal/` and takes a few minutes. To start
again from scratch, delete `build/codal/`.

## Code layout

| Folder | What | Tested on the PC |
|---|---|---|
| `station/` | Hardware-free logic: `StationController` (selection + domain calls), `InputDecoder` (readings → Commands), `SerialCommandReader` (characters from the dashboard → Commands), `event_writer` (serial line), `demo_data.hpp` | yes: `tests/cpp/` |
| `device/` | Touches the hardware: `InputHandler` (reads buttons, accelerometer and serial input every 20 ms), `FeedbackOutput` (LEDs + speaker), `main.cpp` (wiring) | no: `tests/hardware/` |
| `codal.json`, `cxx20.cmake`, `build.sh` | CODAL version pin; compile our sources as C++20; the build | — |

## Hardware check

`tests/hardware/checklist.md` describes the input sequence and the recorded results.
`tests/hardware/replay_log.cpp` replays a recorded serial log through the station code and checks that the board
produced exactly the lines the PC-tested code produces for the same inputs.
