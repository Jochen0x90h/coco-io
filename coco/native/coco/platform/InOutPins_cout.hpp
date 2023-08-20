#pragma once

#include <coco/InOutPins.hpp>
#include <string>


namespace coco {

/**
	Implementation of InOutPins that simply writes info about the in/out operations to std::cout
*/
class InOutPins_cout : public InOutPins {
public:

	explicit InOutPins_cout(std::string name) : name(std::move(name)) {}
	~InOutPins_cout() override;

	void get(uint32_t &pins) override;
	void set(uint32_t pins, uint32_t mask = 0xffffffff) override;
	void enableOut(uint32_t pins, uint32_t mask = 0xffffffff) override;

protected:

	std::string name;
	int32_t pins = 0;
};

} // namespace coco
