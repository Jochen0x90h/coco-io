#include <coco/InputDevice.hpp>
#include <coco/platform/Loop_emu.hpp>
#include <string>


namespace coco {

/// @brief Emulated rotary switch knob with push button.
///
class RotaryKnob_emu : public InputDevice, public Loop_emu::GuiHandler, Loop_native::TimeoutHandler {
public:
    /**
        Constructor
        @param loop event loop
        @param id unique id for gui
    */
    RotaryKnob_emu(Loop_emu &loop, bool haveButton, int id);
    ~RotaryKnob_emu() override;

    // InputDevice methods
    int get(void *data, int size) override;
    [[nodiscard]] Awaitable<Events> untilInput(int sequenceNumber) override;

protected:
    void onGui(Gui &gui) override;
    void onTimeout() override;

    Loop_emu &loop_;
    bool haveButton_;
    int id_;
    //TimedTask<Callback<>> callback_;

    // sequence number gets incremented on state change
    int sequenceNumber_ = 0;

    // [0] rotary knob counter, [1] button, [2] long press on button
    int8_t counters_[3] = {};

    //CoroutineTaskList<> changeTasks_;
};

} // namespace coco
