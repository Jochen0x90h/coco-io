#pragma once

#include <coco/Array.hpp>
#include <coco/Trigger.hpp>
#include <coco/platform/Handler.hpp>
#include <coco/platform/gpio.hpp>


namespace coco {

/**
 * Implementation of an input trigger with debounce filter using GPIOTE and RTC0
 * 
 * Resources:
 *	GPIO
 *	NRF_GPIOTE
 * 		IN[0 - N-1]
 *	NRF_RTC0
 *		CC[1]
 */
class Trigger_GPIOTE_RTC0 : public Trigger, public Handler {
public:

	struct Config {
		int pin; // port and pin index
		gpio::Pull pull;
		bool invert;
	};

	explicit Trigger_GPIOTE_RTC0(Array<const Config> configs);
	~Trigger_GPIOTE_RTC0() override;

	[[nodiscard]] Awaitable<Parameters> trigger(uint32_t &risingFlags, uint32_t &fallingFlags) override;

	void handle() override;

protected:

	Array<const Config> configs;

	// next timeout of an input
	int32_t next;

	// states for debounce filter
	struct State {	
		int32_t timeout;
		bool value;
	};
	State states[8];

	// waiting coroutines
	Waitlist<Parameters> waitlist;
};

} // namespace coco
