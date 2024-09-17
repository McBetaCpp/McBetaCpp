#include "world/level/LightUpdate.h"

#include "world/level/Level.h"

LightUpdate::LightUpdate(int_t layer, int_t x0, int_t y0, int_t z0, int_t x1, int_t y1, int_t z1)
{
	this->layer = layer;
	this->x0 = x0;
	this->y0 = y0;
	this->z0 = z0;
	this->x1 = x1;
	this->y1 = y1;
	this->z1 = z1;
}

void LightUpdate::update(Level &level)
{
	int_t xs = x1 - x0 + 1;
	int_t ys = y1 - y0 + 1;
	int_t zs = z1 - z0 + 1;
	int_t size = xs * ys * zs;

	if (size > 0x8000)
	{
		std::cout << "Light too large, skipping!\n";
		return;
	}

	int lastChunkX = 0;
	int lastChunkZ = 0;
	bool checkLastChunk = false;
	bool hasLastChunk = false;

	for (int x = x0; x <= x1; x++)
	{
		for (int z = z0; z <= z1; z++)
		{
			int xc = x >> 4;
			int zc = z >> 4;
			bool hasChunk = false;

			if (checkLastChunk && xc == lastChunkX && zc == lastChunkZ)
			{
				hasChunk = hasLastChunk;
			}
			else
			{
				hasChunk = level.hasChunksAt(x, 0, z, 1);
				if (hasChunk)
				{
					auto chunk = level.getChunk(x >> 4, z >> 4);
					if (chunk->isEmpty())
						hasChunk = false;
				}

				hasLastChunk = hasChunk;
				lastChunkX = xc;
				lastChunkZ = zc;
			}

			if (hasChunk)
			{
				if (y0 < 0) y0 = 0;
				if (y1 >= Level::DEPTH) y1 = Level::DEPTH - 1;

				for (int_t y = y0; y <= y1; y++)
				{
					int_t oldBrightness = level.getBrightness(layer, x, y, z);
					int_t tile = level.getTile(x, y, z);

					int lightBlock = Tile::lightBlock[tile];
					if (lightBlock == 0)
						lightBlock = 1;

					int lightEmission = 0;
					if (layer == LightLayer::Sky)
					{
						if (level.isSkyLit(x, y, z))
							lightEmission = 15;
					}
					else if (layer == LightLayer::Block)
					{
						lightEmission = Tile::lightEmission[tile];
					}

					int newBrightness;
					if (lightBlock >= 15 && lightEmission == 0)
					{
						newBrightness = 0;
					}
					else
					{
						int_t bnx = level.getBrightness(layer, x - 1, y, z);
						int_t bpx = level.getBrightness(layer, x + 1, y, z);
						int_t bny = level.getBrightness(layer, x, y - 1, z);
						int_t bpy = level.getBrightness(layer, x, y + 1, z);
						int_t bnz = level.getBrightness(layer, x, y, z - 1);
						int_t bpz = level.getBrightness(layer, x, y, z + 1);

						newBrightness = bnx;
						if (bpx > newBrightness) newBrightness = bpx;
						if (bny > newBrightness) newBrightness = bny;
						if (bpy > newBrightness) newBrightness = bpy;
						if (bnz > newBrightness) newBrightness = bnz;
						if (bpz > newBrightness) newBrightness = bpz;

						newBrightness -= lightBlock;
						if (newBrightness < 0) newBrightness = 0;

						if (lightEmission > newBrightness)
							newBrightness = lightEmission;
					}

					if (oldBrightness != newBrightness)
					{
						level.setBrightness(layer, x, y, z, newBrightness);
						int_t finalBrightness = newBrightness - 1;
						if (finalBrightness < 0) finalBrightness = 0;

						level.updateLightIfOtherThan(layer, x - 1, y, z, finalBrightness);
						level.updateLightIfOtherThan(layer, x, y - 1, z, finalBrightness);
						level.updateLightIfOtherThan(layer, x, y, z - 1, finalBrightness);
						if ((x + 1) >= x1) level.updateLightIfOtherThan(layer, x + 1, y, z, finalBrightness);
						if ((y + 1) >= y1) level.updateLightIfOtherThan(layer, x, y + 1, z, finalBrightness);
						if ((z + 1) >= z1) level.updateLightIfOtherThan(layer, x, y, z + 1, finalBrightness);
					}
				}
			}
		}
	}
}

bool LightUpdate::expandToContain(int_t x0, int_t y0, int_t z0, int_t x1, int_t y1, int_t z1)
{
	if (x0 >= this->x0 && y0 >= this->y0 && z0 >= this->z0 && x1 <= this->x1 && y1 <= this->y1 && z1 <= this->z1)
		return true;

	int_t skin = 1;
	if (x0 >= (this->x0 - skin) && y0 >= (this->y0 - skin) && z0 >= (this->z0 - skin) && x1 <= (this->x1 + skin) && y1 <= (this->y1 + skin) && z1 <= (this->z1 + skin))
	{
		int_t cxd = this->x1 - this->x0;
		int_t cyd = this->y1 - this->y0;
		int_t czd = this->z1 - this->z0;

		if (x0 > this->x0) x0 = this->x0;
		if (y0 > this->y0) y0 = this->y0;
		if (z0 > this->z0) z0 = this->z0;
		if (x1 < this->x1) x1 = this->x1;
		if (y1 < this->y1) y1 = this->y1;
		if (z1 < this->z1) z1 = this->z1;

		int_t nxd = x1 - x0;
		int_t nyd = y1 - y0;
		int_t nzd = z1 - z0;

		int_t cd = cxd * cyd * czd;
		int_t nd = nxd * nyd * nzd;

		if (nd - cd <= 2)
		{
			this->x0 = x0;
			this->y0 = y0;
			this->z0 = z0;
			this->x1 = x1;
			this->y1 = y1;
			this->z1 = z1;
			return true;
		}
	}

	return false;
}
