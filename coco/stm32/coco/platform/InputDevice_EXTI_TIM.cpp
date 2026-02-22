#include "InputDevice_EXTI_TIM.hpp"
#include <coco/platform/platform.hpp>
#include <coco/platform/exti.hpp>
#include <coco/platform/nvic.hpp>
#include <coco/convert.hpp>
#include <coco/debug.hpp>


namespace coco {

InputDevice_EXTI_TIM::InputDevice_EXTI_TIM(Loop_Queue &loop, Array<const gpio::Config> pinConfigs, Array<const Config> configs,
    const TimerInfo &timerInfo, int prescaler)
    : InputDevice(State::READY)
    , loop_(loop), pinConfigs_(pinConfigs), configs_(configs)
    , timerIrq_(timerInfo.irq<timer::Irq::CC>())
{
    assert(types.size() <= std::size(counters_));
    assert(configs.size() <= std::size(cc_));

    // configure input pins
    int flags = 0;
    for (int i = 0; i < pinConfigs.size(); ++i) {
        auto &config = pinConfigs[i];

        // configure pin as input
        gpio::enableInput(config);

        // get port and EXTI channel (is identical to pin number)
        int port = gpio::getPortIndex(config);
        int line = gpio::getPinIndex(config);
        flags |= 1 << line;

        // configure EXTI port
        exti::setPort(line, port);

        // enable EXTI interrupt
        int irq = exti::irq(line);
        nvic::setPriority(irq, nvic::Priority::LOW);
        nvic::enable(irq);
    }
    extiFlags_ = flags;

    // initialize states and counters
    for (int i = 0; i < configs.size(); ++i) {
        auto &config = configs[i];

        auto init = config.init();
        int state;
        if (init >= Init::INPUT) {
            // get initial state from one or two inputs
            state = int(gpio::getInput(pinConfigs[config.inputIndex]));
            if (config.hasSecondaryInput())
                state |= int(gpio::getInput(pinConfigs[config.inputIndex + 1])) << 1;
        } else {
            // get initial state from config
            state = int(init);
        }
        states_[i] = state;

        // set timeout as invalid
        //timeouts_[i] = -1;
        timeouts_[i] = config.risingDelay.value << 2;

        // init counter
        bool startWithOne = (state != 0 && config.risingAction() == Action::SET_LSB)
            || (state == 0 && config.fallingAction() == Action::SET_LSB);
        counters_[config.counterIndex] = int(startWithOne);
//debug::out << "InputDevice_EXTI_TIM: init config " << dec(i) << ": state=" << dec(state) << " counter=" << dec(startWithOne) << "\n";
    }

    // initialize timer to 4kHz
    timer_ = timerInfo.enableClock()
        .setPrescaler(prescaler)
        .set(timer::Interrupt::COMPARE1, timer::DmaRequest::NONE)
        .start();

    // update to set first timer compare time
    update();

    // detect rising and falling edge
    exti::detectBothEdgesFlags(flags);

    // clear interrupt flags
    exti::clearPendingFlags(flags);

    // enable EXTI interrupts
    exti::enableInterruptFlags(flags);

    // enable timer interrupt
    nvic::setPriority(timerIrq_, nvic::Priority::LOW);
    nvic::enable(timerIrq_);
    //debug::out << "timer irq " << dec(timerIrq_) << "\n";
}

InputDevice_EXTI_TIM::~InputDevice_EXTI_TIM() {
}

int InputDevice_EXTI_TIM::get(void *data, int size) { //const Array<int8_t> &counters) {
    // disable timer interrupt
    nvic::disable(timerIrq_);

    // copy values
    uint8_t *counters = reinterpret_cast<uint8_t *>(data);
    int count = std::min(size, int(std::size(counters_)));
    for (int i = 0; i < count; ++i)
        counters[i] = counters_[i];
    int sequenceNumber = sequenceNumber_;

    // enable timer interrupt
    nvic::enable(timerIrq_);

    return sequenceNumber;
}

Awaitable<Device::Events> InputDevice_EXTI_TIM::untilInput(int sequenceNumber) {
    if (sequenceNumber_ != sequenceNumber)
        return {};
    return {tasks_, Events::READABLE};
}

// gets called from EXTI interrupt
void InputDevice_EXTI_TIM::EXTI_IRQHandler() {
    int inputFlags = exti::pendingFlags(extiFlags_);

    // check if one of "our" EXTIs was triggered
    if (inputFlags != 0) {
        // clear pending interrupt flags at peripheral
        exti::clearPendingFlags(inputFlags);

        for (int i = 0; i < configs_.size(); ++i) {
            auto &config = configs_[i];

            // check if activity is detected for this config
            int inputIndex = gpio::getPinIndex(pinConfigs_[config.inputIndex]);
            bool detected = ((inputFlags >> inputIndex) & 1) != 0;
            if (config.hasSecondaryInput()) {
                int inputIndex2 = gpio::getPinIndex(pinConfigs_[config.inputIndex + 1]);
                detected |= ((inputFlags >> inputIndex2) & 1) != 0;
            }

            if (detected != 0) {
                // get last state
                int last = states_[i];

                // get delay according to detected edge
                auto delay = (last & 1) == 0 ? config.risingDelay : config.fallingDelay;

                // set timeout for the input
                int timeout = timer_.count() + (delay.value << 2);
                timeouts_[i] = timeout;

                // force update (update() gets called via interrupt handler)
                timer_.generate(timer::Event::COMPARE1);
            }
        }
    }
}

// gets called from constructor and timer interrupt
void InputDevice_EXTI_TIM::update() {
    // check inputs and set new timeout
    int maxTimeout;
    int nextTimeout;
    bool needNotify = false;
    int now = timer_.count();
    do {
        nextTimeout = maxTimeout = now + 0x7fff; // 16 bit counter

        // check if an input changed state
        for (int i = 0; i < configs_.size(); ++i) {
            auto &config = configs_[i];

            // get timeout value
            int timeout = timeouts_[i];

            // check if timeout valid
            if (timeout != -1) {
                // check if time elapsed
                if (((timeout - 1 - now) << 16) <= 0) {
                    // yes: get last state
                    int last = states_[i];

                    // get current state from one or two inputs
                    int state = int(gpio::getInput(pinConfigs_[config.inputIndex]));
                    if (config.hasSecondaryInput())
                        state |= int(gpio::getInput(pinConfigs_[config.inputIndex + 1])) << 1;

                    // set new state
                    states_[i] = state;

                    // execute action when main input has toggled
                    if (((state ^ last) & 1) != 0) {
                        auto action = (state & 1) != 0 ? config.risingAction() : config.fallingAction();

                        switch (action) {
                        case Action::NONE:
                            break;
                        case Action::INCREMENT:
                            ++counters_[config.counterIndex];
                            needNotify = true;
                            break;
                        case Action::DECREMENT:
                            --counters_[config.counterIndex];
                            needNotify = true;
                            break;
                        case Action::SET_LSB:
                            counters_[config.counterIndex] |= 1;
                            needNotify = true;
                            break;
                        case Action::INCREMENT_WHEN_ENABLED:
                            if ((state & 2) != 0) {
                                ++counters_[config.counterIndex];
                                needNotify = true;
                            }
                            break;
                        case Action::DECREMENT_WHEN_ENABLED:
                            if ((state & 2) != 0) {
                                --counters_[config.counterIndex];
                                needNotify = true;
                            }
                            break;
                        case Action::INCREMENT_OR_DECREMENT:
                            counters_[config.counterIndex] += (state & 2) - 1;
                            needNotify = true;
                            break;
                        }
                    }

                    timeouts_[i] = -1;
                } else if (((timeout - nextTimeout) << 16) < 0) {
                    // next capture/compare time
                    nextTimeout = timeout;
                }
            }
        }

        // set next compare time and check if it is "in the future"
        timer_.setCompare1(nextTimeout);
        timer_.clear(timer::Status::COMPARE1);
        now = timer_.count();
    } while (((nextTimeout - now) << 16) <= 0);

    // enable/disable interrupt
    /*uint32_t DIER = timer->DIER;
    if (nextTimeout != maxTimeout) {
        // enable interrupt
        DIER |= TIM_DIER_CC1IE << ccIndex;
    } else {
        // disable interrupt
        DIER &= ~(TIM_DIER_CC1IE << ccIndex);
    }
    timer_->DIER = DIER;*/

    if (needNotify && !busy_) {
        ++sequenceNumber_;
        busy_ = true;
        loop_.push(*this);
    }
}

void InputDevice_EXTI_TIM::handle() {
    // gets called from the event loop to inform the application about a state change
    busy_ = false;
    notify(Events::READABLE);
}

} // namespace coco
