#pragma once

#include <coco/Array.hpp>
#include <coco/platform/gpio.hpp>
#include <coco/InOut.hpp>


namespace coco {

/**
 * Implementation of the InOut interface using GPIO
 */
class InOut_GPIO : public InOut {
public:

	struct Config {
		int pin; // port and pin index
		gpio::Mode mode;
		gpio::Pull pull;
		gpio::Drive drive;
		bool invert;
		bool initialValue;
	};

	explicit InOut_GPIO(Array<const Config> configs);
	~InOut_GPIO() override;

	[[nodiscard]] Awaitable<InParameters> get(uint32_t &pins) override;
	void getBlocking(uint32_t &pins) override;

	[[nodiscard]] Awaitable<OutParameters> set(uint32_t pins, uint32_t mask = 0xffffffff) override;
	void setBlocking(uint32_t pins, uint32_t mask = 0xffffffff) override;

	[[nodiscard]] Awaitable<OutParameters> enableOut(uint32_t pins, uint32_t mask = 0xffffffff) override;
	void enableOutBlocking(uint32_t pins, uint32_t mask = 0xffffffff) override;

protected:

	Array<const Config> configs;
};

} // namespace coco
