#pragma once

#include <coco/Array.hpp>
#include <coco/platform/gpio.hpp>
#include <coco/InOutPins.hpp>


namespace coco {

/**
	Implementation of InOutPins using GPIO
*/
class InOutPins_GPIO : public InOutPins {
public:

	struct Config {
		int pin; // port and pin index
		gpio::Pull pull;
		gpio::Drive drive;
		bool invert;
	};

	explicit InOutPins_GPIO(Array<const Config> configs);
	~InOutPins_GPIO() override;

	void get(uint32_t &pins) override;
	void set(uint32_t pins, uint32_t mask = 0xffffffff) override;
	void enableOut(uint32_t pins, uint32_t mask = 0xffffffff) override;

protected:

	Array<const Config> configs;
};

} // namespace coco
