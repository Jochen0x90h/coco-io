#include "InOutPins_cout.hpp"
#include <iostream>
#include <iomanip>


namespace coco {

InOutPins_cout::~InOutPins_cout() {
}

void InOutPins_cout::get(uint32_t &pins) {
	pins = this->pins;
	std::cout << "InOut " << this->name << ": get " << std::hex << pins << std::endl;
}

void InOutPins_cout::set(uint32_t pins, uint32_t mask) {
	this->pins = (this->pins & ~mask) | (pins & mask);
	std::cout << "InOut " << this->name << ": set " << std::hex << this->pins << std::endl;
}

void InOutPins_cout::enableOut(uint32_t pins, uint32_t mask) {
	std::cout << "InOut " << this->name << ": enableOut " << std::hex << pins << std::endl;
}

} // namespace coco
