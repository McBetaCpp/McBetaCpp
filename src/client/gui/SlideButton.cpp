#include "client/gui/SlideButton.h"

#include "client/Minecraft.h"

#include "OpenGL.h"

SlideButton::SlideButton(int_t id, int_t x, int_t y, Options::Option::Element *option, const jstring &msg, float value) : Button(id, x, y, 150, 20, msg)
{
	this->option = option;
	this->value = value;
}

int_t SlideButton::getYImage(bool hovered)
{
	return 0;
}

void SlideButton::renderBg(Minecraft &minecraft, int_t xm, int_t ym)
{
	if (!visible)
		return;

	if (sliding)
	{
		value = static_cast<float>(xm - (x + 4)) / static_cast<float>(w - 8);
		if (value < 0.0f) value = 0.0f;
		if (value > 1.0f) value = 1.0f;
		minecraft.options.set(*option, value);
		msg = minecraft.options.getMessage(*option);
	}

	glColor4f(1.0F, 1.0F, 1.0F, 1.0F);
	blit(x + (int)(value * (w - 8)), y, 0, 66, 4, 20);
	blit(x + (int)(value * (w - 8)) + 4, y, 196, 66, 4, 20);
}

bool SlideButton::clicked(Minecraft &minecraft, int_t xm, int_t xy)
{
	if (Button::clicked(minecraft, xm, xy))
	{
		value = static_cast<float>(xm - (x + 4)) / static_cast<float>(w - 8);
		if (value < 0.0f) value = 0.0f;
		if (value > 1.0f) value = 1.0f;
		minecraft.options.set(*option, value);
		msg = minecraft.options.getMessage(*option);
		sliding = true;
		return true;
	}
	return false;
}

void SlideButton::released(int_t mx, int_t my)
{
	sliding = false;
}
