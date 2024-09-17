#include "client/model/Cube.h"

#include "client/MemoryTracker.h"
#include "client/renderer/Tesselator.h"

Cube::Cube(int_t xTexOffs, int_t yTexOffs)
{
	this->xTexOffs = xTexOffs;
	this->yTexOffs = yTexOffs;
}

void Cube::setTexOffs(int_t xTexOffs, int_t yTexOffs)
{
	this->xTexOffs = xTexOffs;
	this->yTexOffs = yTexOffs;
}

void Cube::addBox(float x0, float y0, float z0, int_t w, int_t h, int_t d)
{
	addBox(x0, y0, z0, w, h, d, 0.0f);
}

void Cube::addBox(float x0, float y0, float z0, int_t w, int_t h, int_t d, float g)
{
	float x1 = x0 + w;
	float y1 = y0 + h;
	float z1 = z0 + d;

	x0 -= g;
	y0 -= g;
	z0 -= g;
	x1 += g;
	y1 += g;
	z1 += g;

	if (mirror)
	{
		float tmp = x1;
		x1 = x0;
		x0 = tmp;
	}

	Vertex u0 = Vertex(x0, y0, z0, 0.0f, 0.0f);
	Vertex u1 = Vertex(x1, y0, z0, 0.0f, 8.0f);
	Vertex u2 = Vertex(x1, y1, z0, 8.0f, 8.0f);
	Vertex u3 = Vertex(x0, y1, z0, 8.0f, 0.0f);

	Vertex l0 = Vertex(x0, y0, z1, 0.0f, 0.0f);
	Vertex l1 = Vertex(x1, y0, z1, 0.0f, 8.0f);
	Vertex l2 = Vertex(x1, y1, z1, 8.0f, 8.0f);
	Vertex l3 = Vertex(x0, y1, z1, 8.0f, 0.0f);

	polygons[0] = Polygon(std::array<Vertex, 4>{ l1, u1, u2, l2 }, xTexOffs + d + w, yTexOffs + d, xTexOffs + d + w + d, yTexOffs + d + h);
	polygons[1] = Polygon(std::array<Vertex, 4>{ u0, l0, l3, u3 }, xTexOffs + 0, yTexOffs + d, xTexOffs + d, yTexOffs + d + h);
	
	polygons[2] = Polygon(std::array<Vertex, 4>{ l1, l0, u0, u1 }, xTexOffs + d, yTexOffs + 0, xTexOffs + d + w, yTexOffs + d);
	polygons[3] = Polygon(std::array<Vertex, 4>{ u2, u3, l3, l2 }, xTexOffs + d + w, yTexOffs + 0, xTexOffs + d + w + w, yTexOffs + d);
	
	polygons[4] = Polygon(std::array<Vertex, 4>{ u1, u0, u3, u2 }, xTexOffs + d, yTexOffs + d, xTexOffs + d + w, yTexOffs + d + h);
	polygons[5] = Polygon(std::array<Vertex, 4>{ l0, l1, l2, l3 }, xTexOffs + d + w + d, yTexOffs + d, xTexOffs + d + w + d + w, yTexOffs + d + h);

	if (mirror)
	{
		for (auto &p : polygons)
			p.mirror();
	}
}

void Cube::addTexBox(float x0, float y0, float z0, int_t w, int_t h, int_t d, int_t tex)
{
	float x1 = x0 + w;
	float y1 = y0 + h;
	float z1 = z0 + d;

	Vertex u0 = Vertex(x0, y0, z0, 0.0f, 0.0f);
	Vertex u1 = Vertex(x1, y0, z0, 0.0f, 8.0f);
	Vertex u2 = Vertex(x1, y1, z0, 8.0f, 8.0f);
	Vertex u3 = Vertex(x0, y1, z0, 8.0f, 0.0f);

	Vertex l0 = Vertex(x0, y0, z1, 0.0f, 0.0f);
	Vertex l1 = Vertex(x1, y0, z1, 0.0f, 8.0f);
	Vertex l2 = Vertex(x1, y1, z1, 8.0f, 8.0f);
	Vertex l3 = Vertex(x0, y1, z1, 8.0f, 0.0f);

	float us = 0.25f;
	float vs = 0.25f;
	float _u0 = ((tex % 16) + 1.0f - us) / 16.0f;
	float v0 = ((tex / 16) + 1.0f - vs) / 16.0f;
	float _u1 = ((tex % 16) + us) / 16.0f;
	float v1 = ((tex / 16) + vs) / 16.0f;

	polygons[0] = Polygon(std::array<Vertex, 4>{ l1, u1, u2, l2 }, _u0, v0, _u1, v1);
	polygons[1] = Polygon(std::array<Vertex, 4>{ u0, l0, l3, u3 }, _u0, v0, _u1, v1);

	polygons[2] = Polygon(std::array<Vertex, 4>{ l1, l0, u0, u1 }, _u0, v0, _u1, v1);
	polygons[3] = Polygon(std::array<Vertex, 4>{ u2, u3, l3, l2 }, _u0, v0, _u1, v1);

	polygons[4] = Polygon(std::array<Vertex, 4>{ u1, u0, u3, u2 }, _u0, v0, _u1, v1);
	polygons[5] = Polygon(std::array<Vertex, 4>{ l0, l1, l2, l3 }, _u0, v0, _u1, v1);
}

void Cube::setPos(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

void Cube::render(float scale)
{
	if (neverRender) return;
	if (!visible) return;
	if (!compiled) compile(scale);

	if (xRot != 0.0f || yRot != 0.0f || zRot != 0.0f)
	{
		glPushMatrix();
		glTranslatef(x * scale, y * scale, z * scale);
		if (zRot != 0.0f) glRotatef(zRot * c, 0.0f, 0.0f, 1.0f);
		if (yRot != 0.0f) glRotatef(yRot * c, 0.0f, 1.0f, 0.0f);
		if (xRot != 0.0f) glRotatef(xRot * c, 1.0f, 0.0f, 0.0f);
		glCallList(list);
		glPopMatrix();
	}
	else if (x != 0.0f || y != 0.0f || z != 0.0f)
	{
		glPushMatrix();
		glTranslatef(x * scale, y * scale, z * scale);
		glCallList(list);
		glPopMatrix();
	}
	else
	{
		glCallList(list);
	}
}

void Cube::translateTo(float scale)
{
	if (neverRender) return;
	if (!visible) return;
	if (!compiled) compile(scale);

	if (xRot != 0.0f || yRot != 0.0f || zRot != 0.0f)
	{
		glTranslatef(x * scale, y * scale, z * scale);
		if (zRot != 0.0f) glRotatef(zRot * c, 0.0f, 0.0f, 1.0f);
		if (yRot != 0.0f) glRotatef(yRot * c, 0.0f, 1.0f, 0.0f);
		if (xRot != 0.0f) glRotatef(xRot * c, 1.0f, 0.0f, 0.0f);
	}
	else if (x != 0.0f || y != 0.0f || z != 0.0f)
	{
		glTranslatef(x * scale, y * scale, z * scale);
	}
}

void Cube::compile(float scale)
{
	list = MemoryTracker::genLists(1);

	glNewList(list, GL_COMPILE);
	Tesselator &t = Tesselator::instance;
	for (auto &p : polygons)
		p.render(t, scale);
	glEndList();

	compiled = true;
}
