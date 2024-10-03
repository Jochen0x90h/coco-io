#include "InOutPort_GPIO.hpp"


namespace coco {

InOutPort_GPIO::InOutPort_GPIO(Array<const Config> configs) : configs(configs) {
    for (const auto &config : configs) {
        gpio::setOutput(config.config, config.initialValue);
        gpio::configure(config.initialMode, config.config);
    }
}

InOutPort_GPIO::~InOutPort_GPIO() {
}

void InOutPort_GPIO::get(uint32_t &pins) {
    pins = 0;
    int bit = 0;
    for (const auto &config : this->configs) {
        pins |= int(gpio::getInput(config.config)) << bit;
        ++bit;
    }
}

void InOutPort_GPIO::set(uint32_t pins, uint32_t function) {
    int bit = 0;
    for (const auto &config : this->configs) {
        gpio::setOutput(config.config, (pins >> bit) & 1, (function >> bit) & 1);
        ++bit;
    }
}

void InOutPort_GPIO::enableOut(uint32_t pins, uint32_t mask) {
    int bit = 0;
    for (const auto &config : this->configs) {
        if ((mask >> bit) & 1)
            gpio::enableOutput(config.config, (pins >> bit) & 1);
        ++bit;
    }
}

} // namespace coco
