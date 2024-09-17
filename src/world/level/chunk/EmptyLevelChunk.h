#pragma once

#include "world/level/chunk/LevelChunk.h"

class EmptyLevelChunk : public LevelChunk
{
public:
	EmptyLevelChunk(Level &level, int_t x, int_t z);

	bool isAt(int_t x, int_t z) override;

	int_t getHeightmap(int_t x, int_t z) override;

	void recalcBlockLights() override;
	void recalcHeightmapOnly() override;
	void recalcHeightmap() override;
	void lightLava() override;

	int_t getTile(int_t x, int_t y, int_t z) override;
	bool setTileAndData(int_t x, int_t y, int_t z, int_t tile, int_t data) override;
	bool setTile(int_t x, int_t y, int_t z, int_t tile) override;
	int_t getData(int_t x, int_t y, int_t z) override;
	void setData(int_t x, int_t y, int_t z, int_t data) override;

	int_t getBrightness(int_t layer, int_t x, int_t y, int_t z) override;
	void setBrightness(int_t layer, int_t x, int_t y, int_t z, int_t brightness) override;
	int_t getRawBrightness(int_t layer, int_t x, int_t y, int_t z) override;

	void addEntity(std::shared_ptr<Entity> entity) override;
	void removeEntity(std::shared_ptr<Entity> entity) override;
	void removeEntity(std::shared_ptr<Entity> entity, int_t y) override;

	bool isSkyLit(int_t x, int_t y, int_t z) override;
	void skyBrightnessChanged() override;

	std::shared_ptr<TileEntity> getTileEntity(int_t x, int_t y, int_t z) override;
	void addTileEntity(std::shared_ptr<TileEntity> tileEntity) override;
	void setTileEntity(int_t x, int_t y, int_t z, std::shared_ptr<TileEntity> tileEntity) override;
	void removeTileEntity(int_t x, int_t y, int_t z) override;

	void load() override;
	void unload() override;
	void markUnsaved() override;

	void getEntities(Entity *ignore, AABB &aabb, std::vector<std::shared_ptr<Entity>> &entities) override;
	void getEntitiesOfCondition(bool (*condition)(Entity &), AABB &aabb, std::vector<std::shared_ptr<Entity>> &entities) override;

	int_t countEntities() override;

	bool shouldSave(bool force) override;

	void setBlocks(byte_t *blocks, int_t y) override;
	int_t getBlocksAndData(byte_t *out, int_t x0, int_t y0, int_t z0, int_t x1, int_t y1, int_t z1) override;
	int_t setBlocksAndData(byte_t *in, int_t x0, int_t y0, int_t z0, int_t x1, int_t y1, int_t z1) override;

	Random getRandom(long_t seed) override;

	bool isEmpty() override;
};
