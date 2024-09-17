#include "client/model/Vertex.h"

Vertex::Vertex()
{

}

Vertex::Vertex(float x, float y, float z, float u, float v) : Vertex(Vec3(x, y, z), u, v)
{

}

Vertex Vertex::remap(float u, float v)
{
	return Vertex(*this, u, v);
}

Vertex::Vertex(Vertex &other, float u, float v)
{
	pos = other.pos;
	this->u = u;
	this->v = v;
}

Vertex::Vertex(Vec3 &pos, float u, float v)
{
	this->pos = pos;
	this->u = u;
	this->v = v;
}
