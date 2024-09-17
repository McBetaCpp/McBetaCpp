#pragma once

#include "java/Type.h"

struct TilePos
{
	int_t x, y, z;

	TilePos(int_t x, int_t y, int_t z) : x(x), y(y), z(z) {}

	// bool operator==(const TilePos &other) const
	// {
	// 	return x == other.x && y == other.y && z == other.z;
	// }
};

template <>
struct std::hash<TilePos>
{
	std::size_t operator()(const TilePos &k) const
	{
		return k.x * 8976890 + k.y * 981131 + k.z;
	}
};
