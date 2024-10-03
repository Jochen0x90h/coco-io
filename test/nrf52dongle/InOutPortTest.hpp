#pragma once

#include <coco/platform/Loop_RTC0.hpp>
#include <coco/platform/InOutPort_GPIO.hpp>


// prevent initialization of debug LEDs so that we can use them in the test
namespace coco {
namespace debug {
void init() {}
}
}


using namespace coco;

InOutPort_GPIO::Config ioConfig[] {
    {gpio::Config::P0_23 | gpio::Config::INVERT, gpio::Mode::OUTPUT, true}, // red LED
    {gpio::Config::P0_22 | gpio::Config::INVERT, gpio::Mode::OUTPUT, true}, // green LED
    {gpio::Config::P0_24 | gpio::Config::INVERT, gpio::Mode::OUTPUT, true}, // blue LED
    {gpio::Config::P0_18 | gpio::Config::INVERT | gpio::Config::PULL_UP, gpio::Mode::INPUT, true} // user button
};

// drivers for InOutTest
struct Drivers {
    Loop_RTC0 loop;
    InOutPort_GPIO io{ioConfig};
};

Drivers drivers;
