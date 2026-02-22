#pragma once

#include <coco/platform/Loop_TIM2.hpp>
#include <coco/platform/InOutPort_gpio.hpp>
#include <coco/board/config.hpp>


using namespace coco;


InOutPort_GPIO::Config ioConfig[] {
    {gpio::PA4, gpio::Mode::INPUT, false},
    {gpio::PA5, gpio::Mode::OUTPUT, false}, // green LED
    {gpio::PA6, gpio::Mode::INPUT, false},
    {gpio::PC13 | gpio::Config::PULL_DOWN, gpio::Mode::INPUT, false} // user button, has 100k pull-down
};

// drivers for InOutTest
struct Drivers {
    coco::Loop_TIM2 loop{APB1_TIMER_CLOCK};
    coco::InOutPort_GPIO io{ioConfig};
};

Drivers drivers;
