#pragma once

#include <coco/platform/Loop_TIM2.hpp>
#include <coco/platform/InOutPins_GPIO.hpp>
#include <coco/board/config.hpp>


// prevent initialization of debug LEDs so that we can use them in test
namespace coco {
namespace debug {
void init() {}
}
}


using namespace coco;

InOutPins_GPIO::Config ioConfig[] {
	{gpio::PB(12), gpio::Pull::DISABLED, gpio::Speed::LOW, gpio::Drive::BOTH, false},
	{gpio::PC(9), gpio::Pull::DISABLED, gpio::Speed::LOW, gpio::Drive::BOTH, false}, // green LED
	{gpio::PC(8), gpio::Pull::DISABLED, gpio::Speed::LOW, gpio::Drive::BOTH, false}, // blue LED
	{gpio::PA(0), gpio::Pull::DOWN, gpio::Speed::LOW, gpio::Drive::BOTH, false} // user button
};

// drivers for InOutTest
struct Drivers {
	coco::Loop_TIM2 loop{APB1_TIMER_CLOCK};
	coco::InOutPins_GPIO io{ioConfig};
};
