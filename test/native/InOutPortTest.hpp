#pragma once

#include <coco/platform/Loop_native.hpp>
#include <coco/platform/InOutPort_cout.hpp>


// drivers for InOutTest
struct Drivers {
    coco::Loop_native loop;
    coco::InOutPort_cout io{"io"};
};

Drivers drivers;
