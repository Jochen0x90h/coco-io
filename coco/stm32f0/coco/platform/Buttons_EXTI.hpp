#pragma once

#include <coco/Input.hpp>
#include <coco/Array.hpp>
#include <coco/platform/Loop_TIM2.hpp>
#include <coco/platform/gpio.hpp>


namespace coco {

/**
	Implementation of up to 8 manually operated buttons with debounce filter EXTI.
	Can be used e.g. for a d-pad.

	Reference manual:
		f0:
			https://www.st.com/resource/en/reference_manual/dm00031936-stm32f0x1stm32f0x2stm32f0x8-advanced-armbased-32bit-mcus-stmicroelectronics.pdf
				EXTI: Section 11.2
		g4:
			https://www.st.com/resource/en/reference_manual/rm0440-stm32g4-series-advanced-armbased-32bit-mcus-stmicroelectronics.pdf
				EXTI: Section 15

	Resources:
		GPIO
			EXTI
*/
class Buttons_EXTI : public Input, public Loop_TIM2::Handler {
public:

	struct Config {
		// port and pin index
		int pin;

		// pull up/down resistor
		gpio::Pull pull;

		// invert input
		bool invert;

		// state index to modify
		uint8_t index;

		// change to apply (e.g. 1 for up and -1 for down)
		int8_t delta;
	};

	explicit Buttons_EXTI(Loop_TIM2 &loop, Array<const Config> configs);
	~Buttons_EXTI() override;

	void getState(const Array<int8_t> &state) override;
	[[nodiscard]] Awaitable<> stateChange() override;

protected:
	void handle() override;
	void handleButtons();

	Loop_TIM2 &loop;
	Array<const Config> configs;
	TimedTask<Callback> callback;

	// mask of used EXTI's
	int mask;

	// change indicator flags
	int flags = 0;
	int flags2 = 0;

	bool values[8] = {};
	int8_t state[8] = {};

	CoroutineTaskList<> changeTasks;
};

} // namespace coco
