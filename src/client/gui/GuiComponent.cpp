#include "client/gui/GuiComponent.h"

#include "client/renderer/Tesselator.h"

void GuiComponent::fill(int_t x0, int_t y0, int_t x1, int_t y1, int_t col)
{
	float a = ((col >> 24) & 0xFF) / 255.0f;
	float r = ((col >> 16) & 0xFF) / 255.0f;
	float g = ((col >> 8) & 0xFF) / 255.0f;
	float b = (col & 0xFF) / 255.0f;

	Tesselator &t = Tesselator::instance;

	glEnable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(r, g, b, a);

	t.begin();
	t.vertex(x0, y1, 0.0);
	t.vertex(x1, y1, 0.0);
	t.vertex(x1, y0, 0.0);
	t.vertex(x0, y0, 0.0);
	t.end();

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

void GuiComponent::fillGradient(int_t x0, int_t y0, int_t x1, int_t y1, int_t col1, int_t col2)
{
	float a1 = (col1 >> 24 & 0xFF) / 255.0F;
	float r1 = (col1 >> 16 & 0xFF) / 255.0F;
	float g1 = (col1 >> 8 & 0xFF) / 255.0F;
	float b1 = (col1 & 0xFF) / 255.0F;
	
	float a2 = (col2 >> 24 & 0xFF) / 255.0F;
	float r2 = (col2 >> 16 & 0xFF) / 255.0F;
	float g2 = (col2 >> 8 & 0xFF) / 255.0F;
	float b2 = (col2 & 0xFF) / 255.0F;

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glShadeModel(GL_SMOOTH);
	
	Tesselator &t = Tesselator::instance;
	t.begin();
	t.color(r1, g1, b1, a1);
	t.vertex(x1, y0, 0.0);
	t.vertex(x0, y0, 0.0);
	t.color(r2, g2, b2, a2);
	t.vertex(x0, y1, 0.0);
	t.vertex(x1, y1, 0.0);
	t.end();
	
	glShadeModel(GL_FLAT);
	glDisable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_TEXTURE_2D);
}

void GuiComponent::drawCenteredString(Font &font, const jstring &str, int_t x, int_t y, int_t color)
{
	font.drawShadow(str, x - font.width(str) / 2, y, color);
}

void GuiComponent::drawString(Font &font, const jstring &str, int_t x, int_t y, int_t color)
{
	font.drawShadow(str, x, y, color);
}

void GuiComponent::blit(int_t x, int_t y, int_t sx, int_t sy, int_t w, int_t h)
{
	const float us = 1.0f / 256.0f;
	const float vs = 1.0f / 256.0f;
	Tesselator &t = Tesselator::instance;
	t.begin();
	t.vertexUV(x + 0, y + h, 0.0, (sx + 0) * us, (sy + h) * vs);
	t.vertexUV(x + w, y + h, 0.0, (sx + w) * us, (sy + h) * vs);
	t.vertexUV(x + w, y + 0, 0.0, (sx + w) * us, (sy + 0) * vs);
	t.vertexUV(x + 0, y + 0, 0.0, (sx + 0) * us, (sy + 0) * vs);
	t.end();
}
