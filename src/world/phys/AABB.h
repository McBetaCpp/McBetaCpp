#pragma once

#include <vector>
#include <memory>
#include <string>

#include "java/Type.h"
#include "java/String.h"

#include "util/Memory.h"

class Vec3;
class HitResult;

class AABB
{
private:
	static std::vector<std::unique_ptr<AABB>> pool;
	static int_t poolPointer;

public:
	double x0, y0, z0;
	double x1, y1, z1;

	static AABB *newPermanent(double x0, double y0, double z0, double x1, double y1, double z1);
	static void resetPool();
	static AABB *newTemp(double x0, double y0, double z0, double x1, double y1, double z1);

public:
	AABB(double x0, double y0, double z0, double x1, double y1, double z1);

	AABB *set(double x0, double y0, double z0, double x1, double y1, double z1);
	AABB *expand(double x, double y, double z) const;
	AABB *grow(double x, double y, double z) const;
	AABB *cloneMove(double x, double y, double z) const;
	double clipXCollide(const AABB &other, double x) const;
	double clipYCollide(const AABB &other, double y) const;
	double clipZCollide(const AABB &other, double z) const;
	bool intersects(const AABB &other) const;
	bool intersectsInner(const AABB &other) const;
	AABB &move(double x, double y, double z);
	bool intersects(double x0, double y0, double z0, double x1, double y1, double z1) const;
	bool contains(const Vec3 &other) const;
	double getSize() const;
	AABB *shrink(double x, double y, double z) const;
	AABB *copy() const;
	HitResult clip(const Vec3 &a, const Vec3 &b) const;
	bool containsX(const Vec3 &other) const;
	bool containsY(const Vec3 &other) const;
	bool containsZ(const Vec3 &other) const;
	void set(const AABB &other);
	jstring toString() const;
};
