#pragma once

#include "client/renderer/culling/Culler.h"
#include "client/renderer/culling/Frustum.h"

class FrustumCuller : public Culler
{
private:
	FrustumData &frustum = Frustum::getFrustum();
	double xOff = 0.0, yOff = 0.0, zOff = 0.0;

public:
	void prepare(double xOff, double yOff, double zOff) override;
	bool cubeFullyInFrustum(double x0, double y0, double z0, double x1, double y1, double z1) override;
	bool cubeInFrustum(double x0, double y0, double z0, double x1, double y1, double z1) override;
	bool isVisible(AABB &aabb) override;
};
