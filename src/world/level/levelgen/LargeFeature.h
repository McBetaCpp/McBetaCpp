#pragma once

#include "java/Type.h"
#include "java/Random.h"

class Level;
class ChunkSource;

class LargeFeature
{
protected:
	int_t radius = 8;

	Random random = Random();

public:
	virtual ~LargeFeature() {}

	void apply(ChunkSource &chunkSource, Level &level, int_t x, int_t z, std::array<ubyte_t, 16 * 16 * 128> &blocks);

protected:
	virtual void addFeature(Level &level, int_t xx, int_t zz, int_t x, int_t z, std::array<ubyte_t, 16 * 16 * 128> &blocks);
};
