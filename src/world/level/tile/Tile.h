#pragma once

#include <string>
#include <array>

#include "Facing.h"
#include "locale/Descriptive.h"

#include "world/level/material/Material.h"
#include "world/phys/AABB.h"

#include "java/Type.h"
#include "java/Random.h"
#include "java/String.h"

class Level;
class LevelSource;
class Entity;
class Player;

class StoneTile;
class GrassTile;
class DirtTile;
class SandTile;
class GravelTile;
class TreeTile;
class LeafTile;

class Tile //: public Descriptive<Tile>
{
private:
	static const jstring TILE_DESCRIPTION_PREFIX;

public:
	enum Shape
	{
		SHAPE_INVISIBLE = -1,
		SHAPE_BLOCK,
		SHAPE_CROSS_TEXTURE,
		SHAPE_TORCH,
		SHAPE_FIRE,
		SHAPE_WATER,
		SHAPE_RED_DUST,
		SHAPE_ROWS,
		SHAPE_DOOR,
		SHAPE_LADDER,
		SHAPE_RAIL,
		SHAPE_STAIRS,
		SHAPE_FENCE,
		SHAPE_LEVER,
		SHAPE_CACTUS,
	};

	// Tile properties
	static std::array<Tile *, 256> tiles;

	static std::array<bool, 256> shouldTick;
	static std::array<bool, 256> solid;
	static std::array<bool, 256> isEntityTile;
	static std::array<int_t, 256> lightBlock;
	static std::array<bool, 256> translucent;
	static std::array<int_t, 256> lightEmission;

	// Tiles
	static StoneTile rock;
	static GrassTile grass;
	static DirtTile dirt;

	static SandTile sand;
	static GravelTile gravel;

	static TreeTile treeTrunk;
	static LeafTile leaves;

	static void initTiles();

public:
	int_t tex = 0;

	int_t id = 0;

protected:
	float destroySpeed = 0.0f;
	float explosionResistance = 0.0f;

public:
	double xx0 = 0.0;
	double yy0 = 0.0;
	double zz0 = 0.0;
	
	double xx1 = 0.0;
	double yy1 = 0.0;
	double zz1 = 0.0;

	float gravity = 1.0f;

	const Material &material;

	float friction = 0.6f;

protected:
	Tile(int_t id, const Material &material);
	Tile(int_t id, int_t tex, const Material &material);

	Tile &setLightBlock(int_t lightBlock);
	Tile &setLightEmission(int_t lightEmission);
	Tile &setExplodeable(float resistance);

private:
	virtual bool isTranslucent();

public:
	virtual bool isCubeShaped();
	virtual Shape getRenderShape();

protected:
	Tile &setDestroyTime(float time);
	void setTicking(bool ticking);

public:
	void setShape(float x0, float y0, float z0, float x1, float y1, float z1);

	virtual float getBrightness(LevelSource &level, int_t x, int_t y, int_t z);

	static bool isFaceVisible(LevelSource &level, int_t x, int_t y, int_t z, Facing face);
	virtual bool shouldRenderFace(LevelSource &level, int_t x, int_t y, int_t z, Facing face);

	virtual int_t getTexture(LevelSource &level, int_t x, int_t y, int_t z, Facing face);
	virtual int_t getTexture(Facing face, int_t data);
	virtual int_t getTexture(Facing face);

	virtual AABB *getTileAABB(Level &level, int_t x, int_t y, int_t z);

	virtual void addAABBs(Level &level, int_t x, int_t y, int_t z, AABB &bb, std::vector<AABB *> &aabbList);
	virtual AABB *getAABB(Level &level, int_t x, int_t y, int_t z);

	virtual bool isSolidRender();

	virtual bool mayPick(int_t data, bool canPickLiquid);
	virtual bool mayPick();

	virtual void tick(Level &level, int_t x, int_t y, int_t z, Random &random);
	virtual void animateTick(Level &level, int_t x, int_t y, int_t z, Random &random);
	virtual void destroy(Level &level, int_t x, int_t y, int_t z, int_t data);
	virtual void neighborChanged(Level &level, int_t x, int_t y, int_t z, int_t tile);
	virtual void addLights(Level &level, int_t x, int_t y, int_t z);
	
	virtual int_t getTickDelay();

	virtual void onPlace(Level &level, int_t x, int_t y, int_t z);
	virtual void onRemove(Level &level, int_t x, int_t y, int_t z);

	virtual int_t getResourceCount(Random &random);
	virtual int_t getResource(int_t data, Random &random);

	float getDestroyProgress(Player &player);

	void spawnResources(Level &level, int_t x, int_t y, int_t z, int_t data);
	void spawnResources(Level &level, int_t x, int_t y, int_t z, int_t data, float chance);

	virtual int_t getSpawnResourcesAuxValue(int_t data);

	virtual HitResult clip(Level &level, int_t x, int_t y, int_t z, Vec3 &from, Vec3 &to);

private:
	virtual bool containsX(Vec3 *vec);
	virtual bool containsY(Vec3 *vec);
	virtual bool containsZ(Vec3 *vec);

public:
	virtual int_t getRenderLayer();

	virtual void stepOn(Level &level, int_t x, int_t y, int_t z, Entity &entity);
	virtual void setPlacedOnFace(Level &level, int_t x, int_t y, int_t z, Facing face);
	virtual void prepareRender(Level &level, int_t x, int_t y, int_t z);
	virtual void attack(Level &level, int_t x, int_t y, int_t z, Player &player);

	virtual void updateShape(LevelSource &level, int_t x, int_t y, int_t z);
	virtual int_t getColor(LevelSource &level, int_t x, int_t y, int_t z);

	virtual void entityInside(Level &level, int_t x, int_t y, int_t z, Entity &entity);

	virtual void updateDefaultShape();

	virtual void playerDestroy(Level &level, int_t x, int_t y, int_t z, int_t data);
};
