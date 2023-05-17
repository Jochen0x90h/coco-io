#pragma once

#include <coco/platform/Loop_RTC0.hpp>
#include <coco/platform/InOut_GPIO.hpp>


using namespace coco;

InOut_GPIO::Config ioConfig[] {
	{gpio::P0(23), gpio::Pull::DISABLED, gpio::Drive::S0D1, true}, // red LED
	{gpio::P0(22), gpio::Pull::DISABLED, gpio::Drive::S0D1, true}, // green LED
	{gpio::P0(24), gpio::Pull::DISABLED, gpio::Drive::S0D1, true}, // blue LED
	{gpio::P0(18), gpio::Pull::UP, gpio::Drive::S0D1, true} // user button
};

// drivers for InOutTest
struct Drivers {
	Loop_RTC0 loop;
	InOut_GPIO io{ioConfig};
};
