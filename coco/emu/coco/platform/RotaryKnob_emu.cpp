#include "RotaryKnob_emu.hpp"
#include <coco/platform/GuiRotaryKnob.hpp>
#include <iostream>


namespace coco {

RotaryKnob_emu::RotaryKnob_emu(Loop_emu &loop, bool haveButton, int id)
    : InputDevice(State::READY)
    , loop(loop), haveButton(haveButton), id(id), callback(makeCallback<RotaryKnob_emu, &RotaryKnob_emu::handleTimeout>(this))
{
    loop.guiHandlers.add(*this);
}

RotaryKnob_emu::~RotaryKnob_emu() {
}

int RotaryKnob_emu::get(void *data, int size) {
    uint8_t *counters = reinterpret_cast<uint8_t *>(data);
    int count = std::min(size, int(std::size(this->counters)));
    for (int i = 0; i < count; ++i)
        counters[i] = this->counters[i];
    return this->sequenceNumber;
}

Awaitable<Device::Events> RotaryKnob_emu::untilInput(int sequenceNumber) {
    if (this->sequenceNumber != sequenceNumber)
        return {};
    return {this->st.tasks, Events::READABLE};
}

void RotaryKnob_emu::handle(Gui &gui) {
    auto result = gui.widget<GuiRotaryKnob>(this->id, 24, 0.1f, this->haveButton);
    if (result.delta) {
        this->counters[0] += *result.delta;
        ++this->sequenceNumber;
        this->st.doAll(Events::READABLE);
    }
    if (result.button) {
        if (*result.button) {
            // button pressed
            ++this->counters[1];
            ++this->sequenceNumber;
            this->st.doAll(Events::READABLE);

            // start timeout for long press
            this->loop.invoke(this->callback, 3s);
        } else {
            // cancel timeout for long press
            this->callback.remove();
        }
    }
}

void RotaryKnob_emu::handleTimeout() {
    ++this->counters[2];
    ++this->sequenceNumber;
    this->st.doAll(Events::READABLE);
}

} // namespace coco
