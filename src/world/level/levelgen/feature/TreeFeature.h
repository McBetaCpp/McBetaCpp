#pragma once

#include "world/level/levelgen/feature/Feature.h"

class TreeFeature : public Feature
{
public:
	virtual bool place(Level &level, Random &random, int_t x, int_t y, int_t z);
};
