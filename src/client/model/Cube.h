#pragma once

#include "client/model/Vertex.h"
#include "client/model/Polygon.h"

#include "util/Mth.h"

class Cube
{
private:
	std::array<Polygon, 6> polygons;
	int_t xTexOffs = 0;
	int_t yTexOffs = 0;

public:
	float x = 0.0f, y = 0.0f, z = 0.0f;
	float xRot = 0.0f, yRot = 0.0f, zRot = 0.0f;

private:
	bool compiled = false;
	int_t list = 0;

public:
	bool mirror = false;
	bool visible = true;
	bool neverRender = false;

private:
	static constexpr float c = Mth::RADDEG;

public:
	Cube(int_t xTexOffs, int_t yTexOffs);
	
	void setTexOffs(int_t xTexOffs, int_t yTexOffs);
	void addBox(float x0, float y0, float z0, int_t w, int_t h, int_t d);
	void addBox(float x0, float y0, float z0, int_t w, int_t h, int_t d, float g);
	void addTexBox(float x0, float y0, float z0, int_t w, int_t h, int_t d, int_t tex);
	void setPos(float x, float y, float z);

	void render(float scale);
	void translateTo(float scale);
	void compile(float scale);
};
