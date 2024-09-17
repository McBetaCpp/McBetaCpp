#pragma once

#include "java/Type.h"
#include "java/Random.h"

class Level;

class Feature
{
public:
	virtual bool place(Level &level, Random &random, int_t x, int_t y, int_t z) = 0;

	virtual void init(double v0, double v1, double v2) {}
};
