#include "world/level/Region.h"

#include "world/level/Level.h"
#include "world/level/material/GasMaterial.h"

Region::Region(Level &level, int_t x0, int_t y0, int_t z0, int_t x1, int_t y1, int_t z1) : level(level)
{
	xc1 = x0 >> 4;
	zc1 = z0 >> 4;
	int_t xc2 = x1 >> 4;
	int_t zc2 = z1 >> 4;
	
	int_t w = xc2 - xc1 + 1;
	h = zc2 - zc1 + 1;

	chunks.resize(w * h);
	for (int_t x = xc1; x <= xc2; x++)
		for (int_t z = zc1; z <= zc2; z++)
			chunks[(x - xc1) * h + (z - zc1)] = level.getChunk(x, z);
}

int_t Region::getTile(int_t x, int_t y, int_t z)
{
	if (y < 0)
		return 0;
	if (y >= Level::DEPTH)
		return 0;

	int_t xc = (x >> 4) - xc1;
	int_t zc = (z >> 4) - zc1;
	std::shared_ptr<LevelChunk> chunk = chunks[xc * h + zc];
	return (chunk == nullptr) ? 0 : chunk->getTile(x & 0xF, y, z & 0xF);
}

std::shared_ptr<TileEntity> Region::getTileEntity(int_t x, int_t y, int_t z)
{
	int_t xc = (x >> 4) - xc1;
	int_t zc = (z >> 4) - zc1;
	return chunks[xc * h + zc]->getTileEntity(x & 0xF, y, z & 0xF);
}

float Region::getBrightness(int_t x, int_t y, int_t z)
{
	return level.dimension->brightnessRamp[getRawBrightness(x, y, z)];
}

int_t Region::getRawBrightness(int_t x, int_t y, int_t z)
{
	return getRawBrightness(x, y, z, true);
}

int_t Region::getRawBrightness(int_t x, int_t y, int_t z, bool neighbors)
{
	if (x < -Level::MAX_LEVEL_SIZE || z < -Level::MAX_LEVEL_SIZE || x >= Level::MAX_LEVEL_SIZE || z >= Level::MAX_LEVEL_SIZE)
		return 15;

	if (neighbors)
	{
		int_t tile = getTile(x, y, z);
		// TODO
		// stoneSlabHalf farmland
	}

	if (y < 0)
		return 0;

	if (y >= Level::DEPTH)
	{
		int_t l = 15 - level.skyDarken;
		if (l < 0)
			l = 0;
		return l;
	}

	int_t xc = (x >> 4) - xc1;
	int_t zc = (z >> 4) - zc1;
	return chunks[xc * h + zc]->getRawBrightness(x & 0xF, y, z & 0xF, level.skyDarken);
}

int_t Region::getData(int_t x, int_t y, int_t z)
{
	if (y < 0)
		return 0;
	if (y >= Level::DEPTH)
		return 0;
	int_t xc = (x >> 4) - xc1;
	int_t zc = (z >> 4) - zc1;
	return chunks[xc * h + zc]->getData(x & 0xF, y, z & 0xF);
}

const Material &Region::getMaterial(int_t x, int_t y, int_t z)
{
	int_t tile = getTile(x, y, z);
	return (tile == 0) ? Material::air : Tile::tiles[tile]->material;
}

bool Region::isSolidTile(int_t x, int_t y, int_t z)
{
	Tile *tile = Tile::tiles[getTile(x, y, z)];
	return (tile == nullptr) ? false : tile->isSolidRender();
}

BiomeSource &Region::getBiomeSource()
{
	return level.getBiomeSource();
}
