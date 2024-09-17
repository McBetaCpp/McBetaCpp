#include "world/phys/Vec3.h"

#include "util/Mth.h"

std::vector<std::unique_ptr<Vec3>> Vec3::pool;
int_t Vec3::poolPointer = 0;

Vec3 *Vec3::newPermanent(double x, double y, double z)
{
	return new Vec3(x, y, z);
}

void Vec3::resetPool()
{
	poolPointer = 0;
}

Vec3 *Vec3::newTemp(double x, double y, double z)
{
	if (poolPointer >= pool.size())
		pool.emplace_back(newPermanent(0.0, 0.0, 0.0));
	return (pool[poolPointer++].get())->set(x, y, z);
}

Vec3::Vec3(double x, double y, double z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}
Vec3 *Vec3::set(double x, double y, double z)
{
	this->x = x;
	this->y = y;
	this->z = z;
	return this;
}

Vec3 *Vec3::interpolateTo(const Vec3 &to, double t) const
{
	double nx = x + (to.x - x) * t;
	double ny = y + (to.y - y) * t;
	double nz = z + (to.z - z) * t;
	return newTemp(nx, ny, nz);
}

Vec3 *Vec3::vectorTo(const Vec3 &to) const
{
	return newTemp(to.x - x, to.y - y, to.z - z);
}
Vec3 *Vec3::normalize() const
{
	double d = Mth::sqrt(x * x + y * y + z * z);
	return (d < 0.0001f) ? newTemp(0.0, 0.0, 0.0) : newTemp(x / d, y / d, z / d);
}

double Vec3::dot(const Vec3 &other) const
{
	return x * other.x + y * other.y + z * other.z;
}

Vec3 *Vec3::cross(const Vec3 &other) const
{
	return newTemp(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
}

Vec3 *Vec3::add(double x, double y, double z) const
{
	return newTemp(this->x + x, this->y + y, this->z + z);
}

double Vec3::distanceTo(const Vec3 &other) const
{
	double dx = other.x - x;
	double dy = other.y - y;
	double dz = other.z - z;
	return Mth::sqrt(dx * dx + dy * dy + dz * dz);
}

double Vec3::distanceToSqr(const Vec3 &other) const
{
	double dx = other.x - x;
	double dy = other.y - y;
	double dz = other.z - z;
	return dx * dx + dy * dy + dz * dz;
}

double Vec3::distanceToSqr(double x, double y, double z) const
{
	double dx = x - this->x;
	double dy = y - this->y;
	double dz = z - this->z;
	return dx * dx + dy * dy + dz * dz;
}

Vec3 *Vec3::scale(double scale) const
{
	return newTemp(x * scale, y * scale, z * scale);
}

double Vec3::length() const
{
	return Mth::sqrt(x * x + y * y + z * z);
}

Vec3 *Vec3::clipX(const Vec3 &other, double x) const
{
	double dx = other.x - this->x;
	double dy = other.y - y;
	double dz = other.z - z;
	if (dx * dx < 1.0e-7)
		return nullptr;
	double v = (x - this->x) / dx;
	return (v < 0.0 || v > 1.0) ? nullptr : newTemp(this->x + dx * v, y + dy * v, z + dz * v);
}

Vec3 *Vec3::clipY(const Vec3 &other, double y) const
{
	double dx = other.x - x;
	double dy = other.y - this->y;
	double dz = other.z - z;
	if (dy * dy < 1.0e-7)
		return nullptr;
	double v = (y - this->y) / dy;
	return (v < 0.0 || v > 1.0) ? nullptr : newTemp(x + dx * v, this->y + dy * v, z + dz * v);
}

Vec3 *Vec3::clipZ(const Vec3 &other, double z) const
{
	double dx = other.x - x;
	double dy = other.y - y;
	double dz = other.z - this->z;
	if (dz * dz < 1.0e-7)
		return nullptr;
	double v = (z - this->z) / dz;
	return (v < 0.0 || v > 1.0) ? nullptr : newTemp(x + dx * v, y + dy * v, this->z + dz * v);
}

jstring Vec3::toString() const
{
	return String::fromUTF8("(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")");
}

Vec3 *Vec3::lerp(const Vec3 &to, double t) const
{
	return newTemp(x + (to.x - x) * t, y + (to.y - y) * t, z + (to.z - z) * t);
}

void Vec3::xRot(float angle)
{
	double c = Mth::cos(angle);
	double s = Mth::sin(angle);
	double nx = x;
	double ny = y * c + z * s;
	double nz = y * c - z * s;
	x = nx;
	y = ny;
	z = nz;
}

void Vec3::yRot(float angle)
{
	double c = Mth::cos(angle);
	double s = Mth::sin(angle);
	double nx = x * c + z * s;
	double ny = y;
	double nz = x * c + z * s;
	x = nx;
	y = ny;
	z = nz;
}

void Vec3::zRot(float angle)
{
	double c = Mth::cos(angle);
	double s = Mth::sin(angle);
	double nx = x * c + y * s;
	double ny = x * c - y * s;
	double nz = z;
	x = nx;
	y = ny;
	z = nz;
}
