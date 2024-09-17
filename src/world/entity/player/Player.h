#pragma once

#include "world/entity/Mob.h"

#include "world/level/tile/Tile.h"

#include "java/Type.h"
#include "java/String.h"

class Player : public Mob
{
public:
	static constexpr int_t MAX_HEALTH = 20;
	static constexpr int_t SWING_DURATION = 8;

	byte_t userType = 0;

	int_t score = 0;

	float oBob = 0.0f;
	float bob = 0.0f;

	bool swinging = false;
	int_t swingTime = 0;

	jstring name;

	int_t dimension = 0;

	jstring cloakTexture;

	double xCloakO = 0.0, yCloakO = 0.0, zCloakO = 0.0;
	double xCloak = 0.0, yCloak = 0.0, zCloak = 0.0;

	int_t dmgSpill = 0;

	Player(Level &level);

	void tick() override;

protected:
	virtual void closeContainer();

public:
	virtual void rideTick() override;

	virtual void resetPos() override;

protected:
	virtual void updateAi() override;

public:
	virtual void aiStep() override;

private:
	void touch(Entity &e);

public:
	virtual void swing();
	virtual void attack(std::shared_ptr<Entity> entity);

	virtual void respawn();

public:
	float getDestroySpeed(Tile &tile);
	bool canDestroy(Tile &tile);

	void readAdditionalSaveData(CompoundTag &tag) override;
	void addAdditionalSaveData(CompoundTag &tag) override;

	float getHeadHeight() override;

	bool hurt(Entity *source, int_t dmg) override;

protected:
	void actuallyHurt(int_t dmg);

public:
	bool isPlayer() override { return true; }
};
