#pragma once

#include <coco/platform/RotaryKnob_emu.hpp>


using namespace coco;

// drivers for ButtonsTest
struct Drivers {
    // event loop
    Loop_emu loop;

    // emulated rotary knob with push button
    RotaryKnob_emu input{loop, true, 100};
};

Drivers drivers;
