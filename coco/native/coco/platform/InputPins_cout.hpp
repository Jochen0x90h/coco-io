#pragma once

#include <coco/InputPins.hpp>
#include <string>


namespace coco {

/**
	Implementation of InOutPins that simply writes info about the in/out operations to std::cout
 */
class InputPins_cout : public InputPins {
public:

	explicit InputPins_cout(std::string name) : name(std::move(name)) {}
	~InputPins_cout() override;

	void get(uint32_t &pins) override;

protected:

	std::string name;
};

} // namespace coco
