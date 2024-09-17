#pragma once

#include <array>

#include "java/Type.h"

class DataLayer
{
public:
	std::array<byte_t, 16 * 128 * 16 / 2> data = {};

	DataLayer();
	DataLayer(byte_t *data);

	int_t get(int_t x, int_t y, int_t z);
	void set(int_t x, int_t y, int_t z, int_t value);

	bool isValid();
	void setAll(int_t value);
};
