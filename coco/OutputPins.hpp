#pragma once

#include <cstdint>


namespace coco {

/**
	General purpose outputs
*/
class OutputPins {
public:

	virtual ~OutputPins() {};

	/**
		Set the value of an output
		@param pins bits for the pins, a set bit sets the output to high
		@param mask mask for outputs to affect
	*/
	virtual void set(uint32_t pins, uint32_t mask = 0xffffffff) = 0;
};

} // namespace coco
