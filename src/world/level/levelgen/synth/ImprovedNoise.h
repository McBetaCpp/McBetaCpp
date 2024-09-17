#pragma once

#include "world/level/levelgen/synth/Synth.h"

#include <array>

#include "java/Type.h"
#include "java/Random.h"

class ImprovedNoise : public Synth
{
private:
	std::array<int_t, 512> p;

public:
	double scale = 0.0;
	double xo = 0.0, yo = 0.0, zo = 0.0;

	ImprovedNoise();
	ImprovedNoise(Random &random);
	ImprovedNoise(Random &&random);

	double noise(double x, double y, double z);

	double fade(double t);
	double lerp(double t, double a, double b);
	double grad2(int_t hash, double x, double y);
	double grad(int_t hash, double x, double y, double z);

	double getValue(double x, double y) override;
	double getValue(double x, double y, double z);

	void add(double *out, double xin, double yin, double zin, int_t xd, int_t yd, int_t zd, double xs, double ys, double zs, double div);
};
