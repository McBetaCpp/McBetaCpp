#pragma once

#include <array>

#include "client/renderer/Tesselator.h"

#include "client/model/Vertex.h"

class Polygon
{
public:
	std::array<Vertex, 4> vertices;
	int_t vertexCount = 0;
	
private:
	bool flipNormalFlag = false;

public:
	Polygon();
	Polygon(std::array<Vertex, 4> &&vertices);
	Polygon(std::array<Vertex, 4> &&vertices, int_t u0, int_t v0, int_t u1, int_t v1);
	Polygon(std::array<Vertex, 4> &&vertices, float u0, float v0, float u1, float v1);

	void mirror();
	void render(Tesselator &t, float scale);
	
	Polygon &flipNormal();
};
