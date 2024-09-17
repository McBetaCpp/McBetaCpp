#pragma once

#include "client/gui/Screen.h"

class PauseScreen : public Screen
{
private:
	int_t saveStep = 0;
	int_t visibleTime = 0;

public:
	PauseScreen(Minecraft &minecraft);

	void init() override;

protected:
	void buttonClicked(Button &button) override;

public:
	void tick() override;
	void render(int_t xm, int_t ym, float a) override;
};
