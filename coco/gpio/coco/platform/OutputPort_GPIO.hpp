#pragma once

#include <coco/Array.hpp>
#include <coco/platform/gpio.hpp>
#include <coco/OutputPort.hpp>
#include <string>


namespace coco {

/**
    Implementation of OutputPort using GPIO
*/
class OutputPort_GPIO : public OutputPort {
public:
    struct Config {
        // pin, port and configuration such as open drain
        gpio::Config config;

        // initial value (inverted by INVERT flag in config)
        bool initialValue;
    };

    explicit OutputPort_GPIO(Array<const Config> configs);
    ~OutputPort_GPIO() override;

    void set(uint32_t pins, uint32_t mask = 0xffffffff) override;

protected:

    Array<const Config> configs;
};

} // namespace coco
