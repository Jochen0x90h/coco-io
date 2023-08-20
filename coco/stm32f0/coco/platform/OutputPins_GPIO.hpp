#pragma once

#include <coco/Array.hpp>
#include <coco/platform/gpio.hpp>
#include <coco/OutputPins.hpp>
#include <string>


namespace coco {

/**
	Implementation of OutputPins using GPIO
*/
class OutputPins_GPIO : public OutputPins {
public:

	struct Config {
		// port and pin index
		int pin;

		// initial value (without invert)
		bool initialValue;

		// output speed
		gpio::Speed speed;

		// drive both/down
		gpio::Drive drive;

		// pull up/down resistor (without invert)
		gpio::Pull pull;

		// invert output value
		bool invert;
	};

	explicit OutputPins_GPIO(Array<const Config> configs);
	~OutputPins_GPIO() override;

	void set(uint32_t pins, uint32_t mask = 0xffffffff) override;

protected:

	Array<const Config> configs;
};

} // namespace coco
