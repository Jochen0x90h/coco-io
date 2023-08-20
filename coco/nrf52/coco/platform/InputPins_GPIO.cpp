#include "InputPins_GPIO.hpp"


namespace coco {

InputPins_GPIO::InputPins_GPIO(Array<const Config> configs) : configs(configs) {
	for (const auto &config : configs) {
		gpio::configureInput(config.pin, config.pull);
	}
}

InputPins_GPIO::~InputPins_GPIO() {
}

void InputPins_GPIO::get(uint32_t &pins) {
	pins = 0;
	int bit = 1;
	for (const auto &config : this->configs) {
		if (gpio::getInput(config.pin) != config.invert)
			pins |= bit;
		bit <<= 1;
	}
}

} // namespace coco
