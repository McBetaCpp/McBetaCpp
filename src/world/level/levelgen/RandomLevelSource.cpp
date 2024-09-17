#include "world/level/levelgen/RandomLevelSource.h"

#include "world/level/Level.h"

#include "world/level/tile/Tile.h"
#include "world/level/tile/StoneTile.h"
#include "world/level/tile/GrassTile.h"
#include "world/level/tile/DirtTile.h"
#include "world/level/tile/SandTile.h"
#include "world/level/tile/GravelTile.h"

#include "world/level/levelgen/feature/TreeFeature.h"
#include "world/level/levelgen/LargeCaveFeature.h"

TreeFeature tree_feature_lol;
LargeCaveFeature large_cave_feature;

RandomLevelSource::RandomLevelSource(Level &level, long_t seed) : level(level),
	random(seed),
	lperlinNoise1(random, 16),
	lperlinNoise2(random, 16),
	perlinNoise1(random, 8),
	perlinNoise2(random, 4),
	perlinNoise3(random, 4),
	scaleNoise(random, 10),
	depthNoise(random, 16),
	forestNoise(random, 8)
{

}

void RandomLevelSource::prepareHeights(int_t x, int_t z, ubyte_t *tiles, double *temperatures)
{
	getHeights(buffer.data(), x * BUFFER_WIDTH, 0, z * BUFFER_WIDTH, BUFFER_WIDTH_1, BUFFER_HEIGHT_1, BUFFER_WIDTH_1);

	for (int_t xi = 0; xi < BUFFER_WIDTH; xi++)
	{
		for (int_t zi = 0; zi < BUFFER_WIDTH; zi++)
		{
			for (int_t yi = 0; yi < BUFFER_HEIGHT; yi++)
			{
				double ddiv = 0.125;

				double v000 = buffer[((xi + 0) * BUFFER_WIDTH_1 + zi + 0) * BUFFER_HEIGHT_1 + yi + 0];
				double v010 = buffer[((xi + 0) * BUFFER_WIDTH_1 + zi + 1) * BUFFER_HEIGHT_1 + yi + 0];
				double v100 = buffer[((xi + 1) * BUFFER_WIDTH_1 + zi + 0) * BUFFER_HEIGHT_1 + yi + 0];
				double v110 = buffer[((xi + 1) * BUFFER_WIDTH_1 + zi + 1) * BUFFER_HEIGHT_1 + yi + 0];
				double d001 = (buffer[((xi + 0) * BUFFER_WIDTH_1 + zi + 0) * BUFFER_HEIGHT_1 + yi + 1] - v000) * ddiv;
				double d011 = (buffer[((xi + 0) * BUFFER_WIDTH_1 + zi + 1) * BUFFER_HEIGHT_1 + yi + 1] - v010) * ddiv;
				double d101 = (buffer[((xi + 1) * BUFFER_WIDTH_1 + zi + 0) * BUFFER_HEIGHT_1 + yi + 1] - v100) * ddiv;
				double d111 = (buffer[((xi + 1) * BUFFER_WIDTH_1 + zi + 1) * BUFFER_HEIGHT_1 + yi + 1] - v110) * ddiv;

				for (int_t cyi = 0; cyi < CHUNK_HEIGHT; cyi++)
				{
					double ddiv2 = 0.25;

					double vx00 = v000;
					double vx10 = v010;
					double dx00 = (v100 - v000) * ddiv2;
					double dx10 = (v110 - v010) * ddiv2;

					for (int_t cxi = 0; cxi < CHUNK_WIDTH; cxi++)
					{
						int_t i = ((cxi + CHUNK_WIDTH * xi) * (16 * 128)) | ((zi * CHUNK_WIDTH) * (128)) | (cyi + CHUNK_HEIGHT * yi);
						int_t pitch = 128;

						double ddiv3 = 0.25;

						double vxx0 = vx00;
						double dxx0 = (vx10 - vx00) * ddiv3;

						for (int_t czi = 0; czi < CHUNK_WIDTH; czi++)
						{
							double temperature = temperatures[(cxi + CHUNK_WIDTH * xi) * 16 + (czi + zi * CHUNK_WIDTH)];

							int_t tile = 0;

							if ((yi * CHUNK_HEIGHT + cyi) <= Level::SEA_LEVEL)
							{
								
							}

							if (vxx0 > 0.0)
								tile = Tile::rock.id;

							tiles[i] = tile;

							i += pitch;
							vxx0 += dxx0;
						}

						vx00 += dx00;
						vx10 += dx10;
					}

					v000 += d001;
					v010 += d011;
					v100 += d101;
					v110 += d111;
				}
			}
		}
	}
}

void RandomLevelSource::buildSurfaces(int_t x, int_t z, ubyte_t *tiles)
{
	int_t seaLevel = Level::SEA_LEVEL + 1;

	double scale = 1.0 / 32.0;
	perlinNoise2.getRegion(sandBuffer.data(), x * 16, z * 16, 0.0, 16, 16, 1, scale, scale, 1.0);
	perlinNoise2.getRegion(gravelBuffer.data(), z * 16, 109.0134, x * 16, 16, 1, 16, scale, 1.0, scale);
	perlinNoise3.getRegion(depthBuffer.data(), x * 16, z * 16, 0.0, 16, 16, 1, scale * 2.0, scale * 2.0, scale * 2.0);

	for (int_t x = 0; x < 16; x++)
	{
		for (int_t z = 0; z < 16; z++)
		{
			bool isSand = (sandBuffer[x + z * 16] + random.nextDouble() * 0.2) > 0.0;
			bool isGravel = (gravelBuffer[x + z * 16] + random.nextDouble() * 0.2) > 3.0;

			int_t depth = static_cast<int_t>(depthBuffer[x + z * 16] / 3.0 + 3.0 + random.nextDouble() * 0.25);
			int_t depthI = 0;

			int_t topTile = Tile::grass.id;
			int_t fillerTile = Tile::dirt.id;

			for (int_t y = Level::DEPTH - 1; y >= 0; y--)
			{
				int_t i = (x * 16 + z) * Level::DEPTH + y;
				
				// Bedrock
				if (y <= 0 + random.nextInt(5))
				{
					continue;
				}

				// Perform filling
				int_t oldTile = tiles[i];
				if (oldTile == 0)
				{
					depthI = -1;
					continue;
				}

				if (oldTile == Tile::rock.id)
				{
					if (depthI == -1)
					{
						if (depth <= 0)
						{
							topTile = 0;
							fillerTile = Tile::rock.id;
						}
						else if (y >= seaLevel - 4 && y <= seaLevel + 1)
						{
							topTile = Tile::grass.id;
							fillerTile = Tile::dirt.id;

							if (isGravel) topTile = 0;
							if (isGravel) fillerTile = Tile::gravel.id;
							if (isSand) topTile = Tile::sand.id;
							if (isSand) fillerTile = Tile::sand.id;
						}

						depthI = depth;
						if (y >= seaLevel - 1)
							tiles[i] = topTile;
						else
							tiles[i] = fillerTile;
						continue;
					}
					else if (depthI > 0)
					{
						depthI--;
						tiles[i] = fillerTile;
					}
				}
			}
		}
	}
}

std::shared_ptr<LevelChunk> RandomLevelSource::getChunk(int_t x, int_t z)
{
	random.setSeed(x * 341873128712LL + z * 132897987541LL);

	std::shared_ptr<LevelChunk> chunk = Util::make_shared<LevelChunk>(level, x, z);

	level.getBiomeSource().getBiomeBlock(x * 16, z * 16, 16, 16);

	double *temperatures = level.getBiomeSource().temperatures.data();
	prepareHeights(x, z, chunk->blocks.data(), temperatures);

	buildSurfaces(x, z, chunk->blocks.data());

	large_cave_feature.apply(*this, level, x, z, chunk->blocks);

	chunk->recalcHeightmap();

	return chunk;
}

void RandomLevelSource::getHeights(double *out, int_t x, int_t y, int_t z, int_t xd, int_t yd, int_t zd)
{
	double xscale = 684.412;
	double yscale = 684.412;

	double *temperatures = level.getBiomeSource().temperatures.data();
	double *downfalls = level.getBiomeSource().downfalls.data();

	scaleNoise.getRegion(sr.data(), x, z, xd, zd, 1.121, 1.121, 0.5);
	depthNoise.getRegion(dr.data(), x, z, xd, zd, 200.0, 200.0, 0.5);
	perlinNoise1.getRegion(pnr.data(), x, y, z, xd, yd, zd, xscale / 80.0, yscale / 160.0, xscale / 80.0);
	lperlinNoise1.getRegion(ar.data(), x, y, z, xd, yd, zd, xscale, yscale, xscale);
	lperlinNoise2.getRegion(br.data(), x, y, z, xd, yd, zd, xscale, yscale, xscale);

	int_t i3 = 0;
	int_t i2 = 0;

	int_t inc = 16 / xd;
	for (int_t xi = 0; xi < xd; xi++)
	{
		int_t bx = xi * inc + inc / 2;
		for (int_t zi = 0; zi < zd; zi++)
		{
			int_t bz = zi * inc + inc / 2;

			double temperature = temperatures[bx * 16 + bz];
			double downfall = downfalls[bx * 16 + bz] * temperature;

			double factor = 1.0 - downfall;
			factor *= factor;
			factor *= factor;
			factor = 1.0 - factor;

			double sv = (sr[i2] + 256.0) / 512.0;
			sv *= factor;
			if (sv > 1.0) sv = 1.0;

			double dv = dr[i2] / 8000.0;
			if (dv < 0.0) dv = -dv * 0.3;
			dv = dv * 3.0 - 2.0;

			if (dv < 0.0)
			{
				dv /= 2.0;
				if (dv < -1.0) dv = -1.0;
				dv /= 1.4;
				dv /= 2.0;
				sv = 0.0;
			}
			else
			{
				if (dv > 1.0) dv = 1.0;
				dv /= 8.0;
			}

			if (sv < 0.0)
				sv = 0.0;
			sv += 0.5;

			dv = dv * yd / 16.0;

			double height = yd / 2.0 + dv * 4.0;

			i2++;

			for (int_t yi = 0; yi < yd; yi++)
			{
				double final = 0.0;
				
				double heightFactor = (yi - height) * 12.0 / sv;
				if (heightFactor < 0.0) heightFactor *= 4.0;

				double av = ar[i3] / 512.0;
				double bv = br[i3] / 512.0;
				double pnv = (pnr[i3] / 10.0 + 1.0) / 2.0;

				if (pnv < 0.0)
					final = av;
				else if (pnv > 1.0)
					final = bv;
				else
					final = av + (bv - av) * pnv;

				final -= heightFactor;

				if (yi > yd - 4)
				{
					double factor = (yi - (yd - 4)) / 3.0f;
					final = final * (1.0 - factor) + -10.0 * factor;
				}

				out[i3] = final;
				i3++;
			}
		}
	}
}

bool RandomLevelSource::hasChunk(int_t x, int_t z)
{
	return true;
}

void RandomLevelSource::postProcess(ChunkSource &parent, int_t x, int_t z)
{
	int_t cx = x * 16;
	int_t cz = z * 16;

	random.setSeed(level.seed);
	long_t cs0 = random.nextLong() / 2LL * 2LL + 1LL;
	long_t cs1 = random.nextLong() / 2LL * 2LL + 1LL;
	random.setSeed((x * cs0 + z * cs1) ^ level.seed);

	double scale;

	// SPAWN TREES AND HAVE FUN
	scale = 0.5;
	int_t forest_value = (forestNoise.getValue(cx * scale, cz * scale) / 8.0 + random.nextDouble() * 4.0 + 4.0) / 3.0;

	int_t trees = 0;
	if (random.nextInt(10) == 0)
		trees++;

	trees += forest_value + 5; // My awesome fun forest

	for (int_t i = 0; i < trees; i++)
	{
		int_t tx = cx + random.nextInt(16) + 8;
		int_t tz = cz + random.nextInt(16) + 8;
		tree_feature_lol.init(1.0, 1.0, 1.0);
		tree_feature_lol.place(level, random, tx, level.getHeightmap(tx, tz), tz);
	}
}

bool RandomLevelSource::save(bool force, std::shared_ptr<ProgressListener> progressListener)
{
	return true;
}

bool RandomLevelSource::tick()
{
	return false;
}

bool RandomLevelSource::shouldSave()
{
	return true;
}

jstring RandomLevelSource::gatherStats()
{
	return u"RandomLevelSource";
}
