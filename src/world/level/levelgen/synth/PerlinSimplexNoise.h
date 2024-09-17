#pragma once

#include "world/level/levelgen/synth/Synth.h"

#include <list>

#include "world/level/levelgen/synth/SimplexNoise.h"

#include "java/Type.h"
#include "java/Random.h"

class PerlinSimplexNoise : public Synth
{
private:
	std::list<SimplexNoise> noiseLevels;
	int_t levels = 0;

public:
	PerlinSimplexNoise(int_t levels);
	PerlinSimplexNoise(Random &random, int_t levels);

	double getValue(double x, double y) override;
	double getValue(double x, double y, double z);

	void getRegion(double *out, double x, double y, int_t xSize, int_t ySize, double xScale, double yScale, double inscale);
	void getRegion(double *out, double x, double y, int_t xSize, int_t ySize, double xScale, double yScale, double inscale, double indiv);
	void getRegion(double *out, double x, double y, double z, int_t xSize, int_t ySize, int_t zSize, double xScale, double yScale, double zScale);
};
