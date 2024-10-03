#include "InputPort_GPIO.hpp"


namespace coco {

InputPort_GPIO::InputPort_GPIO(Array<const Config> configs) : configs(configs) {
    for (const auto &config : configs) {
        gpio::configureInput(config.config);
    }
}

InputPort_GPIO::~InputPort_GPIO() {
}

void InputPort_GPIO::get(uint32_t &pins) {
    pins = 0;
    int bit = 0;
    for (const auto &config : this->configs) {
        pins |= int(gpio::getInput(config.config)) << bit;
        ++bit;
    }
}

} // namespace coco
