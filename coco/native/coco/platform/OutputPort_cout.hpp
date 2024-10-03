#pragma once

#include <coco/OutputPort.hpp>
#include <string>


namespace coco {

/**
    Implementation of OutputPort interface that simply writes info about the out operations to std::cout
*/
class OutputPort_cout : public OutputPort {
public:

    explicit OutputPort_cout(std::string name) : name(std::move(name)) {}
    ~OutputPort_cout() override;

    void set(uint32_t pins, uint32_t mask = 0xffffffff) override;

protected:

    std::string name;
    int32_t pins = 0;
};

} // namespace coco
