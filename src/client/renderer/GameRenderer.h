#pragma once

#include <memory>

#include "client/renderer/ItemInHandRenderer.h"

#include "world/entity/Entity.h"

#include "java/Type.h"
#include "java/System.h"
#include "java/Random.h"

class Minecraft;

class GameRenderer
{
private:
	Minecraft &mc;

	float renderDistance = 0.0f;

	ItemInHandRenderer itemInHandRenderer;

	int_t ticks = 0;
	std::shared_ptr<Entity> hovered;

	double zoom = 1.0;
	double zoom_x = 0.0;
	double zoom_y = 0.0;

	long_t lastActiveTime = System::currentTimeMillis();

	Random random = Random();

public:
	volatile int xMod = 0, yMod = 0;

	float fr = 0.0f, fg = 0.0f, fb = 0.0f;
private:
	float fogBrO = 0.0f, fogBr = 0.0f;

public:
	GameRenderer(Minecraft &mc);

	void tick();
	void pick(float a);

private:
	float getFov(float a);

	void bobHurt(float a);
	void bobView(float a);
	void moveCameraToPlayer(float a);

	void setupCamera(float a, int_t eye);
	void renderItemInHand(float a, int_t eye);

public:
	void render(float a);
	void renderLevel(float a);

	void setupGuiScreen();
private:
	void setupClearColor(float a);
	void setupFog(int_t mode);

public:
	void updateAllChunks();
};
