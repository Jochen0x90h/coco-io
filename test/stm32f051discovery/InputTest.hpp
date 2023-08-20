#pragma once

#include <coco/platform/Loop_TIM2.hpp>
#include <coco/platform/Buttons_EXTI.hpp>
#include <coco/board/config.hpp>


using namespace coco;


Buttons_EXTI::Config buttonsConfig[] {
	{gpio::PA(0), gpio::Pull::DOWN, false, 0, 1} // user button
};

// drivers for ButtonsTest
struct Drivers {
	Loop_TIM2 loop{APB1_TIMER_CLOCK};
	Buttons_EXTI input{loop, buttonsConfig};
};
