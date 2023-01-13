#pragma once

#include <coco/InOut.hpp>
#include <string>


namespace coco {

/**
 * Implementation of an InOut interface that simply writes info about the in/out operations to std::cout
 */
class InOut_cout : public InOut {
public:

	explicit InOut_cout(std::string name) : name(std::move(name)) {}
	~InOut_cout() override;

	[[nodiscard]] Awaitable<InParameters> get(uint32_t &pins) override;
	void getBlocking(uint32_t &pins) override;

	[[nodiscard]] Awaitable<OutParameters> set(uint32_t pins, uint32_t mask = 0xffffffff) override;
	void setBlocking(uint32_t pins, uint32_t mask = 0xffffffff) override;

	[[nodiscard]] Awaitable<OutParameters> enableOut(uint32_t pins, uint32_t mask = 0xffffffff) override;
	void enableOutBlocking(uint32_t pins, uint32_t mask = 0xffffffff) override;

protected:

	std::string name;
	int32_t pins = 0;
};

} // namespace coco
