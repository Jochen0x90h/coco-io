#pragma once

#include <coco/Array.hpp>
#include <coco/platform/gpio.hpp>
#include <coco/OutputPort.hpp>
#include <string>


namespace coco {

/// @brief Implementation of OutputPort using gpio HAL.
///
class OutputPort_GPIO : public OutputPort {
public:
    struct Config {
        // pin, port and configuration such as open drain
        gpio::Config config;

        // initial output value (inverted by INVERT flag in config)
        bool initialValue;
    };

    /// @brief Constructor.
    /// @param configs array of pin configurations, must not be a temporary value
    explicit OutputPort_GPIO(Array<const Config> configs);
    ~OutputPort_GPIO() override;

    void set(uint32_t pins, uint32_t mask = 0xffffffff) override;

protected:

    Array<const Config> configs;
};

} // namespace coco
