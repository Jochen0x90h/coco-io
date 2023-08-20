#pragma once

#include <cstdint>


namespace coco {

/**
	General purpose inputs
*/
class InputPins {
public:
	virtual ~InputPins() {}

	/**
		Read the current value of an input
		@param pins bits for the pins
	*/
	virtual void get(uint32_t &pins) = 0;
};

} // namespace coco
