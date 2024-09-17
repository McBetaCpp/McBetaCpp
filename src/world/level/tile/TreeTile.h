#pragma once

#include "world/level/tile/Tile.h"

class TreeTile : public Tile
{
public:
	static constexpr int_t DARK_TRUNK = 1;
	static constexpr int_t BIRCH_TRUNK = 2;

	TreeTile(int_t id);
	
	int_t getResourceCount(Random &random) override;
	int_t getResource(int_t data, Random &random) override;

	void onRemove(Level &level, int_t x, int_t y, int_t z) override;

	int_t getTexture(Facing face, int_t data) override;

	int_t getSpawnResourcesAuxValue(int_t data) override;
};
