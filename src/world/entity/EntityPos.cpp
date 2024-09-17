#include "world/entity/EntityPos.h"

#include "world/entity/Entity.h"

EntityPos::EntityPos(double x, double y, double z, float yRot, float xRot)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->yRot = yRot;
	this->xRot = xRot;
	rot = true;
	move = true;
}

EntityPos::EntityPos(double x, double y, double z)
{
	this->x = x;
	this->y = y;
	this->z = z;
	move = true;
	rot = false;
}

EntityPos::EntityPos(float yRot, float xRot)
{
	this->yRot = yRot;
	this->xRot = xRot;
	rot = true;
	move = false;
}

EntityPos EntityPos::lerp(const Entity &entity, float a) const
{
	double lx = entity.x + (x - entity.x) * a;
	double ly = entity.y + (y - entity.y) * a;
	double lz = entity.z + (z - entity.z) * a;

	float dyrot = yRot - entity.yRot;
	float dxrot = xRot - entity.xRot;
	while (dyrot >= 180.0f)
		dyrot -= 360.0f;
	while (dyrot < -180.0f)
		dyrot += 360.0f;
	while (dxrot >= 180.0f)
		dxrot -= 360.0f;
	while (dxrot < -180.0f)
		dxrot += 360.0f;

	float lyrot = entity.yRot + dyrot * a;
	float lxrot = entity.xRot + dxrot * a;
	while (lyrot >= 180.0f)
		lyrot -= 360.0f;
	while (lyrot < -180.0f)
		lyrot += 360.0f;
	while (lxrot >= 180.0f)
		lxrot -= 360.0f;
	while (lxrot < -180.0f)
		lxrot += 360.0f;

	return (rot && move) ? EntityPos(lx, ly, lz, lyrot, lxrot) : (rot ? EntityPos(lyrot, lxrot) : EntityPos(lx, ly, lz));
}
