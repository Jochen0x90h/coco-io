#pragma once

#include "OutputPins.hpp"


namespace coco {

/**
	Implementation of OutputPins that does nothing
*/
class OutputPins_nop : public OutputPins {
public:

	virtual ~OutputPins_nop();

	virtual void set(uint32_t pins, uint32_t mask = 0xffffffff) override;
};

} // namespace coco
