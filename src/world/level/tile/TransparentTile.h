#pragma once

#include "world/level/tile/Tile.h"

class TransparentTile : public Tile
{
protected:
	bool allowSame = false;

public:
	TransparentTile(int_t id, int_t tex, const Material &material, bool allowSame);

	virtual bool isSolidRender() override;

	virtual bool shouldRenderFace(LevelSource &level, int_t x, int_t y, int_t z, Facing face) override;

	virtual bool blocksLight();
};
