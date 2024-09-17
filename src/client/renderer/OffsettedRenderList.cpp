#include "client/renderer/OffsettedRenderList.h"

#include <iostream>

#include "OpenGL.h"

void OffsettedRenderList::init(int_t x, int_t y, int_t z, double xOff, double yOff, double zOff)
{
	inited = true;
	lists.clear();
	this->x = x;
	this->y = y;
	this->z = z;

	this->xOff = xOff;
	this->yOff = yOff;
	this->zOff = zOff;
}

bool OffsettedRenderList::isAt(int_t x, int_t y, int_t z)
{
	if (!inited) return false;
	return this->x == x && this->y == y && this->z == z;
}

void OffsettedRenderList::add(int_t list)
{
	lists.push_back(list);
	if (lists.size() == 0x10000) render();
}

void OffsettedRenderList::render()
{
	if (!inited) return;
	if (!rendered)
		rendered = true;

	if (!lists.empty())
	{
		glPushMatrix();
		glTranslatef(static_cast<float>(x) - xOff, static_cast<float>(y) - yOff, static_cast<float>(z) - zOff);
		glCallLists(lists.size(), GL_UNSIGNED_INT, lists.data());
		glPopMatrix();
	}
}

void OffsettedRenderList::clear()
{
	inited = false;
	rendered = false;
	lists.clear();
}
