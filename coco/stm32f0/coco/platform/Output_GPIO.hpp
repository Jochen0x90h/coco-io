#pragma once

#include <coco/Array.hpp>
#include <coco/platform/gpio.hpp>
#include <coco/Output.hpp>
#include <string>


namespace coco {

/**
 * Implementation of an Output interface using GPIO
 */
class Output_GPIO : public Output {
public:

	struct Config {
		int pin; // port and pin index
		gpio::Pull pull;
		gpio::Drive drive;
		bool invert;
		bool initialValue;
	};


	explicit Output_GPIO(Array<const Config> configs);
	~Output_GPIO() override;

	[[nodiscard]] Awaitable<OutParameters> set(uint32_t pins, uint32_t mask = 0xffffffff) override;
	void setBlocking(uint32_t pins, uint32_t mask = 0xffffffff) override;

protected:

	Array<const Config> configs;
};

} // namespace coco
