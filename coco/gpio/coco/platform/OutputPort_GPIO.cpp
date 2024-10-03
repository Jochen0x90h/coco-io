#include "OutputPort_GPIO.hpp"


namespace coco {

OutputPort_GPIO::OutputPort_GPIO(Array<const Config> configs) : configs(configs) {
    for (const auto &config : configs) {
        gpio::configureOutput(config.config, config.initialValue);
    }
}

OutputPort_GPIO::~OutputPort_GPIO() {
}

void OutputPort_GPIO::set(uint32_t pins, uint32_t function) {
    int bit = 0;
    for (const auto &config : this->configs) {
        gpio::setOutput(config.config, (pins >> bit) & 1, (function >> bit) & 1);
        ++bit;
    }
}

} // namespace coco
