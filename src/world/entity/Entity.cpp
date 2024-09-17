#include "world/entity/Entity.h"

#include "world/level/Level.h"

#include "util/Mth.h"

int_t Entity::entityCounter = 0;

Entity::Entity(Level &level) : level(level)
{
	setPos(0.0, 0.0, 0.0);
	// TODO
}

void Entity::resetPos()
{
	while (y > 0.0)
	{
		setPos(x, y, z);
		if (level.getCubes(*this, bb).empty())
			break;
		y++;
	}

	xd = yd = zd = 0.0;
	xRot = 0.0f;
}

void Entity::remove()
{
	removed = true;
}

void Entity::setSize(float width, float height)
{
	bbWidth = width;
	bbHeight = height;
}

void Entity::setPos(const EntityPos &pos)
{
	if (pos.move)
		setPos(pos.x, pos.y, pos.z);
	else
		setPos(x, y, z);

	if (pos.rot)
		setRot(pos.yRot, pos.xRot);
	else
		setRot(yRot, xRot);
}

void Entity::setRot(float yRot, float xRot)
{
	this->yRot = yRot;
	this->xRot = xRot;
}

void Entity::setPos(double x, double y, double z)
{
	this->x = x;
	this->y = y;
	this->z = z;

	float hw = bbWidth / 2.0f;
	float h = bbHeight;
	bb.set(x - hw, y - heightOffset + ySlideOffset, z - hw, x + hw, y - heightOffset + ySlideOffset + h, z + hw);
}

void Entity::turn(float yRot, float xRot)
{
	float ox = this->xRot;
	float oy = this->yRot;

	this->yRot += yRot * 0.15;
	this->xRot -= xRot * 0.15;
	if (this->xRot < -90.0f) this->xRot = -90.0f;
	if (this->xRot > 90.0f) this->xRot = 90.0f;

	xRotO += this->xRot - ox;
	yRotO += this->yRot - oy;
}

void Entity::interpolateTurn(float yRot, float xRot)
{
	this->yRot = this->yRot + yRot * 0.15;
	this->xRot = this->xRot - xRot * 0.15;
	if (this->xRot < -90.0f) this->xRot = -90.0f;
	if (this->xRot > 90.0f) this->xRot = 90.0f;
}

void Entity::tick()
{
	baseTick();
}

void Entity::baseTick()
{
	if (riding != nullptr && riding->removed)
		riding = nullptr;

	tickCount++;

	// Setup interpolation
	walkDistO = walkDist;
	xo = x;
	yo = y;
	zo = z;
	yRotO = yRot;
	xRotO = xRot;

	// Water check
	if (isInWater())
	{
		if (!wasInWater && !firstTick)
		{
			// TODO: splash
		}

		fallDistance = 0.0f;
		wasInWater = true;
		onFire = 0;
	}
	else
	{
		wasInWater = false;
	}

	// Fire check
	if (level.isOnline)
	{
		onFire = 0;
	}
	else
	{
		if (onFire > 0)
		{
			if (fireImmune)
			{
				onFire -= 4;
				if (onFire < 0) onFire = 0;
			}
			else
			{
				if (onFire % 20 == 0)
					hurt(nullptr, 1);
				onFire--;
			}
		}
	}

	// Lava check
	if (isInLava())
		lavaHurt();

	// Out of world check
	if (y < -64.0)
		outOfWorld();

	// Flag check
	if (!level.isOnline)
	{
		setSharedFlag(FLAG_ONFIRE, onFire > 0);
		setSharedFlag(FLAG_RIDING, riding != nullptr);
	}

	firstTick = false;
}

void Entity::lavaHurt()
{
	if (!fireImmune)
	{
		hurt(nullptr, 4);
		onFire = 600;
	}
}

void Entity::outOfWorld()
{
	remove();
}

bool Entity::isFree(double xd, double yd, double zd, double skin)
{
	AABB *tbb = bb.grow(skin, skin, skin)->cloneMove(xd, yd, zd);
	auto cubes = level.getCubes(*this, *tbb);
	return !cubes.empty() ? false : !level.containsAnyLiquid(*tbb);
}

bool Entity::isFree(double xd, double yd, double zd)
{
	AABB *tbb = bb.cloneMove(xd, yd, zd);
	auto cubes = level.getCubes(*this, *tbb);
	return !cubes.empty() ? false : !level.containsAnyLiquid(*tbb);
}

void Entity::move(double xd, double yd, double zd)
{
	if (noPhysics)
	{
		bb.move(xd, yd, zd);
		x = (bb.x0 + bb.x1) / 2.0;
		y = bb.y0 + heightOffset - ySlideOffset;
		z = (bb.z0 + bb.z1) / 2.0;
		return;
	}

	// xd *= 20.0f;
	// yd *= 6.0f;
	// zd *= 20.0f;

	double ox = x;
	double oz = z;
	double oxd = xd;
	double oyd = yd;
	double ozd = zd;

	AABB *oldBb = bb.copy();

	// Keep us from moving off platforms when sneaking
	bool sneaking = onGround && isSneaking();
	if (sneaking)
	{
		double safeStep = 0.05;

		while (xd != 0.0 && level.getCubes(*this, *bb.cloneMove(xd, -1.0, 0.0)).empty())
		{
			if (xd < safeStep && xd >= -safeStep)
				xd = 0.0;
			else if (xd > 0.0)
				xd -= safeStep;
			else
				xd += safeStep;
			oxd = xd;
		}

		while (zd != 0.0 && level.getCubes(*this, *bb.cloneMove(0.0, -1.0, zd)).empty())
		{
			if (zd < safeStep && zd >= -safeStep)
				zd = 0.0;
			else if (zd > 0.0)
				zd -= safeStep;
			else
				zd += safeStep;
			ozd = zd;
		}
	}

	// Clip against collisions
	const auto &cubes = level.getCubes(*this, *bb.expand(xd, yd, zd));

	for (auto &cube : cubes)
		yd = cube->clipYCollide(bb, yd);
	bb.move(0.0, yd, 0.0);
	if (!slide && oyd != yd)
		xd = yd = zd = 0;
	bool grounded = onGround || (oyd != yd && oyd < 0.0);

	for (auto &cube : cubes)
		xd = cube->clipXCollide(bb, xd);
	bb.move(xd, 0.0, 0.0);
	if (!slide && oxd != xd)
		xd = yd = zd = 0;

	for (auto &cube : cubes)
		zd = cube->clipZCollide(bb, zd);
	bb.move(0.0, 0.0, zd);
	if (!slide && ozd != zd)
		xd = yd = zd = 0;

	// Check for steps
	if (footSize > 0.0f && grounded && ySlideOffset < 0.05f && (oxd != xd || ozd != zd))
	{
		double ooxd = xd;
		double ooyd = yd;
		double oozd = zd;

		xd = oxd;
		yd = footSize;
		zd = ozd;

		AABB *obb = bb.copy();
		bb.set(*obb);

		const auto &cubes = level.getCubes(*this, *bb.expand(xd, yd, zd));

		for (auto &cube : cubes)
			yd = cube->clipYCollide(bb, yd);
		bb.move(0.0, yd, 0.0);
		if (!slide && oyd != yd)
			xd = yd = zd = 0;

		for (auto &cube : cubes)
			xd = cube->clipXCollide(bb, xd);
		bb.move(xd, 0.0, 0.0);
		if (!slide && oxd != xd)
			xd = yd = zd = 0;

		for (auto &cube : cubes)
			zd = cube->clipZCollide(bb, zd);
		bb.move(0.0, 0.0, zd);
		if (!slide && ozd != zd)
			xd = yd = zd = 0;

		if (ooxd * ooxd + oozd * oozd >= xd * xd + zd * zd)
		{
			xd = ooxd;
			yd = ooyd;
			zd = oozd;
			bb.set(*obb);
		}
		else
		{
			ySlideOffset += 0.5;
		}
	}

	// Update body
	x = (bb.x0 + bb.x1) / 2.0;
	y = bb.y0 + heightOffset - ySlideOffset;
	z = (bb.z0 + bb.z1) / 2.0;

	horizontalCollision = oxd != xd || ozd != zd;
	verticalCollision = oyd != yd;
	onGround = oyd != yd && oyd < 0.0;
	collision = horizontalCollision || verticalCollision;
	
	checkFallDamage(yd, onGround);

	if (oxd != xd)
		this->xd = 0.0;
	if (oyd != yd)
		this->yd = 0.0;
	if (ozd != zd)
		this->zd = 0.0;

	// Footsteps
	float fdx = x - ox;
	float fdz = z - oz;

	if (makeStepSound && !sneaking)
	{
		walkDist += Mth::sqrt(fdx * fdx + fdz * fdz) * 0.6;

		int_t sx = Mth::floor(x);
		int_t sy = Mth::floor(y - 0.2 - heightOffset);
		int_t sz = Mth::floor(z);

		int_t stile = level.getTile(sx, sy, sz);
		if (walkDist > nextStep && stile > 0)
		{
			nextStep++;

			// TODO: play footsteps

			Tile::tiles[stile]->stepOn(level, sx, sy, sz, *this);
		}
	}

	// Check tile collisions
	int_t x0 = Mth::floor(bb.x0);
	int_t x1 = Mth::floor(bb.x1);
	int_t y0 = Mth::floor(bb.y0);
	int_t y1 = Mth::floor(bb.y1);
	int_t z0 = Mth::floor(bb.z0);
	int_t z1 = Mth::floor(bb.z1);

	if (level.hasChunksAt(x0, y0, z0, x1, y1, z1))
	{
		for (int_t x = x0; x <= x1; x++)
		{
			for (int_t y = y0; y <= y1; y++)
			{
				for (int_t z = z0; z <= z1; z++)
				{
					int_t tile = level.getTile(x, y, z);
					if (tile > 0)
						Tile::tiles[tile]->entityInside(level, x, y, z, *this);
				}
			}
		}
	}

	ySlideOffset *= 0.4f;

	// Fire collisions
	
	// xd /= 20.0f;
	// yd /= 6.0f;
	// zd /= 20.0f;
}

void Entity::checkFallDamage(double yd, bool onGround)
{

}

AABB *Entity::getCollideBox()
{
	return nullptr;
}

void Entity::burn(int_t a0)
{

}

void Entity::causeFallDamage(float distance)
{

}

bool Entity::isInWater()
{
	return false;
}

bool Entity::isUnderLiquid(const Material &material)
{
	return false;
}

float Entity::getHeadHeight()
{
	return 0.0f;
}

bool Entity::isInLava()
{
	return false;
}

void Entity::moveRelative(float x, float z, float acc)
{
	float targetSqr = Mth::sqrt(x * x + z * z);
	if (!(targetSqr < 0.01F))
	{
		if (targetSqr < 1.0F) targetSqr = 1.0F;

		targetSqr = acc / targetSqr;
		x *= targetSqr;
		z *= targetSqr;

		float s = Mth::sin(yRot * Mth::PI / 180.0F);
		float c = Mth::cos(yRot * Mth::PI / 180.0F);
		xd += x * c - z * s;
		zd += z * c + x * s;
	}
}

float Entity::getBrightness(float a)
{
	int_t x = Mth::floor(this->x);
	double byo = (bb.y1 - bb.y0) * 0.66;
	int_t y = Mth::floor(this->y - heightOffset + byo);
	int_t z = Mth::floor(this->z);
	return level.hasChunksAt(Mth::floor(bb.x0), Mth::floor(bb.y0), Mth::floor(bb.z0), Mth::floor(bb.x1), Mth::floor(bb.y1), Mth::floor(bb.z1)) ? level.getBrightness(x, y, z) : 0.0f;
}

void Entity::absMoveTo(double x, double y, double z, float yRot, float xRot)
{
	xo = this->x = x;
	yo = this->y = y + heightOffset;
	zo = this->z = z;
	yRotO = this->yRot = yRot;
	xRotO = this->xRot = xRot;
	ySlideOffset = 0.0f;

	double dyRot = (yRotO - yRot);
	if (dyRot < -180.0)
		yRotO += 360.0;
	if (dyRot >= 180.0)
		yRotO -= 360.0;

	setPos(x, y, z);
	setRot(yRot, xRot);
}

void Entity::moveTo(double x, double y, double z, float yRot, float xRot)
{
	xOld = xo = this->x = x;
	yOld = yo = this->y = y + heightOffset;
	zOld = zo = this->z = z;
	this->yRot = yRot;
	this->xRot = xRot;
	setPos(x, y, z);
}

float Entity::distanceTo(const Entity &entity)
{
	float dx = x - entity.x;
	float dy = y - entity.y;
	float dz = z - entity.z;
	return Mth::sqrt(dx * dx + dy * dy + dz * dz);
}

double Entity::distanceToSqr(double x, double y, double z)
{
	float dx = this->x - x;
	float dy = this->y - y;
	float dz = this->z - z;
	return dx * dx + dy * dy + dz * dz;
}

double Entity::distanceTo(double x, double y, double z)
{
	float dx = this->x - x;
	float dy = this->y - y;
	float dz = this->z - z;
	return Mth::sqrt(dx * dx + dy * dy + dz * dz);
}

double Entity::distanceToSqr(const Entity &entity)
{
	float dx = x - entity.x;
	float dy = y - entity.y;
	float dz = z - entity.z;
	return dx * dx + dy * dy + dz * dz;
}

void Entity::playerTouch(Player &player)
{

}

void Entity::push(Entity &entity)
{

}

void Entity::push(double x, double y, double z)
{

}

void Entity::markHurt()
{

}

bool Entity::hurt(Entity *source, int_t dmg)
{
	return false;
}

bool Entity::intersects(double x0, double y0, double z0, double x1, double y1, double z1)
{
	return false;
}

bool Entity::isPickable()
{
	return false;
}

bool Entity::isPushable()
{
	return false;
}

bool Entity::isShootable()
{
	return false;
}

void Entity::awardKillScore(Entity &source, int_t dmg)
{

}

bool Entity::shouldRender(Vec3 &v)
{
	double dx = x - v.x;
	double dy = y - v.y;
	double dz = z - v.z;
	double d = dx * dx + dy * dy + dz * dz;
	return shouldRenderAtSqrDistance(d);
}

bool Entity::shouldRenderAtSqrDistance(double distance)
{
	double size = bb.getSize();
	size *= 64 * viewScale;
	return distance < (size * size);
}

jstring Entity::getTexture()
{
	return u"";
}

bool Entity::isCreativeModeAllowed()
{
	return false;
}

bool Entity::save(CompoundTag &tag)
{
	jstring id = getEncodeId();
	if (removed || id.empty())
		return false;

	tag.putString(u"id", id);
	saveWithoutId(tag);
	return true;
}

void Entity::saveWithoutId(CompoundTag &tag)
{
	std::shared_ptr<ListTag> listTag = Util::make_shared<ListTag>();
	listTag->add(Util::make_shared<DoubleTag>(x));
	listTag->add(Util::make_shared<DoubleTag>(y));
	listTag->add(Util::make_shared<DoubleTag>(z));
	tag.put(u"Pos", listTag);

	listTag = Util::make_shared<ListTag>();
	listTag->add(Util::make_shared<DoubleTag>(xd));
	listTag->add(Util::make_shared<DoubleTag>(yd));
	listTag->add(Util::make_shared<DoubleTag>(zd));
	tag.put(u"Motion", listTag);

	listTag = Util::make_shared<ListTag>();
	listTag->add(Util::make_shared<FloatTag>(yRot));
	listTag->add(Util::make_shared<FloatTag>(xRot));
	tag.put(u"Rotation", listTag);

	tag.putFloat(u"FallDistance", fallDistance);
	tag.putShort(u"Fire", onFire);
	tag.putShort(u"Air", airSupply);
	tag.putBoolean(u"OnGround", onGround);

	addAdditionalSaveData(tag);
}

void Entity::load(CompoundTag &tag)
{
	std::shared_ptr<ListTag> posTag = tag.getList(u"Pos");
	std::shared_ptr<ListTag> motionTag = tag.getList(u"Motion");
	std::shared_ptr<ListTag> rotationTag = tag.getList(u"Rotation");

	setPos(0.0, 0.0, 0.0);
	xd = (reinterpret_cast<DoubleTag*>(motionTag->get(0).get()))->data;
	yd = (reinterpret_cast<DoubleTag*>(motionTag->get(1).get()))->data;
	zd = (reinterpret_cast<DoubleTag*>(motionTag->get(2).get()))->data;
	xo = xOld = x = (reinterpret_cast<DoubleTag*>(posTag->get(0).get()))->data;
	yo = yOld = y = (reinterpret_cast<DoubleTag*>(posTag->get(1).get()))->data;
	zo = zOld = z = (reinterpret_cast<DoubleTag*>(posTag->get(2).get()))->data;
	yRotO = yRot = (reinterpret_cast<FloatTag*>(rotationTag->get(0).get()))->data;
	xRotO = xRot = (reinterpret_cast<FloatTag*>(rotationTag->get(1).get()))->data;

	fallDistance = tag.getFloat(u"FallDistance");
	onFire = tag.getShort(u"Fire");
	airSupply = tag.getShort(u"Air");
	onGround = tag.getBoolean(u"OnGround");

	setPos(x, y, z);

	readAdditionalSaveData(tag);
}

void Entity::readAdditionalSaveData(CompoundTag &tag)
{

}

void Entity::addAdditionalSaveData(CompoundTag &tag)
{

}

float Entity::getShadowHeightOffs()
{
	return bbHeight / 2.0f;
}

// TODO
// spawnAtLocation
// spawnAtLocation
// spawnAtLocation

bool Entity::isAlive()
{
	return false;
}

bool Entity::isInWall()
{
	return false;
}

bool Entity::interact(Player &player)
{
	return false;
}

AABB *Entity::getCollideAgainstBox(Entity &entity)
{
	return nullptr;
}

void Entity::rideTick()
{

}

void Entity::positionRider()
{

}

double Entity::getRidingHeight()
{
	return 0.0;
}

double Entity::getRideHeight()
{
	return 0.0;
}

void Entity::ride(std::shared_ptr<Entity> entity)
{

}

void Entity::lerpTo(double x, double y, double z, float yRot, float xRot, int_t steps)
{

}

float Entity::getPickRadius()
{
	return 0.0f;
}

Vec3 *Entity::getLookAngle()
{
	return nullptr;
}

void Entity::handleInsidePortal()
{

}

void Entity::lerpMotion(double x, double y, double z)
{

}

void Entity::handleEntityEvent(byte_t event)
{

}

void Entity::animateHurt()
{

}

void Entity::prepareCustomTextures()
{

}

// TODO
// getEquipmentSlots

void Entity::setEquippedSlot(int_t slot, int_t itemId, int_t auxValue)
{

}

bool Entity::isOnFire()
{
	return false;
}

bool Entity::isRiding()
{
	return false;
}

bool Entity::isSneaking()
{
	return false;
}

void Entity::setSneaking(bool sneaking)
{

}

bool Entity::getSharedFlag(int_t flag)
{
	return false;
}

void Entity::setSharedFlag(int_t flag, bool value)
{

}
