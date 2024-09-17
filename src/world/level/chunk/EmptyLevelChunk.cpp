#include "world/level/chunk/EmptyLevelChunk.h"

#include "world/level/Level.h"

EmptyLevelChunk::EmptyLevelChunk(Level &level, int_t x, int_t z) : LevelChunk(level, x, z)
{

}

bool EmptyLevelChunk::isAt(int_t x, int_t z)
{
	return this->x == x && this->z == z;
}

int_t EmptyLevelChunk::getHeightmap(int_t x, int_t z)
{
	return 0;
}

void EmptyLevelChunk::recalcBlockLights()
{

}

void EmptyLevelChunk::recalcHeightmapOnly()
{

}

void EmptyLevelChunk::recalcHeightmap()
{

}

void EmptyLevelChunk::lightLava()
{

}

int_t EmptyLevelChunk::getTile(int_t x, int_t y, int_t z)
{
	return 0;
}

bool EmptyLevelChunk::setTileAndData(int_t x, int_t y, int_t z, int_t tile, int_t data)
{
	return true;
}

bool EmptyLevelChunk::setTile(int_t x, int_t y, int_t z, int_t tile)
{
	return true;
}

int_t EmptyLevelChunk::getData(int_t x, int_t y, int_t z)
{
	return 0;
}

void EmptyLevelChunk::setData(int_t x, int_t y, int_t z, int_t data)
{

}

int_t EmptyLevelChunk::getBrightness(int_t layer, int_t x, int_t y, int_t z)
{
	return 0;
}

void EmptyLevelChunk::setBrightness(int_t layer, int_t x, int_t y, int_t z, int_t brightness)
{

}

int_t EmptyLevelChunk::getRawBrightness(int_t layer, int_t x, int_t y, int_t z)
{
	return 0;
}

void EmptyLevelChunk::addEntity(std::shared_ptr<Entity> entity)
{

}

void EmptyLevelChunk::removeEntity(std::shared_ptr<Entity> entity)
{

}

void EmptyLevelChunk::removeEntity(std::shared_ptr<Entity> entity, int_t y)
{

}

bool EmptyLevelChunk::isSkyLit(int_t x, int_t y, int_t z)
{
	return false;
}

void EmptyLevelChunk::skyBrightnessChanged()
{

}

std::shared_ptr<TileEntity> EmptyLevelChunk::getTileEntity(int_t x, int_t y, int_t z)
{
	return nullptr;
}

void EmptyLevelChunk::addTileEntity(std::shared_ptr<TileEntity> tileEntity)
{

}

void EmptyLevelChunk::setTileEntity(int_t x, int_t y, int_t z, std::shared_ptr<TileEntity> tileEntity)
{

}

void EmptyLevelChunk::removeTileEntity(int_t x, int_t y, int_t z)
{

}

void EmptyLevelChunk::load()
{

}

void EmptyLevelChunk::unload()
{

}

void EmptyLevelChunk::markUnsaved()
{

}

void EmptyLevelChunk::getEntities(Entity *ignore, AABB &aabb, std::vector<std::shared_ptr<Entity>> &entities)
{

}

void EmptyLevelChunk::getEntitiesOfCondition(bool (*condition)(Entity &), AABB &aabb, std::vector<std::shared_ptr<Entity>> &entities)
{

}

int_t EmptyLevelChunk::countEntities()
{
	return 0;
}

bool EmptyLevelChunk::shouldSave(bool force)
{
	return false;
}

void EmptyLevelChunk::setBlocks(byte_t *blocks, int_t y)
{

}

int_t EmptyLevelChunk::getBlocksAndData(byte_t *out, int_t x0, int_t y0, int_t z0, int_t x1, int_t y1, int_t z1)
{
	int_t w = x1 - x0;
	int_t h = y1 - y0;
	int_t d = z1 - z0;
	int_t size = w * h * d;
	int_t fsize = size + (size / 2 * 3);
	std::fill(out, out + fsize, 0);
	return fsize;
}

int_t EmptyLevelChunk::setBlocksAndData(byte_t *in, int_t x0, int_t y0, int_t z0, int_t x1, int_t y1, int_t z1)
{
	int_t w = x1 - x0;
	int_t h = y1 - y0;
	int_t d = z1 - z0;
	int_t size = w * h * d;
	int_t fsize = size + (size / 2 * 3);
	return fsize;
}

Random EmptyLevelChunk::getRandom(long_t xor)
{
	return Random(level.seed + (x * x * 4987142) + (x * 5947611) + (z * z) * 4392871LL + (z * 389711) ^ xor);
}

bool EmptyLevelChunk::isEmpty()
{
	return true;
}