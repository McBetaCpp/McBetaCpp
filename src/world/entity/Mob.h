#pragma once

#include <string>

#include "world/entity/Entity.h"
#include "world/phys/AABB.h"
#include "world/phys/HitResult.h"

#include "java/Type.h"
#include "java/Math.h"

class Mob : public Entity
{
public:
	virtual jstring getEncodeId() const override { return u"Mob"; }

public:
	static constexpr int_t ATTACK_DURATION = 5;

	int_t invulnerableDuration = 20;

	float timeOffs = 0.0f;

	float rotA = (Math::random() + 1.0) * 0.01f;
	float yBodyRot = 0.0f;
	float yBodyRotO = 0.0f;

protected:
	float oRun = 0.0f;
	float run = 0.0f;

	float animStep = 0.0f;
	float animStepO = 0.0f;

	bool hasHair = true;

	jstring textureName = u"/mob/char.png";

	bool allowAlpha = true;

	float rotOffs = 0.0f;
	jstring modelName;

	float bobStrength = 1.0f;

	int_t deathScore = 0;

	float renderOffset = 0.0f;

public:
	bool interpolateOnly = false;

	float oAttackAnim = 0.0f;
	float attackAnim = 0.0f;

	int_t health = 10;
	int_t lastHealth = 0;

private:
	int_t ambientSoundTime = 0;

public:
	int_t hurtTime = 0, hurtDuration = 0;

	float hurtDir = 0.0f;

	int_t deathTime = 0;
	int_t attackTime = 0;

	float oTilt = 0.0f;
	float tilt = 0.0f;

protected:
	bool dead = false;

public:
	int_t modelNum = -1;

	float animSpeed = static_cast<float>(Math::random() + 0.9 + 0.1);

	float walkAnimSpeedO = 0.0f;
	float walkAnimSpeed = 0.0f;
	float walkAnimPos = 0.0f;

protected:
	int_t lSteps = 0;
	double lx = 0.0, ly = 0.0, lz = 0.0;
	double lyr = 0.0, lxr = 0.0;

private:
	float fallTime = 0.0f;

protected:
	int_t lastHurt = 0;
	int_t noActionTime = 0;

	float xxa = 0.0f, yya = 0.0f;
	float yRotA = 0.0f;

	bool jumping = false;
	float defaultLookAngle = 0.0f;
	float runSpeed = 0.7f;

private:
	std::shared_ptr<Entity> lookingAt;
	int_t lookTime = 0;

public:
	Mob(Level &level);

protected:
	virtual void defineSynchedData();

public:
	bool canSee(const Entity &entity);

	jstring getTexture() override;

	bool isPickable() override;
	bool isPushable() override;

	virtual float getHeadHeight() override;

	virtual int_t getAmbientSoundInterval();

	virtual void baseTick() override;

	void spawnAnim();

	virtual void rideTick() override;

	virtual void lerpTo(double x, double y, double z, float yRot, float xRot, int_t steps) override;

	void superTick();
	virtual void tick() override;

protected:
	virtual void setSize(float width, float height) override;

public:
	void heal(int_t heal);
	virtual bool hurt(Entity *source, int_t dmg) override;
	virtual void animateHurt() override;

protected:
	virtual void actuallyHurt(int_t dmg);

	virtual float getSoundVolume();

	virtual jstring getAmbientSound();
	virtual jstring getHurtSound();
	virtual jstring getDeathSound();

public:
	void knockback(Entity &source, int_t unknown, double x, double z);
	void die(Entity *source);

protected:
	virtual void dropDeathLoot();
	virtual int_t getDeathLoot();

	virtual void causeFallDamage(float distance) override;

public:
	virtual void travel(float x, float z);

	virtual bool onLadder();

	virtual bool isShootable() override;

protected:
	virtual void addAdditionalSaveData(CompoundTag &tag) override;
	virtual void readAdditionalSaveData(CompoundTag &tag) override;
	
public:
	virtual bool isAlive() override;
	virtual bool isWaterMob();

	virtual void aiStep();

protected:
	virtual void jumpFromGround();

	virtual void updateAi();

public:
	void lookAt(Entity &entity, float speed);

private:
	float rotlerp(float from, float to, float speed);

public:
	virtual void beforeRemove();

	virtual bool canSpawn();

protected:
	virtual void outOfWorld() override;

public:
	float getAttackAnim(float a);
	Vec3 *getPos(float a);
	Vec3 *getLookAngle() override;
	Vec3 *getViewVector(float a);

	HitResult pick(float length, float a);

	virtual int_t getMaxSpawnClusterSize();

	// TODO
	// ItemInstance getCarriedItem

	virtual void handleEntityEvent(byte_t event) override;
};
