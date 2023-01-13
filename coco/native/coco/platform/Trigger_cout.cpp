#include "Trigger_cout.hpp"
#include <iostream>


namespace coco {

Trigger_cout::~Trigger_cout() {
}

Awaitable<Trigger::Parameters> Trigger_cout::trigger(uint32_t &risingFlags, uint32_t &fallingFlags) {
	std::cout << "Trigger " << this->name << std::endl;
	return {};
}

} // namespace coco
