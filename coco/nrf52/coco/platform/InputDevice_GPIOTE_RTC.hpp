#pragma once

#include <coco/Array.hpp>
#include <coco/InputDevice.hpp>
#include <coco/Frequency.hpp>
#include <coco/platform/Loop_Queue.hpp>
#include <coco/platform/gpio.hpp>
#include <coco/platform/rtc.hpp>


/// @brief Interface to mechanical buttons with debounce filter
/// https://infocenter.nordicsemi.com/topic/ps_nrf52840/gpiote.html?cp=5_0_0_5_9
///
/// Resources:
///   NRF_GPIOTE
///     EVENTS_IN number of events as needed by configuration
///   RTC timer for debounce timeout
namespace coco {

class InputDevice_GPIOTE_RTC : public InputDevice, public Loop_Queue::Handler {
public:
    enum class Init : uint8_t {
        // initial state is low
        LOW = 0,

        // initial state is high
        HIGH = 1,

        // initial state is taken from input(s)
        INPUT = 4,
    };

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

        // init and action on rising/falling edge of main input
        uint16_t config;

        // delay for debounce or long press detection
        Milliseconds<> risingDelay;
        Milliseconds<> fallingDelay;


        constexpr Config(uint8_t inputIndex, uint8_t counterIndex,
            Init init, Action risingAction, Action fallingAction,
            Milliseconds<> risingDelay, Milliseconds<> fallingDelay)
            : inputIndex(inputIndex), counterIndex(counterIndex),
            config(int(init) | (int(risingAction) << 4) | (int(fallingAction) << 10)),
            risingDelay(risingDelay), fallingDelay(fallingDelay) {}

        Init init() const {
            return Init(config & 15);
        }

        Action risingAction() const {
            return Action((config >> 4) & 63);
        }

        Action fallingAction() const {
            return Action(config >> 10);
        }

        bool hasSecondaryInput() const {
            return (config & ((16 << 4) | (16 << 10))) != 0;
        }
    };

    /// @brief Constructor
    /// @param loop event loop
    /// @param pinConfigs array of up to 8 pin configurations, must not be a temporary value
    /// @param configs array of up to 8 input types, must not be a temporary value
    /// @param firstEventIndex first index of GPIOTE events to use
    /// @param rtcInfo info of rtc to use
    /// @param ccIndex capture/compare channel to use
    InputDevice_GPIOTE_RTC(Loop_Queue &loop, Array<const gpio::Config> pinConfigs, Array<const Config> configs,
        int firstEventIndex, const rtc::Info &rtcInfo, int ccIndex = 0);
    ~InputDevice_GPIOTE_RTC();

    // InputDevice methods
    int get(void *data, int size) override;
    [[nodiscard]] Awaitable<Events> untilInput(int sequenceNumber) override;


    /// @brief Call from interrupt handler of the GPIOTE (GPIOTE_IRQHandler())
    ///
    void GPIOTE_IRQHandler();

    /// @brief Call from interrupt handler of the RTC (e.g. RTC1_IRQHandler())
    ///
    void RTC_IRQHandler() {
        // check if "our" capture/compare generated the interrupt
        if (this->rtc_->EVENTS_COMPARE[this->ccIndex_])
            update();
    }

protected:
    void update();

    // gets called from the event loop to notify the main application
    void handle() override;

    Loop_Queue &loop_;

    Array<const gpio::Config> pinConfigs_;
    Array<const Config> configs_;

    int firstEventIndex_;

    NRF_RTC_Type *rtc_;
    int rtcIrq_;
    int ccIndex_;

    uint8_t states_[8];
    int timeouts_[8];

    int sequenceNumber_ = 0;
    int8_t counters_[8] = {};

    // coroutines waiting for new input
    std::atomic<bool> busy_ = false;
};

} // namespace coco
