#pragma once

#include "Facing.h"

#include "java/Type.h"

class Minecraft;
class Player;
class Entity;
class Level;

class GameMode
{
protected:
	Minecraft &minecraft;

public:
	bool instaBuild = false;

	GameMode(Minecraft &minecraft);

	virtual ~GameMode() {}

	virtual void initLevel(std::shared_ptr<Level> level);

	virtual void startDestroyBlock(int_t x, int_t y, int_t z, Facing face);
	virtual bool destroyBlock(int_t x, int_t y, int_t z, Facing face);
	virtual void continueDestroyBlock(int_t x, int_t y, int_t z, Facing face);
	virtual void stopDestroyBlock();

	virtual void render(float a);

	virtual float getPickRange();

	//virtual bool useItem(); // TODO

	virtual void initPlayer(std::shared_ptr<Player> player);

	virtual void tick();

	virtual bool canHurtPlayer();

	virtual void adjustPlayer(std::shared_ptr<Player> player);

	//virtual bool useItemOn();

	virtual std::shared_ptr<Player> createPlayer(Level &level);

	virtual void interact(std::shared_ptr<Player> &player, std::shared_ptr<Entity> &entity);
	virtual void attack(std::shared_ptr<Player> &player, std::shared_ptr<Entity> &entity);

	virtual bool isCreativeMode() const { return false; }
};
