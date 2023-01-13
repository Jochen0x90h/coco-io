#include "Input_GPIO.hpp"


namespace coco {

Input_GPIO::Input_GPIO(Array<const Config> configs) : configs(configs) {
	for (const auto &config : configs) {
		gpio::configureInput(config.pin, config.pull);
	}
}

Input_GPIO::~Input_GPIO() {
}

Awaitable<Input::InParameters> Input_GPIO::get(uint32_t &pins) {
	getBlocking(pins);
	return {};
}

void Input_GPIO::getBlocking(uint32_t &pins) {
	pins = 0;
	int bit = 1;
	for (const auto &config : this->configs) {
		if (gpio::getInput(config.pin) != config.invert)
			pins |= bit;
		bit <<= 1;
	}
}

} // namespace coco
