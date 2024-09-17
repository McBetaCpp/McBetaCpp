#pragma once

#include "client/renderer/culling/FrustumData.h"

#include <array>

#include "java/Type.h"

class Frustum : public FrustumData
{
private:
	static Frustum frustum;

	// std::array<float, 16> _proj = {};
	// std::array<float, 16> _modl = {};
	// std::array<float, 16> _clip = {};

public:
	static FrustumData &getFrustum();

private:
	void normalizePlane(std::array<std::array<float, 16>, 16> &frustum, int_t side);
	void calculateFrustum();
};
