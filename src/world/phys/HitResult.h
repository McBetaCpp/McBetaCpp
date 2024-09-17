#pragma once

#include <memory>

#include "Facing.h"

#include <java/Type.h>

#include "util/Memory.h"

class Entity;
class Vec3;

class HitResult
{
public:
	enum class Type
	{
		NONE = -1,
		TILE,
		ENTITY
	};
	Type type = Type::TILE;

	int_t x = 0, y = 0, z = 0;
	Facing f = Facing::DOWN;

	Vec3 *pos = nullptr;
	std::shared_ptr<Entity> entity;

	HitResult();
	HitResult(int_t x, int_t y, int_t z, Facing f, const Vec3 &pos);
	HitResult(std::shared_ptr<Entity> &entity);

	double distanceTo(std::shared_ptr<Entity> &entity) const;
};
