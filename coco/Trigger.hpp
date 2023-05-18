#pragma once

#include <coco/Coroutine.hpp>
#include <cstdint>


namespace coco {

/**
	Input trigger that can be used to wait for state changes on IO pins.
	Implementations may have a debounce filter for mechanical switches connected to IO pins.
*/
class Trigger {
public:

	// Internal helper: Stores the parameters in the awaitable during co_await
	struct Parameters {
		uint32_t *risingFlags;
		uint32_t *fallingFlags;
	};

	virtual ~Trigger();

	/**
		Wait for a trigger event on multiple inputs. When a trigger occurs all coroutines are resumed that wait for it.
		For example to wait on rising edge on input 0 and falling edge on input 2:
		int risingFlags = 1 << 0; int fallingFlags = 1 << 2; co_await trigger(risingFlags, fallingFlags);
		@param risingFlags in: for each input a flag indicateing that we want to wait for a rising edge, out: flags indicating if rising edge has been detected
		@param fallingFlags out: for each input a flag indicating that we want to wait for a falling edge, out: flags indicating if falling edge has been detected
		@return use co_await on return value to wait for a trigger
	*/
	[[nodiscard]] virtual Awaitable<Parameters> trigger(uint32_t &risingFlags, uint32_t &fallingFlags) = 0;
};

} // namespace coco
