#pragma once

#include <memory>

#include "world/phys/AABB.h"
#include "world/entity/EntityPos.h"
#include "world/entity/EntityIO.h"

#include "nbt/CompoundTag.h"
#include "nbt/ListTag.h"
#include "nbt/FloatTag.h"
#include "nbt/DoubleTag.h"

#include "java/Type.h"
#include "java/Random.h"
#include "java/String.h"

class Level;
class Material;
class Player;

class Entity
{
public:
	virtual jstring getEncodeId() const { return u""; }

public:
	static constexpr int_t TOTAL_AIR_SUPPLY = 300;

private:
	static int_t entityCounter;
public:
	int_t entityId = entityCounter++;

	double viewScale = 1.0;
	bool blocksBuilding = false;

	std::shared_ptr<Entity> rider;
	std::shared_ptr<Entity> riding;

	Level &level;

	double xo = 0.0, yo = 0.0, zo = 0.0;
	double x = 0.0, y = 0.0, z = 0.0;
	double xd = 0.0, yd = 0.0, zd = 0.0;

	float yRot = 0.0, xRot = 0.0;
	float yRotO = 0.0, xRotO = 0.0;

	AABB bb = AABB(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

	bool onGround = false;
	bool horizontalCollision = false;
	bool verticalCollision = false;
	bool collision = false;
	bool hurtMarked = false;
	bool slide = true;
	bool removed = false;

	float heightOffset = 0.0f;

	float bbWidth = 0.6f, bbHeight = 1.8f;

	float walkDistO = 0.0f;
	float walkDist = 0.0f;

protected:
	bool makeStepSound = true;
	float fallDistance = 0.0f;

private:
	int_t nextStep = 1;

public:
	double xOld = 0.0, yOld = 0.0, zOld = 0.0;

	float ySlideOffset = 0.0f;
	float footSize = 0.0f;

	bool noPhysics = false;

	float pushthrough = 0.0f;

	bool hovered = false;

protected:
	Random random = Random();

public:
	int_t tickCount = 0;

	int_t flameTime = 1;
	int_t onFire = 0;

protected:
	int_t airCapacity = TOTAL_AIR_SUPPLY;
	bool wasInWater = false;
	
public:
	int_t invulnerableTime = 0;
	int_t airSupply = TOTAL_AIR_SUPPLY;

private:
	bool firstTick = true;

public:
	jstring customTextureUrl;
	jstring customTextureUrl2;

protected:
	bool fireImmune = false;
	// TODO SynchedEntityData

private:
	static constexpr int_t DATA_SHARED_FLAGS_ID = 0;
	static constexpr int_t FLAG_ONFIRE = 0;
	static constexpr int_t FLAG_SNEAKING = 1;
	static constexpr int_t FLAG_RIDING = 2;

	double xRideRotA = 0.0, yRideRotA = 0.0;

public:
	bool inChunk = false;
	int_t xChunk = 0, yChunk = 0, zChunk = 0;

	int_t xp = 0, yp = 0, zp = 0;

	Entity(Level &level);

	virtual ~Entity() {}

protected:
	virtual void resetPos();

public:
	void remove();

protected:
	virtual void setSize(float width, float height);
	void setPos(const EntityPos &pos);
	void setRot(float yRot, float xRot);

public:
	void setPos(double x, double y, double z);
	void turn(float yRot, float xRot);
	void interpolateTurn(float yRot, float xRot);

	virtual void tick();
	virtual void baseTick();

protected:
	void lavaHurt();
	virtual void outOfWorld();

public:
	bool isFree(double xd, double yd, double zd, double skin);
	bool isFree(double xd, double yd, double zd);
	void move(double xd, double yd, double zd);

protected:
	void checkFallDamage(double yd, bool onGround);

public:
	virtual AABB *getCollideBox();

protected:
	void burn(int_t a0);

	virtual void causeFallDamage(float distance);

public:
	bool isInWater();
	bool isUnderLiquid(const Material &material);

	virtual float getHeadHeight();

	bool isInLava();

	void moveRelative(float x, float z, float acc);

	float getBrightness(float a);

	void absMoveTo(double x, double y, double z, float yRot, float xRot);
	void moveTo(double x, double y, double z, float yRot, float xRot);

	float distanceTo(const Entity &entity);
	double distanceToSqr(double x, double y, double z);
	double distanceTo(double x, double y, double z);
	double distanceToSqr(const Entity &entity);

	virtual void playerTouch(Player &player);

	void push(Entity &entity);
	void push(double x, double y, double z);

protected:
	void markHurt();

public:
	virtual bool hurt(Entity *source, int_t dmg);

	bool intersects(double x0, double y0, double z0, double x1, double y1, double z1);

	virtual bool isPickable();
	virtual bool isPushable();
	virtual bool isShootable();

	virtual void awardKillScore(Entity &source, int_t dmg);

	bool shouldRender(Vec3 &z);
	bool shouldRenderAtSqrDistance(double distance);

	virtual jstring getTexture();

	virtual bool isCreativeModeAllowed();

	bool save(CompoundTag &tag);
	void saveWithoutId(CompoundTag &tag);
	void load(CompoundTag &tag);

protected:
	virtual void readAdditionalSaveData(CompoundTag &tag);
	virtual void addAdditionalSaveData(CompoundTag &tag);

public:
	float getShadowHeightOffs();

	// TODO
	// spawnAtLocation
	// spawnAtLocation
	// spawnAtLocation

	virtual bool isAlive();
	bool isInWall();

	virtual bool interact(Player &player);
	virtual AABB *getCollideAgainstBox(Entity &entity);

	virtual void rideTick();
	void positionRider();
	virtual double getRidingHeight();
	virtual double getRideHeight();

	void ride(std::shared_ptr<Entity> entity);

	virtual void lerpTo(double x, double y, double z, float yRot, float xRot, int_t steps);

	virtual float getPickRadius();
	virtual Vec3 *getLookAngle();

	virtual void handleInsidePortal();
	void lerpMotion(double x, double y, double z);

	virtual void handleEntityEvent(byte_t event);
	virtual void animateHurt();
	virtual void prepareCustomTextures();

	// TODO
	// getEquipmentSlots

	void setEquippedSlot(int_t slot, int_t itemId, int_t auxValue);

	bool isOnFire();
	bool isRiding();
	virtual bool isSneaking();

	void setSneaking(bool sneaking);

protected:
	bool getSharedFlag(int_t flag);
	void setSharedFlag(int_t flag, bool value);

public:
	virtual bool isPlayer() { return false; }
};
