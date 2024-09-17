#include "world/level/chunk/DataLayer.h"

#include <algorithm>

DataLayer::DataLayer()
{

}

DataLayer::DataLayer(byte_t *data)
{
	std::copy(data, data + this->data.size(), this->data.begin());
}

int_t DataLayer::get(int_t x, int_t y, int_t z)
{
	int_t i = (x << 11) | (z << 7) | y;
	int_t b = i >> 1;
	int_t l = i & 1;
	if (!l)
		return data[b] & 0xF;
	else
		return (data[b] >> 4) & 0xF;
}

void DataLayer::set(int_t x, int_t y, int_t z, int_t value)
{
	int_t i = (x << 11) | (z << 7) | y;
	int_t b = i >> 1;
	int_t l = i & 1;
	if (!l)
		data[b] = (data[b] & 0xF0) | (value & 0xF);
	else
		data[b] = (data[b] & 0x0F) | ((value & 0xF) << 4);
}

bool DataLayer::isValid()
{
	return true;
}

void DataLayer::setAll(int_t value)
{
	// I think this is meant to be an OR
	byte_t b = (value) & (value << 4);
	for (int_t i = 0; i < data.size(); i++)
		data[i] = b;
}
