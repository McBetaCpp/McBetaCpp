#include "world/level/tile/GrassTile.h"

#include "world/level/tile/DirtTile.h"

#include "world/level/LevelSource.h"
#include "world/level/GrassColor.h"

GrassTile::GrassTile(int_t id) : Tile(id, Material::dirt)
{
	
}

int_t GrassTile::getTexture(LevelSource &level, int_t x, int_t y, int_t z, Facing face)
{
	if (face == Facing::UP)
		return 0;
	else if (face == Facing::DOWN)
		return 2;
	// TODO: snow check
	return 3;
}

int_t GrassTile::getColor(LevelSource &level, int_t x, int_t y, int_t z)
{
	level.getBiomeSource().getBiomeBlock(x, z, 1, 1);
	double temperature = level.getBiomeSource().temperatures[0];
	double downfall = level.getBiomeSource().downfalls[0];
	return GrassColor::get(temperature, downfall);
}

void GrassTile::tick(Level &level, int_t x, int_t y, int_t z, Random &random)
{
	
}

int_t GrassTile::getResource(int_t data, Random &random)
{
	return Tile::dirt.id;
}
