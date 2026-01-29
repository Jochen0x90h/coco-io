#pragma once

#include <coco/Array.hpp>
#include <coco/InputDevice.hpp>
#include <coco/Frequency.hpp>
#include <coco/platform/Loop_Queue.hpp>
#include <coco/platform/gpio.hpp>
#include <coco/platform/timer.hpp>


/// @brief Interface to mechanical buttons with debounce filter
///
/// Reference manual:
///   f0:
///     https://www.st.com/resource/en/reference_manual/dm00031936-stm32f0x1stm32f0x2stm32f0x8-advanced-armbased-32bit-mcus-stmicroelectronics.pdf
///       EXTI: Section 11.2
///       TIM
///       Code Examples: Section A.6.2
///   g4:
///     https://www.st.com/resource/en/reference_manual/rm0440-stm32g4-series-advanced-armbased-32bit-mcus-stmicroelectronics.pdf
///       EXTI: Section 15
///       TIM
///
/// Resources:
///   EXTI number of events as needed by configuration
///   TIM timer for debounce timeout
namespace coco {

class InputDevice_EXTI_TIM : public InputDevice, public Loop_Queue::Handler {
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

        /// @brief Increment value. Use on rising edge for buttons.
        INCREMENT = 1,

        /// @brief Decrement value. Use on rising edge for up/down buttons.
        DECREMENT = 2,

        /// @brief Set least significant bit of value. Use on rising edge with INCREMENT on falling edge to obtain the actual button state in the LSB.
        SET_LSB = 3,

        /// @brief Increment when secondary input is high. Use on rising edge with DECREMENT_WHEN_ENABLED on falling edge for quadrature decoder.
        INCREMENT_WHEN_ENABLED = 16,

        /// @brief Decrement when secondary input is high. Use on falling edge with INCREMENT_WHEN_ENABLED on rising edge for quadrature decoder.
        DECREMENT_WHEN_ENABLED = 17,

        /// @brief Increment when secondary input is high, decrement when secondary input is low (pulse/direction).
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

protected:
    // require timer with one capture/compare channel and captuer/compare interrupt
    using TimerInfo = timer::Info<timer::Feature::CC1, timer::Irq::CC>;

    /// @brief Internal Constructor
    /// @param loop event loop
    /// @param pinConfigs array of up to 12 pin configurations, must not be a temporary value
    /// @param configs array of up to 12 configurations, must not be a temporary value
    /// @param timerInfo info of timer to use
    /// @param prescaler timer prescaler value for 4kHz
    InputDevice_EXTI_TIM(Loop_Queue &loop, Array<const gpio::Config> pinConfigs, Array<const Config> configs,
        const TimerInfo &timerInfo, int prescaler);

public:
    /// @brief Constructor
    /// @param loop event loop
    /// @param pinConfigs array of up to 12 pin configurations, must not be a temporary value
    /// @param configs array of up to 12 configurations, must not be a temporary value
    /// @param timerInfo info of timer to use
    /// @param timerClock timer clock frequency
    InputDevice_EXTI_TIM(Loop_Queue &loop, Array<const gpio::Config> pinConfigs, Array<const Config> configs,
        const TimerInfo &timerInfo, Kilohertz<> timerClock)
        : InputDevice_EXTI_TIM(loop, pinConfigs, configs, timerInfo, (timerClock.value >> 2) - 1) {}
    ~InputDevice_EXTI_TIM();

    // InputDevice methods
    int get(void *data, int size) override;
    [[nodiscard]] Awaitable<Events> untilInput(int sequenceNumber) override;


    /// @brief Call from interrupt handler of all used EXTI channels (e.g. EXTI15_10_IRQHandler())
    ///
    void EXTI_IRQHandler();

    /// @brief Call from capture/compare interrupt handler of the timer (e.g. TIM1_CC_IRQHandler or TIM3_IRQHandler())
    ///
    void TIM_IRQHandler() {
        // check if "our" timer generated the interrupt
        if ((timer_.status() & timer::Status::COMPARE1) != 0)
            update();
    }

protected:
    void update();

    // gets called from the event loop to notify the main application
    void handle() override;

    Loop_Queue &loop_;

    Array<const gpio::Config> pinConfigs_;
    Array<const Config> configs_;

    TimerInfo::Instance timer_;
    int timerIrq_;

    int extiFlags_;

    uint8_t states_[12];
    int timeouts_[12];

    int sequenceNumber_ = 0;
    int8_t counters_[12] = {};

    // coroutines waiting for new input
    std::atomic<bool> busy_ = false;
};

} // namespace coco
