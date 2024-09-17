#include "client/gui/Button.h"

#include "client/Minecraft.h"
#include "client/renderer/Tesselator.h"

Button::Button(int_t id, int_t x, int_t y, jstring msg) : Button(id, x, y, 200, 20, msg)
{

}

Button::Button(int_t id, int_t x, int_t y, int_t w, int_t h, jstring msg)
{
	this->id = id;
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;
	this->msg = msg;
}

int_t Button::getYImage(bool hovered)
{
	int_t res = 1;
	if (!active) res = 0;
	else if (hovered) res = 2;
	return res;
}

void Button::render(Minecraft &minecraft, int_t xm, int_t ym)
{
	if (!visible)
		return;

	Font *font = minecraft.font.get();
	if (!font)
		return;

	glBindTexture(GL_TEXTURE_2D, minecraft.textures.loadTexture(u"/gui/gui.png"));
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	bool hovered = xm >= x && ym >= y && xm < x + w && ym < y + h;
	int_t yImage = getYImage(hovered);

	blit(x, y, 0, 46 + yImage * 20, w / 2, h);
	blit(x + w / 2, y, 200 - w / 2, 46 + yImage * 20, w / 2, h);

	renderBg(minecraft, xm, ym);

	if (!active)
		drawCenteredString(*font, msg, x + w / 2, y + (h - 8) / 2, 0xFFA0A0A0);
	else if (hovered)
		drawCenteredString(*font, msg, x + w / 2, y + (h - 8) / 2, 0xFFFFA0);
	else
		drawCenteredString(*font, msg, x + w / 2, y + (h - 8) / 2, 0xE0E0E0);

}

void Button::renderBg(Minecraft &minecraft, int_t xm, int_t ym)
{

}

void Button::released(int_t mx, int_t my)
{

}

bool Button::clicked(Minecraft &minecraft, int_t mx, int_t my)
{
	return active && mx >= x && my >= y && mx < x + w && my < y + h;
}
