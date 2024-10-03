#pragma once

#include <coco/Array.hpp>
#include <coco/platform/gpio.hpp>
#include <coco/InputPort.hpp>
#include <string>


namespace coco {

/**
    Implementation of InputPort using GPIO
*/
class InputPort_GPIO : public InputPort {
public:
    struct Config {
        // pin, port and configuration such as pull up/down
        gpio::Config config;
    };

    /**
     * Constructor
     * @param configs array of pin configurations, must not be a temporary value
     */
    explicit InputPort_GPIO(Array<const Config> configs);
    ~InputPort_GPIO() override;

    void get(uint32_t &pins) override;

protected:

    Array<const Config> configs;
};

} // namespace coco
