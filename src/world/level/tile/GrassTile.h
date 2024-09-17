#pragma once

#include "world/level/tile/Tile.h"

class GrassTile : public Tile
{
public:
	static constexpr int_t MIN_BRIGHTNESS = 4;

	GrassTile(int_t id);

	int_t getTexture(LevelSource &level, int_t x, int_t y, int_t z, Facing face) override;
	int_t getColor(LevelSource &level, int_t x, int_t y, int_t z) override;

	void tick(Level &level, int_t x, int_t y, int_t z, Random &random) override;

	int_t getResource(int_t data, Random &random) override;
};
