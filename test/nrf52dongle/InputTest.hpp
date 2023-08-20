#pragma once

#include <coco/platform/Loop_RTC0.hpp>
#include <coco/platform/RotaryButton_QDEC_GPIOTE.hpp>
#include <coco/board/config.hpp>


using namespace coco;


// drivers for ButtonsTest
struct Drivers {
	Loop_RTC0 loop{};
	RotaryButton_QDEC_GPIOTE input{loop,
		gpio::P0(4), gpio::P0(5), // quadrature decoder pins
		gpio::P0(6), 0}; // button pin and GPIOTE index
};
