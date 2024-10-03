#pragma once

#include <cstdint>


namespace coco {

/**
    Virtual port of general purpose input pins
*/
class InputPort {
public:
    virtual ~InputPort() {}

    /**
        Read the current value of an input
        @param pins pins bit mask for the input pins
    */
    virtual void get(uint32_t &pins) = 0;
};

} // namespace coco
