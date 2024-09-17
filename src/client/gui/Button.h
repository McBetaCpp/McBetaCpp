#pragma once

#include <string>

#include "client/gui/GuiComponent.h"

#include "java/Type.h"

class Minecraft;

class Button : public GuiComponent
{
protected:
	int_t w = 200;
	int_t h = 20;

public:
	int_t x = 0;
	int_t y = 0;

	jstring msg;

	Button(int_t id, int_t x, int_t y, jstring msg);

	int_t id = 0;
	bool active = true;
	bool visible = true;

	Button(int_t id, int_t x, int_t y, int_t w, int_t h, jstring msg);

protected:
	virtual int_t getYImage(bool hovered);

public:
	virtual void render(Minecraft &minecraft, int_t xm, int_t ym);

protected:
	virtual void renderBg(Minecraft &minecraft, int_t xm, int_t ym);

public:
	virtual void released(int_t mx, int_t my);

	virtual bool clicked(Minecraft &minecraft, int_t mx, int_t my);

	virtual bool isSmallButton() const { return false; }
};
