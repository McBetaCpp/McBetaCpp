#include "world/level/levelgen/feature/CaveFeature.h"

#include "world/level/Level.h"

#include "world/level/tile/Tile.h"
#include "world/level/tile/GrassTile.h"
#include "world/level/tile/DirtTile.h"

#include "util/Mth.h"

bool CaveFeature::place(Level &level, Random &random, int_t x, int_t y, int_t z)
{
	float f = random.nextFloat() * Mth::PI;
	double d1 = 8.0;
	double d2 = (x + 8) + Mth::sin(f) * d1;
	double d3 = (x + 8) - Mth::sin(f) * d1;
	double d4 = (z + 8) + Mth::cos(f) * d1;
	double d5 = (z + 8) - Mth::cos(f) * d1;
	double d6 = (y + random.nextInt(8) + 2);
	double d7 = (y + random.nextInt(8) + 2);
	double d8 = random.nextDouble() * 4.0 + 2.0;
	double d9 = random.nextDouble() * 0.6;
	long_t l = random.nextLong();
	random.setSeed(l);

	std::vector<TilePos> arrayList;
	
	for (int_t b = 0; b <= 16; b++)
	{
		double d10 = d2 + (d3 - d2) * b / 16.0;
		double d11 = d6 + (d7 - d6) * b / 16.0;
		double d12 = d4 + (d5 - d4) * b / 16.0;
		double d13 = random.nextDouble();
		double d14 = (Mth::sin(b / 16.0F * Mth::PI) * d8 + 1.0) * d13 + 1.0;
		double d15 = (Mth::sin(b / 16.0F * Mth::PI) * d8 + 1.0) * d13 + 1.0;
		for (int_t i = d10 - d14 / 2.0; i <= d10 + d14 / 2.0; i++)
		{
			for (int_t j = d11 - d15 / 2.0; j <= d11 + d15 / 2.0; j++)
			{
				for (int_t k = d12 - d14 / 2.0; k <= d12 + d14 / 2.0; k++)
				{
					double d16 = (i + 0.5 - d10) / d14 / 2.0;
					double d17 = (j + 0.5 - d11) / d15 / 2.0;
					double d18 = (k + 0.5 - d12) / d14 / 2.0;
					if (d16 * d16 + d17 * d17 + d18 * d18 < random.nextDouble() * d9 + 1.0 - d9 && !level.isEmptyTile(i, j, k))
					{
						for (int m = i - 2; m <= i + 1; m++)
						{
							for (int n = j - 1; n <= j + 1; n++)
							{
								for (int i1 = k - 1; i1 <= k + 1; i1++)
								{
									if (m <= x || i1 <= z || m >= x + 16 - 1 || i1 >= z + 16 - 1)
										return false;
									if (level.getMaterial(m, n, i1).isLiquid())
										return false;
								}
							}
						}
						arrayList.emplace_back(i, j, k);
					}
				}
			}
		}
	}
	for (auto &tilePos : arrayList)
		level.setTileNoUpdate(tilePos.x, tilePos.y, tilePos.z, 0);

	for (auto &tilePos : arrayList)
	{
		if (level.getTile(tilePos.x, tilePos.y - 1, tilePos.z) == Tile::dirt.id && level.getRawBrightness(tilePos.x, tilePos.y, tilePos.z) > 8)
			level.setTileNoUpdate(tilePos.x, tilePos.y - 1, tilePos.z, Tile::grass.id);
	}
	return true;
}
