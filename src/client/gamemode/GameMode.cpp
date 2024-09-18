#include "client/gamemode/GameMode.h"

#include "client/Minecraft.h"
#include "world/level/Level.h"

GameMode::GameMode(Minecraft &minecraft) : minecraft(minecraft)
{

}

void GameMode::initLevel(std::shared_ptr<Level> level)
{

}

void GameMode::startDestroyBlock(int_t x, int_t y, int_t z, Facing face)
{
	destroyBlock(x, y, z, face);
}

bool GameMode::destroyBlock(int_t x, int_t y, int_t z, Facing face)
{
	// TODO
	// particleEngine

	Level &level = *minecraft.level;
	Tile *oldTile = Tile::tiles[level.getTile(x, y, z)];
	int_t data = level.getData(x, y, z);
	bool changed = level.setTile(x, y, z, 0);
	if (oldTile != nullptr && changed)
	{
		oldTile->destroy(level, x, y, z, data);
	}
	return false;
}

void GameMode::continueDestroyBlock(int_t x, int_t y, int_t z, Facing face)
{

}

void GameMode::stopDestroyBlock()
{

}

void GameMode::render(float a)
{

}

float GameMode::getPickRange()
{
	return 5.0f;
}

//bool GameMode::useItem(); // TODO

void GameMode::initPlayer(std::shared_ptr<Player> player)
{

}

void GameMode::tick()
{

}

bool GameMode::canHurtPlayer()
{
	return true;
}

void GameMode::adjustPlayer(std::shared_ptr<Player> player)
{

}

//bool GameMode::useItemOn();

std::shared_ptr<Player> GameMode::createPlayer(Level &level)
{
	return Util::make_shared<LocalPlayer>(minecraft, level, minecraft.user.get(), level.dimension->id);
}

void GameMode::interact(std::shared_ptr<Player> &player, std::shared_ptr<Entity> &entity)
{
	player->interact(entity);
}

void GameMode::attack(std::shared_ptr<Player> &player, std::shared_ptr<Entity> &entity)
{
	player->attack(entity);
}
