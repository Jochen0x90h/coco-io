#pragma once

#include <coco/Array.hpp>
#include <coco/platform/gpio.hpp>
#include <coco/InOutPort.hpp>


namespace coco {

/**
    Implementation of InOutPort using GPIO
*/
class InOutPort_GPIO : public InOutPort {
public:
    struct Config {
        // pin, port and configuration such as pull up/down or open drain
        gpio::Config config;

        // initial mode (input or output)
        gpio::Mode initialMode;

        // initial value (inverted by INVERT flag in config)
        bool initialValue;
    };

    explicit InOutPort_GPIO(Array<const Config> configs);
    ~InOutPort_GPIO() override;

    void get(uint32_t &pins) override;
    void set(uint32_t pins, uint32_t mask = 0xffffffff) override;
    void enableOut(uint32_t pins, uint32_t mask = 0xffffffff) override;

protected:

    Array<const Config> configs;
};

} // namespace coco
