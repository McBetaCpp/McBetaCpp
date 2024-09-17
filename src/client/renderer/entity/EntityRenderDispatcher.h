#pragma once

#include "world/level/Level.h"

#include "client/Options.h"
#include "client/renderer/Textures.h"
#include "client/gui/Font.h"

class PlayerRenderer;

class EntityRenderDispatcher
{
public:
	static EntityRenderDispatcher instance;

	static PlayerRenderer playerRenderer;

private:
	Font *font = nullptr;

public:
	static double xOff;
	static double yOff;
	static double zOff;

	Textures *textures = nullptr;

	std::shared_ptr<Level> level = nullptr;

	std::shared_ptr<Player> player = nullptr;
	float playerRotY = 0.0f, playerRotX = 0.0f;

	Options *options = nullptr;

	double xPlayer = 0.0, yPlayer = 0.0, zPlayer = 0.0;

	void prepare(std::shared_ptr<Level> level, Textures &textures, Font &font, std::shared_ptr<Player> player, Options &options, float a);
	void render(Entity &entity, float a);
	void render(Entity &entity, double x, double y, double z, float rot, float a);

	void setLevel(std::shared_ptr<Level> level);

	double distanceToSqr(double x, double y, double z);

	Font &getFont();
};
