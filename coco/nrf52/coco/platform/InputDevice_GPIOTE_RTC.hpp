#pragma once

#include <coco/Array.hpp>
#include <coco/InputDevice.hpp>
#include <coco/Frequency.hpp>
#include <coco/platform/Loop_Queue.hpp>
#include <coco/platform/gpio.hpp>
#include <coco/platform/rtc.hpp>


/**
 * Interface to mechanical buttons with debounce filter
 * https://infocenter.nordicsemi.com/topic/ps_nrf52840/gpiote.html?cp=5_0_0_5_9
 *
 * Resources:
 *   NRF_GPIOTE
 *    EVENTS_IN number of events as needed by configuration
 *    RTC timer for debounce timeout
 */
namespace coco {

class InputDevice_GPIOTE_RTC : public InputDevice, public Loop_Queue::Handler {
public:
    enum class Action : uint8_t {
        /// @brief No action
        NONE = 0,

        /// @brief Increment value. Use on rising edge for buttons
        INCREMENT = 1,

        /// @brief Decrement value. Use on rising edge for up/down buttons
        DECREMENT = 2,

        /// @brief Set least significant bit of value. Use on rising edge with INCREMENT on falling edge to obtain the actual state
        SET_LSB = 3,

        /// @brief Increment when secondary input is high. Use on rising edge with DECREMENT_WHEN_ENABLED on falling edge for quadrature decoder
        INCREMENT_WHEN_ENABLED = 16,

        /// @brief Decrement when secondary input is high. Use on falling edge with INCREMENT_WHEN_ENABLED on rising edge for quadrature decoder
        DECREMENT_WHEN_ENABLED = 17,

        /// @brief Increment when secondary input is high, decrement when secondary input is low
        INCREMENT_OR_DECREMENT = 18,
    };

    struct Config {
        // index of main input, secondary input is inputIndex + 1
        uint8_t inputIndex;

        // index of counter to manipulate
        uint8_t counterIndex;

        // action on rising edge of main input
        Action risingAction;

        // action on falling edge of main input
        Action fallingAction;

        // delay for debounce or long press detection
        Milliseconds<> risingDelay;
        Milliseconds<> fallingDelay;
    };

    /**
     * Constructor
     * @param loop event loop
     * @param pinConfigs array of up to 8 pin configurations, must not be a temporary value
     * @param configs array of up to 8 input types, must not be a temporary value
     * @param firstEventIndex first index of GPIOTE events to use
     * @param rtcInfo info of rtc to use
     * @param ccIndex capture/compare channel to use
     */
    InputDevice_GPIOTE_RTC(Loop_Queue &loop, Array<const gpio::Config> pinConfigs, Array<const Config> configs,
        int firstEventIndex, const rtc::Info &rtcInfo, int ccIndex);
    ~InputDevice_GPIOTE_RTC();

    // InputDevice methods
    int get(void *data, int size) override;
    [[nodiscard]] Awaitable<Events> untilInput(int sequenceNumber) override;


    /**
     * Call from interrupt handler of the GPIOTE (GPIOTE_IRQHandler())
     */
    void GPIOTE_IRQHandler();

    /**
     * Call from interrupt handler of the RTC (e.g. RTC1_IRQHandler())
     */
    void RTC_IRQHandler() {
        // check if "our" capture/compare generated the interrupt
        if (this->rtc->EVENTS_COMPARE[this->ccIndex])
            update();
    }

protected:
    void update();

    // gets called from the event loop to notify the main application
    void handle() override;

    Loop_Queue &loop;

    Array<const gpio::Config> pinConfigs;
    Array<const Config> configs;

    int firstEventIndex;

    NRF_RTC_Type *rtc;
    int rtcIrq;
    int ccIndex;

    uint8_t states[8];
    int timeouts[8];

    int sequenceNumber = 0;
    int8_t counters[8] = {};

    // coroutines waiting for new input
    std::atomic<bool> busy = false;
};

} // namespace coco
