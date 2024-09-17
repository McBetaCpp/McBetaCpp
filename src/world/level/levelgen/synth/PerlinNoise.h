#pragma once

#include "world/level/levelgen/synth/Synth.h"

#include <list>

#include "world/level/levelgen/synth/ImprovedNoise.h"

#include "java/Type.h"
#include "java/Random.h"

class PerlinNoise : public Synth
{
private:
	std::list<ImprovedNoise> noiseLevels;
	int_t levels = 0;

public:
	PerlinNoise(int_t levels);
	PerlinNoise(Random &random, int_t levels);
	PerlinNoise(Random &&random, int_t levels);

	double getValue(double x, double y) override;
	double getValue(double x, double y, double z);

	void getRegion(double *out, double x, double y, double z, int_t xSize, int_t ySize, int_t zSize, double xScale, double yScale, double zScale);
	void getRegion(double *out, double x, double y, int_t xSize, int_t ySize, double xScale, double yScale, double unknown);
};
