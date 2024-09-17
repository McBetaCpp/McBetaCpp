#pragma once

#include "world/level/levelgen/synth/PerlinSimplexNoise.h"

#include "java/Type.h"

class Level;

class BiomeSource
{
private:
	PerlinSimplexNoise temperatureMap;
	PerlinSimplexNoise downfallMap;
	PerlinSimplexNoise noiseMap;

public:
	std::array<double, 16 * 16> temperatures = {};
	std::array<double, 16 * 16> downfalls = {};
	std::array<double, 16 * 16> noises = {};

private:
	static constexpr float zoom = 2.0f;
	static constexpr float tempScale = 0.025f;
	static constexpr float downfallScale = 0.05f;
	static constexpr float noiseScale = 0.25f;

public:
	BiomeSource(Level &level);

	double getTemperature(int_t x, int_t z);

	void getBiomeBlock(int_t x, int_t z, int_t xd, int_t zd);
};
