#include "OutputPort_cout.hpp"
#include <iostream>
#include <iomanip>


namespace coco {

OutputPort_cout::~OutputPort_cout() {
}

void OutputPort_cout::set(uint32_t pins, uint32_t mask) {
    this->pins = (this->pins & ~mask) | (pins & mask);
    std::cout << "Output " << this->name << ": set " << std::hex << this->pins << std::endl;
}

} // namespace coco
