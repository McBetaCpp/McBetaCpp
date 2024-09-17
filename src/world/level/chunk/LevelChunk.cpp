#include "world/level/chunk/LevelChunk.h"

#include "world/level/Level.h"
#include "world/level/tile/Tile.h"

bool LevelChunk::touchedSky = false;

LevelChunk::LevelChunk(Level &level, int_t x, int_t z) : level(level)
{
	this->x = x;
	this->z = z;
}

LevelChunk::LevelChunk(Level &level, const ubyte_t *blocks, int_t x, int_t z) : LevelChunk(level, x, z)
{
	std::copy(blocks, blocks + this->blocks.size(), this->blocks.begin());
}

bool LevelChunk::isAt(int_t x, int_t z)
{
	return (this->x == x) && (this->z == z);
}

int_t LevelChunk::getHeightmap(int_t x, int_t z)
{
	// recalcHeightmapOnly();
	return heightmap[(z * 16) | x];
}

void LevelChunk::recalcBlockLights()
{

}

void LevelChunk::recalcHeightmapOnly()
{
	int_t min = Level::DEPTH - 1;

	for (int_t x = 0; x < 16; x++)
	{
		for (int_t z = 0; z < 16; z++)
		{
			int_t y = Level::DEPTH - 1;
			int_t i = ((x * 16) + z) * Level::DEPTH;
			while (y > 0 && Tile::lightBlock[blocks[i + y - 1]] == 0)
				y--;
			heightmap[(z * 16) | x] = y;
			if (y < min)
				min = y;
		}
	}

	minHeight = min;
	unsaved = true;
}

void LevelChunk::recalcHeightmap()
{
	int_t min = Level::DEPTH - 1;

	for (int_t x = 0; x < 16; x++)
	{
		for (int_t z = 0; z < 16; z++)
		{
			// Calculate height map value
			int_t y = Level::DEPTH - 1;
			int_t i = ((x * 16) + z) * Level::DEPTH;
			while (y > 0 && Tile::lightBlock[blocks[i + y - 1]] == 0)
				y--;
			heightmap[(z * 16) | x] = y;
			if (y < min)
				min = y;

			// Populate sky light values
			if (!level.dimension->hasCeiling)
			{
				int_t light = 15;
				int_t ly = Level::DEPTH - 1;
				do
				{
					light -= Tile::lightBlock[static_cast<ubyte_t>(blocks[i + ly])];
					if (light > 0)
						skyLight.set(x, ly, z, light);
				} while (--ly > 0 && light > 0);
			}
		}
	}

	minHeight = min;

	for (int_t x = 0; x < 16; x++)
		for (int_t z = 0; z < 16; z++)
			lightGaps(x, z);

	unsaved = true;
}

void LevelChunk::lightLava()
{
	int_t my = 32;

	for (int_t x = 0; x < 16; x++)
	{
		for (int_t z = 0; z < 16; z++)
		{
			int_t i = ((x * 16) + z) * Level::DEPTH;

			// Set light emissions
			for (int_t y = 0; y < Level::DEPTH; y++)
			{
				int_t emission = Tile::lightEmission[blocks[i + y]];
				if (emission > 0)
					blockLight.set(x, y, z, emission);
			}

			// Propogate upwards
			int_t light = 15;
			for (int_t y = my - 2; my < Level::DEPTH && light > 0;)
			{
				y++;

				ubyte_t b = blocks[i + y];
				int_t block = Tile::lightBlock[b];
				int_t emission = Tile::lightEmission[b];

				if (block == 0)
					block = 1;
				light -= block;
				if (emission > light)
					light = emission;
				if (light < 0)
					light = 0;

				blockLight.set(x, y, z, light);
			}
		}
	}

	level.updateLight(LightLayer::Block, x * 16, my - 1, z * 16, x * 16 + 16, my + 1, z * 16 + 16);
	unsaved = true;
}

void LevelChunk::lightGaps(int_t x, int_t z)
{
	int_t height = getHeightmap(x, z);
	int_t wx = this->x * 16 + x;
	int_t wz = this->z * 16 + z;
	lightGap(wx - 1, wz, height);
	lightGap(wx + 1, wz, height);
	lightGap(wx, wz - 1, height);
	lightGap(wx, wz + 1, height);
}

void LevelChunk::lightGap(int_t x, int_t z, int_t otherHeight)
{
	int_t levelHeight = level.getHeightmap(x, z);
	if (levelHeight > otherHeight)
	{
		level.updateLight(LightLayer::Sky, x, otherHeight, z, x, levelHeight, z);
		unsaved = true;
	}
	else if (levelHeight < otherHeight)
	{
		level.updateLight(LightLayer::Sky, x, levelHeight, z, x, otherHeight, z);
		unsaved = true;
	}
}

void LevelChunk::recalcHeight(int_t x, int_t y, int_t z)
{
	int_t oldHeight = heightmap[(z * 16) | x];
	int_t newHeight = oldHeight;
	if (y > oldHeight)
		newHeight = y;

	int_t ri = (x * (16 * 128)) | (z * 128);
	while (newHeight > 0 && Tile::lightBlock[blocks[ri + newHeight - 1]] == 0)
		newHeight--;

	if (newHeight == oldHeight)
		return;

	level.lightColumnChanged(x, z, newHeight, oldHeight);
	heightmap[(z * 16) | x] = newHeight;

	if (newHeight < minHeight)
	{
		minHeight = newHeight;
	}
	else
	{
		int_t iy = Level::DEPTH - 1;
		for (int_t x = 0; x < 16; x++)
			for (int_t z = 0; z < 16; z++)
				if (heightmap[(z * 16) | x] < iy)
					iy = heightmap[(z * 16) | x];
		minHeight = iy;
	}

	int_t wx = this->x * 16 + x;
	int_t wz = this->z * 16 + z;
	if (newHeight < oldHeight)
	{
		for (int_t yi = newHeight; yi < oldHeight; yi++)
			skyLight.set(x, yi, z, 15);
	}
	else
	{
		level.updateLight(LightLayer::Sky, wx, oldHeight, wz, wx, newHeight, wz);
		for (int_t yi = oldHeight; yi < newHeight; yi++)
			skyLight.set(x, yi, z, 0);
	}

	int_t light = 15;
	int_t ly = newHeight;
	while (newHeight > 0 && light > 0)
	{
		--newHeight;

		int_t block = Tile::lightBlock[getTile(x, newHeight, z)];
		if (block == 0)
			block = 1;
		light -= block;
		if (light <= 0)
			light = 0;
		skyLight.set(x, newHeight, z, light);
	}

	while (newHeight > 0 && Tile::lightBlock[getTile(x, newHeight - 1, z)] == 0)
		newHeight--;
	if (newHeight != ly)
		level.updateLight(LightLayer::Sky, wx - 1, newHeight, wz - 1, wx + 1, ly, wz + 1);

	unsaved = true;
}

int_t LevelChunk::getTile(int_t x, int_t y, int_t z)
{
	return blocks[(x * (16 * 128)) | (z * 128) | y];
}

bool LevelChunk::setTileAndData(int_t x, int_t y, int_t z, int_t tile, int_t data)
{
	return false;
}

bool LevelChunk::setTile(int_t x, int_t y, int_t z, int_t tile)
{
	int_t oldHeight = heightmap[(z * 16) | x];
	int_t oldTile = blocks[(x * (16 * 128)) | (z * 128) | y];
	if (oldTile == tile)
		return false;

	int_t wx = this->x * 16 + x;
	int_t wz = this->z * 16 + z;
	blocks[(x * 128 * 16) | (z * 128) | y] = tile;
	if (oldTile != 0)
		Tile::tiles[oldTile]->onRemove(level, wx, y, wz);
	data.set(x, y, z, 0);

	if (Tile::lightBlock[tile] != 0)
	{
		if (y >= oldHeight)
			recalcHeight(x, y + 1, z);
	}
	else if (y == oldHeight - 1)
	{
		recalcHeight(x, y, z);
	}

	level.updateLight(LightLayer::Sky, wx, y, wz, wx, y, wz);
	level.updateLight(LightLayer::Block, wx, y, wz, wx, y, wz);
	lightGaps(x, z);
	if (tile != 0 && !level.isOnline)
		Tile::tiles[tile]->onPlace(level, wx, y, wz);

	unsaved = true;
	return true;
}

int_t LevelChunk::getData(int_t x, int_t y, int_t z)
{
	return data.get(x, y, z);
}

void LevelChunk::setData(int_t x, int_t y, int_t z, int_t data)
{
	unsaved = true;
	this->data.set(x, y, z, data);
}

int_t LevelChunk::getBrightness(int_t lightLayer, int_t x, int_t y, int_t z)
{
	if (lightLayer == LightLayer::Sky)
		return skyLight.get(x, y, z);
	if (lightLayer == LightLayer::Block)
		return blockLight.get(x, y, z);
	return 0;
}

void LevelChunk::setBrightness(int_t lightLayer, int_t x, int_t y, int_t z, int_t brightness)
{
	unsaved = true;
	if (lightLayer == LightLayer::Sky)
		skyLight.set(x, y, z, brightness);
	else if (lightLayer == LightLayer::Block)
		blockLight.set(x, y, z, brightness);
}

int_t LevelChunk::getRawBrightness(int_t x, int_t y, int_t z, int_t darken)
{
	int_t light = skyLight.get(x, y, z);
	if (light > 0)
		touchedSky = true;
	light -= darken;

	int_t lightBlock = blockLight.get(x, y, z);
	if (lightBlock > light)
		light = lightBlock;

	return light;
}

void LevelChunk::addEntity(std::shared_ptr<Entity> entity)
{

}

void LevelChunk::removeEntity(std::shared_ptr<Entity> entity)
{

}

void LevelChunk::removeEntity(std::shared_ptr<Entity> entity, int_t y)
{

}

bool LevelChunk::isSkyLit(int_t x, int_t y, int_t z)
{
	return y >= heightmap[(z * 16) | x];
}

void LevelChunk::skyBrightnessChanged()
{
	int_t x0 = x * 16;
	int_t y0 = minHeight - 16;
	int_t z0 = z * 16;
	int_t x1 = x * 16 + 16;
	int_t y1 = Level::DEPTH;
	int_t z1 = z * 16 + 16;
	level.setTilesDirty(x0, y0, z0, x1, y1, z1);
}

std::shared_ptr<TileEntity> LevelChunk::getTileEntity(int_t x, int_t y, int_t z)
{
	return nullptr;
}

void LevelChunk::addTileEntity(std::shared_ptr<TileEntity> tileEntity)
{

}

void LevelChunk::setTileEntity(int_t x, int_t y, int_t z, std::shared_ptr<TileEntity> tileEntity)
{

}

void LevelChunk::removeTileEntity(int_t x, int_t y, int_t z)
{

}

void LevelChunk::load()
{

}

void LevelChunk::unload()
{
	loaded = false;
	for (auto &i : tileEntities)
		level.tileEntityList.erase(i.second);
	for (auto &b : entityBlocks)
		level.removeEntities(b);
}

void LevelChunk::markUnsaved()
{
	unsaved = true;
}

void LevelChunk::getEntities(Entity *ignore, AABB &aabb, std::vector<std::shared_ptr<Entity>> &entities)
{

}

void LevelChunk::getEntitiesOfCondition(bool (*condition)(Entity &), AABB &aabb, std::vector<std::shared_ptr<Entity>> &entities)
{

}

int_t LevelChunk::countEntities()
{
	int_t count = 0;
	for (auto &b : entityBlocks)
		count += b.size();
	return count;
}

bool LevelChunk::shouldSave(bool force)
{
	if (dontSave)
		return false;

	if (force)
	{
		if (lastSaveHadEntities && level.time != lastSaveTime)
			return true;
	}
	else
	{
		if (lastSaveHadEntities && level.time >= lastSaveTime + 600)
			return true;
	}

	return unsaved;
}

void LevelChunk::setBlocks(byte_t *blocks, int_t y)
{

}

int_t LevelChunk::getBlocksAndData(byte_t *out, int_t x0, int_t y0, int_t z0, int_t x1, int_t y1, int_t z1)
{
	return 0;
}

int_t LevelChunk::setBlocksAndData(byte_t *in, int_t x0, int_t y0, int_t z0, int_t x1, int_t y1, int_t z1)
{
	return 0;
}

Random LevelChunk::getRandom(long_t seed)
{
	return Random();
}

bool LevelChunk::isEmpty()
{
	return false;
}
