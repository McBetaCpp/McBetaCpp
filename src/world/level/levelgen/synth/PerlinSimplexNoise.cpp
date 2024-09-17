#include "world/level/levelgen/synth/PerlinSimplexNoise.h"

PerlinSimplexNoise::PerlinSimplexNoise(int_t levels) : PerlinSimplexNoise(Random(), levels)
{

}

PerlinSimplexNoise::PerlinSimplexNoise(Random &random, int_t levels)
{
	this->levels = levels;

	for (int_t i = 0; i < levels; i++)
		noiseLevels.emplace_back(random);
}

double PerlinSimplexNoise::getValue(double x, double y)
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

double PerlinSimplexNoise::getValue(double x, double y, double z)
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

void PerlinSimplexNoise::getRegion(double *out, double x, double y, int_t xSize, int_t ySize, double xScale, double yScale, double inscale)
{
	return getRegion(out, x, y, xSize, ySize, xScale, yScale, inscale, 0.5);
}

void PerlinSimplexNoise::getRegion(double *out, double x, double y, int_t xSize, int_t ySize, double xScale, double yScale, double inscale, double indiv)
{
	xScale /= 1.5;
	yScale /= 1.5;

	std::fill(out, out + xSize * ySize, 0.0);

	double div = 1.0;
	double scale = 1.0;

	for (auto &i : noiseLevels)
	{
		i.add(out, x, y, xSize, ySize, xScale * scale, yScale * scale, 0.55 / div);
		scale *= inscale;
		div *= indiv;
	}
}

void PerlinSimplexNoise::getRegion(double *out, double x, double y, double z, int_t xSize, int_t ySize, int_t zSize, double xScale, double yScale, double zScale)
{
	xScale /= 1.5;
	yScale /= 1.5;

	std::fill(out, out + xSize * ySize * zSize, 0.0);

	double scale = 1.0;

	for (auto &i : noiseLevels)
	{
		i.add(out, x, y, z, xSize, ySize, zSize, xScale * scale, yScale * scale, zScale * scale, 0.55 / scale);
		scale *= 0.5;
	}
}
