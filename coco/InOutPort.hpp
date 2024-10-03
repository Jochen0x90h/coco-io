#pragma once

#include "InputPort.hpp"
#include "OutputPort.hpp"


namespace coco {

/**
    Virtual port of general purpose input/output pins with output enable. The initial direction (input or output) is
    implementation dependent.
*/
class InOutPort : public InputPort, public OutputPort {
public:

    ~InOutPort() override {}

    /**
        Enable the output driver
        @param pins bit mask for the pins, a set bit enables the output
        @param mask mask for outputs to affect
    */
    virtual void enableOut(uint32_t pins, uint32_t mask = 0xffffffff) = 0;
};

} // namespace coco
