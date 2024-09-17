#include "world/level/tile/TreeTile.h"

#include "world/level/Level.h"

TreeTile::TreeTile(int_t id) : Tile(id, Material::wood)
{

}

int_t TreeTile::getResourceCount(Random &random)
{
	return 1;
}

int_t TreeTile::getResource(int_t data, Random &random)
{
	return id;
}

void TreeTile::onRemove(Level &level, int_t x, int_t y, int_t z)
{
	if (level.isOnline)
		return;

	// Update leaves? TODO
}

int_t TreeTile::getTexture(Facing face, int_t data)
{
	return (face == Facing::UP) ? 21 : ((face == Facing::DOWN) ? 21 : ((data == DARK_TRUNK) ? 116 : ((data == BIRCH_TRUNK) ? 117 : 20)));
}

int_t TreeTile::getSpawnResourcesAuxValue(int_t data)
{
	return data;
}
