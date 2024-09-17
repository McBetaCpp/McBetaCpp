#pragma once

#include "client/gui/GuiComponent.h"

#include "java/Random.h"

class Minecraft;

class Gui : public GuiComponent
{
private:
	static constexpr int_t MAX_MESSAGE_WIDTH = 320;

	Random random;

	int_t tickCount = 0;

	jstring nowPlayingString = u"";
	int_t nowPlayingTime = 0;

	Minecraft &minecraft;

public:
	float progress = 0.0f;
	float tbr = 1.0f;

	Gui(Minecraft &minecraft);
	void render(float a, bool inScreen, int_t xm, int_t ym);

	void tick();
};
