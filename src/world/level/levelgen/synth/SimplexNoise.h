#pragma once

#include <array>

#include "java/Type.h"
#include "java/Random.h"

class SimplexNoise
{
private:
	static constexpr int_t grad3[12][3] = {
		{ 1, 1, 0 }, { -1, 1, 0 }, { 1, -1, 0 }, { -1, -1, 0 },
		{ 1, 0, 1 }, { -1, 0, 1 }, { 1, 0, -1 }, { -1, 0, -1 },
		{ 0, 1, 1 }, { 0, -1, 1 }, { 0, 1, -1 }, { 0, -1, -1 }
	};

	std::array<int_t, 512> p;

public:
	double scale = 0.0;

	double xo = 0.0, yo = 0.0, zo = 0.0;

private:
	static constexpr double sqrt3 = 1.7320508075688772935274463415059;
	static constexpr double sqrt5 = 2.2360679774997896964091736687313;

	static constexpr double F2 = 0.5 * (sqrt3 - 1.0);
	static constexpr double G2 = (3.0 - sqrt3) / 6.0;
	static constexpr double F3 = 1.0 / 3.0;
	static constexpr double G3 = 1.0 / 6.0;
	static constexpr double F4 = (sqrt5 - 1.0) / 4.0;
	static constexpr double G4 = (5.0 - sqrt5) / 20.0;

public:
	SimplexNoise();
	SimplexNoise(Random &random);

private:
	static int_t fastfloor(double x);
	static double dot(const int_t *g, double x, double y);
	static double dot(const int_t *g, double x, double y, double z);

public:
	double getValue(double xin, double yin);
	double getValue(double xin, double yin, double zin);

	void add(double *out, double x, double y, int_t xd, int_t yd, double xs, double ys, double mul);
	void add(double *out, double x, double y, double z, int_t xd, int_t yd, int_t zd, double xs, double ys, double zs, double mul);
};
