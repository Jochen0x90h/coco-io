#pragma once

#include <coco/Output.hpp>
#include <string>


namespace coco {

/**
 * Implementation of the Output interface that simply writes info about the out operations to std::cout
 */
class Output_cout : public Output {
public:

	explicit Output_cout(std::string name) : name(std::move(name)) {}
	~Output_cout() override;

	[[nodiscard]] Awaitable<OutParameters> set(uint32_t pins, uint32_t mask = 0xffffffff) override;
	void setBlocking(uint32_t pins, uint32_t mask = 0xffffffff) override;

protected:

	std::string name;
	int32_t pins = 0;
};

} // namespace coco
