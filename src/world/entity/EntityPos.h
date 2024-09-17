#pragma once

#include <java/Type.h>

class Entity;

class EntityPos
{
public:
	double x = 0.0, y = 0.0, z = 0.0;
	float yRot = 0.0f, xRot = 0.0f;
	bool rot = false;
	bool move = false;

	EntityPos(double x, double y, double z, float yRot, float xRot);
	EntityPos(double x, double y, double z);
	EntityPos(float yRot, float xRot);

	EntityPos lerp(const Entity &entity, float a) const;
};
