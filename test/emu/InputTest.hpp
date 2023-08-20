#pragma once

#include <coco/platform/RotaryButton_emu.hpp>


using namespace coco;

// drivers for ButtonsTest
struct Drivers {
	Loop_emu loop;
	RotaryButton_emu input{loop, true, 100};
};
