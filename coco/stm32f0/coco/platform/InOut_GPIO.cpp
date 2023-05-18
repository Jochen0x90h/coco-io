#include "InOut_GPIO.hpp"


namespace coco {

InOut_GPIO::InOut_GPIO(Array<const Config> configs) : configs(configs) {
	for (const auto &config : configs) {
		gpio::configureInOut(config.pin, gpio::Mode::INPUT, config.pull, config.speed, config.drive);
	}
}

InOut_GPIO::~InOut_GPIO() {
}

Awaitable<InOut::InParameters> InOut_GPIO::get(uint32_t &pins) {
	getBlocking(pins);
	return {};
}

void InOut_GPIO::getBlocking(uint32_t &pins) {
	pins = 0;
	int bit = 1;
	for (const auto &config : this->configs) {
		if (gpio::getInput(config.pin) != config.invert)
			pins |= bit;
		bit <<= 1;
	}
}

Awaitable<InOut::OutParameters> InOut_GPIO::set(uint32_t pins, uint32_t mask) {
	setBlocking(pins, mask);
	return {};
}

void InOut_GPIO::setBlocking(uint32_t pins, uint32_t mask) {
	int bit = 1;
	for (const auto &config : this->configs) {
		if (mask & bit)
			gpio::setOutput(config.pin, ((pins & bit) != 0) != config.invert);
		bit <<= 1;
	}
}

Awaitable<InOut::OutParameters> InOut_GPIO::enableOut(uint32_t pins, uint32_t mask) {
	enableOutBlocking(pins, mask);
	return {};
}

void InOut_GPIO::enableOutBlocking(uint32_t pins, uint32_t mask) {
	int bit = 1;
	for (const auto &config : this->configs) {
		if (mask & bit)
			gpio::setMode(config.pin, ((pins & bit) != 0) ? gpio::Mode::OUTPUT : gpio::Mode::INPUT);
		bit <<= 1;
	}
}

} // namespace coco
