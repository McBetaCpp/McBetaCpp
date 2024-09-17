#pragma once

#include "world/phys/Vec3.h"

#include "java/Type.h"

class Vertex
{
public:
	Vec3 pos = Vec3(0.0, 0.0, 0.0);
	float u = 0.0f, v = 0.0f;

	Vertex();
	Vertex(float x, float y, float z, float u, float v);
	Vertex remap(float u, float v);
	Vertex(const Vertex &other, float u, float v);
	Vertex(Vertex &&other, float u, float v);
	Vertex(const Vec3 &pos, float u, float v);
};
