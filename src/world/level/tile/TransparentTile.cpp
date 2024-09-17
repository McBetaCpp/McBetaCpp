#include "world/level/tile/TransparentTile.h"

#include "world/level/Level.h"

TransparentTile::TransparentTile(int_t id, int_t tex, const Material &material, bool allowSame) : Tile(id, tex, material)
{
	this->allowSame = allowSame;
}

bool TransparentTile::isSolidRender()
{
	return false;
}

bool TransparentTile::shouldRenderFace(LevelSource &level, int_t x, int_t y, int_t z, Facing face)
{
	int_t t = level.getTile(x, y, z);
	if (!allowSame && t == id)
		return false;
	return Tile::shouldRenderFace(level, x, y, z, face);
}

bool TransparentTile::blocksLight()
{
	return false;
}
