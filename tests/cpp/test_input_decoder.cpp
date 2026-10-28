#include <optional>

#include "check.hpp"
#include "input_decoder.hpp"

using station::Command;
using station::InputDecoder;
using station::InputSample;

namespace {

const InputSample FLAT{false, false, 0};
const InputSample B_DOWN{false, true, 0};
const InputSample A_DOWN{true, false, 0};
const InputSample TILTED_LEFT{false, false, -700};
const InputSample TILTED_RIGHT{false, false, 700};

// A decoder that has seen the board flat once, as after start-up.
InputDecoder ready_decoder()
{
    InputDecoder decoder;
    decoder.update(FLAT);
    return decoder;
}

}  // namespace

std::vector<Test> input_decoder_tests()
{
    return {
        {"Input: button B gives Borrow when released, not while held (REQ-HW-01)",
         [] {
             InputDecoder decoder = ready_decoder();
             CHECK(!decoder.update(B_DOWN).has_value());
             CHECK(!decoder.update(B_DOWN).has_value());
             CHECK(decoder.update(FLAT) == Command::Borrow);
             CHECK(!decoder.update(FLAT).has_value());
         }},
        {"Input: button A gives Return when released (REQ-HW-02)",
         [] {
             InputDecoder decoder = ready_decoder();
             decoder.update(A_DOWN);
             CHECK(decoder.update(FLAT) == Command::Return);
         }},
        {"Input: tilt left gives NextStudent, tilt right gives NextItem (REQ-HW-03)",
         [] {
             InputDecoder decoder = ready_decoder();
             CHECK(decoder.update(TILTED_LEFT) == Command::NextStudent);
             decoder.update(FLAT);
             CHECK(decoder.update(TILTED_RIGHT) == Command::NextItem);
         }},
        {"Input: a tilt counts once, until the board is flat again (A-13)",
         [] {
             InputDecoder decoder = ready_decoder();
             CHECK(decoder.update(TILTED_LEFT) == Command::NextStudent);
             CHECK(!decoder.update(TILTED_LEFT).has_value());
             CHECK(!decoder.update({false, false, -400}).has_value());   // between 300 and 500 mg: not flat yet
             CHECK(!decoder.update(TILTED_LEFT).has_value());
             decoder.update(FLAT);
             CHECK(decoder.update(TILTED_LEFT) == Command::NextStudent);
         }},
        {"Input: a small tilt (under 500 mg) gives nothing",
         [] {
             InputDecoder decoder = ready_decoder();
             CHECK(!decoder.update({false, false, 450}).has_value());
             CHECK(!decoder.update({false, false, -450}).has_value());
         }},
        {"Input: a board tilted at start-up must be flat once before a tilt counts (A-13)",
         [] {
             InputDecoder decoder;
             CHECK(!decoder.update(TILTED_LEFT).has_value());
             decoder.update(FLAT);
             CHECK(decoder.update(TILTED_LEFT) == Command::NextStudent);
         }},
    };
}
