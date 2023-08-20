#pragma once

#include <coco/platform/Loop_TIM2.hpp>
#include <coco/platform/InOutPins_GPIO.hpp>
#include <coco/board/config.hpp>


// prevent initialization of debug LED so that we can use it in test
namespace coco {
namespace debug {
void init() {}
}
}


using namespace coco;


InOutPins_GPIO::Config ioConfig[] {
	/*{gpio::PB(11), gpio::Pull::DISABLED, gpio::Speed::LOW, gpio::Drive::DOWN, true},
	{gpio::PB(9), gpio::Pull::DISABLED, gpio::Speed::LOW, gpio::Drive::DOWN, true}, // green LED
	{gpio::PB(8), gpio::Pull::DISABLED, gpio::Speed::LOW, gpio::Drive::DOWN, true},
	{gpio::PB(5), gpio::Pull::UP, gpio::Speed::LOW, gpio::Drive::DOWN, true} // user button*/
	{gpio::PA(4), gpio::Pull::DISABLED, gpio::Speed::LOW, gpio::Drive::BOTH, false},
	{gpio::PA(5), gpio::Pull::DISABLED, gpio::Speed::LOW, gpio::Drive::BOTH, false}, // green LED
	{gpio::PA(6), gpio::Pull::DISABLED, gpio::Speed::LOW, gpio::Drive::BOTH, false},
	{gpio::PC(13), gpio::Pull::DISABLED, gpio::Speed::LOW, gpio::Drive::BOTH, false} // user button
};

// drivers for InOutTest
struct Drivers {
	coco::Loop_TIM2 loop{APB1_TIMER_CLOCK};
	coco::InOutPins_GPIO io{ioConfig};
};
