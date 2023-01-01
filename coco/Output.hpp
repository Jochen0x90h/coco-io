#pragma once

#include <coco/Coroutine.hpp>
#include <cstdint>


namespace coco {

/**
 * General purpose outputs
 */
class Output {
public:

	// Internal helper: Stores the parameters in the awaitable during co_await
	struct OutParameters {
		uint32_t pins;
		uint32_t mask;
	};


	virtual ~Output();

	/**
	 * Read an input directly without debounce filter
	 * @param pins bits for the pins, a set bit sets the output to high
	 * @param mask mask for outputs to affect
	 * @return use co_await on return value to await completion
	 */
	[[nodiscard]] virtual Awaitable<OutParameters> set(uint32_t pins, uint32_t mask = 0xffffffff) = 0;

	/**
	 * Read an input directly without debounce filter
	 * @param pins bits for the pins, a set bit sets the output to high
	 * @param mask mask for outputs to affect
	 */
	virtual void setBlocking(uint32_t pins, uint32_t mask = 0xffffffff) = 0;
};

} // namespace coco
