#include "RotaryButton_emu.hpp"
#include "GuiRotaryButton.hpp"
#include <iostream>


namespace coco {

RotaryButton_emu::RotaryButton_emu(Loop_emu &loop, bool haveButton, int id)
	: haveButton(haveButton), id(id)
{
	loop.guiHandlers.add(*this);
}

RotaryButton_emu::~RotaryButton_emu() {
}

void RotaryButton_emu::getState(const Array<int8_t> &state) {
	for (int i = 0; i < std::min(state.size(), 2); ++i)
		state[i] = this->state[i];
}

Awaitable<> RotaryButton_emu::stateChange() {
	return {this->changeTasks};
}

void RotaryButton_emu::handle(Gui &gui) {
	auto result = gui.widget<GuiRotaryButton>(this->id, this->haveButton);
	if (result.delta) {
		this->state[0] += *result.delta;
		this->changeTasks.doAll();
	}
	if (result.button && *result.button) {
		++this->state[1];
		this->changeTasks.doAll();
	}
}

} // namespace coco
