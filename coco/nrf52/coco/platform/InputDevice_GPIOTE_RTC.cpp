#include "InputDevice_GPIOTE_RTC.hpp"
#include <coco/platform/platform.hpp>
#include <coco/platform/nvic.hpp>
//#include <coco/debug.hpp>


namespace coco {

InputDevice_GPIOTE_RTC::InputDevice_GPIOTE_RTC(Loop_Queue &loop, Array<const gpio::Config> pinConfigs, Array<const Config> configs,
    int firstEventIndex, const rtc::Info &rtcInfo, int ccIndex)
    : InputDevice(State::READY)
    , loop_(loop), pinConfigs_(pinConfigs), configs_(configs), firstEventIndex_(firstEventIndex)
    , rtc_(rtcInfo.rtc), rtcIrq_(rtcInfo.irq), ccIndex_(ccIndex)
{
    assert(types.size() <= std::size(counters_));
    assert(configs.size() <= std::size(cc_));

    // configure input pins
    for (int i = 0; i < pinConfigs.size(); ++i) {
        auto &config = pinConfigs[i];
        int eventIndex = firstEventIndex + i;

        // configure input pin
        gpio::enableInput(config);

        // configure GPIOTE event
        NRF_GPIOTE->CONFIG[eventIndex] = N(GPIOTE_CONFIG_MODE, Event)
            | N(GPIOTE_CONFIG_POLARITY, Toggle)
            | V(GPIOTE_CONFIG_PSEL, gpio::getPinPortIndex(config));
        NRF_GPIOTE->EVENTS_IN[eventIndex] = 0; // clear pending event

        // enable GPIOTE event interrupt
        NRF_GPIOTE->INTENSET = 1 << eventIndex;
    }

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
        timeouts_[i] = config.risingDelay.value * 2048 / 125; // 16384 / 1000;

        // init counter
        bool startWithOne = (state != 0 && config.risingAction() == Action::SET_LSB)
            || (state == 0 && config.fallingAction() == Action::SET_LSB);
        counters_[config.counterIndex] = int(startWithOne);
    }

    // initialize RTC to 16384Hz
    auto rtc = rtcInfo.rtc;
    rtc->INTENSET = 1 << (RTC_INTENSET_COMPARE0_Pos + ccIndex);
    rtc->PRESCALER = 1; // 16384Hz
    rtc->TASKS_START = TRIGGER;

    // update to set first timer compare time
    update();

    // enable timer interrupt
    nvic::setPriority(rtcInfo.irq, nvic::Priority::LOW);
    nvic::enable(rtcInfo.irq);

    // enable GPIOTE interrupt
    nvic::setPriority(GPIOTE_IRQn, nvic::Priority::LOW);
    nvic::enable(GPIOTE_IRQn);
}

InputDevice_GPIOTE_RTC::~InputDevice_GPIOTE_RTC() {
}

int InputDevice_GPIOTE_RTC::get(void *data, int size) {
    // disable interrupts
    nvic::disable(rtcIrq_);
    nvic::disable(GPIOTE_IRQn);

    // copy values
    uint8_t *counters = reinterpret_cast<uint8_t *>(data);
    int count = std::min(size, int(std::size(counters_)));
    for (int i = 0; i < count; ++i)
        counters[i] = counters_[i];
    int sequenceNumber = sequenceNumber_;

    // enable interrupts
    nvic::enable(GPIOTE_IRQn);
    nvic::enable(rtcIrq_);

    return sequenceNumber;
}

Awaitable<Device::Events> InputDevice_GPIOTE_RTC::untilInput(int sequenceNumber) {
    if (sequenceNumber_ != sequenceNumber)
        return {};
    return {st.tasks, Events::READABLE};
}

void InputDevice_GPIOTE_RTC::GPIOTE_IRQHandler() {
    // collect detection flags
    int inputFlags = 0;
    for (int i = 0; i < pinConfigs_.size(); ++i) {
        int eventIndex = firstEventIndex_ + i;
        inputFlags |= NRF_GPIOTE->EVENTS_IN[eventIndex] << i;

        // clear pending interrupt flag at peripheral
        NRF_GPIOTE->EVENTS_IN[eventIndex] = 0;
    }

    if (inputFlags != 0) {
        bool needUpdate = false;
        for (int i = 0; i < configs_.size(); ++i) {
            auto &config = configs_[i];

            // check if activity is detected for this config
            int inputIndex = config.inputIndex;
            bool detected = ((inputFlags >> inputIndex) & 1) != 0;
            if (config.hasSecondaryInput()) {
                ++inputIndex;
                detected |= ((inputFlags >> inputIndex) & 1) != 0;
            }

            if (detected != 0) {
                // get last state
                int last = states_[i];

                // get delay according to detected edge
                auto delay = (last & 1) == 0 ? config.risingDelay : config.fallingDelay;

                // set timeout for the input
                int timeout = rtc_->COUNTER + delay.value * 2048 / 125;//16384 / 1000;
                timeouts_[i] = timeout;

                // force update
                needUpdate = true;
            }
        }

        if (needUpdate)
            update();
    }
}

// gets called from timer interrupt
void InputDevice_GPIOTE_RTC::update() {
    int maxTimeout;
    int nextTimeout;
    bool needNotify = false;
    int now = rtc_->COUNTER;
    do {
        nextTimeout = maxTimeout = now + 0x7fffff; // 24 bit counter

        // check if an input changed state
        for (int i = 0; i < configs_.size(); ++i) {
            auto &config = configs_[i];

            // get timeout value
            int timeout = timeouts_[i];

            // check if timeout valid
            if (timeout != -1) {
                // check if time elapsed
                if (((timeout - 6 - now) << 8) <= 0) {
                    // yes: get last state
                    int last = states_[i];

                    // get current state from one or two inputs
                    int state = int(gpio::getInput(pinConfigs_[config.inputIndex]));
                    if (config.hasSecondaryInput())
                        state |= gpio::getInput(pinConfigs_[config.inputIndex + 1]) << 1;

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
                } else if (((timeout - nextTimeout) << 8) < 0) {
                    // next capture/compare time
                    nextTimeout = timeout;
                }
            }
        }

        // set next compare time and check if it is "in the future"
        rtc_->CC[ccIndex_] = nextTimeout;
        rtc_->EVENTS_COMPARE[ccIndex_] = 0;
        now = rtc_->COUNTER;
    } while (((nextTimeout - 2 - now) << 8) <= 0);

    // enable/disable interrupt
    /*if (nextTimeout != maxTimeout) {
        // enable interrupt
        rtc->INTENSET = 1 << (RTC_INTENSET_COMPARE0_Pos + ccIndex);
    } else {
        // disable interrupt
        rtc->INTENCLR = 1 << (RTC_INTENSET_COMPARE0_Pos + ccIndex);
    }*/

    if (needNotify && !busy_) {
        ++sequenceNumber_;
        busy_ = true;
        loop_.push(*this);
    }
}

void InputDevice_GPIOTE_RTC::handle() {
    // gets called from the event loop to inform the application about a state change
    busy_ = false;
    st.notify(Events::READABLE);
}

} // namespace coco
