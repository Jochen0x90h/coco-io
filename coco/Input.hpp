#pragma once

#include <coco/Coroutine.hpp>
#include <cstdint>


namespace coco {

/**
 * General purpose inputs
 */
class Input {
public:

	// Internal helper: Stores the parameters in the awaitable during co_await
	struct InParameters {
		uint32_t *pins;
	};


	virtual ~Input();

	/**
	 * Read an input directly without debounce filter
	 * @param pins bits for the pins
	 * @return use co_await on return value to await completion
	 */
	[[nodiscard]] virtual Awaitable<InParameters> get(uint32_t &pins) = 0;

	/**
	 * Read an input directly without debounce filter
	 * @param pins bits for the pins
	 */
	virtual void getBlocking(uint32_t &pins) = 0;
};

} // namespace coco
