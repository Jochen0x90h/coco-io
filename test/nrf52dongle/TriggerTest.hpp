#pragma once

#include <coco/platform/Loop_RTC0.hpp>
#include <coco/platform/Trigger_GPIOTE_RTC0.hpp>


using namespace coco;

Trigger_GPIOTE_RTC0::Config triggerConfig[] {
	{gpio::P0(18), gpio::Pull::UP, true} // user button
};

// drivers for InOutTest
struct Drivers {
	Loop_RTC0 loop;
	Trigger_GPIOTE_RTC0 trigger{loop, triggerConfig};
};
