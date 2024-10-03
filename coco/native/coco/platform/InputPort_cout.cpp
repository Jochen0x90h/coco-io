#include "InputPort_cout.hpp"
#include <iostream>
#include <iomanip>


namespace coco {

InputPort_cout::~InputPort_cout() {
}

void InputPort_cout::get(uint32_t &pins) {
    pins = 0;
    std::cout << "Inputs " << this->name << ": get " << std::hex << pins << std::endl;
}

} // namespace coco
