#pragma once

#include <coco/platform/Gui.hpp>


namespace coco {

/**
	Rotary button (incremental encoder knob with push button) on the emulator gui.
	Usage: gui.widget<GuiRotaryButton>(id, haveButton);
*/
class GuiRotaryButton : public Gui::Widget {
public:
	struct Result {
		std::optional<int> delta;
		std::optional<bool> button;
	};

	/**
		Constructor
		@param haveButton true to add a push button inside the rotary knob
	*/
	GuiRotaryButton(bool haveButton) : haveButton(haveButton), value(0), lastValue(0) {}
	~GuiRotaryButton() override;

	Result update(Gui &gui);

	void touch(bool first, float x, float y) override;
	void release() override;

protected:
	// renderer for a wheel with button
	class Wheel : public Gui::Renderer {
	public:
		Wheel();

		Gui::Size draw(float x, float y, float angle, const float *outerColor, const float *innerColor);

	protected:
		GLint angleUniform;
		GLint outerColorUniform;
		GLint innerColorUniform;
	};

	bool haveButton;

	// current (mechanical) increment count in 16:16 format
	uint32_t value;
	uint32_t lastValue;

	// current button state
	bool button = false;
	bool lastButton = false;

	// last mouse position
	float x = 0;
	float y = 0;
};

} // namespace coco
