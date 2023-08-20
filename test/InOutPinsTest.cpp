#include <coco/Loop.hpp>
//#include <coco/debug.hpp> // no debug as test outputs are connected to debug LEDs
#include <InOutPinsTest.hpp>


using namespace coco;

Coroutine test(Loop &loop, InOutPins &io) {
	// set LED pins to output
	io.enableOut(7, 7);

	int count = 0;
	while (true) {
		// set output pins
		io.set(count);
		co_await loop.sleep(500ms);

		// clear counter if button is pressed, otherwise count up
		uint32_t pins;
		io.get(pins);
		if (pins & (1 << 3))
			count = 0;
		else
			++count;
	}
}

int main() {
	Drivers drivers;

	test(drivers.loop, drivers.io);

	drivers.loop.run();
}
