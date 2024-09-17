#pragma once

#include "client/gui/Button.h"

#include "client/Options.h"

class SlideButton : public Button
{
public:
	float value = 1.0f;
	bool sliding = false;
	
private:
	Options::Option::Element *option = nullptr;

public:
	SlideButton(int_t id, int_t x, int_t y, Options::Option::Element *option, const jstring &msg, float value);

protected:
	int_t getYImage(bool hovered) override;

	void renderBg(Minecraft &minecraft, int_t xm, int_t ym) override;

public:
	bool clicked(Minecraft &minecraft, int_t xm, int_t xy) override;
	void released(int_t mx, int_t my) override;
};
