#include "InputDevice_GPIOTE_RTC.hpp"
#include <coco/platform/platform.hpp>
#include <coco/platform/nvic.hpp>
//#include <coco/debug.hpp>


namespace coco {

InputDevice_GPIOTE_RTC::InputDevice_GPIOTE_RTC(Loop_Queue &loop, Array<const gpio::Config> pinConfigs, Array<const Config> configs,
    int firstEventIndex, const rtc::Info &rtcInfo, int ccIndex)
    : InputDevice(State::READY)
    , loop(loop), pinConfigs(pinConfigs), configs(configs), firstEventIndex(firstEventIndex)
    , rtc(rtcInfo.rtc), rtcIrq(rtcInfo.irq), ccIndex(ccIndex)
{
    assert(types.size() <= std::size(this->counters));
    assert(configs.size() <= std::size(this->cc));

    // configure input pins
    for (int i = 0; i < pinConfigs.size(); ++i) {
        auto &config = pinConfigs[i];
        int eventIndex = firstEventIndex + i;

        // configure input pin
        gpio::configureInput(config);

        // configure GPIOTE event
        NRF_GPIOTE->CONFIG[eventIndex] = N(GPIOTE_CONFIG_MODE, Event)
            | N(GPIOTE_CONFIG_POLARITY, Toggle)
            | V(GPIOTE_CONFIG_PSEL, gpio::getPinIndex(config));
        NRF_GPIOTE->EVENTS_IN[eventIndex] = 0; // clear pending event

        // enable GPIOTE event interrupt
        NRF_GPIOTE->INTENSET = 1 << eventIndex;
    }

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

    // initialize RTC to 16384Hz
    auto rtc = rtcInfo.rtc;
    rtc->INTENSET = 1 << (RTC_INTENSET_COMPARE0_Pos + ccIndex);
    rtc->PRESCALER = 1; // 16384Hz
    rtc->TASKS_START = TRIGGER;

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
    nvic::disable(this->rtcIrq);
    nvic::disable(GPIOTE_IRQn);

    // copy values
    uint8_t *counters = reinterpret_cast<uint8_t *>(data);
    int count = std::min(size, int(std::size(this->counters)));
    for (int i = 0; i < count; ++i)
        counters[i] = this->counters[i];
    int sequenceNumber = this->sequenceNumber;

    // enable interrupts
    nvic::enable(GPIOTE_IRQn);
    nvic::enable(this->rtcIrq);

    return sequenceNumber;
}

Awaitable<Device::Events> InputDevice_GPIOTE_RTC::untilInput(int sequenceNumber) {
    if (this->sequenceNumber != sequenceNumber)
        return {};
    return {this->st.tasks, Events::READABLE};
}

void InputDevice_GPIOTE_RTC::GPIOTE_IRQHandler() {
    // collect detection flags
    int inputFlags = 0;
    for (int i = 0; i < this->pinConfigs.size(); ++i) {
        int eventIndex = this->firstEventIndex + i;
        inputFlags |= NRF_GPIOTE->EVENTS_IN[eventIndex] << i;

        // clear pending interrupt flag at peripheral
        NRF_GPIOTE->EVENTS_IN[eventIndex] = 0;
    }

    if (inputFlags != 0) {
        bool needUpdate = false;
        for (int i = 0; i < this->configs.size(); ++i) {
            auto &config = this->configs[i];

            // check if activity is detected for this config
            int inputIndex = config.inputIndex;
            bool detected = ((inputFlags >> inputIndex) & 1) != 0;
            if (((int(config.risingAction) | int(config.fallingAction)) & 16) != 0) {
                ++inputIndex;
                detected |= ((inputFlags >> inputIndex) & 1) != 0;
            }

            if (detected != 0) {
                // get last state
                int last = this->states[i];

                // set timeout for the input
                auto delay = (last & 1) == 0 ? config.risingDelay : config.fallingDelay;

                int timeout = this->rtc->COUNTER + delay.value * 16384 / 1000;
                this->timeouts[i] = timeout;

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
    do {
        int now = this->rtc->COUNTER;
        nextTimeout = maxTimeout = now + 0x7fffff; // 24 bit counter

        // check if an input changed state
        for (int i = 0; i < this->configs.size(); ++i) {
            auto &config = this->configs[i];

            // get timeout value
            int timeout = this->timeouts[i];

            // check if time elapsed
            if (timeout != -1) {
                if (((timeout - 6 - now) << 8) <= 0) {
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
                } else if (((timeout - nextTimeout) << 8) < 0) {
                    // next capture/compare time
                    nextTimeout = timeout;
                }
            }
        }

        // set next compare time and check if it is "in the future"
        this->rtc->CC[this->ccIndex] = nextTimeout;
        this->rtc->EVENTS_COMPARE[this->ccIndex] = 0;
    } while (((nextTimeout - 2 - this->rtc->COUNTER) << 8) <= 0);

    // enable/disable interrupt
    /*if (nextTimeout != maxTimeout) {
        // enable interrupt
        rtc->INTENSET = 1 << (RTC_INTENSET_COMPARE0_Pos + this->ccIndex);
    } else {
        // disable interrupt
        rtc->INTENCLR = 1 << (RTC_INTENSET_COMPARE0_Pos + this->ccIndex);
    }*/

    if (needNotify && !this->busy) {
        ++this->sequenceNumber;
        this->busy = true;
        this->loop.push(*this);
    }
}

void InputDevice_GPIOTE_RTC::handle() {
    // gets called from the event loop to inform the application about a state change
    this->busy = false;
    this->st.doAll(Events::READABLE);
}

} // namespace coco
