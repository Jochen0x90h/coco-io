#include "Trigger_GPIOTE_RTC0.hpp"
#include <coco/platform/platform.hpp>


namespace coco {

Trigger_GPIOTE_RTC0::Trigger_GPIOTE_RTC0(Array<const Config> configs) : configs(configs) {
    assert(configs.size() <= 8);
    for (int index = 0; index < configs.size(); ++index) {
		auto &config = configs[index];

		// configure as input
		gpio::configureInput(config.pin, config.pull);
	
		// configure event
		NRF_GPIOTE->CONFIG[index] = N(GPIOTE_CONFIG_MODE, Event)
			| N(GPIOTE_CONFIG_POLARITY, Toggle)
			| V(GPIOTE_CONFIG_PSEL, config.pin);	
		NRF_GPIOTE->EVENTS_IN[index] = 0; // clear pending event
				
		// initialize state
		auto &state = states[index];
		state.timeout = 0x7fffff;
		state.value = gpio::getInput(config.pin) != config.invert;	
	}

	// enable GPIOTE interrupts
	NRF_GPIOTE->INTENSET = ~(0xffffffff << configs.size());

	// use channel 1 of RTC0 (initialized by SystemInit())
	this->next = 0x7fffff;
	NRF_RTC0->CC[1] = this->next;
	NRF_RTC0->INTENSET = N(RTC_INTENSET_COMPARE1, Set);

	// add to list of handlers
	coco::handlers.add(*this);
}

Trigger_GPIOTE_RTC0::~Trigger_GPIOTE_RTC0() {
}

Awaitable<Trigger::Parameters> Trigger_GPIOTE_RTC0::trigger(uint32_t &risingFlags, uint32_t &fallingFlags) {
    return {this->waitlist, &risingFlags, &fallingFlags};
}

void Trigger_GPIOTE_RTC0::handle() {
    if (isInterruptPending(GPIOTE_IRQn)) {
		// debounce timeout after about 50ms
		int32_t timeout = NRF_RTC0->COUNTER + 50 * 16384 / 1000;
		for (int index = 0; index < this->configs.size(); ++index) {
			if (NRF_GPIOTE->EVENTS_IN[index]) {
				// clear pending interrupt flags at peripheral
				NRF_GPIOTE->EVENTS_IN[index] = 0;

				auto &state = states[index];			

				// set debounce timeout
				state.timeout = timeout;
				
				// check if this is the next timeout (counter is only 24 bit)
				if (((timeout - this->next) << 8) < 0) {
					this->next = timeout;
					NRF_RTC0->CC[1] = timeout;
				}
			}		
		}

		// clear pending interrupt at NVIC
		clearInterrupt(GPIOTE_IRQn);
	}
	if (NRF_RTC0->EVENTS_COMPARE[1]) {
		do {
			// clear pending interrupt flags at peripheral and NVIC
			NRF_RTC0->EVENTS_COMPARE[1] = 0;
			clearInterrupt(RTC0_IRQn);

			// get current time
			int time = this->next;
			
			// add RTC interval (is 24 bit)
			this->next += 0x7fffff;
			
			for (int index = 0; index < this->configs.size(); ++index) {
				auto &input = this->configs[index];
				auto &state = states[index];
				
				// check if debounce timeout for this input elapsed
				if (state.timeout == time) {
					state.timeout += 0x7fffff;

					// read input value
					bool value = gpio::getInput(input.pin) != input.invert;
					bool old = state.value;
					state.value = value;

					uint32_t risingFlag = int(value && !old) << index;
					uint32_t fallingFlag = int(!value && old) << index;					

					// resume all coroutines that wait for an event on this input
					this->waitlist.resumeAll([risingFlag, fallingFlag] (Parameters &p) {
						if ((*p.risingFlags & risingFlag) != 0 || (*p.fallingFlags & fallingFlag) != 0) {
							*p.risingFlags = risingFlag;
							*p.fallingFlags = fallingFlag;
							return true;
						}
						return false;
					});
				} else if (state.timeout < this->next) {
					// this input is next
					this->next = state.timeout;
				}
			}
			NRF_RTC0->CC[1] = this->next;
		
			// repeat until next timeout is in the future
		} while (((int32_t(NRF_RTC0->COUNTER) - this->next) << 8) >= 0);
	}
}

} // namespace coco
