#pragma once

#include <coco/platform/Loop_native.hpp>
#include <coco/platform/Trigger_cout.hpp>


// drivers for InOutTest
struct Drivers {
	coco::Loop_native loop;
	coco::Trigger_cout trigger{"trigger"};
};
