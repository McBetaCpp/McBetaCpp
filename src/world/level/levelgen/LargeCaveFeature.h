#pragma once

#include "world/level/levelgen/LargeFeature.h"

class LargeCaveFeature : public LargeFeature
{
protected:
	void addRoom(int_t cx, int_t cz, std::array<ubyte_t, 16 * 16 * 128> &blocks, double x, double y, double z);
	void addTunnel(int_t cx, int_t cz, std::array<ubyte_t, 16 * 16 * 128> &blocks, double x, double y, double z, float paramFloat1, float paramFloat2, float paramFloat3, int paramInt3, int paramInt4, double paramDouble4);
	void addFeature(Level &level, int_t xx, int_t zz, int_t x, int_t z, std::array<ubyte_t, 16 * 16 * 128> &blocks) override;
};
