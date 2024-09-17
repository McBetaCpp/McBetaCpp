#pragma once

#include <memory>
#include <vector>
#include <array>
#include <unordered_set>
#include <unordered_map>

#include "world/level/chunk/DataLayer.h"
#include "world/level/TilePos.h"
#include "world/level/tile/entity/TileEntity.h"

#include "java/Type.h"
#include "java/Random.h"

class Level;
class Entity;
class AABB;

class LevelChunk
{
public:
	static bool touchedSky;

	std::array<ubyte_t, 16 * 128 * 16> blocks = {};

	bool loaded = false;

	Level &level;

	DataLayer data;
	DataLayer skyLight;
	DataLayer blockLight;

	std::array<ubyte_t, 16 * 16> heightmap = {};

	int_t minHeight = 0;

	int_t x = 0, z = 0;

	std::unordered_map<TilePos, std::shared_ptr<TileEntity>> tileEntities;
	std::array<std::unordered_set<std::shared_ptr<Entity>>, 8> entityBlocks;

	bool terrainPopulated = false;
	bool unsaved = false;
	bool dontSave = false;
	bool lastSaveHadEntities = false;
	long_t lastSaveTime = 0;

	LevelChunk(Level &level, int_t x, int_t z);
	LevelChunk(Level &level, const ubyte_t *blocks, int_t x, int_t z);

	virtual ~LevelChunk() {}

	virtual bool isAt(int_t x, int_t z);

	virtual int_t getHeightmap(int_t x, int_t z);

	virtual void recalcBlockLights();
	virtual void recalcHeightmapOnly();
	virtual void recalcHeightmap();
	virtual void lightLava();

private:
	void lightGaps(int_t x, int_t z);
	void lightGap(int_t x, int_t z, int_t height);
	void recalcHeight(int_t x, int_t y, int_t z);
	
public:
	virtual int_t getTile(int_t x, int_t y, int_t z);
	virtual bool setTileAndData(int_t x, int_t y, int_t z, int_t tile, int_t data);
	virtual bool setTile(int_t x, int_t y, int_t z, int_t tile);

	virtual int_t getData(int_t x, int_t y, int_t z);
	virtual void setData(int_t x, int_t y, int_t z, int_t data);

	virtual int_t getBrightness(int_t lightLayer, int_t x, int_t y, int_t z);
	virtual void setBrightness(int_t lightLayer, int_t x, int_t y, int_t z, int_t brightness);

	virtual int_t getRawBrightness(int_t x, int_t y, int_t z, int_t darken);

	virtual void addEntity(std::shared_ptr<Entity> entity);
	virtual void removeEntity(std::shared_ptr<Entity> entity);
	virtual void removeEntity(std::shared_ptr<Entity> entity, int_t y);

	virtual bool isSkyLit(int_t x, int_t y, int_t z);

	virtual void skyBrightnessChanged();

	virtual std::shared_ptr<TileEntity> getTileEntity(int_t x, int_t y, int_t z);

	virtual void addTileEntity(std::shared_ptr<TileEntity> tileEntity);
	virtual void setTileEntity(int_t x, int_t y, int_t z, std::shared_ptr<TileEntity> tileEntity);
	virtual void removeTileEntity(int_t x, int_t y, int_t z);

	virtual void load();
	virtual void unload();
	virtual void markUnsaved();

	virtual void getEntities(Entity *ignore, AABB &aabb, std::vector<std::shared_ptr<Entity>> &entities);
	virtual void getEntitiesOfCondition(bool (*condition)(Entity&), AABB &aabb, std::vector<std::shared_ptr<Entity>> &entities);

	virtual int_t countEntities();

	virtual bool shouldSave(bool force);

	virtual void setBlocks(byte_t *blocks, int_t y);
	virtual int_t getBlocksAndData(byte_t *out, int_t x0, int_t y0, int_t z0, int_t x1, int_t y1, int_t z1);
	virtual int_t setBlocksAndData(byte_t *in, int_t x0, int_t y0, int_t z0, int_t x1, int_t y1, int_t z1);

	virtual Random getRandom(long_t seed);

	virtual bool isEmpty();
};