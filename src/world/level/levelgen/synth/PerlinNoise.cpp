#include "world/level/levelgen/synth/PerlinNoise.h"

PerlinNoise::PerlinNoise(int_t levels) : PerlinNoise(Random(), levels)
{

}

PerlinNoise::PerlinNoise(Random &random, int_t levels)
{
	this->levels = levels;

	for (int_t i = 0; i < levels; i++)
		noiseLevels.emplace_back(random);
}

double PerlinNoise::getValue(double x, double y)
{
	double result = 0.0;
	double scale = 1.0;

	for (auto &i : noiseLevels)
	{
		result += i.getValue(x * scale, y * scale) / scale;
		scale /= 2.0;
	}

	return result;
}

double PerlinNoise::getValue(double x, double y, double z)
{
	double result = 0.0;
	double scale = 1.0;

	for (auto &i : noiseLevels)
	{
		result += i.getValue(x * scale, y * scale, z * scale) / scale;
		scale /= 2.0;
	}

	return result;
}

void PerlinNoise::getRegion(double *out, double x, double y, double z, int_t xSize, int_t ySize, int_t zSize, double xScale, double yScale, double zScale)
{
	std::fill(out, out + xSize * ySize * zSize, 0.0);

	double scale = 1.0;
	for (auto &i : noiseLevels)
	{
		i.add(out, x, y, z, xSize, ySize, zSize, xScale * scale, yScale * scale, zScale * scale, scale);
		scale /= 2.0;
	}
}

void PerlinNoise::getRegion(double *out, double x, double y, int_t xSize, int_t ySize, double xScale, double yScale, double unknown)
{
	getRegion(out, x, 10.0, y, xSize, 1, ySize, xScale, 1.0, yScale);
}
