#pragma once

#include <coco/Array.hpp>
#include <coco/platform/gpio.hpp>
#include <coco/Input.hpp>
#include <string>


namespace coco {

/**
 * Implementation of an Input interface using GPIO
 */
class Input_GPIO : public InOut {
public:

	struct Config {
		int pin; // port and pin index
		gpio::Pull pull;
		bool invert;
	};


	explicit Input_GPIO(Array<const Config> configs);
	~Input_GPIO() override;

	[[nodiscard]] Awaitable<InParameters> get(uint32_t &pins) override;
	void getBlocking(uint32_t &pins) override;

protected:

	Array<const Config> configs;
};

} // namespace coco
