#include "world/level/levelgen/synth/ImprovedNoise.h"

#include <iostream>

#include "util/Mth.h"

ImprovedNoise::ImprovedNoise() : ImprovedNoise(Random())
{

}

ImprovedNoise::ImprovedNoise(Random &random)
{
	xo = random.nextDouble() * 256.0;
	yo = random.nextDouble() * 256.0;
	zo = random.nextDouble() * 256.0;

	for (int_t i = 0; i < 256; i++)
		p[i] = i;
	for (int_t i = 0; i < 256; i++)
	{
		int_t j = random.nextInt(256 - i) + i;
		int_t tmp = p[i];
		p[i] = p[j];
		p[j] = tmp;
		p[i + 256] = p[i];
	}
}

double ImprovedNoise::noise(double x, double y, double z)
{
	x += xo;
	y += yo;
	z += zo;

	int_t X = Mth::floor(x);
	int_t Y = Mth::floor(y);
	int_t Z = Mth::floor(z);

	x -= X;
	y -= Y;
	z -= Z;

	X &= 0xFF;
	Y &= 0xFF;
	Z &= 0xFF;

	double u = fade(x);
	double v = fade(y);
	double w = fade(z);

	int_t A = p[X] + Y;
	int_t AA = p[A] + Z;
	int_t AB = p[A + 1] + Z;
	int_t B = p[X + 1] + Y;
	int_t BA = p[B] + Z;
	int_t BB = p[B + 1] + Z;

	return lerp(w, lerp(v, lerp(u, grad(p[AA], x, y, z), // AND ADD
		grad(p[BA], x - 1, y, z)), // BLENDED
		lerp(u, grad(p[AB], x, y - 1, z), // RESULTS
			grad(p[BB], x - 1, y - 1, z))),// FROM  8
		lerp(v, lerp(u, grad(p[AA + 1], x, y, z - 1), // CORNERS
			grad(p[BA + 1], x - 1, y, z - 1)), // OF CUBE
			lerp(u, grad(p[AB + 1], x, y - 1, z - 1), grad(p[BB + 1], x - 1, y - 1, z - 1))));
}

double ImprovedNoise::fade(double t)
{
	return t * t * t * (t * (t * 6 - 15) + 10);
}

double ImprovedNoise::lerp(double t, double a, double b)
{
	return a + t * (b - a);
}

double ImprovedNoise::grad2(int_t hash, double x, double y)
{
	int_t h = hash & 0xF;
	double	u = (1 - ((h & 0x8) >> 3)) * x,
		v = (h < 4) ? 0.0 : ((h == 12 || h == 14) ? x : y);
	return (((h & 1) == 0) ? u : -u) + (((h & 2) == 0) ? v : -v);
}

double ImprovedNoise::grad(int_t hash, double x, double y, double z)
{
	int_t h = hash & 0xF;
	double	u = (h < 8) ? x : y,
		v = (h < 4) ? y : ((h == 12 || h == 14) ? x : z);
	return (((h & 1) == 0) ? u : -u) + (((h & 2) == 0) ? v : -v);
}

double ImprovedNoise::getValue(double x, double y)
{
	return noise(x, y, 0.0);
}

double ImprovedNoise::getValue(double x, double y, double z)
{
	return noise(x, y, z);
}

void ImprovedNoise::add(double *out, double xin, double yin, double zin, int_t xd, int_t yd, int_t zd, double xs, double ys, double zs, double div)
{
	double inv_div = 1 / div;
	double *outp = out;

	if (yd == 1)
	{
		for (int_t xi = 0; xi < xd; xi++)
		{
			double x = (xin + xi) * xs + xo;
			int_t __X = Mth::floor(x);
			int_t _X = __X & 0xFF;
			x -= __X;

			double x_fade = fade(x);
			for (int_t zi = 0; zi < zd; zi++)
			{
				double z = (zin + zi) * zs + zo;
				int_t __Z = Mth::floor(z);
				int_t _Z = __Z & 0xFF;
				z -= __Z;

				double z_fade = fade(z);

				int_t p0 = p[_X]     + 0;
				int_t p1 = p[p0]     + _Z;
				int_t p2 = p[_X + 1] + 0;
				int_t p3 = p[p2]     + _Z;

				double l0 = lerp(x_fade, grad2(p[p1],     x,      z),       grad(p[p3],     x - 1.0, 0.0, z));
				double l1 = lerp(x_fade, grad (p[p1 + 1], x, 0.0, z - 1.0), grad(p[p3 + 1], x - 1.0, 0.0, z - 1.0));
				double out = lerp(z_fade, l0, l1);
				*outp++ += out * inv_div;
			}
		}
		return;
	}


	int_t lastY = -1;

	double l0 = 0.0, l1 = 0.0, l2 = 0.0, l3 = 0.0;

	for (int_t xi = 0; xi < xd; xi++)
	{
		double x = (xin + xi) * xs + xo;
		int_t __X = Mth::floor(x);
		int_t _X = __X & 0xFF;
		x -= __X;

		double x_fade = fade(x);
		for (int_t zi = 0; zi < zd; zi++)
		{
			double z = (zin + zi) * zs + zo;
			int_t __Z = Mth::floor(z);
			int_t _Z = __Z & 0xFF;
			z -= __Z;

			double z_fade = fade(z);
			for (int_t yi = 0; yi < yd; yi++)
			{
				double y = (yin + yi) * ys + yo;
				int_t __Y = Mth::floor(y);
				int_t _Y = __Y & 0xFF;
				y -= __Y;

				double y_fade = fade(y);
				if (yi == 0 || _Y != lastY)
				{
					lastY = _Y;

					int_t p0 = p[_X]     + _Y;
					int_t p1 = p[p0]     + _Z;
					int_t p2 = p[p0 + 1] + _Z;
					int_t p3 = p[_X + 1] + _Y;
					int_t p4 = p[p3]     + _Z;
					int_t p5 = p[p3 + 1] + _Z;

					l0 = lerp(x_fade, grad(p[p1],     x, y,     z),     grad(p[p4],     x - 1, y,     z));
					l1 = lerp(x_fade, grad(p[p2],     x, y - 1, z),     grad(p[p5],     x - 1, y - 1, z));
					l2 = lerp(x_fade, grad(p[p1 + 1], x, y,     z - 1), grad(p[p4 + 1], x - 1, y,     z - 1));
					l3 = lerp(x_fade, grad(p[p2 + 1], x, y - 1, z - 1), grad(p[p5 + 1], x - 1, y - 1, z - 1));
				}

				double l4 = lerp(y_fade, l0, l1);
				double l5 = lerp(y_fade, l2, l3);
				double l6 = lerp(z_fade, l4, l5);

				*outp++ += l6 * inv_div;
			}
		}
	}
}
