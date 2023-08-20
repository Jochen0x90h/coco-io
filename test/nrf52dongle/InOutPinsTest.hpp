#pragma once

#include <coco/platform/Loop_RTC0.hpp>
#include <coco/platform/InOutPins_GPIO.hpp>


// prevent initialization of debug LEDs so that we can use them in test
namespace coco {
namespace debug {
void init() {}
}
}


using namespace coco;

InOutPins_GPIO::Config ioConfig[] {
	{gpio::P0(23), gpio::Pull::DISABLED, gpio::Drive::S0D1, true}, // red LED
	{gpio::P0(22), gpio::Pull::DISABLED, gpio::Drive::S0D1, true}, // green LED
	{gpio::P0(24), gpio::Pull::DISABLED, gpio::Drive::S0D1, true}, // blue LED
	{gpio::P0(18), gpio::Pull::UP, gpio::Drive::S0D1, true} // user button
};

// drivers for InOutTest
struct Drivers {
	Loop_RTC0 loop;
	InOutPins_GPIO io{ioConfig};
};
