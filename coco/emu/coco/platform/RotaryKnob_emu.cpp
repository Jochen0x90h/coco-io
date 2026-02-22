#include "RotaryKnob_emu.hpp"
#include <coco/platform/GuiRotaryKnob.hpp>
#include <iostream>


namespace coco {

RotaryKnob_emu::RotaryKnob_emu(Loop_emu &loop, bool haveButton, int id)
    : InputDevice(State::READY)
    , loop_(loop), haveButton_(haveButton), id_(id), callback_(makeCallback<RotaryKnob_emu, &RotaryKnob_emu::handleTimeout>(this))
{
    loop.guiHandlers.add(*this);
}

RotaryKnob_emu::~RotaryKnob_emu() {
}

int RotaryKnob_emu::get(void *data, int size) {
    uint8_t *counters = reinterpret_cast<uint8_t *>(data);
    int count = std::min(size, int(std::size(counters_)));
    for (int i = 0; i < count; ++i)
        counters[i] = counters_[i];
    return sequenceNumber_;
}

Awaitable<Device::Events> RotaryKnob_emu::untilInput(int sequenceNumber) {
    // don't wait if the sequence number has changed
    if (sequenceNumber_ != sequenceNumber)
        return {};
    return {tasks_, Events::READABLE};
}

void RotaryKnob_emu::handle(Gui &gui) {
    auto result = gui.widget<GuiRotaryKnob>(id_, 24, 0.1f, haveButton_);
    if (result.delta) {
        counters_[0] += *result.delta;
        ++sequenceNumber_;
        notify(Events::READABLE);
    }
    if (result.button) {
        if (*result.button) {
            // button pressed
            ++counters_[1];
            ++sequenceNumber_;
            notify(Events::READABLE);

            // start timeout for long press
            loop_.invoke(callback_, 3s);
        } else {
            // cancel timeout for long press
            callback_.remove();
        }
    }
}

void RotaryKnob_emu::handleTimeout() {
    ++counters_[2];
    ++sequenceNumber_;
    notify(Events::READABLE);
}

} // namespace coco
