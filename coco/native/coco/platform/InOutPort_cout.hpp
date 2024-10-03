#pragma once

#include <coco/InOutPort.hpp>
#include <string>


namespace coco {

/**
    Implementation of InOutPort that simply writes info about the in/out operations to std::cout
*/
class InOutPort_cout : public InOutPort {
public:

    explicit InOutPort_cout(std::string name) : name(std::move(name)) {}
    ~InOutPort_cout() override;

    void get(uint32_t &pins) override;
    void set(uint32_t pins, uint32_t mask = 0xffffffff) override;
    void enableOut(uint32_t pins, uint32_t mask = 0xffffffff) override;

protected:

    std::string name;
    int32_t pins = 0;
};

} // namespace coco
