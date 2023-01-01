#include "InOutOut.hpp"
#include <iostream>
#include <iomanip>


namespace coco {

InOutOut::~InOutOut() {
}

Awaitable<InOut::InParameters> InOutOut::get(uint32_t &pins) {
	getBlocking(pins);
	return {};
}

void InOutOut::getBlocking(uint32_t &pins) {
	this->pins = pins;
	std::cout << "InOut " << this->name << ": get " << std::hex << this->pins << std::endl;
}

Awaitable<InOut::OutParameters> InOutOut::set(uint32_t pins, uint32_t mask) {
	setBlocking(pins, mask);
	return {};
}

void InOutOut::setBlocking(uint32_t pins, uint32_t mask) {
	this->pins = (this->pins & ~mask) | (pins & mask);
	std::cout << "InOut " << this->name << ": set " << std::hex << this->pins << std::endl;
}

Awaitable<InOut::OutParameters> InOutOut::enableOut(uint32_t pins, uint32_t mask) {
	enableOutBlocking(pins, mask);
	return {};
}

void InOutOut::enableOutBlocking(uint32_t pins, uint32_t mask) {
	std::cout << "InOut " << this->name << ": enableOut " << std::hex << pins << std::endl;
}

} // namespace coco
