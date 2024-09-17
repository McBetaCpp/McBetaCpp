#include "world/level/biome/BiomeSource.h"

#include "world/level/Level.h"

BiomeSource::BiomeSource(Level &level) :
	temperatureMap(Random(level.seed * 9871LL), 4),
	downfallMap(Random(level.seed * 39811LL), 4),
	noiseMap(Random(level.seed * 543321LL), 2)
{
	
}

double BiomeSource::getTemperature(int_t x, int_t z)
{
	temperatureMap.getRegion(temperatures.data(), x, z, 1, 1, 0.025, 0.025, 0.5);
	return temperatures[0];
}

void BiomeSource::getBiomeBlock(int_t x, int_t z, int_t xd, int_t zd)
{
	temperatureMap.getRegion(temperatures.data(), x, z, xd, xd, 0.025, 0.025, 1.0 / 4.0);
	downfallMap.getRegion(downfalls.data(), x, z, xd, xd, 0.05, 0.05, 1.0 / 3.0);
	noiseMap.getRegion(noises.data(), x, z, xd, xd, 0.25, 0.25, 1.0 / 1.7);

	int_t i2 = 0;

	for (int_t xi = 0; xi < xd; xi++)
	{
		for (int_t zi = 0; zi < zd; zi++)
		{
			double nv = noises[i2] * 1.1 + 0.5;

			double d2 = 0.01;
			double d3 = 1.0 - d2;

			double temperature = (temperatures[i2] * 0.15 + 0.7) * d3 + nv * d2;

			d2 = 0.002;
			d3 = 1.0 - d2;

			double downfall = (downfalls[i2] * 0.15 + 0.5) * d3 + nv * d2;

			temperature = 1.0 - (1.0 - temperature) * (1.0 - temperature);

			if (temperature < 0.0) temperature = 0.0;
			if (downfall < 0.0) downfall = 0.0;
			if (temperature > 1.0) temperature = 1.0;
			if (downfall > 1.0) downfall = 1.0;

			temperatures[i2] = temperature;
			downfalls[i2] = downfall;

			i2++;
		}
	}
}
