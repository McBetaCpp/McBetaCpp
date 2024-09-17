#pragma once

#include "world/level/tile/Tile.h"

#include "java/Type.h"

class StoneTile : public Tile
{
public:
	StoneTile(int_t id, int_t texture);
	int_t getResource(int_t data, Random &random) override;
};