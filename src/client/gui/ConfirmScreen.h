#pragma once

#include "client/gui/Screen.h"

class ConfirmScreen : public Screen
{
private:
	std::shared_ptr<Screen> parent;
	jstring title1;
	jstring title2;
	int_t id;

public:
	ConfirmScreen(Minecraft &minecraft, std::shared_ptr<Screen> parent, jstring title1, jstring title2, int_t id);

	void init() override;

protected:
	void buttonClicked(Button &button) override;

public:
	void render(int_t xm, int_t ym, float a) override;
};
