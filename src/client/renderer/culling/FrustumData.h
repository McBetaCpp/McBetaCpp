#pragma once

#include <array>

#include "world/phys/AABB.h"

#include "java/Type.h"

class FrustumData
{
public:
	static constexpr int_t RIGHT = 0;
	static constexpr int_t LEFT = 1;
	static constexpr int_t BOTTOM = 2;
	static constexpr int_t TOP = 3;
	static constexpr int_t BACK = 4;
	static constexpr int_t FRONT = 5;

	static constexpr int_t A = 0;
	static constexpr int_t B = 1;
	static constexpr int_t C = 2;
	static constexpr int_t D = 3;

	std::array<std::array<float, 16>, 16> m_Frustum = {};
	std::array<float, 16> proj = {};
	std::array<float, 16> modl = {};
	std::array<float, 16> clip = {};

	bool pointInFrustum(float x, float y, float z);
	bool sphereInFrustum(float x, float y, float z, float radius);
	bool cubeFullyInFrustum(double x1, double y1, double z1, double x2, double y2, double z2);
	bool cubeInFrustum(double x1, double y1, double z1, double x2, double y2, double z2);
	bool isVisible(AABB &aabb);
};
