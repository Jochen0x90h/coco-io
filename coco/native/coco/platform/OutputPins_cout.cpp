#include "OutputPins_cout.hpp"
#include <iostream>
#include <iomanip>


namespace coco {

OutputPins_cout::~OutputPins_cout() {
}

void OutputPins_cout::set(uint32_t pins, uint32_t mask) {
	this->pins = (this->pins & ~mask) | (pins & mask);
	std::cout << "Output " << this->name << ": set " << std::hex << this->pins << std::endl;
}

} // namespace coco
