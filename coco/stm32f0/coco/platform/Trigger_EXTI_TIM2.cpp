#include "Trigger_EXTI_TIM2.hpp"
#include <coco/platform/platform.hpp>


namespace coco {

Trigger_EXTI_TIM2::Trigger_EXTI_TIM2(Loop_TIM2 &loop, Array<const Config> configs) : configs(configs) {
    assert(configs.size() <= 8);
    for (int index = 0; index < configs.size(); ++index) {
		auto &config = configs[index];

		// configure as input
		gpio::configureInput(config.pin, config.pull);

		// configure event
		// see chapter 11.2 in reference manual and A.6.2 for code example
		int port = config.pin >> 4;
		int pin = config.pin & 15;
		auto &EXTICR = SYSCFG->EXTICR[pin >> 2];
		EXTICR = EXTICR | (port << (pin & 3));
		EXTI->FTSR = EXTI->FTSR | 1 << pin; // detect falling edge
		EXTI->RTSR = EXTI->RTSR | 1 << pin; // detect rising edge
		EXTI->PR = 1 << pin; // clear pending interrupt			
		EXTI->IMR = EXTI->IMR | 1 << pin; // enable interrupt
			
		// initialize state
		auto &state = states[index];
		state.timeout = 0x7fffffff;
		state.value = gpio::getInput(config.pin) != config.invert;		
	}

	// use channel 2 of TIM2 (initialized by SystemInit())
	this->next = 0x7fffffff;
	TIM2->CCR2 = this->next;
	TIM2->DIER = TIM2->DIER | TIM_DIER_CC2IE; // interrupt enable

	// add to list of handlers
	loop.handlers.add(*this);
}

Trigger_EXTI_TIM2::~Trigger_EXTI_TIM2() {
}

Awaitable<Trigger::Parameters> Trigger_EXTI_TIM2::trigger(uint32_t &risingFlags, uint32_t &fallingFlags) {
    return {this->waitlist, &risingFlags, &fallingFlags};
}

void Trigger_EXTI_TIM2::handle() {
	int PR = EXTI->PR;
	if ((PR & 0xffff) != 0) {
		// debounce timeout after about 50ms
		int32_t timeout = TIM2->CNT + 50;
		for (int index = 0; index < this->configs.size(); ++index) {
			auto &config = this->configs[index];
			int pos = config.pin & 15;
			if (PR & (1 << pos)) {
				auto &state = this->states[index];			

				// set debounce timeout
				state.timeout = timeout;
				
				// check if this is the next timeout
				if ((timeout - this->next) < 0) {
					this->next = timeout;
					TIM2->CCR2 = timeout;
				}
			}		
		}

		// clear pending interrupt flags at peripheral and NVIC
		EXTI->PR = 0xffff;
		clearInterrupt(EXTI0_1_IRQn);
		clearInterrupt(EXTI2_3_IRQn);
		clearInterrupt(EXTI4_15_IRQn);
	}
	if (TIM2->SR & TIM_SR_CC2IF) {
		do {
			// clear pending interrupt flags at peripheral and NVIC
			TIM2->SR = ~TIM_SR_CC2IF;
			clearInterrupt(TIM2_IRQn);

			// get current time
			auto time = this->next;
			
			// add RTC interval
			this->next += 0x7fffffff;
			
			for (int index = 0; index < this->configs.size(); ++index) {
				auto &config = this->configs[index];
				auto &state = this->states[index];
				
				// check if debounce timeout for this input elapsed
				if (state.timeout == time) {
					state.timeout += 0x7fffffff;

					// read input value
					bool value = gpio::getInput(config.pin) != config.invert;
					bool old = state.value;
					state.value = value;

					uint32_t risingFlag = int(value && !old) << index;
					uint32_t fallingFlag = int(!value && old) << index;					

					// resume coroutines that wait for an event on this input
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
			TIM2->CCR2 = this->next;
		
			// repeat until next timeout is in the future
		} while (int32_t(TIM2->CNT) - this->next >= 0);
	}
}

} // namespace coco
