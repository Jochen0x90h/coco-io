#include "OutputPins_GPIO.hpp"


namespace coco {

OutputPins_GPIO::OutputPins_GPIO(Array<const Config> configs) : configs(configs) {
	for (const auto &config : configs) {
		gpio::configureOutput(config.pin, config.initialValue, config.speed, config.drive, config.pull);
	}
}

OutputPins_GPIO::~OutputPins_GPIO() {
}

void OutputPins_GPIO::set(uint32_t pins, uint32_t mask) {
	int bit = 1;
	for (const auto &config : this->configs) {
		if (mask & bit)
			gpio::setOutput(config.pin, ((pins & bit) != 0) != config.invert);
		bit <<= 1;
	}
}

} // namespace coco
