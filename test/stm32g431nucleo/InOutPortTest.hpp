#pragma once

#include <coco/platform/Loop_TIM2.hpp>
#include <coco/platform/InOutPort_GPIO.hpp>
#include <coco/board/config.hpp>


// prevent initialization of debug LED so that we can use it in the test
namespace coco {
namespace debug {
void init() {}
}
}


using namespace coco;


InOutPort_GPIO::Config ioConfig[] {
    {gpio::Config::PA4, gpio::Mode::INPUT, false},
    {gpio::Config::PA5, gpio::Mode::OUTPUT, false}, // green LED
    {gpio::Config::PA6, gpio::Mode::INPUT, false},
    {gpio::Config::PC13, gpio::Mode::INPUT, false} // user button
};

// drivers for InOutTest
struct Drivers {
    coco::Loop_TIM2 loop{APB1_TIMER_CLOCK};
    coco::InOutPort_GPIO io{ioConfig};
};

Drivers drivers;
