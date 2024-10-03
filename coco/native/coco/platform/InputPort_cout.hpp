#pragma once

#include <coco/InputPort.hpp>
#include <string>


namespace coco {

/**
    Implementation of InOutPort that simply writes info about the in/out operations to std::cout
 */
class InputPort_cout : public InputPort {
public:

    explicit InputPort_cout(std::string name) : name(std::move(name)) {}
    ~InputPort_cout() override;

    void get(uint32_t &pins) override;

protected:

    std::string name;
};

} // namespace coco
