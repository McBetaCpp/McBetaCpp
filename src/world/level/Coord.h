#pragma once

#include "java/Type.h"

class Coord
{
public:
	int_t x = 0, y = 0, z = 0;

	Coord(int_t x, int_t y, int_t z) : x(x), y(y), z(z) {}
};
