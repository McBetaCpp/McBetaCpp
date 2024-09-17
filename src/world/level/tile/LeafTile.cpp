#include "world/level/tile/LeafTile.h"

#include "world/level/Level.h"

#include "world/level/FoliageColor.h"

LeafTile::LeafTile(int_t id, int_t tex) : TransparentTile(id, tex, Material::wood, false)
{
	oTex = tex;
	setTicking(true);
}

int_t LeafTile::getColor(LevelSource &level, int_t x, int_t y, int_t z)
{
	int_t data = level.getData(x, y, z);
	if ((data & EVERGREEN_LEAF) == EVERGREEN_LEAF)
		return FoliageColor::getEvergreenColor();
	if ((data & BIRCH_LEAF) == BIRCH_LEAF)
		return FoliageColor::getBirchColor();

	level.getBiomeSource().getBiomeBlock(x, z, 1, 1);
	double temperature = level.getBiomeSource().temperatures[0];
	double downfall = level.getBiomeSource().downfalls[0];

	return FoliageColor::get(temperature, downfall);
}

void LeafTile::onRemove(Level &level, int_t x, int_t y, int_t z)
{

}

void LeafTile::tick(Level &level, int_t x, int_t y, int_t z, Random &random)
{

}

void LeafTile::die(Level &level, int_t x, int_t y, int_t z)
{

}

int_t LeafTile::getResourceCount(Random &random)
{
	return (random.nextInt(16) == 0) ? 1 : 0;
}
int_t LeafTile::getResource(int_t data, Random &random)
{
	// TODO: sapling
	return id;
}

bool LeafTile::isSolidRender()
{
	return !allowSame;
}

int_t LeafTile::getTexture(Facing face, int_t data)
{
	return ((data & LEAF_TYPE_MASK) == EVERGREEN_LEAF) ? (tex + 80) : tex;
}

void LeafTile::setFancy(bool fancy)
{
	allowSame = fancy;
	tex = oTex + (fancy ? 0 : 1);
}

void LeafTile::stepOn(Level &level, int_t x, int_t y, int_t z, Entity &entity)
{
	Tile::stepOn(level, x, y, z, entity);
}
