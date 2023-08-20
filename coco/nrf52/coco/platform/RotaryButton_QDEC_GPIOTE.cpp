#include "RotaryButton_QDEC_GPIOTE.hpp"
#include <coco/platform/platform.hpp>
#include <coco/platform/nvic.hpp>
#include <coco/debug.hpp>


namespace coco {

RotaryButton_QDEC_GPIOTE::RotaryButton_QDEC_GPIOTE(Loop_RTC0 &loop, int aPin, int bPin, int buttonPin, int gpioteIndex,
	bool buttonInvert, gpio::Pull pull)
	: loop(loop), buttonPin(buttonPin), gpioteIndex(gpioteIndex), buttonInvert(buttonInvert)
	, callback(makeCallback<RotaryButton_QDEC_GPIOTE, &RotaryButton_QDEC_GPIOTE::handleButton>(this))
{
	configureInput(aPin, pull);
	configureInput(bPin, pull);
	configureInput(buttonPin, pull);

	// configure quadrature decoder
	//NRF_QDEC->SHORTS = N(QDEC_SHORTS_REPORTRDY_RDCLRACC, Enabled); // clear accumulator register on report
	NRF_QDEC->INTENSET = N(QDEC_INTENSET_REPORTRDY, Set);
	NRF_QDEC->SAMPLEPER = N(QDEC_SAMPLEPER_SAMPLEPER, 1024us);
	NRF_QDEC->REPORTPER = N(QDEC_REPORTPER_REPORTPER, 10Smpl);
	NRF_QDEC->PSEL.A = aPin;
	NRF_QDEC->PSEL.B = bPin;
	NRF_QDEC->DBFEN = N(QDEC_DBFEN_DBFEN, Enabled); // enable debounce filter
	NRF_QDEC->ENABLE = N(QDEC_ENABLE_ENABLE, Enabled);
	NRF_QDEC->TASKS_START = TRIGGER;

	// configure event
	NRF_GPIOTE->CONFIG[gpioteIndex] = N(GPIOTE_CONFIG_MODE, Event)
		| N(GPIOTE_CONFIG_POLARITY, Toggle)
		| V(GPIOTE_CONFIG_PSEL, buttonPin);
	NRF_GPIOTE->EVENTS_IN[gpioteIndex] = 0; // clear pending event

	// enable GPIOTE interrupts
	NRF_GPIOTE->INTENSET = 1 << gpioteIndex;

	// add to list of handlers
	loop.handlers.add(*this);
}

RotaryButton_QDEC_GPIOTE::~RotaryButton_QDEC_GPIOTE() {
}

void RotaryButton_QDEC_GPIOTE::getState(const Array<int8_t> &state) {
	for (int i = 0; i < std::min(state.size(), int(std::size(this->state))); ++i)
		state[i] = this->state[i];
}

Awaitable<> RotaryButton_QDEC_GPIOTE::stateChange() {
	return {this->changeTasks};
}

void RotaryButton_QDEC_GPIOTE::handle() {
	if (NRF_QDEC->EVENTS_REPORTRDY) {
		// clear pending interrupt flags at peripheral and NVIC
		NRF_QDEC->EVENTS_REPORTRDY = 0;
		nvic::clear(QDEC_IRQn);

		int8_t value = NRF_QDEC->ACC >> 2;

		if (value != this->state[0]) {
			this->state[0] = value;
			this->changeTasks.doAll();
		}
	}

	if (nvic::pending(GPIOTE_IRQn)) {
		if (NRF_GPIOTE->EVENTS_IN[this->gpioteIndex]) {
			// clear pending interrupt flags at peripheral
			NRF_GPIOTE->EVENTS_IN[this->gpioteIndex] = 0;

			this->loop.invoke(this->callback, 50ms);
		}
	}
}

void RotaryButton_QDEC_GPIOTE::handleButton() {
	bool value = gpio::getInput(this->buttonPin) != this->buttonInvert;

	if (value && !this->buttonValue) {
		++this->state[1];
		this->changeTasks.doAll();
	}
	this->buttonValue = value;
}

} // namespace coco
