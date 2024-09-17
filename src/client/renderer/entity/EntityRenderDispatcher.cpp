#include "client/renderer/entity/EntityRenderDispatcher.h"

#include "client/renderer/entity/PlayerRenderer.h"

#include "OpenGL.h"

EntityRenderDispatcher EntityRenderDispatcher::instance;

double EntityRenderDispatcher::xOff = 0.0;
double EntityRenderDispatcher::yOff = 0.0;
double EntityRenderDispatcher::zOff = 0.0;

PlayerRenderer EntityRenderDispatcher::playerRenderer = PlayerRenderer(EntityRenderDispatcher::instance);

void EntityRenderDispatcher::prepare(std::shared_ptr<Level> level, Textures &textures, Font &font, std::shared_ptr<Player> player, Options &options, float a)
{
	this->level = level;
	this->textures = &textures;
	this->options = &options;
	this->player = player;
	this->font = &font;

	playerRotY = player->yRotO + (player->yRot - player->yRotO) * a;
	playerRotX = player->xRotO + (player->xRot - player->xRotO) * a;
	xPlayer = player->xOld + (player->x - player->xOld) * a;
	yPlayer = player->yOld + (player->y - player->yOld) * a;
	zPlayer = player->zOld + (player->z - player->zOld) * a;
}

void EntityRenderDispatcher::render(Entity &entity, float a)
{
	double x = entity.xOld + (entity.x - entity.xOld) * a;
	double y = entity.yOld + (entity.y - entity.yOld) * a;
	double z = entity.zOld + (entity.z - entity.zOld) * a;
	float rot = entity.yRotO + (entity.yRot - entity.yRotO) * a;
	
	float brightness = entity.getBrightness(a);
	glColor3f(brightness, brightness, brightness);
	render(entity, x - xOff, y - yOff, z - zOff, rot, a);
}

void EntityRenderDispatcher::render(Entity &entity, double x, double y, double z, float rot, float a)
{
	playerRenderer.render(entity, x, y, z, rot, a);
	playerRenderer.postRender(entity, x, y, z, rot, a);
}

void EntityRenderDispatcher::setLevel(std::shared_ptr<Level> level)
{
	this->level = level;
}

double EntityRenderDispatcher::distanceToSqr(double x, double y, double z)
{
	double dx = x - xPlayer;
	double dy = y - yPlayer;
	double dz = z - zPlayer;
	return dx * dx + dy * dy + dz * dz;
}

Font &EntityRenderDispatcher::getFont()
{
	return *font;
}
