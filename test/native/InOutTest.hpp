#pragma once

#include <coco/platform/Loop_native.hpp>
#include <coco/platform/InOut_cout.hpp>


// drivers for InOutTest
struct Drivers {
	coco::Loop_native loop;
	coco::InOut_cout io{"io"};
};
