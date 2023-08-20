#include "Buttons_EXTI.hpp"
#include <coco/platform/platform.hpp>
#include <coco/platform/nvic.hpp>
#include <coco/debug.hpp>


namespace coco {

Buttons_EXTI::Buttons_EXTI(Loop_TIM2 &loop, Array<const Config> configs)
	: loop(loop), configs(configs)
	, callback(makeCallback<Buttons_EXTI, &Buttons_EXTI::handleButtons>(this))
{
    assert(configs.size() <= 8);
    RCC->APB2ENR =  RCC->APB2ENR | RCC_APB2ENR_SYSCFGEN;
	int mask = 0;
	for (int index = 0; index < configs.size(); ++index) {
		auto &config = configs[index];
		assert(config.index < std::size(this->state));

		// configure as input
		gpio::configureInput(config.pin, config.pull);

		// configure event
		// see chapter 11.2 in reference manual and A.6.2 for code example
		int port = config.pin >> 4;
		int pin = config.pin & 15;
		auto &EXTICR = SYSCFG->EXTICR[pin >> 2];
		EXTICR = EXTICR | (port << (pin & 3) * 4);
		mask |= 1 << pin;
	}
#ifdef STM32F0
	EXTI->FTSR = EXTI->FTSR | mask; // detect falling edge
	EXTI->RTSR = EXTI->RTSR | mask; // detect rising edge
	EXTI->PR = mask; // clear pending interrupt
	EXTI->IMR = EXTI->IMR | mask; // enable interrupt
#endif
#ifdef STM32G4
	EXTI->FTSR1 = EXTI->FTSR1 | mask; // detect falling edge
	EXTI->RTSR1 = EXTI->RTSR1 | mask; // detect rising edge
	EXTI->PR1 = mask; // clear pending interrupt
	EXTI->IMR1 = EXTI->IMR1 | mask; // enable interrupt
#endif
	this->mask = mask;

	// add to list of handlers
	loop.handlers.add(*this);
}

Buttons_EXTI::~Buttons_EXTI() {
}

void Buttons_EXTI::getState(const Array<int8_t> &state) {
	for (int i = 0; i < std::min(state.size(), int(std::size(this->state))); ++i)
		state[i] = this->state[i];
}

[[nodiscard]] Awaitable<> Buttons_EXTI::stateChange() {
	return {this->changeTasks};
}

void Buttons_EXTI::handle() {
#ifdef STM32F0
	int PR = EXTI->PR & this->mask;
#endif
#ifdef STM32G4
	int PR = EXTI->PR1 & this->mask;
#endif
	if (PR != 0) {
		int flags = 0;
		for (int index = 0; index < this->configs.size(); ++index) {
			auto &config = this->configs[index];
			int pin = config.pin & 15;
			if (PR & (1 << pin)) {
				flags |= 1 << index;
			}
		}
		if (flags) {
			if (!this->callback.inList()) {
				this->flags = flags;
				this->loop.invoke(this->callback, 50ms);
			} else {
				this->flags &= ~flags;
				this->flags2 |= flags;
			}
		}

		// clear pending interrupt flags at peripheral and NVIC
#ifdef STM32F0
		EXTI->PR = PR;
		nvic::clear(EXTI0_1_IRQn);
		nvic::clear(EXTI2_3_IRQn);
		nvic::clear(EXTI4_15_IRQn);
#endif
#ifdef STM32G4
		EXTI->PR1 = PR;
		nvic::clear(EXTI0_IRQn);
		nvic::clear(EXTI1_IRQn);
		nvic::clear(EXTI2_IRQn);
		nvic::clear(EXTI3_IRQn);
		nvic::clear(EXTI4_IRQn);
		nvic::clear(EXTI9_5_IRQn);
		nvic::clear(EXTI15_10_IRQn);
#endif
	}
}

void Buttons_EXTI::handleButtons() {
	int flags = this->flags;
	int8_t changed = 0;
	for (int index = 0; index < this->configs.size(); ++index) {
		auto &config = this->configs[index];
		if (flags & (1 << index)) {
			bool value = gpio::getInput(config.pin) != config.invert;
			if (value && !this->values[index]) {
				this->state[config.index] += config.delta;
				changed |= config.delta;
			}
			this->values[index] = value;
		}
	}
	if (changed)
		this->changeTasks.doAll();

	int flags2 = this->flags2;
	if (flags2 != 0) {
		this->flags = flags2;
		this->flags2 = 0;
		this->loop.invoke(this->callback, 50ms);
	}
}

} // namespace coco
