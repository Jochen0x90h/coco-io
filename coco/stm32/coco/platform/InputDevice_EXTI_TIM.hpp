#pragma once

#include <coco/Array.hpp>
#include <coco/InputDevice.hpp>
#include <coco/Frequency.hpp>
#include <coco/platform/Loop_Queue.hpp>
#include <coco/platform/gpio.hpp>
#include <coco/platform/timer.hpp>


/**
 * Interface to mechanical buttons with debounce filter
 *
 * Reference manual:
 *   f0:
 *     https://www.st.com/resource/en/reference_manual/dm00031936-stm32f0x1stm32f0x2stm32f0x8-advanced-armbased-32bit-mcus-stmicroelectronics.pdf
 *       EXTI: Section 11.2
 *       TIM
 *       Code Examples: Section A.6.2
 *   g4:
 *     https://www.st.com/resource/en/reference_manual/rm0440-stm32g4-series-advanced-armbased-32bit-mcus-stmicroelectronics.pdf
 *       EXTI: Section 15
 *       TIM
 *
 * Resources:
 *   EXTI number of events as needed by configuration
 *   TIM timer for debounce timeout
 */
namespace coco {

class InputDevice_EXTI_TIM : public InputDevice, public Loop_Queue::Handler {
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

protected:
    /**
     * Internal Constructor
     * @param loop event loop
     * @param pinConfigs array of up to 12 pin configurations, must not be a temporary value
     * @param configs array of up to 12 configurations, must not be a temporary value
     * @param timerInfo info of timer to use
     * @param prescaler timer prescaler value for 4kHz
     * @param ccIndex capture/compare channel to use
     */
    InputDevice_EXTI_TIM(Loop_Queue &loop, Array<const gpio::Config> pinConfigs, Array<const Config> configs,
        const timer::Info1 &timerInfo, int prescaler, int ccIndex);

public:
    /**
     * Constructor
     * @param loop event loop
     * @param pinConfigs array of up to 12 pin configurations, must not be a temporary value
     * @param configs array of up to 12 configurations, must not be a temporary value
     * @param timerInfo info of timer to use
     * @param timerClock timer clock frequency
     * @param ccIndex capture/compare channel to use
     */
    InputDevice_EXTI_TIM(Loop_Queue &loop, Array<const gpio::Config> pinConfigs, Array<const Config> configs,
        const timer::Info1 &timerInfo, Kilohertz<> timerClock, int ccIndex)
        : InputDevice_EXTI_TIM(loop, pinConfigs, configs, timerInfo, (timerClock.value >> 2) - 1, ccIndex) {}
    ~InputDevice_EXTI_TIM();

    // InputDevice methods
    int get(void *data, int size) override;
    [[nodiscard]] Awaitable<Events> untilInput(int sequenceNumber) override;


    /**
     * Call from interrupt handler of all used EXTI channels (e.g. EXTI15_10_IRQHandler())
     */
    void EXTI_IRQHandler();

    /**
     * Call from interrupt handler of the timer (e.g. TIM3_IRQHandler())
     */
    void TIM_IRQHandler() {
        // check if "our" capture/compare generated the interrupt
        if (this->timer->SR & (TIM_SR_CC1IF << this->ccIndex))
            update();
    }

protected:
    void update();

    // gets called from the event loop to notify the main application
    void handle() override;

    Loop_Queue &loop;

    Array<const gpio::Config> pinConfigs;
    Array<const Config> configs;

    TIM_TypeDef *timer;
    int timerIrq;
    int ccIndex;

    int extiFlags;

    uint8_t states[12];
    int timeouts[12];

    int sequenceNumber = 0;
    int8_t counters[12] = {};

    // coroutines waiting for new input
    std::atomic<bool> busy = false;
};

} // namespace coco
