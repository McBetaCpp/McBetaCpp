#include "world/level/levelgen/LargeFeature.h"

#include "world/level/Level.h"
#include "world/level/chunk/ChunkSource.h"

void LargeFeature::apply(ChunkSource &chunkSource, Level &level, int_t x, int_t z, std::array<ubyte_t, 16 * 16 * 128> &blocks)
{
	int_t radius = this->radius;

	random.setSeed(level.seed);
	long_t cs0 = random.nextLong() / 2LL * 2LL + 1LL;
	long_t cs1 = random.nextLong() / 2LL * 2LL + 1LL;

	for (int_t xx = x - radius; xx <= x + radius; xx++)
	{
		for (int_t zz = z - radius; zz <= z + radius; zz++)
		{
			random.setSeed((xx * cs0 + zz * cs1) ^ level.seed);
			addFeature(level, xx, zz, x, z, blocks);
		}
	}
}

void LargeFeature::addFeature(Level &level, int_t xx, int_t zz, int_t x, int_t z, std::array<ubyte_t, 16 * 16 * 128> &blocks)
{

}
