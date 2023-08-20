#include <coco/Input.hpp>
#include <coco/platform/Loop_emu.hpp>
#include <string>


namespace coco {

/**
	Implementation of an emulated rotary button (rotary knob with push button)
 */
class RotaryButton_emu : public Input, public Loop_emu::GuiHandler {
public:
	/**
		Constructor
		@param loop event loop
		@param id unique id for gui
	*/
	RotaryButton_emu(Loop_emu &loop, bool haveButton, int id);
	~RotaryButton_emu() override;

	void getState(const Array<int8_t> &state) override;
	[[nodiscard]] Awaitable<> stateChange() override;

protected:
	void handle(Gui &gui) override;

	bool haveButton;
	int id;
	int8_t state[2] = {};
	CoroutineTaskList<> changeTasks;
};

} // namespace coco
