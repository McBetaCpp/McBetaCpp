#include "world/entity/player/Player.h"

#include "world/level/Level.h"
#include "world/level/tile/Tile.h"

#include "lwjgl/Keyboard.h"
#include "util/Mth.h"

static void (*const CS_PlaySound)(int id, int mode) = (void(*)(int, int))0x420640;

Player::Player(Level &level) : Mob(level)
{
	heightOffset = 1.62f;
	moveTo(level.xSpawn + 0.5, level.ySpawn + 1, level.zSpawn + 0.5, 0.0f, 0.0f);
	health = 20;
	modelName = u"humanoid";
	rotOffs = 180.0f;
	flameTime = 20;
	textureName = u"/mob/char.png";
}

void Player::tick()
{
	Mob::tick();
}

void Player::closeContainer()
{
	// TODO
}

void Player::rideTick()
{
	Mob::rideTick();
	oBob = bob;
	bob = 0.0f;
}

void Player::resetPos()
{
	heightOffset = 1.62f;
	setSize(0.6f, 1.8f);
	Mob::resetPos();
	health = 20;
	deathTime = 0;
}

void Player::updateAi()
{
	if (swinging)
	{
		if (++swingTime == 8)
		{
			swingTime = 0;
			swinging = false;
		}
	}
	else
	{
		swingTime = 0;
	}

	attackAnim = swingTime / 8.0f;
}

void Player::aiStep()
{
	if (level.difficulty == 0 && health < 20 && (tickCount % 20) * 12 == 0)
		heal(1);

	oBob = bob;
	Mob::aiStep();

	float targetBob = Mth::sqrt(xd * xd + zd * zd);
	float targetTilt = std::atan(-yd * 0.2) * 15.0f;

	if (targetBob > 0.1f) targetBob = 0.1f;
	if (!onGround || health <= 0) targetBob = 0.0f;
	if (onGround || health <= 0) targetTilt = 0.0f;

	bob += (targetBob - bob) * 0.4f;
	tilt += (targetTilt - tilt) * 0.8f;

	if (health > 0)
	{
		auto &es = level.getEntities(this, *bb.grow(1.0, 0.0, 1.0));
		for (const auto &e : es)
			touch(*e);
	}
}

void Player::touch(Entity &e)
{
	e.playerTouch(*this);
}

void Player::swing()
{
	swingTime = -1;
	swinging = true;
}

void Player::attack(std::shared_ptr<Entity> entity)
{
	// TODO
}

void Player::respawn()
{

}

float Player::getDestroySpeed(Tile &tile)
{
	float speed = 1.0f;

	if (!onGround)
		speed /= 5.0f;

	return speed;
}

bool Player::canDestroy(Tile &tile)
{
	// TODO
	return true;
}

void Player::readAdditionalSaveData(CompoundTag &tag)
{
	// TODO
}

void Player::addAdditionalSaveData(CompoundTag &tag)
{
	// TODO
}

float Player::getHeadHeight()
{
	return 0.12f;
}

bool Player::hurt(Entity *source, int_t dmg)
{
	noActionTime = 0;

	if (health <= 0)
		return false;

	// TODO
	// isMonster isArrow

	return dmg == 0 ? false : Mob::hurt(source, dmg);
}

void Player::actuallyHurt(int_t dmg)
{
	// TODO
}
