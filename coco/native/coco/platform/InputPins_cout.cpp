#include "InputPins_cout.hpp"
#include <iostream>
#include <iomanip>


namespace coco {

InputPins_cout::~InputPins_cout() {
}

void InputPins_cout::get(uint32_t &pins) {
	pins = 0;
	std::cout << "Inputs " << this->name << ": get " << std::hex << pins << std::endl;
}

} // namespace coco
