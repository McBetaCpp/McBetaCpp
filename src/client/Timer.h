#pragma once

#include "java/Type.h"

class Timer
{
private:
	static constexpr int_t MAX_TICKS_PER_UPDATE = 10;

public:
	float ticksPerSecond = 0.0f;

private:
	double lastTime = 0.0;

public:
	int_t ticks = 0;
	float a = 0.0f;
	float timeScale = 1.0f;
	float passedTime = 0.0f;

private:
	long_t lastMs = 0;
	long_t lastMsSysTime = 0;
	double adjustTime = 1.0;

public:
	Timer(float ticksPerSecond);
	
	void advanceTime();
	void skipTime();
};
