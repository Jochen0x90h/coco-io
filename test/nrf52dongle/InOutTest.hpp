#pragma once

#include <coco/platform/Loop_RTC0.hpp>
#include <coco/platform/InOut_GPIO.hpp>


using namespace coco;

InOut_GPIO::Config ioConfig[] {
	{gpio::P0(23), gpio::Mode::OUTPUT, gpio::Pull::DISABLED, gpio::Drive::S0D1, true, false}, // red LED
	{gpio::P0(22), gpio::Mode::OUTPUT, gpio::Pull::DISABLED, gpio::Drive::S0D1, true, false}, // green LED
	{gpio::P0(24), gpio::Mode::OUTPUT, gpio::Pull::DISABLED, gpio::Drive::S0D1, true, false}, // blue LED
	{gpio::P0(18), gpio::Mode::INPUT, gpio::Pull::UP, gpio::Drive::S0D1, true, false} // user button
};

// drivers for InOutTest
struct Drivers {
	Loop_RTC0 loop;
	InOut_GPIO io{ioConfig};
};
