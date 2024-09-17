#pragma once

#include "client/MemoryTracker.h"

#include "java/Type.h"

class OffsettedRenderList
{
private:
	int_t x = 0, y = 0, z = 0;
	std::vector<int_t> lists; // = MemoryTracker::createIntBuffer(0x10000);
	float xOff = 0.0f, yOff = 0.0f, zOff = 0.0f;
	bool inited = false;
	bool rendered = false;

public:
	OffsettedRenderList() { lists.reserve(0x10000); }

	void init(int_t x, int_t y, int_t z, double xOff, double yOff, double zOff);

	bool isAt(int_t x, int_t y, int_t z);

	void add(int_t list);

	void render();
	void clear();
};
