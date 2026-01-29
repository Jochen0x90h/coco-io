#include <coco/Loop.hpp>
#include <coco/debug.hpp>
#include <coco/StreamOperators.hpp>
#include <InputDeviceTest.hpp>
#include <ranges>


using namespace coco;


Coroutine inputTest(Loop &loop, InputDevice &input) {
    int8_t lastState[3];
    input.get(lastState);
    while (true) {
        // get input state (rotary knob count, button press, long press)
        int8_t state[3];
        int seq = input.get(state);

        // output on debug LEDs
        if (state[1] != lastState[1]) {
            // set green when button was pressed
            debug::set(debug::GREEN);
            debug::out << "Short press\n";
        } else if (state[2] != lastState[2]) {
            // set magenta when button was long pressed
            debug::set(debug::MAGENTA);
            debug::out << "Long press\n";
        } else {
            // set counter of rotary knob
            debug::set(state[0]);
            debug::out << "Rotate " << dec(state[0]) << "\n";
        }
        //debug::set(state[1]);

        // it is possible to wait at this point
        co_await loop.sleep(100ms);

        // wait until new input data is available, returns immediately if input with a new sequence number is already available
        co_await input.untilInput(seq);

        // copy state into lastState
        std::ranges::copy(state, lastState);
    }
}


int main() {
    debug::out << "InputDeviceTest\n";

    inputTest(drivers.loop, drivers.input);

    drivers.loop.run();
}
