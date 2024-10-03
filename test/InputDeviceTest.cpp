#include <coco/Loop.hpp>
#include <coco/debug.hpp>
#include <InputDeviceTest.hpp>


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
            // set magenta when button was pressed
            debug::set(debug::MAGENTA);
        } else if (state[2] != lastState[2]) {
            // set yellow when button was long pressed
            debug::set(debug::YELLOW);
        } else {
            // set counter of rotary knob
            debug::set(state[0]);
        }
        //debug::set(state[1]);

        // it is possible to wait at this point
        co_await loop.sleep(100ms);

        // wait until new input data is available, returns immediately if input with a new sequence number is already available
        co_await input.untilInput(seq);

        // copy state
        std::copy(state, state + 3, lastState);
    }
}


int main() {
    inputTest(drivers.loop, drivers.input);

    drivers.loop.run();
}
