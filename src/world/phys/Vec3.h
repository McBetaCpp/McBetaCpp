#pragma once

#include <vector>
#include <string>

#include "java/Type.h"
#include "java/String.h"

class Vec3
{
private:
	static std::vector<std::unique_ptr<Vec3>> pool;
	static int_t poolPointer;

public:
	double x, y, z;

	static Vec3 *newPermanent(double x, double y, double z);
	static void resetPool();
	static Vec3 *newTemp(double x, double y, double z);

	Vec3(double x, double y, double z);

private:
	Vec3 *set(double x, double y, double z);

public:
	Vec3 *interpolateTo(const Vec3 &to, double t) const;
	Vec3 *vectorTo(const Vec3 &to) const;
	Vec3 *normalize() const;
	double dot(const Vec3 &other) const;
	Vec3 *cross(const Vec3 &other) const;
	Vec3 *add(double x, double y, double z) const;
	double distanceTo(const Vec3 &other) const;
	double distanceToSqr(const Vec3 &other) const;
	double distanceToSqr(double x, double y, double z) const;
	Vec3 *scale(double scale) const;
	double length() const;
	Vec3 *clipX(const Vec3 &other, double x) const;
	Vec3 *clipY(const Vec3 &other, double y) const;
	Vec3 *clipZ(const Vec3 &other, double z) const;
	jstring toString() const;
	Vec3 *lerp(const Vec3 &to, double t) const;
	void xRot(float angle);
	void yRot(float angle);
	void zRot(float angle);
};
