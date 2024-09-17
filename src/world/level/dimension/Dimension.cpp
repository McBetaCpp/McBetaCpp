#include "world/level/dimension/Dimension.h"

#include "world/level/Level.h"

#include "world/level/tile/Tile.h"
#include "world/level/tile/SandTile.h"

#include "util/Mth.h"

#include "world/level/levelgen/RandomLevelSource.h"
#include "world/level/chunk/storage/OldChunkStorage.h"

Dimension::Dimension(Level &level) : level(level)
{
	biomeSource = Util::make_unique<BiomeSource>(level);

	updateLightRamp();
}

void Dimension::initLevel()
{

}

void Dimension::updateLightRamp()
{
	float f = 0.05f;
	for (int_t i = 0; i < 16; i++)
	{
		float f1 = 1.0f - i / 15.0f;
		brightnessRamp[i] = (1.0f - f1) / (f1 * 3.0f + 1.0f) * (1.0f - f) + f;
	}
}

void Dimension::init()
{

}

ChunkSource *Dimension::createRandomLevelSource()
{
	return new RandomLevelSource(level, level.seed);
}

ChunkStorage *Dimension::createStorage(std::shared_ptr<File> dir)
{
	return new OldChunkStorage(dir, true);
}

bool Dimension::isValidSpawn(int_t x, int_t z)
{
	int_t top = level.getTopTile(x, z);
	return top == Tile::sand.id;
}

float Dimension::getTimeOfDay(int_t time, float add)
{
	int_t time_cycle = time % 24000L;
	float time_cycle_f = (static_cast<float>(time_cycle) + add) / 24000.0f - 0.25f;
	if (time_cycle_f < 0.0f)
		time_cycle_f++;
	if (time_cycle_f > 1.0f)
		time_cycle_f--;

	float temp = time_cycle_f;
	time_cycle_f = 1.0f - static_cast<float>((Mth::cos(time_cycle_f * Mth::PI) + 1.0) / 2.0);
	time_cycle_f = temp + (time_cycle_f - temp) / 3.0f;

	return time_cycle_f;
}

float *Dimension::getSunriseColor(float time, float unknown)
{
	float radial = 0.4f;
	float dot = Mth::cos(time * Mth::PI * 2.0f) * 2.0f + 0.5f;
	float center = -0.0f;

	if (dot >= center - radial && dot <= center + radial)
	{
		float norm = (dot - center) / radial * 0.5f + 0.5f;
		float alpha = 1.0f - (1.0f - Mth::sin(norm * Mth::PI)) * 0.99f;
		alpha *= alpha;

		sunriseCol[0] = norm * 0.3f + 0.7f;
		sunriseCol[1] = norm * norm * 0.7f + 0.2f;
		sunriseCol[2] = norm * norm * 0.0f + 0.2f;
		sunriseCol[3] = alpha;

		return sunriseCol;
	}

	return nullptr;
}

Vec3 *Dimension::getFogColor(float time, float unknown)
{
	float dot = Mth::cos(time * Mth::PI * 2.0f) * 2.0f + 0.5f;
	if (dot < 0.0f)
		dot = 0.0f;
	if (dot > 1.0f)
		dot = 1.0f;

	float r = 0.7529412f;
	float g = 0.84705883f;
	float b = 1.0f;
	r *= dot * 0.94f + 0.06f;
	g *= dot * 0.94f + 0.06f;
	b *= dot * 0.91f + 0.09f;

	return Vec3::newTemp(r, g, b);
}

bool Dimension::mayRespawn()
{
	return true;
}

Dimension *Dimension::getNew(Level &level, int_t id)
{
	if (id == 0)
		return new Dimension(level);
	// if (id == -1)
	// 	return new HellDimension(level);
	return nullptr;
}
