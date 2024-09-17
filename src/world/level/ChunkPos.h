#pragma once

#include "java/Type.h"

#include "world/entity/Entity.h"

class ChunkPos
{
public:
	int_t x, z;

	TilePos(int_t x, int_t z) : x(x), z(z) {}

	// Hash and comparison functions for unordered_map
	bool operator==(const ChunkPos &other) const
	{
		return x == other.x && y == other.y && z == other.z;
	}

	size_t operator()() const
	{
		return (x << 8) | z;
	}

	double distanceToSqr(Entity &entity)
	{
		double cx = this.x * 16 + 8;
		double cz = this.z * 16 + 8;
		double dx = cx - entity.x;
		double dz = cz - entity.z;
		return dx * dx + dz * dz;
	}
};
