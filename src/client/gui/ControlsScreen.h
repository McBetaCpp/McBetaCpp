#pragma once

#include "client/gui/Screen.h"

class ControlsScreen : public Screen
{
private:
	std::shared_ptr<Screen> lastScreen;
protected:
	jstring title = u"Controls";
private:
	Options &options;
	int_t selectedKey = -1;

public:
	ControlsScreen(Minecraft &minecraft, std::shared_ptr<Screen> lastScreen, Options &options);

private:
	int_t getLeftScreenPosition();

public:
	void init() override;
	static int_t BUTTON_WIDTH;
	static int_t ROW_WIDTH;
protected:
	void buttonClicked(Button &button) override;
	void keyPressed(char_t eventCharacter, int_t eventKey) override;
	void render(int_t xm, int_t ym, float a) override;
};
