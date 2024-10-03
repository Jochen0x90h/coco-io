#pragma once

#include <coco/platform/Loop_TIM2.hpp>
#include <coco/platform/InputDevice_EXTI_TIM.hpp>
#include <coco/board/config.hpp>


using namespace coco;


/**
 * Drivers InputTest
 * Use pcb/rotaryButton or other rotary button module and connect it to the nucleo board like this:
 * C (common pin) -> CN10 20 (GND)
 * A -> CN10 22
 * B -> CN10 24
 * S -> CN10 26
 */
struct Drivers {
    // event loop
    Loop_TIM2 loop{APB1_TIMER_CLOCK};

    // input device
    using InputDevice = InputDevice_EXTI_TIM;
    static constexpr gpio::Config inputPinConfigs[] {
        gpio::Config::PB2 | gpio::Config::PULL_UP | gpio::Config::INVERT, //rotary knob A
        gpio::Config::PB1 | gpio::Config::PULL_UP | gpio::Config::INVERT, // rotary knob B
        gpio::Config::PB15 | gpio::Config::PULL_UP | gpio::Config::INVERT, // push button of rotary knob
        //gpio::Config::PC13 | gpio::Config::PULL_DOWN, // blue user button
    };
    static constexpr InputDevice::Config inputConfigs[] {
        {0, 0, InputDevice::Action::INCREMENT_WHEN_ENABLED, InputDevice::Action::DECREMENT_WHEN_ENABLED, 1ms, 1ms}, // quadrature decoder (inputs 0 and 1, counter 0)
        {2, 1, InputDevice::Action::INCREMENT, InputDevice::Action::NONE, 10ms, 10ms}, // button press (input 2, counter 1)
        {2, 2, InputDevice::Action::INCREMENT, InputDevice::Action::NONE, 3s, 10ms}, // button long press (input 2, counter 2)
    };
    InputDevice input{loop,
        inputPinConfigs,
        inputConfigs,
        timer::TIM3_INFO,
        APB2_TIMER_CLOCK,
        0}; // CC index
};

Drivers drivers;

// check in the startup files in coco/platform/system if the interrupt handler name is spelled correctly
extern "C" {
void EXTI1_IRQHandler() {
    drivers.input.EXTI_IRQHandler();
}
void EXTI2_IRQHandler() {
    drivers.input.EXTI_IRQHandler();
}
void EXTI15_10_IRQHandler() {
    drivers.input.EXTI_IRQHandler();
}
void TIM3_IRQHandler() {
    drivers.input.TIM_IRQHandler();
}
}
