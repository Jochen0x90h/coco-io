#include "InOut_cout.hpp"
#include <iostream>
#include <iomanip>


namespace coco {

InOut_cout::~InOut_cout() {
}

Awaitable<InOut::InParameters> InOut_cout::get(uint32_t &pins) {
	getBlocking(pins);
	return {};
}

void InOut_cout::getBlocking(uint32_t &pins) {
	pins = this->pins;
	std::cout << "InOut " << this->name << ": get " << std::hex << pins << std::endl;
}

Awaitable<InOut::OutParameters> InOut_cout::set(uint32_t pins, uint32_t mask) {
	setBlocking(pins, mask);
	return {};
}

void InOut_cout::setBlocking(uint32_t pins, uint32_t mask) {
	this->pins = (this->pins & ~mask) | (pins & mask);
	std::cout << "InOut " << this->name << ": set " << std::hex << this->pins << std::endl;
}

Awaitable<InOut::OutParameters> InOut_cout::enableOut(uint32_t pins, uint32_t mask) {
	enableOutBlocking(pins, mask);
	return {};
}

void InOut_cout::enableOutBlocking(uint32_t pins, uint32_t mask) {
	std::cout << "InOut " << this->name << ": enableOut " << std::hex << pins << std::endl;
}

} // namespace coco
