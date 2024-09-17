#pragma once

#include <string>

#include "java/Type.h"
#include "java/String.h"

#include "client/gui/Font.h"

#include "util/Memory.h"

class GuiComponent
{
protected:
	float blitOffset = 0.0f;

	void fill(int_t x0, int_t y0, int_t x1, int_t y1, int_t col);
	void fillGradient(int_t x0, int_t y0, int_t x1, int_t y1, int_t col1, int_t col2);

public:
	virtual ~GuiComponent() {}

	void drawCenteredString(Font &font, const jstring &str, int_t x, int_t y, int_t color);
	void drawString(Font &font, const jstring &str, int_t x, int_t y, int_t color);
	void blit(int_t x, int_t y, int_t sx, int_t sy, int_t w, int_t h);
};
