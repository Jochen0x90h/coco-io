#pragma once

#include <coco/platform/Loop_TIM2.hpp>
#include <coco/platform/Trigger_EXTI_TIM2.hpp>


using namespace coco;

Trigger_EXTI_TIM2::Config triggerConfig[] {
	{gpio::PA(0), gpio::Pull::DISABLED, false} // user button
};

// drivers for InOutTest
struct Drivers {
	Loop_TIM2 loop;
	Trigger_EXTI_TIM2 trigger{loop, triggerConfig};
};
