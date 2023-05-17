#pragma once

#include <coco/platform/Loop_TIM2.hpp>
#include <coco/platform/InOut_GPIO.hpp>
#include <coco/board/config.hpp>


using namespace coco;

InOut_GPIO::Config ioConfig[] {
	{gpio::PB(12), gpio::Pull::DISABLED, gpio::Speed::LOW, gpio::Drive::PUSH_PULL, false},
	{gpio::PC(9), gpio::Pull::DISABLED, gpio::Speed::LOW, gpio::Drive::PUSH_PULL, false}, // green LED
	{gpio::PC(8), gpio::Pull::DISABLED, gpio::Speed::LOW, gpio::Drive::PUSH_PULL, false}, // blue LED
	{gpio::PA(0), gpio::Pull::DISABLED, gpio::Speed::LOW, gpio::Drive::PUSH_PULL, false} // user button
};

// drivers for InOutTest
struct Drivers {
	coco::Loop_TIM2 loop;
	coco::InOut_GPIO io{ioConfig};
};
