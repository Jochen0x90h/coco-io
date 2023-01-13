#pragma once

#include <coco/InOut.hpp>
#include <string>


namespace coco {

/**
 * Implementation of an InOut interface that simply writes info about the in/out operations to std::cout
 */
class Input_cout : public Input {
public:

	explicit Input_cout(std::string name) : name(std::move(name)) {}
	~Input_cout() override;

	[[nodiscard]] Awaitable<InParameters> get(uint32_t &pins) override;
	void getBlocking(uint32_t &pins) override;

protected:

	std::string name;
};

} // namespace coco
