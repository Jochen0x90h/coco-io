#pragma once

#include <coco/Trigger.hpp>
#include <string>


namespace coco {

/**
 * Dummy Implementation of an input trigger with debounce filter
 */
class Trigger_cout : public Trigger {
public:

	explicit Trigger_cout(std::string name) : name(std::move(name)) {}
	~Trigger_cout() override;

	[[nodiscard]] Awaitable<Parameters> trigger(uint32_t &risingFlags, uint32_t &fallingFlags) override;

protected:

	std::string name;
};

} // namespace coco
