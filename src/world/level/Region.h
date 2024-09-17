#pragma once

#include "world/level/LevelSource.h"
#include "world/level/Level.h"

class Region : public LevelSource
{
private:
	int_t xc1 = 0, zc1 = 0;

	int_t h = 0;
	std::vector<std::shared_ptr<LevelChunk>> chunks;

	Level &level;

public:
	Region(Level &level, int_t x0, int_t y0, int_t z0, int_t x1, int_t y1, int_t z1);

	int_t getTile(int_t x, int_t y, int_t z) override;
	std::shared_ptr<TileEntity> getTileEntity(int_t x, int_t y, int_t z) override;
	float getBrightness(int_t x, int_t y, int_t z) override;
	int_t getRawBrightness(int_t x, int_t y, int_t z);
	int_t getRawBrightness(int_t x, int_t y, int_t z, bool neighbors);
	int_t getData(int_t x, int_t y, int_t z) override;
	const Material &getMaterial(int_t x, int_t y, int_t z) override;
	bool isSolidTile(int_t x, int_t y, int_t z) override;
	BiomeSource &getBiomeSource() override;
};
