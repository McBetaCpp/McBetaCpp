#include "world/level/levelgen/LargeCaveFeature.h"

#include "world/level/Level.h"
#include "world/level/tile/Tile.h"
#include "world/level/tile/DirtTile.h"
#include "world/level/tile/GrassTile.h"
#include "world/level/tile/StoneTile.h"

#include "util/Mth.h"

void LargeCaveFeature::addRoom(int_t cx, int_t cz, std::array<ubyte_t, 16 * 16 * 128> &blocks, double x, double y, double z)
{
	addTunnel(cx, cz, blocks, x, y, z, 1.0f + random.nextFloat() * 6.0f, 0.0f, 0.0f, -1, -1, 0.5);
}

void LargeCaveFeature::addTunnel(int_t cx, int_t cz, std::array<ubyte_t, 16 * 16 * 128> &blocks, double x, double y, double z, float paramFloat1, float paramFloat2, float paramFloat3, int paramInt3, int paramInt4, double paramDouble4)
{
	double d1 = (cx * 16 + 8);
	double d2 = (cz * 16 + 8);
	float f1 = 0.0f;
	float f2 = 0.0f;
	
	Random random(this->random.nextLong());
	
	if (paramInt4 <= 0)
	{
		int j = radius * 16 - 16;
		paramInt4 = j - random.nextInt(j / 4);
	}
	
	bool is_room = false;
	if (paramInt3 == -1)
	{
		paramInt3 = paramInt4 / 2;
		is_room = true;
	}
	
	int_t i = random.nextInt(paramInt4 / 2) + paramInt4 / 4;
	bool bool2 = (random.nextInt(6) == 0) ? true : false;

	while (paramInt3 < paramInt4)
	{
		double d3 = 1.5 + (Mth::sin(paramInt3 * Mth::PI / paramInt4) * paramFloat1 * 1.0f);
		double d4 = d3 * paramDouble4;
		
		float f3 = Mth::cos(paramFloat3);
		float f4 = Mth::sin(paramFloat3);
		
		x += (Mth::cos(paramFloat2) * f3);
		y += f4;
		z += (Mth::sin(paramFloat2) * f3);
		
		if (bool2)
			paramFloat3 *= 0.92f;
		else
			paramFloat3 *= 0.7f;
		
		paramFloat3 += f2 * 0.1f;
		paramFloat2 += f1 * 0.1f;
		
		f2 *= 0.9f;
		f1 *= 0.75f;
		f2 += (random.nextFloat() - random.nextFloat()) * random.nextFloat() * 2.0f;
		f1 += (random.nextFloat() - random.nextFloat()) * random.nextFloat() * 4.0f;
		
		if (!is_room && paramInt3 == i && paramFloat1 > 1.0f)
		{
			addTunnel(cx, cz, blocks, x, y, z, random.nextFloat() * 0.5f + 0.5f, paramFloat2 - (Mth::PI * 0.5f), paramFloat3 / 3.0f, paramInt3, paramInt4, 1.0);
			addTunnel(cx, cz, blocks, x, y, z, random.nextFloat() * 0.5f + 0.5f, paramFloat2 + (Mth::PI * 0.5f), paramFloat3 / 3.0f, paramInt3, paramInt4, 1.0);
			return;
		}
		
		if (is_room || random.nextInt(4) != 0)
		{
			double d5 = x - d1;
			double d6 = z - d2;
			double d7 = (paramInt4 - paramInt3);
			double d8 = (paramFloat1 + 2.0f + 16.0f);
			if (d5 * d5 + d6 * d6 - d7 * d7 > d8 * d8)
				return;

			if (x >= d1 - 16.0 - d3 * 2.0 && z >= d2 - 16.0 - d3 * 2.0 && x <= d1 + 16.0 + d3 * 2.0 && z <= d2 + 16.0 + d3 * 2.0)
			{
				int_t j = Mth::floor(x - d3) - cx * 16 - 1;
				int_t k = Mth::floor(x + d3) - cx * 16 + 1;
				int_t m = Mth::floor(y - d4) - 1;
				int_t n = Mth::floor(y + d4) + 1;
				int_t i1 = Mth::floor(z - d3) - cz * 16 - 1;
				int_t i2 = Mth::floor(z + d3) - cz * 16 + 1;
				if (j < 0)
					j = 0;
				if (k > 16)
					k = 16;
				if (m < 1)
					m = 1;
				if (n > 120)
					n = 120;
				if (i1 < 0)
					i1 = 0;
				if (i2 > 16)
					i2 = 16;
				
				// Check if water overlaps
				bool has_water = false;
				for (int_t i3 = j; !has_water &&i3 < k; i3++) {
					for (int_t i4 = i1; !has_water &&i4 < i2; i4++) {
						for (int_t i5 = n + 1; !has_water &&i5 >= m - 1; i5--) {
							int_t i6 = (i3 * 16 + i4) * 128 + i5;
							if (i5 >= 0 && i5 < 128) {
								// if (blocks[i6] == Tile::water.id || blocks[i6] == Tile::calmWater.id)
								// 	has_water = true;
								if (i5 != m - 1 && i3 != j && i3 != k - 1 && i4 != i1 && i4 != i2 - 1)
									i5 = m;
							}
						}
					}
				}
				if (has_water)
					continue;

				for (int_t i3 = j; i3 < k; i3++)
				{
					double d = ((i3 + cx * 16) + 0.5 - x) / d3;
					for (int_t i4 = i1; i4 < i2; i4++)
					{
						double d9 = ((i4 + cz * 16) + 0.5 - z) / d3;
						int_t i5 = (i3 * 16 + i4) * 128 + n;

						bool hit_grass = false;
						if (d * d + d9 * d9 < 1.0)
						{
							for (int_t i6 = n - 1; i6 >= m; i6--)
							{
								double d10 = (i6 + 0.5 - y) / d4;
								if (d10 > -0.7 && d * d + d10 * d10 + d9 * d9 < 1.0)
								{
									int_t t = blocks[i5];
									if (t == Tile::grass.id)
										hit_grass = true;

									if (t == Tile::rock.id || t == Tile::dirt.id || t == Tile::grass.id)
									{
										if (i6 < 10)
										{
											// blocks[i5] = (byte)Tile.lava.id;
										}
										else
										{
											blocks[i5] = 0;
											if (hit_grass && blocks[i5 - 1] == Tile::dirt.id)
												blocks[i5 - 1] = Tile::grass.id;
										}
									}
								}
								i5--;
							}
						}
					}
				}

				if (is_room)
					break;
			}
		}
		paramInt3++;
	}
}

void LargeCaveFeature::addFeature(Level &level, int_t xx, int_t zz, int_t x, int_t z, std::array<ubyte_t, 16 * 16 * 128> &blocks)
{
	int_t i = random.nextInt(random.nextInt(random.nextInt(40) + 1) + 1);
	if (random.nextInt(15) != 0)
		i = 0;

	for (int_t b = 0; b < i; b++)
	{
		double d1 = (xx * 16 + random.nextInt(16));
		double d2 = random.nextInt(random.nextInt(120) + 8);
		double d3 = (zz * 16 + random.nextInt(16));
		int_t j = 1;
		if (random.nextInt(4) == 0)
		{
			addRoom(x, z, blocks, d1, d2, d3);
			j += random.nextInt(4);
		}
		for (int_t b1 = 0; b1 < j; b1++)
		{
			float f1 = random.nextFloat() * Mth::PI * 2.0f;
			float f2 = (random.nextFloat() - 0.5f) * 2.0f / 8.0f;
			float f3 = random.nextFloat() * 2.0f + random.nextFloat();
			addTunnel(x, z, blocks, d1, d2, d3, f3, f1, f2, 0, 0, 1.0);
		}
	}
}
