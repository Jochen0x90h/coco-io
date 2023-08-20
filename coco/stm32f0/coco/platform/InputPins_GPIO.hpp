#pragma once

#include <coco/Array.hpp>
#include <coco/platform/gpio.hpp>
#include <coco/InputPins.hpp>
#include <string>


namespace coco {

/**
	Implementation of InputPins using GPIO
*/
class InputPins_GPIO : public InputPins {
public:

	struct Config {
		int pin; // port and pin index
		gpio::Pull pull;
		bool invert;
	};

	explicit InputPins_GPIO(Array<const Config> configs);
	~InputPins_GPIO() override;

	void get(uint32_t &pins) override;

protected:

	Array<const Config> configs;
};

} // namespace coco
