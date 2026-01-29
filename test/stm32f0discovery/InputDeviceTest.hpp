#pragma once

#include <coco/platform/Loop_TIM2.hpp>
#include <coco/platform/InputDevice_EXTI_TIM.hpp>
#include <coco/board/config.hpp>


using namespace coco;


/// @brief Drivers InputTest
/// Use pcb/rotaryButton or other rotary button module and connect it to the discovery like this:
/// C (common pin) -> GND
/// A -> PC13
/// B -> PC14
/// S -> PC15
struct Drivers {
    // event loop
    Loop_TIM2 loop{APB1_TIMER_CLOCK};

    // input device
    using InputDevice = InputDevice_EXTI_TIM;
    static constexpr gpio::Config inputPinConfigs[] {
        // input 0: rotary knob A (EXTI13)
        gpio::PC13 | gpio::Config::PULL_UP | gpio::Config::INVERT,

        // input 1: rotary knob B (EXTI14)
        gpio::PC14 | gpio::Config::PULL_UP | gpio::Config::INVERT,

        // input 2: rotary knob push button (EXTI15)
        gpio::PC15 | gpio::Config::PULL_UP | gpio::Config::INVERT,

        // input 3: blue user button (on discovery board, EXTI0)
        gpio::PA0 | gpio::Config::PULL_DOWN,
    };
    static constexpr InputDevice::Config inputConfigs[] {
        // quadrature decoder (listen on inputs 0 and 1, modify counter 0)
        {0, 0, InputDevice::Init::INPUT, InputDevice::Action::INCREMENT_WHEN_ENABLED, InputDevice::Action::DECREMENT_WHEN_ENABLED, 1ms, 1ms},

        // button press (listen on input 2, modify counter 1)
        {2, 1, InputDevice::Init::INPUT, InputDevice::Action::INCREMENT, InputDevice::Action::NONE, 10ms, 10ms},

        // button long press (listen on input 2, modify counter 2)
        {2, 2, InputDevice::Init::INPUT, InputDevice::Action::INCREMENT, InputDevice::Action::NONE, 3s, 10ms},

        // blue user button press (listen on input 3, modify counter 1)
        {3, 1, InputDevice::Init::INPUT, InputDevice::Action::INCREMENT, InputDevice::Action::NONE, 10ms, 10ms},

        // blue user button long press (listen on input 3, modify counter 2)
        {3, 2, InputDevice::Init::INPUT, InputDevice::Action::INCREMENT, InputDevice::Action::NONE, 3s, 10ms},
    };
    InputDevice input{loop,
        inputPinConfigs,
        inputConfigs,
        timer::TIM3_INFO,
        APB1_TIMER_CLOCK};
};

Drivers drivers;

// check in the startup files in coco/platform/system if the interrupt handler name is spelled correctly
extern "C" {
// handlers for all used EXTIs
void EXTI0_1_IRQHandler() {
    drivers.input.EXTI_IRQHandler();
}
void EXTI4_15_IRQHandler() {
    drivers.input.EXTI_IRQHandler();
}

// handler for timer
void TIM3_IRQHandler() {
    drivers.input.TIM_IRQHandler();
}
}
