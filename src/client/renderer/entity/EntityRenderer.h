#pragma once

#include "client/model/Model.h"
#include "client/model/HumanoidModel.h"

#include "client/renderer/TileRenderer.h"
#include "client/gui/Font.h"

#include "world/level/tile/Tile.h"
#include "world/phys/AABB.h"

#include "java/Type.h"
#include "java/String.h"

class EntityRenderDispatcher;

class EntityRenderer
{
protected:
	EntityRenderDispatcher &entityRenderDispatcher;

private:
	std::shared_ptr<Model> model = std::make_unique<HumanoidModel>();
	TileRenderer tileRenderer;

protected:
	float shadowRadius = 0.0f;
	float shadowStrength = 1.0f;

public:
	EntityRenderer(EntityRenderDispatcher &entityRenderDispatcher);

	virtual void render(Entity &entity, double x, double y, double z, float rot, float a) = 0;

protected:
	void bindTexture(const jstring &resourceName);
	void bindTexture(const jstring &urlTexture, const jstring &backupTexture);

private:
	void renderFlame(Entity &e, double x, double y, double z, float a);
	void renderShadow(Entity &e, double x, double y, double z, float pow, float a);

	Level &getLevel();

	void renderTileShadow(Tile &tt, double x, double y, double z, int_t xt, int_t yt, int_t zt, float pow, float r, double xo, double yo, double zo);

public:
	static void render(AABB &bb, double xo, double yo, double zo);
	static void renderFlat(AABB &bb);

	virtual void postRender(Entity &entity, double x, double y, double z, float rot, float a);

	Font &getFont();
};
