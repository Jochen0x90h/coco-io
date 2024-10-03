#pragma once

#include <cstdint>


namespace coco {

/**
    Virtual port of general purpose output pins
*/
class OutputPort {
public:

    virtual ~OutputPort() {};

    /**
        Set or toggle the output pins
        @param pins pins bit mask for the output pins
        @param function function for each pin: 0: keep/toogle, 1: reset/set
    */
    virtual void set(uint32_t pins, uint32_t function = 0xffffffff) = 0;
};

} // namespace coco
