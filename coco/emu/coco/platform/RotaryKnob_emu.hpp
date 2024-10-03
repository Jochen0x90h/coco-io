#include <coco/InputDevice.hpp>
#include <coco/platform/Loop_emu.hpp>
#include <string>


namespace coco {

/**
    Emulated rotary switch knob with push button
 */
class RotaryKnob_emu : public InputDevice, public Loop_emu::GuiHandler {
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
    void handle(Gui &gui) override;
    void handleTimeout();

    Loop_emu &loop;
    bool haveButton;
    int id;
    TimedTask<Callback> callback;

    // sequence number gets incremented on state change
    int sequenceNumber = 0;

    // [0] rotary knob counter, [1] button, [2] long press on button
    int8_t counters[3] = {};

    //CoroutineTaskList<> changeTasks;
};

} // namespace coco
