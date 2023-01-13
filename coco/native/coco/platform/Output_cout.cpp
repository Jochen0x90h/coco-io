#include "Output_cout.hpp"
#include <iostream>
#include <iomanip>


namespace coco {

Output_cout::~Output_cout() {
}

Awaitable<Output::OutParameters> Output_cout::set(uint32_t pins, uint32_t mask) {
	setBlocking(pins, mask);
	return {};
}

void Output_cout::setBlocking(uint32_t pins, uint32_t mask) {
	this->pins = (this->pins & ~mask) | (pins & mask);
	std::cout << "Output " << this->name << ": set " << std::hex << this->pins << std::endl;
}

} // namespace coco
