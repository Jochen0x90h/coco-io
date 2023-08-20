#define _USE_MATH_DEFINES
#include "GuiRotaryButton.hpp"
#include <cmath>


namespace coco {


GuiRotaryButton::~GuiRotaryButton() {
}

GuiRotaryButton::Result GuiRotaryButton::update(Gui &gui) {
	// draw and resize the widget
	float angle = float(this->value & 0xffff) / (65536.0f * 24.0f) * 2.0f * float(M_PI);
	int offset = int(this->button) * 4;
	const float outerColor[] = {0.7f, 0.7f, 0.7f, 1.0f};
	const float innerColor[] = {0, 0, 0, 1,  1, 1, 1, 1};
	resize(gui.draw<Wheel>(angle, outerColor, this->haveButton ? innerColor + offset : outerColor));

	// calculate the result
	Result result;

	// delta
	auto value = this->value & 0xffff0000;
	int delta = int32_t(value - this->lastValue) >> 16;
	this->lastValue = value;
	if (delta != 0)
		result.delta = delta;

	// button
	if (this->haveButton) {
		bool toggle = this->button != this->lastButton;
		this->lastButton = this->button;
		if (toggle)
			result.button = this->button;
	}

	return result;
}

void GuiRotaryButton::touch(bool first, float x, float y) {
	float ax = x - 0.5f;
	float ay = y - 0.5f;
	bool inner = std::sqrt(ax * ax + ay * ay) < 0.1;
	if (first) {
		// check if button (inner circle) was hit
		this->button = inner;
	} else if (!inner) {
		float bx = this->x - 0.5f;
		float by = this->y - 0.5f;

		float d = (ay * bx - ax * by) / (std::sqrt(ax * ax + ay * ay) * std::sqrt(bx * bx + by * by));
		this->value = this->value - int(d * 10000 * 24);
	}
	this->x = x;
	this->y = y;
}

void GuiRotaryButton::release() {
	this->button = false;
}


// Wheel

GuiRotaryButton::Wheel::Wheel()
	: Gui::Renderer("#version 330\n"
		"uniform float angle;\n"
		"uniform vec4 outerColor;\n"
		"uniform vec4 innerColor;\n"
		"in vec2 xy;\n"
		"out vec4 pixel;\n"
		"void main() {\n"
		"vec2 p = xy - vec2(0.5, 0.5f);\n"
		"float a = atan(p.y, p.x) + angle;\n"
		"float radius = sqrt(p.x * p.x + p.y * p.y);\n"
		"float outerRadius = cos(a * 24) * 0.02 + 0.48;\n"
		"float innerRadius = 0.1;\n"
		"float delta = 0.01;\n"
		"float outerMix = smoothstep(outerRadius - delta, outerRadius, radius);\n"
		"float innerMix = smoothstep(innerRadius - delta, innerRadius, radius);\n"
		"vec4 color = (1.0 - innerMix) * innerColor + innerMix * outerColor;\n"
		"pixel = (1.0 - outerMix) * color + outerMix * vec4(0, 0, 0, 1);\n"
		"}\n")
{
	this->angleUniform = getUniformLocation("angle");
	this->outerColorUniform = getUniformLocation("outerColor");
	this->innerColorUniform = getUniformLocation("innerColor");
}

Gui::Size GuiRotaryButton::Wheel::draw(float x, float y, float angle, const float *outerColor, const float *innerColor) {
	const float w = 0.2f;
	const float h = 0.2f;

	setState(x, y, w, h);
	glUniform1f(this->angleUniform, angle);
	glUniform4fv(this->outerColorUniform, 1, outerColor);
	glUniform4fv(this->innerColorUniform, 1, innerColor);
	drawAndResetState();

	return {w, h};
}

} // namespace coco
