#include "client/model/Polygon.h"

Polygon::Polygon()
{

}

Polygon::Polygon(std::array<Vertex, 4> &&vertices) : vertices(std::move(vertices))
{
	vertexCount = this->vertices.size();
}

Polygon::Polygon(std::array<Vertex, 4> &&vertices, int_t u0, int_t v0, int_t u1, int_t v1) : Polygon(std::move(vertices))
{
	float us = 1.0F / 640.0F;
	float vs = 1.0F / 480.0F;
	this->vertices[0] = this->vertices[0].remap(u1 / 64.0f - us, v0 / 32.0f + vs);
	this->vertices[1] = this->vertices[1].remap(u0 / 64.0f + us, v0 / 32.0f + vs);
	this->vertices[2] = this->vertices[2].remap(u0 / 64.0f + us, v1 / 32.0f - vs);
	this->vertices[3] = this->vertices[3].remap(u1 / 64.0f - us, v1 / 32.0f - vs);
}

Polygon::Polygon(std::array<Vertex, 4> &&vertices, float u0, float v0, float u1, float v1) : Polygon(std::move(vertices))
{
	this->vertices[0] = this->vertices[0].remap(u1, v0);
	this->vertices[1] = this->vertices[1].remap(u0, v0);
	this->vertices[2] = this->vertices[2].remap(u0, v1);
	this->vertices[3] = this->vertices[3].remap(u1, v1);
}

void Polygon::mirror()
{
	for (int_t i = 0; i < vertices.size() / 2; i++)
		std::swap(vertices[i], vertices[vertices.size() - i - 1]);
}

void Polygon::render(Tesselator &t, float scale)
{
	Vec3 *v0 = vertices[1].pos.vectorTo(vertices[0].pos);
	Vec3 *v1 = vertices[1].pos.vectorTo(vertices[2].pos);
	Vec3 *n = v1->cross(*v0)->normalize();

	t.begin();

	if (flipNormalFlag)
		t.normal(-n->x, -n->y, -n->z);
	else
		t.normal(n->x, n->y, n->z);

	for (int_t i = 0; i < vertices.size(); i++)
	{
		Vertex &v = vertices[i];
		t.vertexUV(v.pos.x * scale, v.pos.y * scale, v.pos.z * scale, v.u, v.v);
	}

	t.end();
}

Polygon &Polygon::flipNormal()
{
	flipNormalFlag = true;
	return *this;
}
