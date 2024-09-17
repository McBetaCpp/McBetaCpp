#include "java/Random.h"

#include <chrono>
#include <stdexcept>

static constexpr long_t RANDOM_MUL = 0x5DEECE66DLL;
static constexpr long_t RANDOM_ADD = 0xBLL;
static constexpr long_t RANDOM_AND = (1LL << 48) - 1;

Random::Random()
{
	auto now = std::chrono::high_resolution_clock::now();
	auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
	setSeed(static_cast<long_t>(nanos));
}

Random::Random(long_t set_seed)
{
	setSeed(set_seed);
}

void Random::setSeed(long_t set_seed)
{
	seed = (set_seed ^ RANDOM_MUL) & RANDOM_AND;
}

int_t Random::next(int_t bits)
{
	seed = (seed * RANDOM_MUL + RANDOM_ADD) & RANDOM_AND;
	return (int32_t)((uint64_t)seed >> (48 - bits));
}

bool Random::nextBoolean()
{
	return next(1) == 1;
}

int_t Random::nextInt()
{
	return next(32);
}

int_t Random::nextInt(int_t bound)
{
	// Verify that our bound is positive and non-zero
	if (bound <= 0)
		throw std::invalid_argument("bound must be positive");
	
	int_t r = next(31);
	int_t m = bound - 1;
	if ((bound & m) == 0)  // ie Bound is a power of 2
	{
		r = static_cast<int_t>((bound * static_cast<long_t>(r)) >> 31);
	}
	else
	{
		// Reject over-represented candidates
		for (int32_t u = r; u - (r = u % bound) + m < 0; u = next(31));
	}
	return r;
}

long_t Random::nextLong()
{
	return (static_cast<long_t>(next(32)) << 32) + next(32);
}

float Random::nextFloat()
{
	return next(24) / static_cast<float>(1LL << 24);
}

double Random::nextDouble()
{
	return ((static_cast<long_t>(next(27)) << 27) + next(27)) / static_cast<double>(1LL << 54);
}
