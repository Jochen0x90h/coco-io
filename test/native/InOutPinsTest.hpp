#pragma once

#include <coco/platform/Loop_native.hpp>
#include <coco/platform/InOutPins_cout.hpp>


// drivers for InOutTest
struct Drivers {
	coco::Loop_native loop;
	coco::InOutPins_cout io{"io"};
};
