#pragma once

#include <coco/Input.hpp>
#include <coco/platform/Loop_RTC0.hpp>
#include <coco/platform/gpio.hpp>


/**
	Interface to a rotary button using the quadrature decoder
	https://infocenter.nordicsemi.com/index.jsp?topic=%2Fps_nrf52840%2Fqdec.html&cp=5_0_0_5_17

	Dependencies:

	Resources:
		NRF_QDEC
		NRF_GPIOTE
 			EVENTS_IN[gpioteIndex]
*/
namespace coco {

class RotaryButton_QDEC_GPIOTE : public Input, public Loop_RTC0::Handler {
public:
	/**
		Constructor
		@param aPin pin of input A of quadrature decoder
		@param bPin pin of input B of quadrature decoder
		@param buttonPin pin of the button
	*/
	RotaryButton_QDEC_GPIOTE(Loop_RTC0 &loop, int aPin, int bPin, int buttonPin, int gpioteIndex, bool buttonInvert = true, gpio::Pull pull = gpio::Pull::UP);
	~RotaryButton_QDEC_GPIOTE();

	void getState(const Array<int8_t> &state) override;
	[[nodiscard]] Awaitable<> stateChange() override;

protected:
	void handle() override;
	void handleButton();

	Loop_RTC0 &loop;
	int buttonPin;
	int gpioteIndex;
	bool buttonInvert;
	TimedTask<Callback> callback;

	bool buttonValue = false;
	int8_t state[2] = {};

	CoroutineTaskList<> changeTasks;
};

} // namespace coco
