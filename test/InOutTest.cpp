#include <coco/loop.hpp>
//#include <coco/debug.hpp> // no debug as test outputs are connected to debug LEDs
#include <coco/board/InOutTest.hpp>


using namespace coco;

Coroutine timer1(InOut &io) {
	int count = 0;
	while (true) {
		io.setBlocking(count);
		co_await loop::sleep(500ms);

		// clear counter if button is pressed, otherwise count up
		uint32_t pins;
		io.getBlocking(pins);
		if (pins & (1 << 3))
			count = 0;
		else
			++count;
	}
}

int main() {
	loop::init();
	board::InOutTest drivers;

	timer1(drivers.io);
	
	loop::run();
}
