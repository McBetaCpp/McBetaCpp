#include "world/phys/HitResult.h"

#include "world/entity/Entity.h"
#include "world/phys/Vec3.h"

HitResult::HitResult()
{
	this->type = Type::NONE;
}

HitResult::HitResult(int_t x, int_t y, int_t z, Facing f, const Vec3 &pos)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->f = f;
	this->pos = Vec3::newTemp(pos.x, pos.y, pos.z);
	this->type = Type::TILE;
}

HitResult::HitResult(std::shared_ptr<Entity> &entity)
{
	this->entity = entity;
	this->pos = Vec3::newTemp(entity->x, entity->y, entity->z);
	this->type = Type::ENTITY;
}

double HitResult::distanceTo(std::shared_ptr<Entity> &entity) const
{
	double dx = entity->x - pos->x;
	double dy = entity->y - pos->y;
	double dz = entity->z - pos->z;
	return dx * dx + dy * dy + dz * dz;
}
