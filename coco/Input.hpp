#pragma once

#include <coco/Array.hpp>
#include <coco/Coroutine.hpp>
#include <cstdint>


namespace coco {

/**
	Interface for digital inputs such as buttons and rotary knobs.
	Each element has state represented by an integer that can count state changes, button pressess or rotary knob
	movements. This way no input action is missed even if the interface is queried every now and then (principle is
	inspired by Microsoft's GameInput)
*/
class Input {
public:

	virtual ~Input() {}

	/**
		Get the current state of the buttons
		@param state state of buttons
	*/
	virtual void getState(const Array<int8_t> &state) = 0;

	/**
		Wait for a state change
	*/
	[[nodiscard]] virtual Awaitable<> stateChange() = 0;
};

} // namespace coco
