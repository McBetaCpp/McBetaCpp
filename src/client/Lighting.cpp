#include "client/Lighting.h"

#include "world/phys/Vec3.h"

#include "OpenGL.h"

std::vector<float> Lighting::lb = MemoryTracker::createFloatBuffer(16);

void Lighting::turnOff()
{
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHT1);
	glDisable(GL_COLOR_MATERIAL);
}

void Lighting::turnOn()
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	float a = 0.4f;
	float d = 0.6f;
	float s = 0.0f;

	Vec3 *l = Vec3::newTemp(0.2, 1.0, -0.7)->normalize();
	glLightfv(GL_LIGHT0, GL_POSITION, getBuffer(l->x, l->y, l->z, 0.0));
	glLightfv(GL_LIGHT0, GL_DIFFUSE, getBuffer(d, d, d, 1.0f));
	glLightfv(GL_LIGHT0, GL_AMBIENT, getBuffer(0.0f, 0.0f, 0.0f, 1.0f));
	glLightfv(GL_LIGHT0, GL_SPECULAR, getBuffer(s, s, s, 1.0f));

	l = Vec3::newTemp(-0.2, 1.0, 0.7)->normalize();
	glLightfv(GL_LIGHT1, GL_POSITION, getBuffer(l->x, l->y, l->z, 0.0));
	glLightfv(GL_LIGHT1, GL_DIFFUSE, getBuffer(d, d, d, 1.0f));
	glLightfv(GL_LIGHT1, GL_AMBIENT, getBuffer(0.0f, 0.0f, 0.0f, 1.0f));
	glLightfv(GL_LIGHT1, GL_SPECULAR, getBuffer(s, s, s, 1.0f));

	glShadeModel(GL_FLAT);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, getBuffer(a, a, a, 1.0f));
}

float *Lighting::getBuffer(double a, double b, double c, double d)
{
	return getBuffer(static_cast<float>(a), static_cast<float>(b), static_cast<float>(c), static_cast<float>(d));
}
float *Lighting::getBuffer(float a, float b, float c, float d)
{
	lb[0] = a;
	lb[1] = b;
	lb[2] = c;
	lb[3] = d;
	return lb.data();
}
