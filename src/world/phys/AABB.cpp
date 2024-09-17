#include "world/phys/AABB.h"

#include "world/phys/HitResult.h"
#include "world/phys/Vec3.h"

std::vector<std::unique_ptr<AABB>> AABB::pool;
int_t AABB::poolPointer = 0;

AABB *AABB::newPermanent(double x0, double y0, double z0, double x1, double y1, double z1)
{
	return new AABB(x0, y0, z0, x1, y1, z1);
}

void AABB::resetPool()
{
	poolPointer = 0;
}

AABB *AABB::newTemp(double x0, double y0, double z0, double x1, double y1, double z1)
{
	if (poolPointer >= pool.size())
		pool.emplace_back(newPermanent(0.0, 0.0, 0.0, 0.0, 0.0, 0.0));
	return (pool[poolPointer++].get())->set(x0, y0, z0, x1, y1, z1);
}


AABB::AABB(double x0, double y0, double z0, double x1, double y1, double z1)
{
	this->x0 = x0;
	this->y0 = y0;
	this->z0 = z0;
	this->x1 = x1;
	this->y1 = y1;
	this->z1 = z1;
}


AABB *AABB::set(double x0, double y0, double z0, double x1, double y1, double z1)
{
	this->x0 = x0;
	this->y0 = y0;
	this->z0 = z0;
	this->x1 = x1;
	this->y1 = y1;
	this->z1 = z1;
	return this;
}

AABB *AABB::expand(double x, double y, double z) const
{
	double nx0 = x0;
	double ny0 = y0;
	double nz0 = z0;
	double nx1 = x1;
	double ny1 = y1;
	double nz1 = z1;
	if (x < 0.0)
		nx0 += x;
	if (x > 0.0)
		nx1 += x;
	if (y < 0.0)
		ny0 += y;
	if (y > 0.0)
		ny1 += y;
	if (z < 0.0)
		nz0 += z;
	if (z > 0.0)
		nz1 += z;
	return newTemp(nx0, ny0, nz0, nx1, ny1, nz1);
}

AABB *AABB::grow(double x, double y, double z) const
{
	double nx0 = x0 - x;
	double ny0 = y0 - y;
	double nz0 = z0 - z;
	double nx1 = x1 + x;
	double ny1 = y1 + y;
	double nz1 = z1 + z;
	return newTemp(nx0, ny0, nz0, nx1, ny1, nz1);
}

AABB *AABB::cloneMove(double x, double y, double z) const
{
	return newTemp(x0 + x, y0 + y, z0 + z, x1 + x, y1 + y, z1 + z);
}

double AABB::clipXCollide(const AABB &other, double x) const
{
	if (other.y1 <= y0 || other.y0 >= y1)
		return x;
	if (other.z1 <= z0 || other.z0 >= z1)
		return x;
	if (x > 0.0 && other.x1 <= x0)
	{
		double d = x0 - other.x1;
		if (d < x)
			x = d;
	}
	if (x < 0.0 && other.x0 >= x1)
	{
		double d = x1 - other.x0;
		if (d > x)
			x = d;
	}
	return x;
}

double AABB::clipYCollide(const AABB &other, double y) const
{
	if (other.x1 <= x0 || other.x0 >= x1)
		return y;
	if (other.z1 <= z0 || other.z0 >= z1)
		return y;
	if (y > 0.0 && other.y1 <= y0)
	{
		double d = y0 - other.y1;
		if (d < y)
			y = d;
	}
	if (y < 0.0 && other.y0 >= y1)
	{
		double d = y1 - other.y0;
		if (d > y)
			y = d;
	}
	return y;
}

double AABB::clipZCollide(const AABB &other, double z) const
{
	if (other.x1 <= x0 || other.x0 >= x1)
		return z;
	if (other.y1 <= y0 || other.y0 >= y1)
		return z;
	if (z > 0.0 && other.z1 <= z0)
	{
		double d = z0 - other.z1;
		if (d < z)
			z = d;
	}
	if (z < 0.0 && other.z0 >= z1)
	{
		double d = z1 - other.z0;
		if (d > z)
			z = d;
	}
	return z;
}

bool AABB::intersects(const AABB &other) const
{
	return (other.x1 <= x0 || other.x0 >= x1) ? false : ((other.y1 <= y0 || other.y0 >= y1) ? false : (!(other.z1 <= z0 || other.z0 >= z1)));
}

bool AABB::intersectsInner(const AABB &other) const
{
	return (other.x1 < x0 || other.x0 > x1) ? false : ((other.y1 < y0 || other.y0 > y1) ? false : (!(other.z1 < z0 || other.z0 > z1)));
}

AABB &AABB::move(double x, double y, double z)
{
	x0 += x;
	y0 += y;
	z0 += z;
	x1 += x;
	y1 += y;
	z1 += z;
	return *this;
}

bool AABB::intersects(double x0, double y0, double z0, double x1, double y1, double z1) const
{
	return (x1 <= this->x0 || x0 >= this->x1) ? false : ((y1 <= this->y0 || y0 >= this->y1) ? false : (!(z1 <= this->z0 || z0 >= this->z1)));
}

bool AABB::contains(const Vec3 &other) const
{
	return (other.x <= x0 || other.x >= x1) ? false : ((other.y <= y0 || other.y >= y1) ? false : (!(other.z <= z0 || other.z >= z1)));
}

double AABB::getSize() const
{
	double dx = x1 - x0;
	double dy = y1 - y0;
	double dz = z1 - z0;
	return (dx * dy * dz) / 3.0;
}

AABB *AABB::shrink(double x, double y, double z) const
{
	double nx0 = x0;
	double ny0 = y0;
	double nz0 = z0;
	double nx1 = x1;
	double ny1 = y1;
	double nz1 = z1;
	if (x < 0.0)
		nx0 -= x;
	if (x > 0.0)
		nx1 -= x;
	if (y < 0.0)
		ny0 -= y;
	if (y > 0.0)
		ny1 -= y;
	if (z < 0.0)
		nz0 -= z;
	if (z > 0.0)
		nz1 -= z;
	return newTemp(nx0, ny0, nz0, nx1, ny1, nz1);
}

AABB *AABB::copy() const
{
	return newTemp(x0, y0, z0, x1, y1, z1);
}

HitResult AABB::clip(const Vec3 &a, const Vec3 &b) const
{
	Vec3 *clipx0 = a.clipX(b, x0);
	Vec3 *clipx1 = a.clipX(b, x1);
	Vec3 *clipy0 = a.clipY(b, y0);
	Vec3 *clipy1 = a.clipY(b, y1);
	Vec3 *clipz0 = a.clipZ(b, z0);
	Vec3 *clipz1 = a.clipZ(b, z1);

	if (!containsX(*clipx0))
		clipx0 = nullptr;
	if (!containsX(*clipx1))
		clipx1 = nullptr;
	if (!containsY(*clipy0))
		clipy0 = nullptr;
	if (!containsY(*clipy1))
		clipy1 = nullptr;
	if (!containsZ(*clipz0))
		clipz0 = nullptr;
	if (!containsZ(*clipz1))
		clipz1 = nullptr;

	Vec3 *closest = nullptr;
	if (clipx0 != nullptr && (closest == nullptr || a.distanceToSqr(*clipx0) < a.distanceToSqr(*closest)))
		closest = clipx0;
	if (clipx1 != nullptr && (closest == nullptr || a.distanceToSqr(*clipx1) < a.distanceToSqr(*closest)))
		closest = clipx1;
	if (clipy0 != nullptr && (closest == nullptr || a.distanceToSqr(*clipy0) < a.distanceToSqr(*closest)))
		closest = clipy0;
	if (clipy1 != nullptr && (closest == nullptr || a.distanceToSqr(*clipy1) < a.distanceToSqr(*closest)))
		closest = clipy1;
	if (clipz0 != nullptr && (closest == nullptr || a.distanceToSqr(*clipz0) < a.distanceToSqr(*closest)))
		closest = clipz0;
	if (clipz1 != nullptr && (closest == nullptr || a.distanceToSqr(*clipz1) < a.distanceToSqr(*closest)))
		closest = clipz1;

	if (closest == nullptr)
		return HitResult();

	Facing f = Facing::NONE;
	if (closest == clipx0)
		f = Facing::WEST;
	if (closest == clipx1)
		f = Facing::EAST;
	if (closest == clipy0)
		f = Facing::DOWN;
	if (closest == clipy1)
		f = Facing::UP;
	if (closest == clipz0)
		f = Facing::NORTH;
	if (closest == clipz1)
		f = Facing::SOUTH;

	return HitResult(closest->x, closest->y, closest->z, f, *closest);
}

bool AABB::containsX(const Vec3 &other) const
{
	return other.y >= y0 && other.y <= y1 && other.z >= z0 && other.z <= z1;
}

bool AABB::containsY(const Vec3 &other) const
{
	return other.x >= x0 && other.x <= x1 && other.z >= z0 && other.z <= z1;
}

bool AABB::containsZ(const Vec3 &other) const
{
	return other.x >= x0 && other.x <= x1 && other.y >= y0 && other.y <= y1;
}

void AABB::set(const AABB &other)
{
	x0 = other.x0;
	y0 = other.y0;
	z0 = other.z0;
	x1 = other.x1;
	y1 = other.y1;
	z1 = other.z1;
}

jstring AABB::toString() const
{
	return String::fromUTF8("box[" + std::to_string(x0) + ", " + std::to_string(y0) + ", " + std::to_string(z0) + " -> " + std::to_string(x1) + ", " + std::to_string(y1) + ", " + std::to_string(z1) + "]");
}
