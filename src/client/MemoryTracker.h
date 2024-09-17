#pragma once

#include <vector>

#include "java/Type.h"

class MemoryTracker
{
private:
	static std::vector<int_t> lists;
	static std::vector<int_t> textures;

public:
	static int_t genLists(int_t count);
	static void genTextures(std::vector<int_t> &ib);
	static void release();
	static std::vector<byte_t> createByteBuffer(int_t size);
	static std::vector<short_t> createShortBuffer(int_t size);
	static std::vector<char_t> createCharBuffer(int_t size);
	static std::vector<int_t> createIntBuffer(int_t size);
	static std::vector<long_t> createLongBuffer(int_t size);
	static std::vector<float> createFloatBuffer(int_t size);
	static std::vector<double> createDoubleBuffer(int_t size);
};
