#include "Input_cout.hpp"
#include <iostream>
#include <iomanip>


namespace coco {

Input_cout::~Input_cout() {
}

Awaitable<InOut::InParameters> Input_cout::get(uint32_t &pins) {
	getBlocking(pins);
	return {};
}

void Input_cout::getBlocking(uint32_t &pins) {
	pins = 0;
	std::cout << "Inputs " << this->name << ": get " << std::hex << pins << std::endl;
}

} // namespace coco
