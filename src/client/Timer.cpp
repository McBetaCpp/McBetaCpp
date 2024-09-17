#include "Timer.h"

#include "java/System.h"

// The original timer code is very buggy, and has some issues with the
// port, partially due to threads being used differently, among other things.
// This macro reimplements the 1.15 timer code
// Uncomment to use the original implementation
#define USE_ALTERNATE_TIMER

Timer::Timer(float ticksPerSecond)
{
	this->ticksPerSecond = ticksPerSecond;
	lastMs = System::currentTimeMillis();
	lastMsSysTime = System::nanoTime() / 1000000L;
}

void Timer::advanceTime()
{
#ifdef USE_ALTERNATE_TIMER
	long_t msSysTime = System::nanoTime() / 1000000L;
	passedTime += static_cast<float>(msSysTime - lastMsSysTime) / 1000.0f * timeScale * ticksPerSecond;
	lastMsSysTime = msSysTime;

	ticks = static_cast<int_t>(passedTime);
	passedTime -= ticks;
	if (ticks > MAX_TICKS_PER_UPDATE) ticks = MAX_TICKS_PER_UPDATE;
	a = passedTime;
#else
	long_t nowMs = System::currentTimeMillis();
	long_t passedMs = nowMs - lastMs;
	long_t msSysTime = System::nanoTime() / 1000000L;

	if (passedMs > 1000)
	{
		long passedMsSysTime = msSysTime - lastMsSysTime;

		double adjustTimeT = passedMs / passedMsSysTime;
		adjustTime += (adjustTimeT - adjustTime) * 0.2;

		lastMs = nowMs;
		lastMsSysTime = msSysTime;
	}

	if (passedMs < 0)
	{
		lastMs = nowMs;
		lastMsSysTime = msSysTime;
	}

	double now = msSysTime / 1000.0;
	double passedSeconds = (now - lastTime) * adjustTime;
	lastTime = now;

	if (passedSeconds < 0.0) passedSeconds = 0.0;
	if (passedSeconds > 1.0) passedSeconds = 1.0;

	passedTime = passedTime + passedSeconds * timeScale * ticksPerSecond;

	ticks = static_cast<int_t>(passedTime);
	passedTime -= ticks;
	if (ticks > MAX_TICKS_PER_UPDATE) ticks = MAX_TICKS_PER_UPDATE;
	a = passedTime;
#endif
}

void Timer::skipTime()
{
#ifdef USE_ALTERNATE_TIMER
	long_t msSysTime = System::nanoTime() / 1000000L;
	lastMsSysTime = msSysTime;
	ticks = 0;
	a = 0.0f;
#else
	long_t nowMs = System::currentTimeMillis();
	long_t passedMs = nowMs - lastMs;
	long_t msSysTime = System::nanoTime() / 1000000L;

	if (passedMs > 1000)
	{
		long_t passedMsSysTime = msSysTime - lastMsSysTime;

		double adjustTimeT = passedMs / passedMsSysTime;
		adjustTime += (adjustTimeT - adjustTime) * 0.2;

		lastMs = nowMs;
		lastMsSysTime = msSysTime;
	}

	if (passedMs < 0)
	{
		lastMs = nowMs;
		lastMsSysTime = msSysTime;
	}

	double now = msSysTime / 1000.0;
	double passedSeconds = (now - lastTime) * adjustTime;
	lastTime = now;

	if (passedSeconds < 0.0) passedSeconds = 0.0;
	if (passedSeconds > 1.0) passedSeconds = 1.0;

	passedTime = passedTime + passedSeconds * timeScale * ticksPerSecond;

	ticks = 0;
	if (ticks > MAX_TICKS_PER_UPDATE) ticks = MAX_TICKS_PER_UPDATE;
	passedTime -= ticks;
#endif
}
