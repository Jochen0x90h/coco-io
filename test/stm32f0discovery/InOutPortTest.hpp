#pragma once

#include <coco/platform/Loop_TIM2.hpp>
#include <coco/platform/InOutPort_GPIO.hpp>
#include <coco/board/config.hpp>


// prevent initialization of debug LEDs so that we can use them in the test
namespace coco {
namespace debug {
void init() {}
}
}


using namespace coco;

InOutPort_GPIO::Config ioConfig[] {
    {gpio::Config::PB12, gpio::Mode::INPUT, false},
    {gpio::Config::PC9, gpio::Mode::OUTPUT, false}, // green LED
    {gpio::Config::PC8, gpio::Mode::OUTPUT, false}, // blue LED
    {gpio::Config::PA0 | gpio::Config::PULL_DOWN, gpio::Mode::INPUT, false} // user button
};

// drivers for InOutTest
struct Drivers {
    coco::Loop_TIM2 loop{APB1_TIMER_CLOCK};
    coco::InOutPort_GPIO io{ioConfig};
};

Drivers drivers;
