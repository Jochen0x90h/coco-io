#pragma once

#include "Input.hpp"
#include "Output.hpp"


namespace coco {

/**
	General purpose inputs and outputs
*/
class InOut : public Input, public Output {
public:

	~InOut() override;

	/**
		Enable the output driver
		@param pins bits for the pins, a set bit enables the output
		@param mask mask for outputs to affect
		@return use co_await on return value to await completion
	*/
	[[nodiscard]] virtual Awaitable<OutParameters> enableOut(uint32_t pins, uint32_t mask = 0xffffffff) = 0;

	//virtual void enableOutBlocking(uint32_t pins, uint32_t mask = 0xffffffff) = 0;
};

} // namespace coco
