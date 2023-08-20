#pragma once

#include <coco/OutputPins.hpp>
#include <string>


namespace coco {

/**
	Implementation of OutputPins interface that simply writes info about the out operations to std::cout
*/
class OutputPins_cout : public OutputPins {
public:

	explicit OutputPins_cout(std::string name) : name(std::move(name)) {}
	~OutputPins_cout() override;

	void set(uint32_t pins, uint32_t mask = 0xffffffff) override;

protected:

	std::string name;
	int32_t pins = 0;
};

} // namespace coco
