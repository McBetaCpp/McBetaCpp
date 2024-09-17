#include "world/level/levelgen/feature/TreeFeature.h"

#include "world/level/Level.h"

#include "world/level/tile/Tile.h"
#include "world/level/tile/GrassTile.h"
#include "world/level/tile/DirtTile.h"
#include "world/level/tile/TreeTile.h"
#include "world/level/tile/LeafTile.h"

#include "util/Mth.h"

bool TreeFeature::place(Level &level, Random &random, int_t x, int_t y, int_t z)
{
	int_t height = random.nextInt(3) + 4;

	// Check if area is obstructed
	bool canPlace = true;
	if (y < 1 || y + height + 1 > Level::DEPTH)
		return false;

	for (int_t yy = y; yy <= y + 1 + height; yy++)
	{
		int_t rad = 1;
		if (yy == y) rad = 0;
		if (yy >= y + 1 + height - 2) rad = 2;

		for (int_t xx = x - rad; xx <= x + rad && canPlace; xx++)
		{
			for (int_t zz = z - rad; zz <= z + rad && canPlace; zz++)
			{
				if (yy >= 0 && yy < Level::DEPTH)
				{
					int_t b = level.getTile(xx, yy, zz);
					if (b != 0 && b != Tile::leaves.id)
						canPlace = false;
				}
				else
				{
					canPlace = false;
				}
			}
		}
	}

	if (!canPlace)
		return false;

	// Check if block below is grass, and replace with dirt
	int_t below_tile = level.getTile(x, y - 1, z);
	if ((below_tile != Tile::grass.id && below_tile != Tile::dirt.id) || y >= Level::DEPTH - height - 1)
		return false;

	level.setTileNoUpdate(x, y - 1, z, Tile::dirt.id);

	// Place tree swaggy style
	for (int_t yy = y - 3 + height; yy <= y + height; yy++)
	{
		int_t off = yy - (y + height);
		int_t rad = 1 - off / 2;

		for (int_t xx = x - rad; xx <= x + rad; xx++)
		{
			int_t dx = xx - x;
			for (int_t zz = z - rad; zz <= z + rad; zz++)
			{
				int_t dz = zz - z;
				if (std::abs(dx) != rad || std::abs(dz) != rad || (random.nextInt(2) != 0 && off != 0) && !Tile::solid[level.getTile(xx, yy, zz)])
					level.setTileNoUpdate(xx, yy, zz, Tile::leaves.id);
			}
		}
	}

	for (int_t yy = 0; yy < height; yy++)
	{
		int_t b = level.getTile(x, y + yy, z);
		if (b == 0 || b == Tile::leaves.id)
			level.setTileNoUpdate(x, y + yy, z, Tile::treeTrunk.id);
	}

	return true;
}
