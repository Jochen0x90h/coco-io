#include <coco/Loop.hpp>
#include <coco/debug.hpp>
#include <TriggerTest.hpp>


using namespace coco;

Coroutine test(Trigger &trigger) {
	int count = 1;
	while (true) {
		debug::set(count);

		uint32_t risingFlags = count & 1;
		uint32_t fallingFlags = (count >> 1) & 1;
		co_await trigger.trigger(risingFlags, fallingFlags);

		++count;
		if ((count & 3) == 0)
			++count;
	}
}

int main() {
	debug::init();
	Drivers drivers;

	test(drivers.trigger);
	
	drivers.loop.run();
}
