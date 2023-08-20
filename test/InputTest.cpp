#include <coco/Loop.hpp>
#include <coco/debug.hpp>
#include <InputTest.hpp>


using namespace coco;


Coroutine inputTest(Loop &loop, Input &input) {
	while (true) {
		// wait for input state change
		co_await input.stateChange();

		// get state
		int8_t state[2];
		input.getState(state);

		// output state on debug LEDs
		debug::set(state[0]);

		//co_await loop.sleep(500ms);
	}
}


int main() {
	Drivers drivers;

	inputTest(drivers.loop, drivers.input);

	drivers.loop.run();
}
