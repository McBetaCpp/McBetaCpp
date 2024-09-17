#include "world/entity/Mob.h"

#include "world/level/Level.h"

#include "util/Mth.h"

#include <cmath>

Mob::Mob(Level &level) : Entity(level)
{

}

void Mob::defineSynchedData()
{

}

bool Mob::canSee(const Entity &entity)
{
	return false;
}

jstring Mob::getTexture()
{
	return u"/mob/quote.png";
}

bool Mob::isPickable()
{
	return false;
}

bool Mob::isPushable()
{
	return false;
}

float Mob::getHeadHeight()
{
	return 0.0f;
}

int_t Mob::getAmbientSoundInterval()
{
	return 0;
}

void Mob::baseTick()
{
	oAttackAnim = attackAnim;

	Entity::baseTick();

	if (isAlive() && isInWall())
		hurt(nullptr, 1);

	if (fireImmune || level.isOnline)
		onFire = 0;

	// TODO water material

	oTilt = tilt;
	if (attackTime > 0) attackTime--;
	if (hurtTime > 0) hurtTime--;
	if (invulnerableTime > 0) invulnerableTime--;

	if (health <= 0)
	{
		deathTime++;
		if (deathTime > 20)
		{
			beforeRemove();
			remove();
			// TODO
		}
	}

	animStepO = animStep;
	yBodyRotO = yBodyRot;
	yRotO = yRot;
	xRotO = xRot;
}

void Mob::spawnAnim()
{

}

void Mob::rideTick()
{

}

void Mob::lerpTo(double x, double y, double z, float yRot, float xRot, int_t steps)
{

}

void Mob::superTick()
{
	Entity::tick();
}

void Mob::tick()
{
	Entity::tick();

	aiStep();

	double fxd = x - xo;
	double fzd = z - zo;
	float fd = Mth::sqrt(fxd * fxd + fzd * fzd);

	float targetBodyRot = yBodyRot;
	float speedAnimStep = 0.0f;

	oRun = run;

	float targetRun = 0.0f;
	if (fd > 0.05f)
	{
		targetRun = 1.0f;
		speedAnimStep = fd * 3.0f;
		targetBodyRot = std::atan2(fzd, fxd) * 180.0f / Mth::PI - 90.0f;
	}

	if (attackAnim > 0.0f)
		targetBodyRot = yRot;

	if (!onGround)
		targetRun = 0.0f;

	run += (targetRun - run) * 0.3f;

	float deltaBodyRot = targetBodyRot - yBodyRot;
	while (deltaBodyRot < -180.0f) deltaBodyRot += 360.0f;
	while (deltaBodyRot >= 180.0f) deltaBodyRot -= 360.0f;

	yBodyRot += deltaBodyRot * 0.3f;

	float deltaYRot = yRot - yBodyRot;
	while (deltaYRot < -180.0f) deltaYRot += 360.0f;
	while (deltaYRot >= 180.0f) deltaYRot -= 360.0f;

	bool rotAtLimit = deltaYRot < -90.0f || deltaYRot >= 90.0f;
	if (deltaYRot < -75.0f) deltaYRot = -75.0f;
	if (deltaYRot >= 75.0f) deltaYRot = 75.0f;

	yBodyRot = yRot - deltaYRot;
	if (deltaYRot * deltaYRot > 2500.0f)
		yBodyRot += deltaYRot * 0.2f;

	if (rotAtLimit)
		speedAnimStep *= -1.0f;

	while (yRot - yRotO < -180.0f) yRotO -= 360.0f;
	while (yRot - yRotO >= 180.0f) yRotO += 360.0f;

	while (yBodyRot - yBodyRotO < -180.0f) yBodyRotO -= 360.0f;
	while (yBodyRot - yBodyRotO >= 180.0f) yBodyRotO += 360.0f;

	while (xRot - xRotO < -180.0f) xRotO -= 360.0f;
	while (xRot - xRotO >= 180.0f) xRotO += 360.0f;

	animStep += speedAnimStep;
}

void Mob::setSize(float width, float height)
{

}

void Mob::heal(int_t heal)
{

}

bool Mob::hurt(Entity *source, int_t dmg)
{
	return false;
}

void Mob::animateHurt()
{

}

void Mob::actuallyHurt(int_t dmg)
{

}

float Mob::getSoundVolume()
{
	return 0.0f;
}

jstring Mob::getAmbientSound()
{
	return u"";
}

jstring Mob::getHurtSound()
{
	return u"";
}

jstring Mob::getDeathSound()
{
	return u"";
}

void Mob::knockback(Entity &source, int_t unknown, double x, double z)
{

}

void Mob::die(Entity *source)
{

}

void Mob::dropDeathLoot()
{

}

int_t Mob::getDeathLoot()
{
	return 0;
}

void Mob::causeFallDamage(float distance)
{

}

void Mob::travel(float x, float z)
{
	if (isInWater())
	{
		double oy = y;

		moveRelative(x, z, 0.02f);
		move(xd, yd, zd);

		xd *= 0.8;
		yd *= 0.8;
		zd *= 0.8;
		yd -= 0.02;

		if (horizontalCollision && isFree(xd, yd + 0.6 - y + oy, zd))
			yd = 0.3;
	}
	else if (isInLava())
	{
		double oy = y;

		moveRelative(x, z, 0.02f);
		move(xd, yd, zd);

		xd *= 0.5;
		yd *= 0.5;
		zd *= 0.5;
		yd -= 0.02;

		if (horizontalCollision && isFree(xd, yd + 0.6 - y + oy, zd))
			yd = 0.3;
	}
	else
	{
		float friction = 0.91f;
		if (onGround)
		{
			friction = 0.546f;
			int_t tile = level.getTile(Mth::floor(x), Mth::floor(bb.y0) - 1, Mth::floor(z));
			if (tile > 0)
				friction = Tile::tiles[tile]->friction * 0.91f;
		}

		float acceleration = 0.16277136f / (friction * friction * friction);
		moveRelative(x, z, onGround ? (0.1f * acceleration) : 0.02f);

		friction = 0.91f;
		if (onGround)
		{
			friction = 0.546f;
			int_t tile = level.getTile(Mth::floor(x), Mth::floor(bb.y0) - 1, Mth::floor(z));
			if (tile > 0)
				friction = Tile::tiles[tile]->friction * 0.91f;
		}

		if (onLadder())
		{
			fallDistance = 0.0f;
			if (yd < -0.15) yd = -0.15;
		}

		move(xd, yd, zd);

		if (horizontalCollision && onLadder())
			yd = 0.2;

		yd -= 0.08;
		yd *= 0.98;
		xd *= friction;
		zd *= friction;
	}

	walkAnimSpeedO = walkAnimSpeed;
	double fdx = this->x - this->xo;
	double fdz = this->z - this->zo;
	float targetWalkAnimSpeed = Mth::sqrt(fdx * fdx + fdz * fdz) * 4.0f;
	if (targetWalkAnimSpeed > 1.0f) targetWalkAnimSpeed = 1.0f;
	walkAnimSpeed += (targetWalkAnimSpeed - walkAnimSpeed) * 0.4f;
	walkAnimPos += walkAnimSpeed;
}

bool Mob::onLadder()
{
	return false;
}

bool Mob::isShootable()
{
	return false;
}

void Mob::addAdditionalSaveData(CompoundTag &tag)
{

}

void Mob::readAdditionalSaveData(CompoundTag &tag)
{

}

bool Mob::isAlive()
{
	return false;
}

bool Mob::isWaterMob()
{
	return false;
}

void Mob::aiStep()
{
	if (lSteps > 0)
	{
		double xd = x + (lx - x) / lSteps;
		double yd = y + (ly - y) / lSteps;
		double zd = z + (lz - z) / lSteps;

		double deltaYRot = lyr - yRot;
		while (deltaYRot < -180.0f) deltaYRot += 360.0f;
		while (deltaYRot >= 180.0f) deltaYRot -= 360.0f;

		yRot = yRot + deltaYRot / lSteps;
		xRot = xRot + (lxr - xRot) / lSteps;

		--lSteps;

		setPos(xd, yd, zd);
		setRot(yRot, xRot);
	}

	if (health <= 0)
	{
		jumping = false;
		xxa = 0.0f;
		yya = 0.0f;
		yRotA = 0.0f;
	}
	else if (!interpolateOnly)
	{
		updateAi();
	}

	// Jump
	bool inWater = isInWater();
	bool inLava = isInLava();

	if (jumping)
	{
		if (inWater)
			yd += 0.04;
		else if (inLava)
			yd += 0.04;
		else if (onGround)
			jumpFromGround();
	}

	// Movement
	xxa *= 0.98f;
	yya *= 0.98f;
	yRotA *= 0.98f;
	travel(xxa, yya);

	const auto &entities = level.getEntities(this, *bb.grow(0.2, 0.0f, 0.2));
	if (!entities.empty())
	{
		for (const auto &entity : entities)
		{
			if (entity->isPushable())
				entity->push(*this);
		}
	}
}

void Mob::jumpFromGround()
{
	yd = 0.42;
}

void Mob::updateAi()
{

}

void Mob::lookAt(Entity &entity, float speed)
{

}

float Mob::rotlerp(float from, float to, float speed)
{
	return 0.0f;
}

void Mob::beforeRemove()
{

}

bool Mob::canSpawn()
{
	return false;
}

void Mob::outOfWorld()
{
	hurt(nullptr, 4);
}

float Mob::getAttackAnim(float a)
{
	float f = attackAnim - oAttackAnim;
	if (f < 0.0f) f++;
	return oAttackAnim + f * a;
}

Vec3 *Mob::getPos(float a)
{
	if (a == 1.0f)
		return Vec3::newTemp(x, y, z);

	double xd = xo + (x - xo) * a;
	double yd = yo + (y - yo) * a;
	double zd = zo + (z - zo) * a;
	return Vec3::newTemp(xd, yd, zd);
}

Vec3 *Mob::getLookAngle()
{
	return getViewVector(1.0f);
}

Vec3 *Mob::getViewVector(float a)
{
	if (a == 1.0f)
	{
		float cy = Mth::cos(-yRot * Mth::DEGRAD - Mth::PI);
		float sy = Mth::sin(-yRot * Mth::DEGRAD - Mth::PI);
		float cx = -Mth::cos(-xRot * Mth::DEGRAD);
		float sx = Mth::sin(-xRot * Mth::DEGRAD);
		return Vec3::newTemp(sy * cx, sx, cy * cx);
	}
	else
	{
		float xa = yRotO + (yRot - yRotO) * a;
		float ya = xRotO + (xRot - xRotO) * a;
		float cy = Mth::cos(-xa * Mth::DEGRAD - Mth::PI);
		float sy = Mth::sin(-xa * Mth::DEGRAD - Mth::PI);
		float cx = -Mth::cos(-ya * Mth::DEGRAD);
		float sx = Mth::sin(-ya * Mth::DEGRAD);
		return Vec3::newTemp(sy * cx, sx, cy * cx);
	}
}

HitResult Mob::pick(float length, float a)
{
	Vec3 *pos = getPos(a);
	Vec3 *look = getLookAngle();
	Vec3 *to = pos->add(look->x * length, look->y * length, look->z * length);
	return level.clip(*pos, *to);
}

int_t Mob::getMaxSpawnClusterSize()
{
	return 0;
}

// TODO
// ItemInstance getCarriedItem

void Mob::handleEntityEvent(byte_t event)
{

}
