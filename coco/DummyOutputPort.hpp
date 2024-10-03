#pragma once

#include "OutputPort.hpp"


namespace coco {

/**
    Implementation of OutputPort that does nothing
*/
class DummyOutputPort : public OutputPort {
public:

    virtual ~DummyOutputPort();

    virtual void set(uint32_t pins, uint32_t mask = 0xffffffff) override;
};

} // namespace coco
