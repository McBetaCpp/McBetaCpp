#include "world/level/tile/Tile.h"

#include <stdexcept>

#include "world/level/Level.h"
#include "world/level/LevelSource.h"
#include "world/entity/Entity.h"
#include "world/entity/player/Player.h"

// Tile properties
std::array<Tile *, 256> Tile::tiles;

std::array<bool, 256> Tile::shouldTick = {};
std::array<bool, 256> Tile::solid = {};
std::array<bool, 256> Tile::isEntityTile = {};
std::array<int_t, 256> Tile::lightBlock = {};
std::array<bool, 256> Tile::translucent = {};
std::array<int_t, 256> Tile::lightEmission = {};

// Tiles
#include "world/level/tile/StoneTile.h"
#include "world/level/tile/GrassTile.h"
#include "world/level/tile/DirtTile.h"
#include "world/level/tile/SandTile.h"
#include "world/level/tile/GravelTile.h"
#include "world/level/tile/TreeTile.h"
#include "world/level/tile/LeafTile.h"

StoneTile Tile::rock = StoneTile(1, 1);
GrassTile Tile::grass = GrassTile(2);
DirtTile Tile::dirt = DirtTile(3, 2);

SandTile Tile::sand = SandTile(12, 18);
GravelTile Tile::gravel = GravelTile(13, 19);

TreeTile Tile::treeTrunk = TreeTile(17);
LeafTile Tile::leaves = LeafTile(18, 52);

void Tile::initTiles()
{
	rock.setDestroyTime(1.5f);
	grass.setDestroyTime(0.6f);
	dirt.setDestroyTime(0.5f);

	sand.setDestroyTime(0.5f);
	gravel.setDestroyTime(0.6f);

	treeTrunk.setDestroyTime(2.0f);

	leaves.setDestroyTime(0.2f).setLightBlock(1);
}

// Impl
const jstring Tile::TILE_DESCRIPTION_PREFIX = u"tile.";

Tile::Tile(int_t id, const Material &material) : material(material)
{
	if (tiles.at(id) != nullptr)
		throw std::runtime_error("Slot " + std::to_string(id) + " is already occupied");
	tiles[id] = this;

	this->id = id;
	
	setShape(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	
	solid[id] = isSolidRender();
	lightBlock[id] = isSolidRender() ? 255 : 0;
	translucent[id] = isTranslucent();
	isEntityTile[id] = false;
}

Tile::Tile(int_t id, int_t tex, const Material &material) : Tile(id, material)
{
	this->tex = tex;
}

Tile &Tile::setLightBlock(int_t lightBlock)
{
	Tile::lightBlock[id] = lightBlock;
	return *this;
}

Tile &Tile::setLightEmission(int_t lightEmission)
{
	Tile::lightEmission[id] = lightEmission;
	return *this;
}

Tile &Tile::setExplodeable(float resistance)
{
	explosionResistance = resistance * 3.0f;
	return *this;
}

bool Tile::isTranslucent()
{
	return false;
}

bool Tile::isCubeShaped()
{
	return true;
}

Tile::Shape Tile::getRenderShape()
{
	return SHAPE_BLOCK;
}

Tile &Tile::setDestroyTime(float time)
{
	destroySpeed = time;
	return *this;
}

void Tile::setTicking(bool ticking)
{
	shouldTick[id] = ticking;
}

void Tile::setShape(float x0, float y0, float z0, float x1, float y1, float z1)
{
	xx0 = x0;
	yy0 = y0;
	zz0 = z0;
	xx1 = x1;
	yy1 = y1;
	zz1 = z1;
}

float Tile::getBrightness(LevelSource &level, int_t x, int_t y, int_t z)
{
	return level.getBrightness(x, y, z);
}

bool Tile::isFaceVisible(LevelSource &level, int_t x, int_t y, int_t z, Facing face)
{
	if (face == Facing::DOWN)
		y--;
	if (face == Facing::UP)
		y++;
	if (face == Facing::NORTH)
		z--;
	if (face == Facing::SOUTH)
		z++;
	if (face == Facing::WEST)
		x--;
	if (face == Facing::EAST)
		x++;
	return !level.isSolidTile(x, y, z);
}

bool Tile::shouldRenderFace(LevelSource &level, int_t x, int_t y, int_t z, Facing face)
{
	if (face == Facing::DOWN && yy0 > 0.0)
		return true;
	else if (face == Facing::UP && yy1 < 1.0)
		return true;
	else if (face == Facing::NORTH && zz0 > 0.0)
		return true;
	else if (face == Facing::SOUTH && zz1 < 1.0)
		return true;
	else if (face == Facing::WEST && xx0 > 0.0)
		return true;
	else if (face == Facing::EAST && xx1 < 1.0)
		return true;
	else
		return !level.isSolidTile(x, y, z);
}

int_t Tile::getTexture(LevelSource &level, int_t x, int_t y, int_t z, Facing face)
{
	return getTexture(face, level.getData(x, y, z));
}

int_t Tile::getTexture(Facing face, int_t data)
{
	return getTexture(face);
}

int_t Tile::getTexture(Facing face)
{
	return tex;
}

AABB *Tile::getTileAABB(Level &level, int_t x, int_t y, int_t z)
{
	return AABB::newTemp(x + xx0, y + yy0, z + zz0, x + xx1, y + yy1, z + zz1);
}

void Tile::addAABBs(Level &level, int_t x, int_t y, int_t z, AABB &bb, std::vector<AABB *> &aabbList)
{
	AABB *aabb = getAABB(level, x, y, z);
	if (aabb != nullptr && aabb->intersects(bb))
		aabbList.push_back(aabb);
}

AABB *Tile::getAABB(Level &level, int_t x, int_t y, int_t z)
{
	return AABB::newTemp(x + xx0, y + yy0, z + zz0, x + xx1, y + yy1, z + zz1);
}

bool Tile::isSolidRender()
{
	return true;
}

bool Tile::mayPick(int_t data, bool canPickLiquid)
{
	return mayPick();
}

bool Tile::mayPick()
{
	return true;
}

void Tile::tick(Level &level, int_t x, int_t y, int_t z, Random &random)
{

}

void Tile::animateTick(Level &level, int_t x, int_t y, int_t z, Random &random)
{

}

void Tile::destroy(Level &level, int_t x, int_t y, int_t z, int_t data)
{

}

void Tile::neighborChanged(Level &level, int_t x, int_t y, int_t z, int_t tile)
{

}

void Tile::addLights(Level &level, int_t x, int_t y, int_t z)
{

}

int_t Tile::getTickDelay()
{
	return 10;
}

void Tile::onPlace(Level &level, int_t x, int_t y, int_t z)
{

}

void Tile::onRemove(Level &level, int_t x, int_t y, int_t z)
{

}

int_t Tile::getResourceCount(Random &random)
{
	return 1;
}

int_t Tile::getResource(int_t data, Random &random)
{
	return id;
}

float Tile::getDestroyProgress(Player &player)
{
	if (destroySpeed < 0.0f)
		return 0.0f;

	if (!player.canDestroy(*this))
		return 1.0f / destroySpeed / 100.0f;

	return player.getDestroySpeed(*this) / destroySpeed / 30.0f;
}

void Tile::spawnResources(Level &level, int_t x, int_t y, int_t z, int_t data)
{
	spawnResources(level, x, y, z, data, 1.0f);
}

void Tile::spawnResources(Level &level, int_t x, int_t y, int_t z, int_t data, float chance)
{
	// TODO
}

int_t Tile::getSpawnResourcesAuxValue(int_t data)
{
	return 0;
}

HitResult Tile::clip(Level &level, int_t x, int_t y, int_t z, Vec3 &from, Vec3 &to)
{
	updateShape(level, x, y, z);

	Vec3 *localFrom = from.add(-x, -y, -z);
	Vec3 *localTo = to.add(-x, -y, -z);
	Vec3 *cxx0 = localFrom->clipX(*localTo, xx0);
	Vec3 *cxx1 = localFrom->clipX(*localTo, xx1);
	Vec3 *cyy0 = localFrom->clipY(*localTo, yy0);
	Vec3 *cyy1 = localFrom->clipY(*localTo, yy1);
	Vec3 *czz0 = localFrom->clipZ(*localTo, zz0);
	Vec3 *czz1 = localFrom->clipZ(*localTo, zz1);

	if (!containsX(cxx0)) cxx0 = nullptr;
	if (!containsX(cxx1)) cxx1 = nullptr;
	if (!containsY(cyy0)) cyy0 = nullptr;
	if (!containsY(cyy1)) cyy1 = nullptr;
	if (!containsZ(czz0)) czz0 = nullptr;
	if (!containsZ(czz1)) czz1 = nullptr;

	Vec3 *pick = nullptr;
	if (cxx0 != nullptr && (pick == nullptr || localFrom->distanceTo(*cxx0) < localFrom->distanceTo(*pick))) pick = cxx0;
	if (cxx1 != nullptr && (pick == nullptr || localFrom->distanceTo(*cxx1) < localFrom->distanceTo(*pick))) pick = cxx1;
	if (cyy0 != nullptr && (pick == nullptr || localFrom->distanceTo(*cyy0) < localFrom->distanceTo(*pick))) pick = cyy0;
	if (cyy1 != nullptr && (pick == nullptr || localFrom->distanceTo(*cyy1) < localFrom->distanceTo(*pick))) pick = cyy1;
	if (czz0 != nullptr && (pick == nullptr || localFrom->distanceTo(*czz0) < localFrom->distanceTo(*pick))) pick = czz0;
	if (czz1 != nullptr && (pick == nullptr || localFrom->distanceTo(*czz1) < localFrom->distanceTo(*pick))) pick = czz1;
	if (pick == nullptr)
		return HitResult();

	Facing face = Facing::NONE;
	if (pick == cxx0)
		face = Facing::WEST;
	if (pick == cxx1)
		face = Facing::EAST;
	if (pick == cyy0)
		face = Facing::DOWN;
	if (pick == cyy1)
		face = Facing::UP;
	if (pick == czz0)
		face = Facing::NORTH;
	if (pick == czz1)
		face = Facing::SOUTH;

	return HitResult(x, y, z, face, *pick->add(x, y, z));
}

bool Tile::containsX(Vec3 *vec)
{
	if (vec == nullptr)
		return false;
	return vec->y >= yy0 && vec->y <= yy1 && vec->z >= zz0 && vec->z <= zz1;
}

bool Tile::containsY(Vec3 *vec)
{
	if (vec == nullptr)
		return false;
	return vec->x >= xx0 && vec->x <= xx1 && vec->z >= zz0 && vec->z <= zz1;
}

bool Tile::containsZ(Vec3 *vec)
{
	if (vec == nullptr)
		return false;
	return vec->x >= xx0 && vec->x <= xx1 && vec->y >= yy0 && vec->y <= yy1;
}

int_t Tile::getRenderLayer()
{
	return 0;
}

void Tile::stepOn(Level &level, int_t x, int_t y, int_t z, Entity &entity)
{

}

void Tile::setPlacedOnFace(Level &level, int_t x, int_t y, int_t z, Facing face)
{

}

void Tile::prepareRender(Level &level, int_t x, int_t y, int_t z)
{

}

void Tile::attack(Level &level, int_t x, int_t y, int_t z, Player &player)
{

}

void Tile::updateShape(LevelSource &level, int_t x, int_t y, int_t z)
{

}

int_t Tile::getColor(LevelSource &level, int_t x, int_t y, int_t z)
{
	return 0xFFFFFF;
}

void Tile::entityInside(Level &level, int_t x, int_t y, int_t z, Entity &entity)
{

}

void Tile::updateDefaultShape()
{

}

void Tile::playerDestroy(Level &level, int_t x, int_t y, int_t z, int_t data)
{
	spawnResources(level, x, y, z, data);
}
