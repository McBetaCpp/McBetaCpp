#pragma once

#include "world/level/chunk/ChunkSource.h"

#include <array>

#include "world/level/levelgen/synth/PerlinNoise.h"

#include "java/Random.h"

class Level;

class RandomLevelSource : public ChunkSource
{
private:
	static constexpr double SNOW_CUTOFF = 0.5;
	static constexpr double SNOW_SCALE = 0.3;
	static constexpr bool FLOATING_ISLANDS = false;

public:
	static constexpr int_t CHUNK_HEIGHT = 8;
	static constexpr int_t CHUNK_WIDTH = 4;

private:
	Random random;

	PerlinNoise lperlinNoise1;
	PerlinNoise lperlinNoise2;
	PerlinNoise perlinNoise1;
	PerlinNoise perlinNoise2;
	PerlinNoise perlinNoise3;
	PerlinNoise scaleNoise;
	PerlinNoise depthNoise;
	// PerlinNoise floatingIslandScale;
	// PerlinNoise floatingIslandNoise;
	PerlinNoise forestNoise;

	Level &level;

	std::array<double, 16 * 16> sandBuffer = {};
	std::array<double, 16 * 16> gravelBuffer = {};
	std::array<double, 16 * 16> depthBuffer = {};

	static constexpr int_t BUFFER_WIDTH = 16 / CHUNK_WIDTH;
	static constexpr int_t BUFFER_HEIGHT = 128 / CHUNK_HEIGHT;
	static constexpr int_t BUFFER_WIDTH_1 = BUFFER_WIDTH + 1;
	static constexpr int_t BUFFER_HEIGHT_1 = BUFFER_HEIGHT + 1;

	std::array<double, BUFFER_WIDTH_1 * BUFFER_WIDTH_1 * BUFFER_HEIGHT_1> buffer = {};

	std::array<double, BUFFER_WIDTH_1 * BUFFER_WIDTH_1 * BUFFER_HEIGHT_1> pnr = {};
	std::array<double, BUFFER_WIDTH_1 * BUFFER_WIDTH_1 * BUFFER_HEIGHT_1> ar = {};
	std::array<double, BUFFER_WIDTH_1 * BUFFER_WIDTH_1 * BUFFER_HEIGHT_1> br = {};
	std::array<double, BUFFER_WIDTH_1 * BUFFER_WIDTH_1> sr = {};
	std::array<double, BUFFER_WIDTH_1 * BUFFER_WIDTH_1> dr = {};

public:
	RandomLevelSource(Level &level, long_t seed);

	void prepareHeights(int_t x, int_t z, ubyte_t *tiles, double *temperature);
	void buildSurfaces(int_t x, int_t z, ubyte_t *tiles);
	std::shared_ptr<LevelChunk> getChunk(int_t x, int_t z) override;
private:
	void getHeights(double *out, int_t x, int_t y, int_t z, int_t xd, int_t yd, int_t zd);
public:
	bool hasChunk(int_t x, int_t z) override;
private:
	void calcWaterDepths(std::shared_ptr<ChunkSource> parent, int_t x, int_t z);
public:
	void postProcess(ChunkSource &parent, int_t x, int_t z) override;
	bool save(bool force, std::shared_ptr<ProgressListener> progressListener) override;
	bool tick() override;
	bool shouldSave() override;
	jstring gatherStats() override;
};
