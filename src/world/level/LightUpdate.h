#pragma once

#include "world/level/LightLayer.h"

#include "java/Type.h"

class Level;

class LightUpdate
{
public:
	int_t layer = LightLayer::Sky;
	int_t x0 = 0, y0 = 0, z0 = 0;
	int_t x1 = 0, y1 = 0, z1 = 0;

	LightUpdate(int_t layer, int_t x0, int_t y0, int_t z0, int_t x1, int_t y1, int_t z1);

	void update(Level &level);
	bool expandToContain(int_t x0, int_t y0, int_t z0, int_t x1, int_t y1, int_t z1);
};
