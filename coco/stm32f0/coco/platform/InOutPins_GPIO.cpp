#include "InOutPins_GPIO.hpp"


namespace coco {

InOutPins_GPIO::InOutPins_GPIO(Array<const Config> configs) : configs(configs) {
	for (const auto &config : configs) {
		gpio::configureInOut(config.pin, config.pull, config.speed, config.drive);
	}
}

InOutPins_GPIO::~InOutPins_GPIO() {
}

void InOutPins_GPIO::get(uint32_t &pins) {
	pins = 0;
	int bit = 1;
	for (const auto &config : this->configs) {
		if (gpio::getInput(config.pin) != config.invert)
			pins |= bit;
		bit <<= 1;
	}
}

void InOutPins_GPIO::set(uint32_t pins, uint32_t mask) {
	int bit = 1;
	for (const auto &config : this->configs) {
		if (mask & bit)
			gpio::setOutput(config.pin, ((pins & bit) != 0) != config.invert);
		bit <<= 1;
	}
}

void InOutPins_GPIO::enableOut(uint32_t pins, uint32_t mask) {
	int bit = 1;
	for (const auto &config : this->configs) {
		if (mask & bit)
			gpio::setMode(config.pin, ((pins & bit) != 0) ? gpio::Mode::OUTPUT : gpio::Mode::INPUT);
		bit <<= 1;
	}
}

} // namespace coco
