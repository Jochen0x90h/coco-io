#include "InputDevice_EXTI_TIM.hpp"
#include <coco/platform/platform.hpp>
#include <coco/platform/exti.hpp>
#include <coco/platform/nvic.hpp>
#include <coco/debug.hpp>


namespace coco {

InputDevice_EXTI_TIM::InputDevice_EXTI_TIM(Loop_Queue &loop, Array<const gpio::Config> pinConfigs, Array<const Config> configs,
    const timer::Info1 &timerInfo, int prescaler, int ccIndex)
    : InputDevice(State::READY)
    , loop(loop), pinConfigs(pinConfigs), configs(configs)
    , timer(timerInfo.timer), timerIrq(timerInfo.irq), ccIndex(ccIndex)
{
    assert(types.size() <= std::size(this->counters));
    assert(configs.size() <= std::size(this->cc));

    // enable clocks (note two cycles wait time until peripherals can be accessed, see STM32G4 reference manual section 7.2.17)
    // clock of SYSCFG gets enabled in SystemInit()
    timerInfo.rcc.enableClock();

    // configure input pins
    int flags = 0;
    for (int i = 0; i < pinConfigs.size(); ++i) {
        auto &config = pinConfigs[i];

        // configure input pin
        gpio::configureInput(config);

        // configure EXTI port
        int port = extract(config, gpio::Config::PORT_MASK);
        int pin = extract(config, gpio::Config::PIN_MASK);
#ifdef EXTI_EXTICR1_EXTI0
        // C0
        int pos8 = (pin & 3) * 8;
        auto &EXTICR = EXTI->EXTICR[pin >> 2];
        EXTICR = (EXTICR & ~(0xff << pos8)) | (port << pos8);
#else
        // F0, F3, F4, G4
        int pos4 = (pin & 3) * 4;
        auto &EXTICR = SYSCFG->EXTICR[pin >> 2];
        EXTICR = (EXTICR & ~(0xf << pos4)) | (port << pos4);
#endif
        flags |= 1 << pin;

        // enable EXTI interrupt
        int irq = exti::irqs[pin];
        nvic::setPriority(irq, nvic::Priority::LOW);
        nvic::enable(irq);
    }
    this->extiFlags = flags;

    // initialize states and counters
    for (int i = 0; i < configs.size(); ++i) {
        auto &config = configs[i];

        // get current state from one or two inputs
        int state = int(gpio::getInput(pinConfigs[config.inputIndex]));
        if (((int(config.risingAction) | int(config.fallingAction)) & 16) != 0)
            state |= int(gpio::getInput(pinConfigs[config.inputIndex + 1])) << 1;
        this->states[i] = state;

        // set timeout as invalid
        this->timeouts[i] = -1;

        // init counter
        bool startWithOne = (state != 0 && config.risingAction == Action::SET_LSB)
            || (state == 0 && config.fallingAction == Action::SET_LSB);
        this->counters[config.counterIndex] = int(startWithOne);
    }

    // initialize timer to 4kHz
    auto timer = timerInfo.timer;
    timer->PSC = prescaler; // prescaler for 1ms timer resolution
    timer->EGR = TIM_EGR_UG; // update generation so that prescaler takes effect
    timer->DIER = timer->DIER | (TIM_DIER_CC1IE << ccIndex); // interrupt enable for CC1
    timer->CR1 = TIM_CR1_CEN; // enable, count up

    // enable timer interrupt
    nvic::setPriority(timerInfo.irq, nvic::Priority::LOW);
    nvic::enable(timerInfo.irq);

    // enable EXTI interrupts
#ifdef EXTI_FTSR_FT0
    EXTI->FTSR = EXTI->FTSR | flags; // detect falling edge
    EXTI->RTSR = EXTI->RTSR | flags; // detect rising edge
    EXTI->PR = flags; // clear pending interrupt
    EXTI->IMR = EXTI->IMR | flags; // enable interrupt
#endif
#ifdef EXTI_FTSR1_FT0
    EXTI->FTSR1 = EXTI->FTSR1 | flags; // detect falling edge
    EXTI->RTSR1 = EXTI->RTSR1 | flags; // detect rising edge
#ifdef EXTI_RPR1_RPIF0
    // separate interrupt flags for rising and falling
    EXTI->RPR1 = flags; // clear pending interrupt
    EXTI->FPR1 = flags; // clear pending interrupt
#else
    EXTI->PR1 = flags; // clear pending interrupt
#endif
    EXTI->IMR1 = EXTI->IMR1 | flags; // enable interrupt
#endif
}

InputDevice_EXTI_TIM::~InputDevice_EXTI_TIM() {
}

int InputDevice_EXTI_TIM::get(void *data, int size) { //const Array<int8_t> &counters) {
    // disable timer interrupt
    nvic::disable(this->timerIrq);

    // copy values
    uint8_t *counters = reinterpret_cast<uint8_t *>(data);
    int count = std::min(size, int(std::size(this->counters)));
    for (int i = 0; i < count; ++i)
        counters[i] = this->counters[i];
    int sequenceNumber = this->sequenceNumber;

    // enable timer interrupt
    nvic::enable(this->timerIrq);

    return sequenceNumber;
}

Awaitable<Device::Events> InputDevice_EXTI_TIM::untilInput(int sequenceNumber) {
    if (this->sequenceNumber != sequenceNumber)
        return {};
    return {this->st.tasks, Events::READABLE};
}

void InputDevice_EXTI_TIM::EXTI_IRQHandler() {
#ifdef EXTI_PR_PR0
    int inputFlags = EXTI->PR & this->extiFlags;
#endif
#ifdef EXTI_PR1_PIF0
    int inputFlags = EXTI->PR1 & this->extiFlags;
#endif
#ifdef EXTI_RPR1_RPIF0
    int inputFlags = (EXTI->RPR1 | EXTI->FPR1) & this->extiFlags;
#endif

    // check if one of "our" EXTIs was triggered
    if (inputFlags != 0) {
        // clear pending interrupt flags at peripheral
#ifdef EXTI_PR_PR0
        EXTI->PR = inputFlags;
#endif
#ifdef EXTI_PR1_PIF0
        EXTI->PR1 = inputFlags;
#endif
#ifdef EXTI_RPR1_RPIF0
        EXTI->RPR1 = inputFlags;
        EXTI->FPR1 = inputFlags;
#endif
        for (int i = 0; i < this->configs.size(); ++i) {
            auto &config = this->configs[i];

            // check if activity is detected for this config
            int inputIndex = extract(this->pinConfigs[config.inputIndex], gpio::Config::PIN_MASK);
            bool detected = ((inputFlags >> inputIndex) & 1) != 0;
            if (((int(config.risingAction) | int(config.fallingAction)) & 16) != 0) {
                inputIndex = extract(this->pinConfigs[config.inputIndex + 1], gpio::Config::PIN_MASK);
                detected |= ((inputFlags >> inputIndex) & 1) != 0;
            }

            if (detected != 0) {
                // get last state
                int last = this->states[i];

                // set timeout for the input
                auto delay = (last & 1) == 0 ? config.risingDelay : config.fallingDelay;

                int timeout = this->timer->CNT + (delay.value << 2);
                this->timeouts[i] = timeout;

                // force update (update() gets called via interrupt handler)
                this->timer->EGR = TIM_EGR_CC1G << this->ccIndex;
            }
        }
    }
}

// gets called from timer interrupt
void InputDevice_EXTI_TIM::update() {
    // check inputs and set new timeout
    int maxTimeout;
    int nextTimeout;
    bool needNotify = false;
    do {
        int now = this->timer->CNT;
        nextTimeout = maxTimeout = now + 0x7fff; // 16 bit counter

        // check if an input changed state
        for (int i = 0; i < this->configs.size(); ++i) {
            auto &config = this->configs[i];

            // get timeout value
            int timeout = this->timeouts[i];

            // check if time elapsed
            if (timeout != -1) {
                if (((timeout - 1 - now) << 16) <= 0) {
                    // yes: get last state
                    int last = this->states[i];

                    // get current state from one or two inputs
                    int state = int(gpio::getInput(this->pinConfigs[config.inputIndex]));
                    if (((int(config.risingAction) | int(config.fallingAction)) & 16) != 0)
                        state |= gpio::getInput(this->pinConfigs[config.inputIndex + 1]) << 1;

                    // accept state change
                    this->states[i] = state;

                    // execute action when main input has toggled
                    if (((state ^ last) & 1) != 0) {
                        auto action = (state & 1) != 0 ? config.risingAction : config.fallingAction;

                        switch (action) {
                        case Action::NONE:
                            break;
                        case Action::INCREMENT:
                            ++this->counters[config.counterIndex];
                            needNotify = true;
                            break;
                        case Action::DECREMENT:
                            --this->counters[config.counterIndex];
                            needNotify = true;
                            break;
                        case Action::SET_LSB:
                            this->counters[config.counterIndex] |= 1;
                            needNotify = true;
                            break;
                        case Action::INCREMENT_WHEN_ENABLED:
                            if ((state & 2) != 0) {
                                ++this->counters[config.counterIndex];
                                needNotify = true;
                            }
                            break;
                        case Action::DECREMENT_WHEN_ENABLED:
                            if ((state & 2) != 0) {
                                --this->counters[config.counterIndex];
                                needNotify = true;
                            }
                            break;
                        case Action::INCREMENT_OR_DECREMENT:
                            this->counters[config.counterIndex] += (state & 2) - 1;
                            needNotify = true;
                            break;
                        }
                    }

                    this->timeouts[i] = -1;
                } else if (((timeout - nextTimeout) << 16) < 0) {
                    // next capture/compare time
                    nextTimeout = timeout;
                }
            }
        }

        // set next compare time and check if it is "in the future"
        (&this->timer->CCR1)[this->ccIndex] = nextTimeout;
        this->timer->SR = ~(TIM_SR_CC1IF << this->ccIndex);
    } while (((nextTimeout - this->timer->CNT) << 16) <= 0);

    // enable/disable interrupt
    /*uint32_t DIER = this->timer->DIER;
    if (nextTimeout != maxTimeout) {
        // enable interrupt
        DIER |= TIM_DIER_CC1IE << this->ccIndex;
    } else {
        // disable interrupt
        DIER &= ~(TIM_DIER_CC1IE << this->ccIndex);
    }
    this->timer->DIER = DIER;*/

    if (needNotify && !this->busy) {
        ++this->sequenceNumber;
        this->busy = true;
        this->loop.push(*this);
    }
}

void InputDevice_EXTI_TIM::handle() {
    // gets called from the event loop to inform the application about a state change
    this->busy = false;
    this->st.doAll(Events::READABLE);
}

} // namespace coco
