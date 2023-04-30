#include <coco/Loop.hpp>
//#include <coco/debug.hpp> // no debug as test outputs are connected to debug LEDs
#include <InOutTest.hpp>


using namespace coco;

Coroutine test(Loop &loop, InOut &io) {
	int count = 0;
	while (true) {
		co_await io.set(count);
		co_await loop.sleep(500ms);

		// clear counter if button is pressed, otherwise count up
		uint32_t pins;
		co_await io.get(pins);
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
