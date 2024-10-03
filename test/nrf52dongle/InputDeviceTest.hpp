#pragma once

#include <coco/platform/Loop_RTC0.hpp>
#include <coco/platform/InputDevice_GPIOTE_RTC.hpp>
#include <coco/board/config.hpp>


using namespace coco;


/**
 * Drivers InputTest
 * Use pcb/rotaryButton or other rotary button module and connect it to the discovery like this:
 * C (common pin) -> GND
 * A -> P4 (P0.04)
 * B -> P5 (P0.05)
 * S -> P6 (P0.06)
 */
struct Drivers {
    // event loop
    Loop_RTC0 loop{};

    // input device
    using InputDevice = InputDevice_GPIOTE_RTC;
    static constexpr gpio::Config inputPinConfigs[] {
        gpio::Config::P0_4 | gpio::Config::PULL_UP | gpio::Config::INVERT, //rotary knob A
        gpio::Config::P0_5 | gpio::Config::PULL_UP | gpio::Config::INVERT, // rotary knob B
        gpio::Config::P0_6 | gpio::Config::PULL_UP | gpio::Config::INVERT, // push button of rotary knob
        //gpio::Config::P0_18 | gpio::Config::PULL_UP | gpio::Config::INVERT, // user button
    };
    static constexpr InputDevice::Config inputConfigs[] {
        {0, 0, InputDevice::Action::INCREMENT_WHEN_ENABLED, InputDevice::Action::DECREMENT_WHEN_ENABLED, 1ms, 1ms}, // quadrature decoder (inputs 0 and 1, counter 0)
        {2, 1, InputDevice::Action::INCREMENT, InputDevice::Action::NONE, 10ms, 10ms}, // button press (input 2, counter 1)
        {2, 2, InputDevice::Action::INCREMENT, InputDevice::Action::NONE, 3s, 10ms}, // button long press (input 2, counter 2)
    };
    InputDevice input{loop,
        inputPinConfigs,
        inputConfigs,
        0, // first event index
        rtc::RTC1_INFO,
        0}; // CC index
};

Drivers drivers;

// check in the startup files in coco/platform/system if the interrupt handler name is spelled correctly
extern "C" {
void GPIOTE_IRQHandler() {
    drivers.input.GPIOTE_IRQHandler();
}
void RTC1_IRQHandler() {
    drivers.input.RTC_IRQHandler();
}
}
