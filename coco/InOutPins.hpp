#pragma once

#include "InputPins.hpp"
#include "OutputPins.hpp"


namespace coco {

/**
	General purpose inputs and outputs with output enable. The output drivers are disabled by default and the output
	value is undefined
*/
class InOutPins : public InputPins, public OutputPins {
public:

	~InOutPins() override {}

	/**
		Enable the output driver
		@param pins bits for the pins, a set bit enables the output
		@param mask mask for outputs to affect
	*/
	virtual void enableOut(uint32_t pins, uint32_t mask = 0xffffffff) = 0;
};

} // namespace coco
