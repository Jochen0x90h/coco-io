#include "Output_GPIO.hpp"


namespace coco {

Output_GPIO::Output_GPIO(Array<const Config> configs) : configs(configs) {
	for (const auto &config : configs) {
		gpio::setOutput(config.pin, config.initialValue != config.invert);
		gpio::configureOutput(config.pin, config.pull, config.drive);
	}
}

Output_GPIO::~Output_GPIO() {
}

Awaitable<Output::OutParameters> Output_GPIO::set(uint32_t pins, uint32_t mask) {
	setBlocking(pins, mask);
	return {};
}

void Output_GPIO::setBlocking(uint32_t pins, uint32_t mask) {
	int bit = 1;
	for (const auto &config : this->configs) {
		if (mask & bit)
			gpio::setOutput(config.pin, ((pins & bit) != 0) != config.invert);
		bit <<= 1;
	}
}

} // namespace coco
