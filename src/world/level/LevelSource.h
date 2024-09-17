#pragma once

#include "java/Type.h"

#include "world/level/biome/BiomeSource.h"
#include "world/level/material/Material.h"
#include "world/level/tile/entity/TileEntity.h"

class LevelSource
{
public:
	virtual ~LevelSource() {}

	virtual int_t getTile(int_t x, int_t y, int_t z) = 0;
	virtual std::shared_ptr<TileEntity> getTileEntity(int_t x, int_t y, int_t z) = 0;
	virtual float getBrightness(int_t x, int_t y, int_t z) = 0;
	virtual int_t getData(int_t x, int_t y, int_t z) = 0;
	virtual const Material &getMaterial(int_t x, int_t y, int_t z) = 0;
	virtual bool isSolidTile(int_t x, int_t y, int_t z) = 0;
	virtual BiomeSource &getBiomeSource() = 0;
};
