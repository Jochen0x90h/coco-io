#include "InOutPort_cout.hpp"
#include <iostream>
#include <iomanip>


namespace coco {

InOutPort_cout::~InOutPort_cout() {
}

void InOutPort_cout::get(uint32_t &pins) {
    pins = this->pins;
    std::cout << "InOut " << this->name << ": get " << std::hex << pins << std::endl;
}

void InOutPort_cout::set(uint32_t pins, uint32_t mask) {
    this->pins = (this->pins & ~mask) | (pins & mask);
    std::cout << "InOut " << this->name << ": set " << std::hex << this->pins << std::endl;
}

void InOutPort_cout::enableOut(uint32_t pins, uint32_t mask) {
    std::cout << "InOut " << this->name << ": enableOut " << std::hex << pins << std::endl;
}

} // namespace coco
