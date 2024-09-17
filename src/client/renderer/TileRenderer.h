#pragma once

#include "world/level/LevelSource.h"
#include "world/level/tile/Tile.h"

#include "java/Type.h"

class TileRenderer
{
private:
	LevelSource *level = nullptr;
	int_t fixedTexture = -1;

	bool xFlipTexture = false;
	bool noCulling = false;

public:
	TileRenderer(LevelSource *levelSource);
	TileRenderer();

	void tesselateInWorld(Tile &tile, int_t x, int_t y, int_t z, int_t fixedTexture);
	void tesselateInWorldNoCulling(Tile &tile, int_t x, int_t y, int_t z);
	bool tesselateInWorld(Tile &tt, int_t x, int_t y, int_t z);

	bool tesselateBlockInWorld(Tile &tt, int_t x, int_t y, int_t z);
	bool tesselateBlockInWorld(Tile &tt, int_t x, int_t y, int_t z, float r, float g, float b);

	void renderFaceUp(Tile &tt, double x, double y, double z, int_t tex);
	void renderFaceDown(Tile &tt, double x, double y, double z, int_t tex);
	void renderNorth(Tile &tt, double x, double y, double z, int_t tex);
	void renderSouth(Tile &tt, double x, double y, double z, int_t tex);
	void renderWest(Tile &tt, double x, double y, double z, int_t tex);
	void renderEast(Tile &tt, double x, double y, double z, int_t tex);

	void renderCube(Tile &tile, float alpha);
};
