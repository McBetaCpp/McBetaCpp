#pragma once

#include "client/MemoryTracker.h"

class Lighting
{
private:
	static std::vector<float> lb;

public:
	static void turnOff();
	static void turnOn();

	static float *getBuffer(double a, double b, double c, double d);
	static float *getBuffer(float a, float b, float c, float d);
};
