#pragma once

#include "client/renderer/TileRenderer.h"

#include "java/Type.h"

class Minecraft;

class ItemInHandRenderer
{
private:
	Minecraft &mc;

	float height = 0.0f;
	float oHeight = 0.0f;
	TileRenderer tileRenderer;

	int_t lastSlot = 0;

public:
	ItemInHandRenderer(Minecraft &mc);

	void render(float a);
	void renderScreenEffect(float a);
	void renderTex(float a, int_t tex);
	void renderWater(float a);
	void renderFire(float a);

	void tick();
	
	void itemPlaced();
	void itemUsed();
};
